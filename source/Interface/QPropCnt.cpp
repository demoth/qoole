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

// QPropCnt.cpp : implementation file
//

#include "stdafx.h"
#include "Qoole.h"
#include "QMainFrm.h"
#include "QPropCnt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//======================================================================
// QPropPageContent property page
//======================================================================

IMPLEMENT_DYNCREATE(QPropPageContent, QPropPage)

QPropPageContent::QPropPageContent()
				 : QPropPage(QPropPageContent::IDD) {
	//{{AFX_DATA_INIT(QPropPageContent)
	m_bAux = FALSE;
	m_bCorpse = FALSE;
	m_bCurrent0 = FALSE;
	m_bCurrent180 = FALSE;
	m_bCurrent270 = FALSE;
	m_bCurrent90 = FALSE;
	m_bCurrentDown = FALSE;
	m_bCurrentUp = FALSE;
	m_bDetail = FALSE;
	m_bLadder = FALSE;
	m_bLava = FALSE;
	m_bMist = FALSE;
	m_bOrigin = FALSE;
	m_bPlayerClip = FALSE;
	m_bSlime = FALSE;
	m_bSolid = FALSE;
	m_bTranslucent = FALSE;
	m_bWater = FALSE;
	m_bWindow = FALSE;
	m_bMonsterClip = FALSE;
	m_bMonster = FALSE;
	//}}AFX_DATA_INIT

	ctrlsEnabled = false;
}

QPropPageContent::~QPropPageContent() {
}

void QPropPageContent::DoDataExchange(CDataExchange* pDX) {
	QPropPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(QPropPageContent)
	DDX_Check(pDX, IDC_PPC_AUX, m_bAux);
	DDX_Check(pDX, IDC_PPC_CORPSE, m_bCorpse);
	DDX_Check(pDX, IDC_PPC_CURRENT0, m_bCurrent0);
	DDX_Check(pDX, IDC_PPC_CURRENT180, m_bCurrent180);
	DDX_Check(pDX, IDC_PPC_CURRENT270, m_bCurrent270);
	DDX_Check(pDX, IDC_PPC_CURRENT90, m_bCurrent90);
	DDX_Check(pDX, IDC_PPC_CURRENTDOWN, m_bCurrentDown);
	DDX_Check(pDX, IDC_PPC_CURRENTUP, m_bCurrentUp);
	DDX_Check(pDX, IDC_PPC_DETAIL, m_bDetail);
	DDX_Check(pDX, IDC_PPC_LADDER, m_bLadder);
	DDX_Check(pDX, IDC_PPC_LAVA, m_bLava);
	DDX_Check(pDX, IDC_PPC_MIST, m_bMist);
	DDX_Check(pDX, IDC_PPC_ORIGIN, m_bOrigin);
	DDX_Check(pDX, IDC_PPC_PLAYERCLIP, m_bPlayerClip);
	DDX_Check(pDX, IDC_PPC_SLIME, m_bSlime);
	DDX_Check(pDX, IDC_PPC_SOLID, m_bSolid);
	DDX_Check(pDX, IDC_PPC_TRANSLUCENT, m_bTranslucent);
	DDX_Check(pDX, IDC_PPC_WATER, m_bWater);
	DDX_Check(pDX, IDC_PPC_WINDOW, m_bWindow);
	DDX_Check(pDX, IDC_PPC_MONSTERCLIP, m_bMonsterClip);
	DDX_Check(pDX, IDC_PPC_MONSTER, m_bMonster);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(QPropPageContent, QPropPage)
	//{{AFX_MSG_MAP(QPropPageContent)
	ON_BN_CLICKED(IDC_PPC_AUX, OnPpcAux)
	ON_BN_CLICKED(IDC_PPC_CORPSE, OnPpcCorpse)
	ON_BN_CLICKED(IDC_PPC_CURRENT0, OnPpcCurrent0)
	ON_BN_CLICKED(IDC_PPC_CURRENT180, OnPpcCurrent180)
	ON_BN_CLICKED(IDC_PPC_CURRENT270, OnPpcCurrent270)
	ON_BN_CLICKED(IDC_PPC_CURRENT90, OnPpcCurrent90)
	ON_BN_CLICKED(IDC_PPC_CURRENTDOWN, OnPpcCurrentdown)
	ON_BN_CLICKED(IDC_PPC_CURRENTUP, OnPpcCurrentup)
	ON_BN_CLICKED(IDC_PPC_DETAIL, OnPpcDetail)
	ON_BN_CLICKED(IDC_PPC_LADDER, OnPpcLadder)
	ON_BN_CLICKED(IDC_PPC_LAVA, OnPpcLava)
	ON_BN_CLICKED(IDC_PPC_MIST, OnPpcMist)
	ON_BN_CLICKED(IDC_PPC_MONSTER, OnPpcMonster)
	ON_BN_CLICKED(IDC_PPC_MONSTERCLIP, OnPpcMonsterclip)
	ON_BN_CLICKED(IDC_PPC_ORIGIN, OnPpcOrigin)
	ON_BN_CLICKED(IDC_PPC_PLAYERCLIP, OnPpcPlayerclip)
	ON_BN_CLICKED(IDC_PPC_SLIME, OnPpcSlime)
	ON_BN_CLICKED(IDC_PPC_SOLID, OnPpcSolid)
	ON_BN_CLICKED(IDC_PPC_TRANSLUCENT, OnPpcTranslucent)
	ON_BN_CLICKED(IDC_PPC_WATER, OnPpcWater)
	ON_BN_CLICKED(IDC_PPC_WINDOW, OnPpcWindow)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////
// QPropPageContent message handlers

void QPropPageContent::OnUpdate(LPARAM lHint, Object *pScope) {
	if (lHint & DUAV_NOQPROPPAGES || !active)
		return;

	Selector *pSlctr = &(pQMainFrame->GetSelector());
	Object *pEditScope = pSlctr->GetScopePtr();
	if (pScope != NULL && pEditScope != pScope &&
		!pScope->IsMyAncestor(*pEditScope)) {
  		return;
	}

	UINT filter = DUAV_OBJSSEL | DUAV_OBJSMODSTRUCT | DUAV_OBJTEXMODATTRIB;
	if (pScope != NULL && !(lHint & filter))
		return;

	Object *pSelObj = NULL;
	if (pSlctr->GetNumMSelectedObjects() == 1)
		pSelObj = (pSlctr->GetMSelectedObjects())[0].GetPtr();

	if (pSelObj == NULL || !pSelObj->HasBrush()) {
		EnableCtrls(false);
		return;
	}

	EnableCtrls(true);
	OnSlctNewBrush();
	return;
}

void QPropPageContent::EnableCtrls(bool bEnable) {
	if (ctrlsEnabled == bEnable)
		return;

	ctrlsEnabled = bEnable;

	// Clear checks.
	ctrlSolid().SetCheck(0);
	ctrlWindow().SetCheck(0);
	ctrlAux().SetCheck(0);
	ctrlLava().SetCheck(0);
	ctrlSlime().SetCheck(0);
	ctrlWater().SetCheck(0);
	ctrlMist().SetCheck(0);
	ctrlCurrent0().SetCheck(0);
	ctrlCurrent90().SetCheck(0);
	ctrlCurrent180().SetCheck(0);
	ctrlCurrent270().SetCheck(0);
	ctrlCurrentUp().SetCheck(0);
	ctrlCurrentDown().SetCheck(0);
	ctrlPlayerClip().SetCheck(0);
	ctrlMonsterClip().SetCheck(0);
	ctrlOrigin().SetCheck(0);
	ctrlMonster().SetCheck(0);
	ctrlCorpse().SetCheck(0);
	ctrlDetail().SetCheck(0);
	ctrlTranslucent().SetCheck(0);
	ctrlLadder().SetCheck(0);

	// Enable / Disable ctrls
	ctrlSolid().EnableWindow(bEnable);
	ctrlWindow().EnableWindow(bEnable);
	ctrlAux().EnableWindow(bEnable);
	ctrlLava().EnableWindow(bEnable);
	ctrlSlime().EnableWindow(bEnable);
	ctrlWater().EnableWindow(bEnable);
	ctrlMist().EnableWindow(bEnable);
	ctrlCurrent0().EnableWindow(bEnable);
	ctrlCurrent90().EnableWindow(bEnable);
	ctrlCurrent180().EnableWindow(bEnable);
	ctrlCurrent270().EnableWindow(bEnable);
	ctrlCurrentUp().EnableWindow(bEnable);
	ctrlCurrentDown().EnableWindow(bEnable);
	ctrlPlayerClip().EnableWindow(bEnable);
	ctrlMonsterClip().EnableWindow(bEnable);
	ctrlOrigin().EnableWindow(bEnable);
	ctrlMonster().EnableWindow(bEnable);
	ctrlCorpse().EnableWindow(bEnable);
	ctrlDetail().EnableWindow(bEnable);
	ctrlTranslucent().EnableWindow(bEnable);
	ctrlLadder().EnableWindow(bEnable);
}

void QPropPageContent::OnSlctNewBrush() {
	// Update the check boxes.
	Selector *pSlctr = &(pQMainFrame->GetSelector());
	ASSERT(pSlctr->GetNumMSelectedObjects() == 1);
	ASSERT((pSlctr->GetMSelectedObjects())[0].GetPtr()->HasBrush());

	Geometry *pBrush = &((pSlctr->GetMSelectedObjects())[0].GetPtr()->GetBrush());
	FaceTex *pFaceTex = pBrush->GetFaceTexturePtr(0);
	UINT content, a2, a3;
	pFaceTex->GetTAttribs(content, a2, a3);

	// Upload to the ctrls.
	UpdateData(FALSE, content);
}

BOOL QPropPageContent::UpdateData(BOOL bSaveAndValidate, UINT &content) {
	if (!bSaveAndValidate) {
		// Upload to ctrls.
		m_bSolid		= ((content & 0x00000001) != 0);
		m_bWindow		= ((content & 0x00000002) != 0);
		m_bAux			= ((content & 0x00000004) != 0);
		m_bLava			= ((content & 0x00000008) != 0);	
		m_bSlime		= ((content & 0x00000010) != 0);
		m_bWater		= ((content & 0x00000020) != 0);
		m_bMist			= ((content & 0x00000040) != 0);

		m_bPlayerClip	= ((content & 0x00010000) != 0);
		m_bMonsterClip	= ((content & 0x00020000) != 0);
		m_bCurrent0		= ((content & 0x00040000) != 0);
		m_bCurrent90	= ((content & 0x00080000) != 0);
		m_bCurrent180	= ((content & 0x00100000) != 0);
		m_bCurrent270	= ((content & 0x00200000) != 0);
		m_bCurrentUp	= ((content & 0x00400000) != 0);
		m_bCurrentDown	= ((content & 0x00800000) != 0);
		m_bOrigin		= ((content & 0x01000000) != 0);
		m_bMonster		= ((content & 0x02000000) != 0);
		m_bCorpse		= ((content & 0x04000000) != 0);
		m_bDetail		= ((content & 0x08000000) != 0);
		m_bTranslucent	= ((content & 0x10000000) != 0);
		m_bLadder		= ((content & 0x20000000) != 0);

		if (!CWnd::UpdateData(bSaveAndValidate))
			return FALSE;
	}
	else {
		// Download from ctrls.
		if (!CWnd::UpdateData(bSaveAndValidate))
			return FALSE;

		content = 0x00000000;
		content |= (m_bSolid		? 0x00000001 : 0x0);
		content |= (m_bWindow		? 0x00000002 : 0x0);
		content |= (m_bAux			? 0x00000004 : 0x0);
		content |= (m_bLava			? 0x00000008 : 0x0);
		content |= (m_bSlime		? 0x00000010 : 0x0);
		content |= (m_bWater		? 0x00000020 : 0x0);
		content |= (m_bMist			? 0x00000040 : 0x0);

		content |= (m_bPlayerClip	? 0x00010000 : 0x0);
		content |= (m_bMonsterClip	? 0x00020000 : 0x0);
		content |= (m_bCurrent0		? 0x00040000 : 0x0);
		content |= (m_bCurrent90	? 0x00080000 : 0x0);
		content |= (m_bCurrent180	? 0x00100000 : 0x0);
		content |= (m_bCurrent270	? 0x00200000 : 0x0);
		content |= (m_bCurrentUp	? 0x00400000 : 0x0);
		content |= (m_bCurrentDown	? 0x00800000 : 0x0);
		content |= (m_bOrigin		? 0x01000000 : 0x0);
		content |= (m_bMonster		? 0x02000000 : 0x0);
		content |= (m_bCorpse		? 0x04000000 : 0x0);
		content |= (m_bDetail		? 0x08000000 : 0x0);
		content |= (m_bTranslucent	? 0x10000000 : 0x0);
		content |= (m_bLadder		? 0x20000000 : 0x0);
	}

	return TRUE;
}

void QPropPageContent::RegAttribChanged() {
	// Download from ctrls.
	UINT content;
	UpdateData(TRUE, content);

	Selector *pSlctr = &(pQMainFrame->GetSelector());
	ASSERT(pSlctr->GetNumMSelectedObjects() == 1);
	ASSERT((pSlctr->GetMSelectedObjects())[0].GetPtr()->HasBrush());

	OpModContentAttrib *op = new OpModContentAttrib(content);
	pQMainFrame->CommitOperation(*op);
}

//===== check box handlers =====
void QPropPageContent::OnPpcSolid() {
	RegAttribChanged();
}

void QPropPageContent::OnPpcWindow() {
	RegAttribChanged();
}

void QPropPageContent::OnPpcAux() {
	RegAttribChanged();
}

void QPropPageContent::OnPpcLava() {
	RegAttribChanged();
}

void QPropPageContent::OnPpcSlime() {
	RegAttribChanged();
}

void QPropPageContent::OnPpcWater() {
	RegAttribChanged();
}

void QPropPageContent::OnPpcMist() {
	RegAttribChanged();
}

void QPropPageContent::OnPpcPlayerclip() {
	RegAttribChanged();
}

void QPropPageContent::OnPpcMonsterclip() {
	RegAttribChanged();
}

void QPropPageContent::OnPpcCurrent0() {
	RegAttribChanged();
}

void QPropPageContent::OnPpcCurrent90() {
	RegAttribChanged();
}

void QPropPageContent::OnPpcCurrent180() {
	RegAttribChanged();
}

void QPropPageContent::OnPpcCurrent270() {
	RegAttribChanged();
}

void QPropPageContent::OnPpcCurrentup() {
	RegAttribChanged();
}

void QPropPageContent::OnPpcCurrentdown() {
	RegAttribChanged();
}

void QPropPageContent::OnPpcOrigin() {
	RegAttribChanged();
}

void QPropPageContent::OnPpcMonster() {
	RegAttribChanged();
}

void QPropPageContent::OnPpcCorpse() {
	RegAttribChanged();
}

void QPropPageContent::OnPpcDetail() {
	RegAttribChanged();
}

void QPropPageContent::OnPpcTranslucent() {
	RegAttribChanged();
}

void QPropPageContent::OnPpcLadder() {
	RegAttribChanged();
}
