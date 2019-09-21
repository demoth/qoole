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

// QPropSrf.cpp : implementation file
//

#include "stdafx.h"
#include "Qoole.h"
#include "QPropSrf.h"
#include "QMainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//======================================================================
// QPropPageSurface property page
//======================================================================

IMPLEMENT_DYNCREATE(QPropPageSurface, QPropPageFaceSelect)

QPropPageSurface::QPropPageSurface() : QPropPageFaceSelect(QPropPageSurface::IDD) {
	//{{AFX_DATA_INIT(QPropPageSurface)
	m_bFlowing = FALSE;
	m_bHint = FALSE;
	m_bLight = FALSE;
	m_bNoDraw = FALSE;
	m_bSkip = FALSE;
	m_bSky = FALSE;
	m_bSlick = FALSE;
	m_bTrans33 = FALSE;
	m_bTrans66 = FALSE;
	m_iValue = 0;
	m_bWarp = FALSE;
	//}}AFX_DATA_INIT
	ctrlsEnabled = true;
}

QPropPageSurface::~QPropPageSurface() {
}

void QPropPageSurface::DoDataExchange(CDataExchange* pDX) {
	QPropPageFaceSelect::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(QPropPageSurface)
	DDX_Check(pDX, IDC_PPS_FLOWING, m_bFlowing);
	DDX_Check(pDX, IDC_PPS_HINT, m_bHint);
	DDX_Check(pDX, IDC_PPS_LIGHT, m_bLight);
	DDX_Check(pDX, IDC_PPS_NODRAW, m_bNoDraw);
	DDX_Check(pDX, IDC_PPS_SKIP, m_bSkip);
	DDX_Check(pDX, IDC_PPS_SKY, m_bSky);
	DDX_Check(pDX, IDC_PPS_SLICK, m_bSlick);
	DDX_Check(pDX, IDC_PPS_TRANS33, m_bTrans33);
	DDX_Check(pDX, IDC_PPS_TRANS66, m_bTrans66);
	DDX_Check(pDX, IDC_PPS_WARP, m_bWarp);
	//}}AFX_DATA_MAP

	if (validValue)
		DDX_Text(pDX, IDC_PPS_VALUE, m_iValue);
}


BEGIN_MESSAGE_MAP(QPropPageSurface, QPropPageFaceSelect)
	//{{AFX_MSG_MAP(QPropPageSurface)
	ON_BN_CLICKED(IDC_PPS_HINT, OnPPSHint)
	ON_BN_CLICKED(IDC_PPS_LIGHT, OnPPSLight)
	ON_BN_CLICKED(IDC_PPS_NODRAW, OnPPSNoDraw)
	ON_BN_CLICKED(IDC_PPS_SKIP, OnPPSSkip)
	ON_BN_CLICKED(IDC_PPS_SKY, OnPPSSky)
	ON_BN_CLICKED(IDC_PPS_SLICK, OnPPSSlick)
	ON_BN_CLICKED(IDC_PPS_TRANS33, OnPPSTrans33)
	ON_BN_CLICKED(IDC_PPS_TRANS66, OnPPSTrans66)
	ON_BN_CLICKED(IDC_PPS_WARP, OnPPSWarp)
	ON_BN_CLICKED(IDC_PPS_FLOWING, OnPPSFlowing)
	ON_EN_KILLFOCUS(IDC_PPS_VALUE, OnKillFocusPPSValue)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//========================================
// QPropPageSurface message handlers

void QPropPageSurface::OnUpdate(LPARAM lHint, Object *pScope) {
	QPropPageFaceSelect::OnUpdate(lHint, pScope);

	if (lHint & DUAV_NOQPROPPAGES || !active)
		return;

	Selector *pSlctr = &(pQMainFrame->GetSelector());

	if (lHint & DUAV_OBJSSEL || pScope == NULL) {
		// Check for enable state change.
		if (pScope == NULL || ctrlSelectAll().IsWindowEnabled() != ctrlsEnabled) {
			ctrlsEnabled = ctrlSelectAll().IsWindowEnabled();
			ctrlLight().EnableWindow(ctrlsEnabled);
			ctrlSlick().EnableWindow(ctrlsEnabled);
			ctrlSky().EnableWindow(ctrlsEnabled);
			ctrlWarp().EnableWindow(ctrlsEnabled);
			ctrlTrans33().EnableWindow(ctrlsEnabled);
			ctrlTrans66().EnableWindow(ctrlsEnabled);
			ctrlFlowing().EnableWindow(ctrlsEnabled);
			ctrlNoDraw().EnableWindow(ctrlsEnabled);
			ctrlHint().EnableWindow(ctrlsEnabled);
			ctrlSkip().EnableWindow(ctrlsEnabled);
			ctrlValue().EnableWindow(ctrlsEnabled);
		}
	}

	UINT filter = DUAV_OBJSSEL | DUAV_OBJSMODSTRUCT | DUAV_OBJTEXMODATTRIB;
	if (lHint & filter || pScope == NULL) {
		if (!ctrlsEnabled) { // Reset the ctrls.
			ctrlLight().SetCheck(0);
			ctrlSlick().SetCheck(0);
			ctrlSky().SetCheck(0);
			ctrlWarp().SetCheck(0);
			ctrlTrans33().SetCheck(0);
			ctrlTrans66().SetCheck(0);
			ctrlFlowing().SetCheck(0);
			ctrlNoDraw().SetCheck(0);
			ctrlHint().SetCheck(0);
			ctrlSkip().SetCheck(0);
		}
		else if (pSlctr->GetSelectFaceIndex() == -1) {
			// Reset the ctrls' infomation.
			OnSlctNewFace();
		}
	}
}

void QPropPageSurface::OnPPFSSlctAll() {
	QPropPageFaceSelect::OnPPFSSlctAll();

	// Update the ctrls' information.
	OnSlctNewFace();
}

BOOL QPropPageSurface::UpdateData(BOOL bSaveAndValidate,
								  UINT &attrib, UINT &value) {
	if (!bSaveAndValidate) {
		// Update to the controls.
		m_bLight   = ((attrib & 0x0000001) != 0);
		m_bSlick   = ((attrib & 0x0000002) != 0);
		m_bSky     = ((attrib & 0x0000004) != 0);
		m_bWarp    = ((attrib & 0x0000008) != 0);
		m_bTrans33 = ((attrib & 0x0000010) != 0);
		m_bTrans66 = ((attrib & 0x0000020) != 0);
		m_bFlowing = ((attrib & 0x0000040) != 0);
		m_bNoDraw  = ((attrib & 0x0000080) != 0);
		m_bHint    = ((attrib & 0x0000100) != 0);
		m_bSkip    = ((attrib & 0x0000200) != 0);
		m_iValue   = value;

		if (!CWnd::UpdateData(bSaveAndValidate))
			return FALSE;
	}
	else {
		// Download from the controls.
		if (!CWnd::UpdateData(bSaveAndValidate))
			return FALSE;

		attrib = 0x00000000;
		attrib |= (m_bLight   ? 0x0000001 : 0x0);
		attrib |= (m_bSlick   ? 0x0000002 : 0x0);
		attrib |= (m_bSky     ? 0x0000004 : 0x0);
		attrib |= (m_bWarp    ? 0x0000008 : 0x0);
		attrib |= (m_bTrans33 ? 0x0000010 : 0x0);
		attrib |= (m_bTrans66 ? 0x0000020 : 0x0);
		attrib |= (m_bFlowing ? 0x0000040 : 0x0);
		attrib |= (m_bNoDraw  ? 0x0000080 : 0x0);
		attrib |= (m_bHint    ? 0x0000100 : 0x0);
		attrib |= (m_bSkip    ? 0x0000200 : 0x0);
		value = m_iValue;
	}

	return TRUE;
}

void QPropPageSurface::OnSlctNewFace() {
	// Update the check boxes.
	Selector *pSlctr = &(pQMainFrame->GetSelector());
	ASSERT(pSlctr->GetNumMSelectedObjects() == 1);
	ASSERT((pSlctr->GetMSelectedObjects())[0].GetPtr()->HasBrush());

	Geometry *pBrush = &((pSlctr->GetMSelectedObjects())[0].GetPtr()->GetBrush());
	FaceTex *pFaceTex;
	UINT a1, attrib, value, unAttrib = 0xFFFFFFFF;
	int faceIndex = pSlctr->GetSelectFaceIndex();

	validValue = true;
	if (faceIndex == -1) { // Apply to all
		UINT b1, b2, b3;
		attrib = 0x0;

		for(int i = 0; i < pBrush->GetNumFaces(); i++) {
			pFaceTex = pBrush->GetFaceTexturePtr(i);
			pFaceTex->GetTAttribs(b1, b2, b3);
			attrib |= b2;
			unAttrib &= b2;
			if (i > 0 && b3 != value)
				validValue = false;
			else
				value = b3;
		}
	}
	else {
		pFaceTex = pBrush->GetFaceTexturePtr(faceIndex);
		pFaceTex->GetTAttribs(a1, attrib, value);
	}

	if(faceIndex == -1)
		pFaceTex = NULL;

	m_texWnd.SetFaceTex(pFaceTex);

	UpdateData(FALSE, attrib, value);

	// Mark the semi-checked attribs when the whole brush's selected.
	if (faceIndex == -1) {
		unAttrib = ~unAttrib;
		if (unAttrib & 0x00000001 && attrib & 0x00000001)	ctrlLight().SetCheck(2);
		if (unAttrib & 0x00000002 && attrib & 0x00000002)	ctrlSlick().SetCheck(2);
		if (unAttrib & 0x00000004 && attrib & 0x00000004)	ctrlSky().SetCheck(2);
		if (unAttrib & 0x00000008 && attrib & 0x00000008)	ctrlWarp().SetCheck(2);
		if (unAttrib & 0x00000010 && attrib & 0x00000010)	ctrlTrans33().SetCheck(2);
		if (unAttrib & 0x00000020 && attrib & 0x00000020)	ctrlTrans66().SetCheck(2);
		if (unAttrib & 0x00000040 && attrib & 0x00000040)	ctrlFlowing().SetCheck(2);
		if (unAttrib & 0x00000080 && attrib & 0x00000080)	ctrlNoDraw().SetCheck(2);
		if (unAttrib & 0x00000100 && attrib & 0x00000100)	ctrlHint().SetCheck(2);
		if (unAttrib & 0x00000200 && attrib & 0x00000200)	ctrlSkip().SetCheck(2);

		// If multi values, then clear field.
		if (!validValue)
			ctrlValue().SetWindowText("");
	}
}

void QPropPageSurface::RegAttribChange(UINT mask) {
	// Download from ctrls.
	UINT newAttrib, newValue;
	UpdateData(TRUE, newAttrib, newValue);

	Selector *pSlctr = &(pQMainFrame->GetSelector());
	ASSERT(pSlctr->GetNumMSelectedObjects() == 1);
	ASSERT((pSlctr->GetMSelectedObjects())[0].GetPtr()->HasBrush());
	int fIndex = pSlctr->GetSelectFaceIndex();

	OpModSurfaceAttrib *op;
	if (mask != 0x0)
		op = new OpModSurfaceAttrib(fIndex, newAttrib, mask);
	else
		op = new OpModSurfaceAttrib(fIndex, newValue);

	pQMainFrame->CommitOperation(*op);
}

//===== Check button handlers =====
void QPropPageSurface::OnPPSLight() {
	int state = ctrlLight().GetCheck();
	if (state == 2)
		ctrlLight().SetCheck(0);
	RegAttribChange(0x00000001);
}

void QPropPageSurface::OnPPSSlick() {
	int state = ctrlSlick().GetCheck();
	if (state == 2)
		ctrlSlick().SetCheck(0);
	RegAttribChange(0x00000002);
}

void QPropPageSurface::OnPPSSky() {
	int state = ctrlSky().GetCheck();
	if (state == 2)
		ctrlSky().SetCheck(0);
	RegAttribChange(0x00000004);
}

void QPropPageSurface::OnPPSWarp() {
	int state = ctrlWarp().GetCheck();
	if (state == 2)
		ctrlWarp().SetCheck(0);
	RegAttribChange(0x00000008);
}

void QPropPageSurface::OnPPSTrans33() {
	int state = ctrlTrans33().GetCheck();
	if (state == 2)
		ctrlTrans33().SetCheck(0);
	RegAttribChange(0x00000010);
}

void QPropPageSurface::OnPPSTrans66() {
	int state = ctrlTrans66().GetCheck();
	if (state == 2)
		ctrlTrans66().SetCheck(0);
	RegAttribChange(0x00000020);
}

void QPropPageSurface::OnPPSFlowing() {
	int state = ctrlFlowing().GetCheck();
	if (state == 2)
		ctrlFlowing().SetCheck(0);
	RegAttribChange(0x00000040);
}

void QPropPageSurface::OnPPSNoDraw() {
	int state = ctrlNoDraw().GetCheck();
	if (state == 2)
		ctrlNoDraw().SetCheck(0);
	RegAttribChange(0x00000080);
}

void QPropPageSurface::OnPPSHint() {
	int state = ctrlHint().GetCheck();
	if (state == 2)
		ctrlHint().SetCheck(0);
	RegAttribChange(0x00000100);
}

void QPropPageSurface::OnPPSSkip() {
	int state = ctrlSkip().GetCheck();
	if (state == 2)
		ctrlSkip().SetCheck(0);
	RegAttribChange(0x00000200);
}

void QPropPageSurface::OnKillFocusPPSValue() {
	char strBuf[10];
	int value;
	ctrlValue().GetWindowText(strBuf, 10);
	if (sscanf(strBuf, "%d", &value) == 1)
		validValue = true;
	else {
		validValue = false;
		ctrlValue().SetWindowText("");
	}

	if (validValue && (UINT) value != m_iValue)
		RegAttribChange(0x0);
}
