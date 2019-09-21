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

// QPropPg.cpp : implementation file
//

#include "stdafx.h"
#include "Qoole.h"
#include "QMainFrm.h"
#include "QPropPg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//======================================================================
// QPropPage property page
//======================================================================

IMPLEMENT_DYNCREATE(QPropPage, CPropertyPage)

QPropPage::QPropPage() {
}

QPropPage::QPropPage(UINT nIDTemplate)
		  : CPropertyPage(nIDTemplate) {
	//{{AFX_DATA_INIT(QPropPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	QooleDoc *pDoc = pQMainFrame->GetDeskTopDocument();
	RegisterDocument(pDoc);
	active = false;
}

QPropPage::~QPropPage() {
}

void QPropPage::DoDataExchange(CDataExchange* pDX) {
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(QPropPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(QPropPage, CPropertyPage)
	//{{AFX_MSG_MAP(QPropPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// QPropPage message handlers

BOOL QPropPage::OnSetActive() {
	if (!CPropertyPage::OnSetActive())
		return FALSE;

	active = true;
	this->OnUpdate(NULL, NULL);
	return TRUE;
}

BOOL QPropPage::OnKillActive() {
	if (!CPropertyPage::OnKillActive())
		return FALSE;

	active = false;
	return TRUE;
}
