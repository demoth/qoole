/*
Copyright (C) 1996-1997 GX Media, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

// QPropSheet.cpp : implementation file
//

#include "stdafx.h"

#include "Qoole.h"
#include "QPropSht.h"
#include "QMainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//======================================================================
// QPropSheet
//======================================================================

IMPLEMENT_DYNAMIC(QPropSheet, CPropertySheet)

QPropSheet::QPropSheet(CWnd *pParentWnd, UINT iSelectPage, CRect *pRect)
		   :CPropertySheet("Properties", pParentWnd, iSelectPage) {
	// Add Pages...
	AddPage(&m_FilePage);
	AddPage(&m_ObjectPage);
	AddPage(&m_EntityPage);
	AddPage(&m_TexturePage);

	// Add additional game specific properties pages.
	ASSERT(pQMainFrame->GetDeskTopDocument() != NULL);
	Game *pGame = pQMainFrame->GetDeskTopDocument()->GetGame();
	ASSERT(pGame != NULL);
	DWORD pagesFlag = pGame->GetExtraPropPages();

	if ((pagesFlag & PROPPAGE_ADDSURFACE) != 0)
		AddPage(&m_SurfacePage);
	if ((pagesFlag & PROPPAGE_ADDCONTENT) != 0)
		AddPage(&m_ContentPage);

	// Create the window.
	DWORD style = WS_SYSMENU | WS_POPUP | WS_CAPTION | DS_MODALFRAME;
	BOOL ok = Create(pParentWnd, style);
	ASSERT(ok);
	if (pRect != NULL) {
		MoveWindow(pRect);
	}
	else {
		CRect rect;
		pParentWnd->GetWindowRect(&rect);
		int btn = rect.bottom - 40;
		GetWindowRect(&rect);
		btn = btn - rect.bottom;
		rect.top +=	btn;
		rect.bottom += btn;
		MoveWindow(rect, false);
	}

	ShowWindow(SW_SHOW);
}

QPropSheet::~QPropSheet() {
}


BEGIN_MESSAGE_MAP(QPropSheet, CPropertySheet)
	//{{AFX_MSG_MAP(QPropSheet)
	ON_WM_NCDESTROY()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

////////////////////////////////////////
// QPropSheet message handlers

void QPropSheet::RegisterDocument(QooleDoc *pOldDoc, QooleDoc *pNewDoc) {
	Game *pOldGame = (pOldDoc ? pOldDoc->GetGame() : NULL);
	Game *pNewGame = (pNewDoc ? pNewDoc->GetGame() : NULL);

	if (pOldGame != NULL && pNewGame != NULL &&
		strcmp(pOldGame->GetName(), pNewGame->GetName()) != 0) {
		DWORD pageFlag = pOldGame->GetExtraPropPages();
		if ((pageFlag & PROPPAGE_ADDSURFACE) != 0)
			RemovePage(&m_SurfacePage);
		if ((pageFlag & PROPPAGE_ADDCONTENT) != 0)
			RemovePage(&m_ContentPage);

		pageFlag = pNewGame->GetExtraPropPages();
		if ((pageFlag & PROPPAGE_ADDSURFACE) != 0)
			AddPage(&m_SurfacePage);
		if ((pageFlag & PROPPAGE_ADDCONTENT) != 0)
			AddPage(&m_ContentPage);
	}

	int i;
	for(i = 0; i < GetPageCount(); i++) {
		QPropPage *pPage = (QPropPage *) GetPage(i);
		pPage->RegisterDocument(pNewDoc);
	}
}

void QPropSheet::OnNcDestroy() {
	CRect rect;
	GetWindowRect(&rect);

	// Register destruction of window with main frame.
	QMainFrame *pFrame = (QMainFrame *) GetParent();
	pFrame->OnDestroyPropWnd(rect);

	CPropertySheet::OnNcDestroy();

	delete this;
}

void QPropSheet::OnKillFocus(CWnd* pNewWnd) {
	CPropertySheet::OnKillFocus(pNewWnd);
}
