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

// qpropmsc.cpp : implementation file
//

#include "stdafx.h"
#include "Qoole.h"
#include "QMainFrm.h"
#include "QPropMsc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//======================================================================
// QPropPageFile property page
//======================================================================

IMPLEMENT_DYNCREATE(QPropPageFile, QPropPage)

QPropPageFile::QPropPageFile() : QPropPage(QPropPageFile::IDD) {
	//{{AFX_DATA_INIT(QPropPageFile)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

QPropPageFile::~QPropPageFile() {
}

void QPropPageFile::DoDataExchange(CDataExchange* pDX) {
	QPropPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(QPropPageFile)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(QPropPageFile, QPropPage)
	//{{AFX_MSG_MAP(QPropPageFile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// QPropPageFile message handlers

BOOL QPropPageFile::OnInitDialog() {
	QPropPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void QPropPageFile::OnUpdate(LPARAM lHint, Object *pScope) {
	if (lHint & DUAV_NOQPROPPAGES || !active)
		return;

	QooleDoc *pDoc = pQMainFrame->GetDeskTopDocument();
	Game *pGame = pDoc->GetGame();

	// Filename.
	SetDlgItemText(IDC_PPF_FILEFILENAME, pDoc->GetPathName());

	// Last modified.
	char shortPath[256], textBuf[256];
	::GetShortPathName(pDoc->GetPathName(), shortPath, 256);
	strcpy(textBuf, LFile::GetModifyTime(shortPath));
	textBuf[strlen(textBuf) - 1] = '\0';
	SetDlgItemText(IDC_PPF_LASTMODIFIED, textBuf);

	// Game.
	SetDlgItemText(IDC_PPF_GAMETYPE, pGame->GetName());
}

//======================================================================
// QPropPageObject property page
//======================================================================

IMPLEMENT_DYNCREATE(QPropPageObject, QPropPage)

QPropPageObject::QPropPageObject() : QPropPage(QPropPageObject::IDD) {
	//{{AFX_DATA_INIT(QPropPageObject)
	//}}AFX_DATA_INIT
}

QPropPageObject::~QPropPageObject() {
}

void QPropPageObject::DoDataExchange(CDataExchange* pDX) {
	QPropPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(QPropPageObject)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(QPropPageObject, QPropPage)
	//{{AFX_MSG_MAP(QPropPageObject)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

////////////////////////////////////////
// QPropPageObject message handlers

BOOL QPropPageObject::OnInitDialog() {
	QPropPage::OnInitDialog();
	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void QPropPageObject::OnUpdate(LPARAM lHint, Object *pScope) {
	if (lHint & DUAV_NOQPROPPAGES || !active)
		return;

	Selector *pSlctr = &(pQMainFrame->GetSelector());
	Object *pEditScope = pSlctr->GetScopePtr();
	if (pScope != NULL && pEditScope != pScope &&
		!pScope->IsMyAncestor(*pEditScope)) {
		return;
	}

	char textBuf[64];

	if (pScope == NULL || (lHint & (DUAV_OBJSSEL | DUAV_SCOPECHANGED))) {
		int num = pSlctr->GetNumMSelectedObjects();
		sprintf(textBuf, "%d", num);
		ctlObjCount().SetWindowText(textBuf);
	}

	if (pScope == NULL || (lHint & (DUAV_OBJSMODSTRUCT | DUAV_SCOPECHANGED))) {
		IterLinkList<ObjectPtr> iter(pSlctr->GetMSelectedObjects());
		iter.Reset();
		int brushCount, entCount;
		brushCount = entCount = 0;
		Object *pSelObj;
		while (!iter.IsDone()) {
			pSelObj = iter.GetNext()->GetPtr();
			brushCount += pSelObj->CountBrushes();
			entCount += pSelObj->CountEntities();
		}
		sprintf(textBuf, "%d", entCount);
		ctlEntCount().SetWindowText(textBuf);
		sprintf(textBuf, "%d", brushCount);
		ctlBrushCount().SetWindowText(textBuf);
	}

	if (pScope == NULL ||
		(lHint & (DUAV_OBJSMODATTRIB | DUAV_SCOPECHANGED | DUAV_OBJSSEL))) {
		Vector3d pVec;
		pSlctr->GetMSelectedObjectsCenter(pVec);
		sprintf(textBuf, "(%.0f, %.0f, %.0f)", pVec.GetX(), pVec.GetY(), pVec.GetZ());
		ctlObjPos().SetWindowText(textBuf);

		IterLinkList<ObjectPtr> iter(pSlctr->GetMSelectedObjects());
		iter.Reset();
		Object *pSelObj = NULL;
		Vector3d minVec, maxVec, tMinVec, tMaxVec;

		if (!iter.IsDone()) {
			pSelObj = iter.GetNext()->GetPtr();
			pVec = pSelObj->GetPosition();
			pSelObj->GetBoundingVectors(tMinVec, tMaxVec);
			tMinVec.AddVector(pVec);
			tMaxVec.AddVector(pVec);
		}

		while (!iter.IsDone()) {
			pSelObj = iter.GetNext()->GetPtr();
			pVec = pSelObj->GetPosition();
			pSelObj->GetBoundingVectors(minVec, maxVec);
			minVec.AddVector(pVec);
			maxVec.AddVector(pVec);
			tMinVec.NewVector(Min(minVec.GetX(), tMinVec.GetX()),
							  Min(minVec.GetY(), tMinVec.GetY()),
							  Min(minVec.GetZ(), tMinVec.GetZ()));
			tMaxVec.NewVector(Max(maxVec.GetX(), tMaxVec.GetX()),
							  Max(maxVec.GetY(), tMaxVec.GetY()),
							  Max(maxVec.GetZ(), tMaxVec.GetZ()));
		}
		tMaxVec.SubVector(tMinVec);
		sprintf(textBuf, "(%.0f, %.0f, %.0f)\n",
				tMaxVec.GetX(), tMaxVec.GetY(), tMaxVec.GetZ());
		ctlObjDimen().SetWindowText(textBuf);
	}

	UpdateData(FALSE);
}
