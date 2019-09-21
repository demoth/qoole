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

// qproptex.cpp : implementation file
//

#include "stdafx.h"
#include "Qoole.h"
#include "QooleOp.h"
#include "QMainFrm.h"
#include "QPropTex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//======================================================================
// QPropPageFaceSelect property page
//======================================================================

IMPLEMENT_DYNCREATE(QPropPageFaceSelect, QPropPage)

QPropPageFaceSelect::QPropPageFaceSelect() {
}

QPropPageFaceSelect::QPropPageFaceSelect(int nID) : QPropPage(nID) {
	//{{AFX_DATA_INIT(QPropPageFaceSelect)
	m_bSelectAll = FALSE;
	m_bSelectFace = FALSE;
	//}}AFX_DATA_INIT
}

QPropPageFaceSelect::~QPropPageFaceSelect() {
	// Will this be called?
	// Disable face selection rendering in qviews.
	if (pQDoc != NULL) {
		RenderFaceSlct(false);
		pQDoc->UpdateAllViews(NULL, DUAV_QVIEWS, NULL);
	}
}

void QPropPageFaceSelect::DoDataExchange(CDataExchange* pDX) {
	QPropPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(QPropPageFaceSelect)
	DDX_Control(pDX, IDC_PPFS_TEXTURE, m_texWnd);
	DDX_Check(pDX, IDC_PPFS_SLCTALL, m_bSelectAll);
	DDX_Check(pDX, IDC_PPFS_SLCTFACE, m_bSelectFace);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(QPropPageFaceSelect, QPropPage)
	//{{AFX_MSG_MAP(QPropPageFaceSelect)
	ON_BN_CLICKED(IDC_PPFS_SLCTALL, OnPPFSSlctAll)
	ON_BN_CLICKED(IDC_PPFS_SLCTFACE, OnPPFSSlctFace)
	ON_BN_CLICKED(IDC_PPFS_PREVFACE, OnPPFSPrevFace)
	ON_BN_CLICKED(IDC_PPFS_NEXTFACE, OnPPFSNextFace)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////
// QPropPageFaceSelect message handlers

BOOL QPropPageFaceSelect::OnInitDialog() {
	CDialog::OnInitDialog();
	m_texWnd.OnInit();
	return TRUE;
}

void QPropPageFaceSelect::RenderFaceSlct(bool slct) {
	ASSERT(pQDoc != NULL);

	CView *pView;
	POSITION pos = pQDoc->GetFirstViewPosition();
	while (pos != NULL) {
		pView = pQDoc->GetNextView(pos);
		if (!pView->IsKindOf(RUNTIME_CLASS(QView)))
			continue;
		((QView *) pView)->RenderFaceSlct(slct);
   }   
}

BOOL QPropPageFaceSelect::OnSetActive() {
	if (!QPropPage::OnSetActive())
		return FALSE;

	Selector *pSlctr = &(pQMainFrame->GetSelector());

	// Reenable face selection rendering if a face was already selected.
	if (pSlctr->GetSelectFaceIndex() != -1)
		pQDoc->UpdateAllViews(NULL, DUAV_QVIEWS, NULL);

	return TRUE;
}

BOOL QPropPageFaceSelect::OnKillActive() {
	if (!QPropPage::OnKillActive())
		return FALSE;

	// Disable face selection rendering in qviews.
	if (pQDoc != NULL && m_bSelectFace) {
		RenderFaceSlct(false);
		pQDoc->UpdateAllViews(NULL, DUAV_QVIEWS, NULL);
	}

	return TRUE;
}

void QPropPageFaceSelect::OnUpdate(LPARAM lHint, Object *pScope) {

	if (lHint & DUAV_NOQPROPPAGES || !active)
		return;

	Selector *pSlctr = &(pQMainFrame->GetSelector());
	Object *pEditScope = pSlctr->GetScopePtr();
	if (pScope != NULL && pEditScope != pScope &&
		!pScope->IsMyAncestor(*pEditScope)) {
		return;
	}

	UINT filter = DUAV_OBJSSEL | // DUAV_SCOPECHANGED |
				  DUAV_OBJSMODSTRUCT | DUAV_OBJTEXMODATTRIB;
	if (pScope != NULL && !(lHint & filter)) {
		// Changes dont affect us.
		return;
	}

	// Need to check if we need to disable/enable ctrls.
	if (pSlctr->GetNumMSelectedObjects() == 1 &&
		(pSlctr->GetMSelectedObjects())[0].GetPtr()->HasBrush()) {
		// Enable the face selection ctrls.
		ctrlSelectAll().EnableWindow(true);
		ctrlSelectFace().EnableWindow(true);
	}
	else {
		// Disable the face selection ctrls.
		ctrlSelectAll().EnableWindow(false);
		ctrlSelectFace().EnableWindow(false);
	}

	if (pSlctr->GetSelectFaceIndex() == -1) {
		// Default to slct all when obj selection changed.
		QPropPageFaceSelect::OnPPFSSlctAll();
	}
	else {
		// Only occurs when switching tabs with pre-selected face.
		//  and when undo/redo involves entire brush <-> face slct change.
		ctrlFacePrev().EnableWindow(true);
		ctrlFaceNext().EnableWindow(true);
		m_bSelectFace = TRUE;
		m_bSelectAll = !m_bSelectFace;
		UpdateData(FALSE);
		RenderFaceSlct(true);
		OnSlctNewFace();
	}
}

void QPropPageFaceSelect::OnPPFSSlctAll() {
	ctrlSelectAll().SetCheck(1);
	ctrlSelectFace().SetCheck(0);
	ctrlFacePrev().EnableWindow(false);
	ctrlFaceNext().EnableWindow(false);

	m_texWnd.SetFaceTex(NULL);

	if (m_bSelectAll)
		return;

	m_bSelectAll = TRUE;
	m_bSelectFace = !m_bSelectAll;

	UpdateData(FALSE);

	// Disable face selection rendering in qviews.
	RenderFaceSlct(false);

	Selector *pSlctr = &(pQMainFrame->GetSelector());
	pSlctr->SelectFaceIndex(-1);

	pQDoc->UpdateAllViews(NULL, DUAV_QVIEWS, NULL);
}

void QPropPageFaceSelect::OnPPFSSlctFace() {
	ctrlSelectAll().SetCheck(0);
	ctrlSelectFace().SetCheck(1);
	ctrlFacePrev().EnableWindow(true);
	ctrlFaceNext().EnableWindow(true);

	if (m_bSelectFace)
		return;

	m_bSelectFace = TRUE;
	m_bSelectAll = !m_bSelectFace;

	UpdateData(FALSE);

	// Enable face selection rendering in qviews
	RenderFaceSlct(true);

	// Select the first face.
	Selector *pSlctr = &(pQMainFrame->GetSelector());
	QView *pQView = pQMainFrame->GetStdQView(ID_VIEW_NEW3D);
	Vector3d viewVec;

	if (pQView != NULL) {
		// Use the 3d view for face alignment calc.
		Object *pSelObj = (pSlctr->GetMSelectedObjects())[0].GetPtr();
		Vector3d posVec;
		SphrVector oriVec;
		float zoomVal;

		viewVec = pSelObj->GetPosition();
		pQView->GetViewState(posVec, oriVec, zoomVal);
		viewVec.SubVector(posVec);
	}
	else {
		// Just use the top window.
		viewVec.NewVector(0.0f, 0.0f, -1.0f);
	}

	pSlctr->ResetSelectFace(viewVec);
	this->OnSlctNewFace();

	pQDoc->UpdateAllViews(NULL, DUAV_QVIEWS, NULL);
}

void QPropPageFaceSelect::OnPPFSPrevFace() {
	ASSERT(m_bSelectFace);

	Selector *pSlctr = &(pQMainFrame->GetSelector());
	QView *pQView = pQMainFrame->GetStdQView(ID_VIEW_NEW3D);
	Vector3d viewVec;

	if (pQView != NULL) {
		// Use the 3d view for face alignment calc.
		Object *pSelObj = (pSlctr->GetMSelectedObjects())[0].GetPtr();
		Vector3d posVec;
		SphrVector oriVec;
		float zoomVal;

		viewVec = pSelObj->GetPosition();
		pQView->GetViewState(posVec, oriVec, zoomVal);
		viewVec.SubVector(posVec);
	}
	else {
		// Just use the top window.
		viewVec.NewVector(0.0f, 0.0f, -1.0f);
	}

	pSlctr->BackwardSelectFace(viewVec);
	OnSlctNewFace();

	pQDoc->UpdateAllViews(NULL, DUAV_QVIEWS, NULL);
}

void QPropPageFaceSelect::OnPPFSNextFace() {
	ASSERT(m_bSelectFace);

	Selector *pSlctr = &(pQMainFrame->GetSelector());
	QView *pQView = pQMainFrame->GetStdQView(ID_VIEW_NEW3D);
	Vector3d viewVec;
	
	if (pQView != NULL) {
		// Use the 3d view for face alignment calc.
		Object *pSelObj = (pSlctr->GetMSelectedObjects())[0].GetPtr();
		Vector3d posVec;
		SphrVector oriVec;
		float zoomVal;

		viewVec = pSelObj->GetPosition();
		pQView->GetViewState(posVec, oriVec, zoomVal);
		viewVec.SubVector(posVec);
	}
	else {
		// Just use the top window.
		viewVec.NewVector(0.0f, 0.0f, -1.0f);
	}

	pSlctr->ForwardSelectFace(viewVec);
	this->OnSlctNewFace();

	pQDoc->UpdateAllViews(NULL, DUAV_QVIEWS, NULL);
}

//======================================================================
// QPropPageTexture property page
//======================================================================

IMPLEMENT_DYNCREATE(QPropPageTexture, QPropPageFaceSelect)

QPropPageTexture::QPropPageTexture()
				 :QPropPageFaceSelect(QPropPageTexture::IDD) {
	//{{AFX_DATA_INIT(QPropPageTexture)
	m_iTextureLock = 0;
	m_iXOffset = 0;
	m_iYOffset = 0;
	m_fXScale = 1.0f;
	m_fYScale = 1.0f;
	m_fRotAng = 0.0f;
	//}}AFX_DATA_INIT

	m_hBitMapUp = ::LoadBitmap(AfxGetInstanceHandle(), "TriangleUp");
	m_hBitMapDown = ::LoadBitmap(AfxGetInstanceHandle(), "TriangleDown");
	m_hBitMapLeft = ::LoadBitmap(AfxGetInstanceHandle(), "TriangleLeft");
	m_hBitMapRight = ::LoadBitmap(AfxGetInstanceHandle(), "TriangleRight");

	pInEditCtrl = NULL;
	ctrlsEnabled = true;
}

QPropPageTexture::~QPropPageTexture() {
	::DeleteObject(m_hBitMapUp);
	::DeleteObject(m_hBitMapDown);
	::DeleteObject(m_hBitMapLeft);
	::DeleteObject(m_hBitMapRight);
}

void QPropPageTexture::DoDataExchange(CDataExchange* pDX) {
	QPropPageFaceSelect::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(QPropPageTexture)
	DDX_Check(pDX, IDC_PPT_TEXTLOCK, m_iTextureLock);
	DDX_Text(pDX, IDC_PPT_XOFFSET, m_iXOffset);
	DDX_Text(pDX, IDC_PPT_YOFFSET, m_iYOffset);
	DDX_Text(pDX, IDC_PPT_XSCALE, m_fXScale);
	DDX_Text(pDX, IDC_PPT_YSCALE, m_fYScale);
	DDX_Text(pDX, IDC_PPT_ROTANG, m_fRotAng);
	// }}AFX_DATA_MAP
	// DDV_MinMaxInt(pDX, m_iRotAng, 0, 360);
}

BEGIN_MESSAGE_MAP(QPropPageTexture, QPropPageFaceSelect)
	//{{AFX_MSG_MAP(QPropPageTexture)
	ON_BN_CLICKED(IDC_PPFS_SLCTALL, OnPPFSSlctAll)
	ON_BN_CLICKED(IDC_PPFS_SLCTFACE, OnPPFSSlctFace)
	ON_EN_SETFOCUS(IDC_PPT_XOFFSET, OnSetFocusPPTEditCtrls)
	ON_EN_KILLFOCUS(IDC_PPT_XOFFSET, OnKillFocusPPTXOffset)
	ON_EN_CHANGE(IDC_PPT_XOFFSET, OnChangePPTEditCtrls)
	ON_BN_CLICKED(IDC_PPT_XOFFSETDEC, OnPPTXOffsetDec)
	ON_BN_CLICKED(IDC_PPT_XOFFSETINC, OnPPTXOffsetInc)
	ON_EN_SETFOCUS(IDC_PPT_YOFFSET, OnSetFocusPPTEditCtrls)
	ON_EN_KILLFOCUS(IDC_PPT_YOFFSET, OnKillFocusPPTYOffset)
	ON_EN_CHANGE(IDC_PPT_YOFFSET, OnChangePPTEditCtrls)
	ON_BN_CLICKED(IDC_PPT_YOFFSETDEC, OnPPTYOffsetDec)
	ON_BN_CLICKED(IDC_PPT_YOFFSETINC, OnPPTYOffsetInc)
	ON_EN_SETFOCUS(IDC_PPT_XSCALE, OnSetFocusPPTEditCtrls)
	ON_EN_KILLFOCUS(IDC_PPT_XSCALE, OnKillFocusPPTXScale)
	ON_EN_CHANGE(IDC_PPT_XSCALE, OnChangePPTEditCtrls)
	ON_BN_CLICKED(IDC_PPT_XSCALEDEC, OnPPTXScaleDec)
	ON_BN_CLICKED(IDC_PPT_XSCALEINC, OnPPTXScaleInc)
	ON_EN_SETFOCUS(IDC_PPT_YSCALE, OnSetFocusPPTEditCtrls)
	ON_EN_KILLFOCUS(IDC_PPT_YSCALE, OnKillFocusPPTYScale)
	ON_EN_CHANGE(IDC_PPT_YSCALE, OnChangePPTEditCtrls)
	ON_BN_CLICKED(IDC_PPT_YSCALEDEC, OnPPTYScaleDec)
	ON_BN_CLICKED(IDC_PPT_YSCALEINC, OnPPTYScaleInc)
	ON_EN_SETFOCUS(IDC_PPT_ROTANG, OnSetFocusPPTEditCtrls)
	ON_EN_KILLFOCUS(IDC_PPT_ROTANG, OnKillFocusPPTRotAng)
	ON_EN_CHANGE(IDC_PPT_ROTANG, OnChangePPTEditCtrls)
	ON_BN_CLICKED(IDC_PPT_ROTANGDEC, OnPPTRotAngDec)
	ON_BN_CLICKED(IDC_PPT_ROTANGINC, OnPPTRotAngInc)
	ON_BN_CLICKED(IDC_PPT_TEXTLOCK, OnPPTTextureLock)
	ON_BN_CLICKED(IDC_PPT_AUTOALIGN, OnPPTAutoAlign)
	ON_BN_CLICKED(IDC_PPT_RESET, OnPPTReset)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

////////////////////////////////////////
// QPropPageTexture message handlers

BOOL QPropPageTexture::OnInitDialog() {
	QPropPageFaceSelect::OnInitDialog();

	ctrlXOffsetDec().SetBitmap(m_hBitMapLeft);
	ctrlXOffsetInc().SetBitmap(m_hBitMapRight);
	ctrlYOffsetInc().SetBitmap(m_hBitMapUp);
	ctrlYOffsetDec().SetBitmap(m_hBitMapDown);

	ctrlXScaleDec().SetBitmap(m_hBitMapLeft);
	ctrlXScaleInc().SetBitmap(m_hBitMapRight);
	ctrlYScaleInc().SetBitmap(m_hBitMapUp);
	ctrlYScaleDec().SetBitmap(m_hBitMapDown);

	ctrlRotAngDec().SetBitmap(m_hBitMapLeft);
	ctrlRotAngInc().SetBitmap(m_hBitMapRight);

	return TRUE;
}

void QPropPageTexture::OnUpdate(LPARAM lHint, Object *pScope) {
	QPropPageFaceSelect::OnUpdate(lHint, pScope);

	if (lHint & DUAV_NOQPROPPAGES || !active)
		return;

	DisableCtrls();
}

void QPropPageTexture::OnPPFSSlctAll() {
	QPropPageFaceSelect::OnPPFSSlctAll();

	DisableCtrls();
}

void QPropPageTexture::OnPPFSSlctFace() {
	QPropPageFaceSelect::OnPPFSSlctFace();

	DisableCtrls();
}

void QPropPageTexture::DisableCtrls() {

	if (m_bSelectAll) {
		// if (!ctrlsEnabled)
		// 	return;

		ctrlsEnabled = false;

		m_iXOffset = m_iYOffset = 0;
		m_fXScale = m_fYScale = 1.0f;
		m_fRotAng = 0.0f;
		m_iTextureLock = 0;
		UpdateData(FALSE);
	}
	else {
		// if (ctrlsEnabled)
		//	return;
		ctrlsEnabled = true;
	}

	BOOL flag = ctrlsEnabled;

	ctrlXOffset().EnableWindow(flag);
	ctrlXOffsetDec().EnableWindow(flag);
	ctrlXOffsetInc().EnableWindow(flag);
	ctrlYOffset().EnableWindow(flag);
	ctrlYOffsetDec().EnableWindow(flag);
	ctrlYOffsetInc().EnableWindow(flag);

	ctrlXScale().EnableWindow(flag);
	ctrlXScaleDec().EnableWindow(flag);
	ctrlXScaleInc().EnableWindow(flag);
	ctrlYScale().EnableWindow(flag);
	ctrlYScaleDec().EnableWindow(flag);
	ctrlYScaleInc().EnableWindow(flag);

	ctrlRotAng().EnableWindow(flag);
	ctrlRotAngDec().EnableWindow(flag);
	ctrlRotAngInc().EnableWindow(flag);

	Selector *pSlctr = &(pQMainFrame->GetSelector());
	int lockState = 0;
	if (!flag && pSlctr->GetNumMSelectedObjects() == 1 &&
		(pSlctr->GetMSelectedObjects())[0].GetPtr()->HasBrush()) {

		flag = true;

		// Iter faces of the brush and check for tex lock.
		Object *pObj = (pSlctr->GetMSelectedObjects())[0].GetPtr();
		Geometry *pBrush = &(pObj->GetBrush());
		FaceTex *pFaceTex;
		int numFaces = pBrush->GetNumFaces();
		bool unlock = false, lock = false;
		for(int i = 0; i < numFaces; i++) {
			pFaceTex = pBrush->GetFaceTexturePtr(i);
			if (pFaceTex->IsTexLocked())
				lock = true;
			else
				unlock = true;
		}
		if (unlock && lock)
			lockState = 2;
		else if (!lock)
			lockState = 0;
		else
			lockState = 1;
	}
	else if (m_bSelectFace) {
		lockState = ctrlTextLock().GetCheck();
	}

	ctrlTextLock().EnableWindow(flag);
	// Hack.
	ctrlTextLock().SetButtonStyle(BS_AUTO3STATE);
	ctrlTextLock().SetCheck(lockState);

	ctrlAlign().EnableWindow(flag);
	ctrlReset().EnableWindow(flag);
}

//===== New face has been selected
void QPropPageTexture::OnSlctNewFace() {
	// Get the new texture info and update controls.
	Selector *pSlctr = &(pQMainFrame->GetSelector());
	ASSERT(pSlctr->GetNumMSelectedObjects() == 1);

	Object *pSelObj = (pSlctr->GetMSelectedObjects())[0].GetPtr();
	ASSERT(pSelObj->HasBrush());

	Geometry *pBrush = &(pSelObj->GetBrush());
	int faceIndex = pSlctr->GetSelectFaceIndex();
	FaceTex *pFaceTex = pBrush->GetFaceTexturePtr(faceIndex);

	float fRotAng;
	pFaceTex->GetTInfo(m_iXOffset, m_iYOffset, fRotAng, m_fXScale, m_fYScale);
	m_fRotAng = fRotAng;
	m_iTextureLock = (pFaceTex->IsTexLocked() ? 1 : 0);

	m_texWnd.SetFaceTex(pFaceTex);

	UpdateData(FALSE);
}

//===== Commit manipulation change =====

void QPropPageTexture::RegAttribChange() {
	Selector *pSlctr = &(pQMainFrame->GetSelector());
	ASSERT(pSlctr->GetNumMSelectedObjects() == 1);
	ASSERT((pSlctr->GetMSelectedObjects())[0].GetPtr()->HasBrush());

	int faceIndex = pSlctr->GetSelectFaceIndex();
	ASSERT(faceIndex != -1);

	OpTextureFaceManip *op =
		new OpTextureFaceManip(faceIndex, oldXOff, oldYOff, oldRot,
							   oldXScl,oldYScl, (m_iTextureLock != 0));
	pQMainFrame->CommitOperation(*op);
}

// Remember state when gaining focus.
void QPropPageTexture::OnSetFocusPPTEditCtrls() {
	if (pInEditCtrl == NULL) {
		pInEditCtrl = GetFocus();
		oldXOff = m_iXOffset;
		oldYOff = m_iYOffset;
		oldXScl = m_fXScale;
		oldYScl = m_fYScale;
		oldRot = m_fRotAng;
	}
}

// Update doc and views to display change.
void QPropPageTexture::OnChangePPTEditCtrls() {
	// UpdateData(TRUE);
	// Manually get the num values from the edit ctrls.
	CString text;

	GetDlgItemText(IDC_PPT_XOFFSET, text);
	m_iXOffset = atoi(text);
	GetDlgItemText(IDC_PPT_YOFFSET, text);
	m_iYOffset = atoi(text);
	GetDlgItemText(IDC_PPT_XSCALE, text);
	m_fXScale = (float) atof(text);
	GetDlgItemText(IDC_PPT_YSCALE, text);
	m_fYScale = (float) atof(text);
	GetDlgItemText(IDC_PPT_ROTANG, text);
	m_fRotAng = (float) atof(text);

	UpdateDoc();
}

void QPropPageTexture::UpdateDoc(bool updateViews) {
	Selector *pSlctr = &(pQMainFrame->GetSelector());
	ASSERT(pSlctr->GetNumMSelectedObjects() == 1);

	Object *pSelObj = (pSlctr->GetMSelectedObjects())[0].GetPtr();
	pQDoc->TextureManipulate(*pSelObj, pSlctr->GetSelectFaceIndex(),
							 m_iXOffset, m_iYOffset, m_fRotAng,
							 m_fXScale, m_fYScale, (m_iTextureLock != 0),
							 false);

	// Manual update the QViews, skipping self.
	if (updateViews) {
 		pQDoc->UpdateAllViews(NULL, DUAV_QVIEWS | DUAV_OBJTEXMODATTRIB,
							  (CObject *) pSelObj);
	}
}

//===== X Offset =====
void QPropPageTexture::OnKillFocusPPTXOffset() {
	CWnd *focusWnd = GetFocus();

	if (!UpdateData(TRUE) && 
		(focusWnd == NULL || focusWnd->GetParent() != this)) {
		char text[10];
		sprintf(text, "%d", oldXOff);
		ctrlXOffset().SetWindowText(text);
		return;
	}

	if (oldXOff != m_iXOffset &&
		focusWnd != &ctrlXOffsetDec() && focusWnd != &ctrlXOffsetInc()) {
		RegAttribChange();
		pInEditCtrl = NULL;
	}
}

void QPropPageTexture::OnPPTXOffsetDec() {
	UINT style = ctrlXOffsetDec().GetButtonStyle() & ~BS_DEFPUSHBUTTON;
	ctrlXOffsetDec().SetButtonStyle(style);

	ctrlXOffset().SetFocus();

	m_iXOffset -= 4;
	UpdateData(FALSE);
	UpdateDoc();
}

void QPropPageTexture::OnPPTXOffsetInc() {
	UINT style = ctrlXOffsetInc().GetButtonStyle() & ~BS_DEFPUSHBUTTON;
	ctrlXOffsetInc().SetButtonStyle(style);

	ctrlXOffset().SetFocus();

	m_iXOffset += 4;
	UpdateData(FALSE);
	UpdateDoc();
}

//===== Y Offset =====
void QPropPageTexture::OnKillFocusPPTYOffset() {
	CWnd *focusWnd = GetFocus();

	if (!UpdateData(TRUE) &&
		(focusWnd == NULL || focusWnd->GetParent() != this)) {
		char text[10];
		sprintf(text, "%d", oldYOff);
		ctrlYOffset().SetWindowText(text);
		return;
	}

	if (oldYOff != m_iYOffset &&
		focusWnd != &ctrlYOffsetDec() && focusWnd != &ctrlYOffsetInc()) {
		RegAttribChange();
		pInEditCtrl = NULL;
	}
}

void QPropPageTexture::OnPPTYOffsetDec() {
	UINT style = ctrlYOffsetDec().GetButtonStyle() & ~BS_DEFPUSHBUTTON;
	ctrlYOffsetDec().SetButtonStyle(style);

	ctrlYOffset().SetFocus();

	m_iYOffset -= 4;
	UpdateData(FALSE);
	UpdateDoc();
}

void QPropPageTexture::OnPPTYOffsetInc() {
	UINT style = ctrlYOffsetInc().GetButtonStyle() & ~BS_DEFPUSHBUTTON;
	ctrlYOffsetInc().SetButtonStyle(style);

	ctrlYOffset().SetFocus();

	m_iYOffset += 4;
	UpdateData(FALSE);
	UpdateDoc();
}

//===== X Scale =====
void QPropPageTexture::OnKillFocusPPTXScale() {
	CWnd *focusWnd = GetFocus();

	if (!UpdateData(TRUE) &&
		(focusWnd == NULL || focusWnd->GetParent() != this)) {
		char text[10];
		sprintf(text, "%d", oldXScl);
		ctrlXScale().SetWindowText(text);
		return;
	}

	if (oldXScl != m_fXScale &&
		focusWnd != &ctrlXScaleDec() && focusWnd != &ctrlXScaleInc()) {
		RegAttribChange();
		pInEditCtrl = NULL;
	}
}

void QPropPageTexture::OnPPTXScaleDec() {
	UINT style = ctrlXScaleDec().GetButtonStyle() & ~BS_DEFPUSHBUTTON;
	ctrlXScaleDec().SetButtonStyle(style);

	ctrlXScale().SetFocus();

	m_fXScale -= 0.25;
	UpdateData(FALSE);
	UpdateDoc();
}

void QPropPageTexture::OnPPTXScaleInc() {
	UINT style = ctrlXScaleInc().GetButtonStyle() & ~BS_DEFPUSHBUTTON;
	ctrlXScaleInc().SetButtonStyle(style);

	ctrlXScale().SetFocus();

	m_fXScale += 0.25;
	UpdateData(FALSE);
	UpdateDoc();
}

//===== Y Scale =====
void QPropPageTexture::OnKillFocusPPTYScale() {
	CWnd *focusWnd = GetFocus();

	if (!UpdateData(TRUE) &&
		(focusWnd == NULL || focusWnd->GetParent() != this)) {
		char text[10];
		sprintf(text, "%d", oldYScl);
		ctrlYScale().SetWindowText(text);
		return;
	}

	if (oldYScl != m_fYScale &&
		focusWnd != &ctrlYScaleDec() && focusWnd != &ctrlYScaleInc()) {
		RegAttribChange();
		pInEditCtrl = NULL;
	}
}

void QPropPageTexture::OnPPTYScaleDec() {
	UINT style = ctrlYScaleDec().GetButtonStyle() & ~BS_DEFPUSHBUTTON;
	ctrlYScaleDec().SetButtonStyle(style);

	ctrlYScale().SetFocus();

	m_fYScale -= 0.25;
	UpdateData(FALSE);
	UpdateDoc();
}

void QPropPageTexture::OnPPTYScaleInc() {
	UINT style = ctrlYScaleInc().GetButtonStyle() & ~BS_DEFPUSHBUTTON;
	ctrlYScaleInc().SetButtonStyle(style);

	ctrlYScale().SetFocus();

	m_fYScale += 0.25;
	UpdateData(FALSE);
	UpdateDoc();
}

//===== Rot Ang =====
void QPropPageTexture::OnKillFocusPPTRotAng() {
	CWnd *focusWnd = GetFocus();

	if (!UpdateData(TRUE)&&
		(focusWnd == NULL || focusWnd->GetParent() != this)) {
		char text[10];
		sprintf(text, "%d", oldRot);
		ctrlRotAng().SetWindowText(text);
		return;
	}

	if (oldRot != m_fRotAng &&
		focusWnd != &ctrlRotAngDec() && focusWnd != &ctrlRotAngInc()) {
		RegAttribChange();
		pInEditCtrl = NULL;
	}
}

void QPropPageTexture::OnPPTRotAngDec() {
	UINT style = ctrlRotAngDec().GetButtonStyle() & ~BS_DEFPUSHBUTTON;
	ctrlRotAngDec().SetButtonStyle(style);

	ctrlRotAng().SetFocus();

	m_fRotAng -= 15.0f;
	UpdateData(FALSE);
	UpdateDoc();
}

void QPropPageTexture::OnPPTRotAngInc() {
	UINT style = ctrlRotAngInc().GetButtonStyle() & ~BS_DEFPUSHBUTTON;
	ctrlRotAngInc().SetButtonStyle(style);

	ctrlRotAng().SetFocus();

	m_fRotAng += 15.0f;
	UpdateData(FALSE);
	UpdateDoc();
}

//===== Texure Lock =====
void QPropPageTexture::OnPPTTextureLock() {
	if (!m_bSelectFace) {
		m_iTextureLock = ctrlTextLock().GetCheck();
		m_iTextureLock = m_iTextureLock % 2;
		TexLockBrush(m_iTextureLock != 0);
		ctrlTextLock().SetCheck(m_iTextureLock);
		return;
	}

	// Remember the old data.
	UpdateData(TRUE);
	oldXOff = m_iXOffset;
	oldYOff = m_iYOffset;
	oldXScl = m_fXScale;
	oldYScl = m_fYScale;
	oldRot = m_fRotAng;
	m_iTextureLock = m_iTextureLock % 2;
	bool oldTexLock = (m_iTextureLock == 0);

	Selector *pSlctr = &(pQMainFrame->GetSelector());
	int faceIndx = pSlctr->GetSelectFaceIndex();
	ASSERT(faceIndx != -1);

	TexLockFace(faceIndx, (m_iTextureLock != 0), m_iXOffset,
				m_iYOffset, m_fRotAng, m_fXScale, m_fYScale);

	UpdateData(FALSE);

	UpdateDoc(true);

	OpTextureFaceManip *op =
		new OpTextureFaceManip(faceIndx, oldXOff, oldYOff,
							   oldRot, oldXScl, oldYScl,
							   oldTexLock);
	pQMainFrame->CommitOperation(*op);
}

void QPropPageTexture::TexLockFace(int faceIndx, bool lockTex,
								   int &xOff, int &yOff, float &rotAng,
								   float &xScl, float &yScl) {
	// Get some ptrs.
	Selector *pSlctr = &(pQMainFrame->GetSelector());
	ASSERT(pSlctr->GetNumMSelectedObjects() == 1);

	Object *pObjNode = (pSlctr->GetMSelectedObjects())[0].GetPtr();
	Geometry *pBrush = &(pObjNode->GetBrush());
	ASSERT(faceIndx >= 0 && faceIndx < pBrush->GetNumFaces());
	FaceTex *pFaceTex = pBrush->GetFaceTexturePtr(faceIndx);
	const Plane *pPlane = &((pBrush->GetPlaneList())[faceIndx].GetPlane());

	if (pFaceTex->IsTexLocked() == lockTex) {
		// Dont need to do any work.
		pFaceTex->GetTInfo(xOff, yOff, rotAng, xScl, yScl);
		return;
	}

	// Derive transformation matrices.
	Matrix44 trans2Rel, trans2Abs, m;
	trans2Rel.SetIdentity();
	trans2Abs.SetIdentity();

	while (!pObjNode->IsRoot()) {
		m = trans2Rel;
		pObjNode->CalInvTransSpaceMatrix(trans2Rel.SetIdentity());
		trans2Rel.Multiply(m);
		pObjNode->CalTransSpaceMatrix(trans2Abs);
		pObjNode = pObjNode->GetParentPtr();
	}

	// Transform rel plane to abs coord.
	Plane absPlane(*pPlane);
	trans2Abs.Transform(absPlane);

	const Plane *pOrigPlane;
	const Vector3d *pEndNorm;

	if (lockTex) {  // Turn locking on.
		// Convert from abs ST coord into rel geometry's ST coord.
		pOrigPlane = &absPlane;
		m = trans2Rel;
		pEndNorm = &(pPlane->GetNorm());
	}
	else {  // Turn locking off.
		// Convert from rel geometry's coord into abs ST coord.
		pOrigPlane = pPlane;
		m = trans2Abs;
		pEndNorm = &(absPlane.GetNorm());
	}

	// Find the new settings.
	int xOffset, yOffset;
	float rAng, xScale, yScale;
	pFaceTex->GetTInfo(xOffset, yOffset, rAng, xScale, yScale);

	// Transform a tmp tex face.
	FaceTex tmpFace("");
	tmpFace.SetTInfo(xOffset, yOffset, rAng, xScale, yScale);
	tmpFace.Transform(m, *pOrigPlane, pEndNorm);

	// Get the new settings.
    tmpFace.GetTInfo(xOff, yOff, rotAng, xScl, yScl);
	rotAng = ROUND3(rotAng);
	xScl = ROUND3(xScl);
	yScl = ROUND3(yScl);
}

void QPropPageTexture::TexLockBrush(bool lockTex) {
	Selector *pSlctr = &(pQMainFrame->GetSelector());
	ASSERT(pSlctr->GetNumMSelectedObjects() == 1);

	Object *pObj = (pSlctr->GetMSelectedObjects())[0].GetPtr();
	Geometry *pBrush = &(pObj->GetBrush());
	FaceTex *pFaceTex;

	int numFaces = pBrush->GetNumFaces();

	int *pOldXOffs = new int[numFaces];
	int *pOldYOffs = new int[numFaces];
	float *pOldRotAngs = new float[numFaces];
	float *pOldXScls = new float[numFaces];
	float *pOldYScls = new float[numFaces];
	bool *pOldTexLocks = new bool[numFaces];

	int xOff, yOff;
	float rotAng, xScl, yScl;

	for(int i = 0; i < numFaces; i++) {
		pFaceTex = pBrush->GetFaceTexturePtr(i);
		pFaceTex->GetTInfo(pOldXOffs[i], pOldYOffs[i], pOldRotAngs[i],
						   pOldXScls[i], pOldYScls[i]);
		pOldTexLocks[i] = pFaceTex->IsTexLocked();

		TexLockFace(i, lockTex, xOff, yOff, rotAng, xScl, yScl);

		pFaceTex->SetTInfo(xOff, yOff, rotAng, xScl, yScl);
		pFaceTex->SetTexLock(lockTex);
	}

	// Manual update the QViews, skipping self.
	pQDoc->UpdateAllViews(NULL, DUAV_QVIEWS | DUAV_OBJTEXMODATTRIB,
						  (CObject *) pObj);

	// Commit operation.
	OpTextureBrushManip *op =
		new OpTextureBrushManip(pOldXOffs, pOldYOffs, pOldRotAngs,
								pOldXScls, pOldYScls, pOldTexLocks);
	pQMainFrame->CommitOperation(*op);

	// Clean up.
	delete pOldXOffs;
	delete pOldYOffs;
	delete pOldRotAngs;
	delete pOldXScls;
	delete pOldYScls;
	delete pOldTexLocks;
}

//===== Auto Align Texture =====
void QPropPageTexture::OnPPTAutoAlign() {
	if (!m_bSelectFace) {
		AutoAlignBrush();
		return;
	}

	// Remember the old data.
	UpdateData(TRUE);
	oldXOff = m_iXOffset;
	oldYOff = m_iYOffset;
	oldXScl = m_fXScale;
	oldYScl = m_fYScale;
	oldRot = m_fRotAng;
	bool oldTexLock = (m_iTextureLock == 0);

	Selector *pSlctr = &(pQMainFrame->GetSelector());
	int faceIndx = pSlctr->GetSelectFaceIndex();
	ASSERT(faceIndx != -1);

	AutoAlignFace(faceIndx, m_iXOffset, m_iYOffset,
				  m_fRotAng, m_fXScale, m_fYScale);

	UpdateData(FALSE);

	UpdateDoc(true);

	OpTextureFaceManip *op =
		new OpTextureFaceManip(pSlctr->GetSelectFaceIndex(),
							   oldXOff, oldYOff, oldRot,
							   oldXScl, oldYScl, oldTexLock);
	pQMainFrame->CommitOperation(*op);
}

void QPropPageTexture::AutoAlignFace(int faceIndx, int &xOff, int &yOff,
									 float &rotAng, float &xScl, float &yScl) {
	// Get some ptrs.
	Selector *pSlctr = &(pQMainFrame->GetSelector());
	ASSERT(pSlctr->GetNumMSelectedObjects() == 1);

	Object *pObjNode = (pSlctr->GetMSelectedObjects())[0].GetPtr();
	Geometry *pBrush = &(pObjNode->GetBrush());
	ASSERT(faceIndx >= 0 && faceIndx < pBrush->GetNumFaces());
	FaceTex *pFaceTex = pBrush->GetFaceTexturePtr(faceIndx);
	const GPolygon *pGPoly = &(pBrush->GetFace(faceIndx));
	Plane plane((pBrush->GetPlaneList())[faceIndx].GetPlane());

	// Derive transformation matrix.
	Matrix44 trans;
	trans.SetIdentity();

	if (!pFaceTex->IsTexLocked()) {
		while (!pObjNode->IsRoot()) {
			pObjNode->CalTransSpaceMatrix(trans);
			pObjNode = pObjNode->GetParentPtr();
		}
		trans.Transform(plane);
	}

	// Iter through face vertices and find bounding vectors.
	int i, vIndx;
	Vector3d pVec, nVec(plane.GetNorm());
	float s, t, sMin, sMax, tMin, tMax;

	for(i = 0; i < pGPoly->GetNumSides(); i++) {
		vIndx = (pGPoly->GetSide(i)).GetStartPoint();
		trans.Transform(pVec, pBrush->GetVertex(vIndx));
		FaceTex::Project2D(nVec, pVec, s, t);

		if (i == 0) {
			sMin = sMax = s;
			tMin = tMax = t;
			continue;
		}

		sMin = Min(s, sMin);
		sMax = Max(s, sMax);
		tMin = Min(t, tMin);
		tMax = Max(t, tMax);
	}

	// Define basis vectors.
	int width, height;
	Vector3d oVec, sVec, tVec;
	FaceTex tmpFace("");

	pFaceTex->GetTDim(width, height);
	tmpFace.ST2XYZ(plane, sMin, tMin, oVec);
	tmpFace.ST2XYZ(plane, (sMax - sMin) / width + sMin, tMin, sVec);
	tmpFace.ST2XYZ(plane, sMin, (tMax - tMin) / height + tMin, tVec);

	// Derive new alignment settings.
	float xOffset, yOffset, rAng, xScale, yScale;
	FaceTex::DeriveCoordSys(oVec, sVec, tVec, nVec,
							xOffset, yOffset, rAng,
							xScale, yScale);

	xOff = ROUNDI(xOffset);
	yOff = -ROUNDI(yOffset);
	rAng = RAD2DEG(rAng);
	rotAng = ROUND3(rAng);
	xScl = ROUND3(xScale);
	yScl = ROUND3(yScale);
}

void QPropPageTexture::AutoAlignBrush(void) {
	Selector *pSlctr = &(pQMainFrame->GetSelector());
	ASSERT(pSlctr->GetNumMSelectedObjects() == 1);

	Object *pObjNode = (pSlctr->GetMSelectedObjects())[0].GetPtr();
	Geometry *pBrush = &(pObjNode->GetBrush());
	FaceTex *pFaceTex;

	int numFaces = pBrush->GetNumFaces();

	int *pOldXOffs = new int[numFaces];
	int *pOldYOffs = new int[numFaces];
	float *pOldRotAngs = new float[numFaces];
	float *pOldXScls = new float[numFaces];
	float *pOldYScls = new float[numFaces];
	bool *pOldTexLocks = new bool[numFaces];

	int xOff, yOff;
	float rotAng, xScl, yScl;

	for(int i = 0; i < numFaces; i++) {
		pFaceTex = pBrush->GetFaceTexturePtr(i);
		pFaceTex->GetTInfo(pOldXOffs[i], pOldYOffs[i], pOldRotAngs[i],
						   pOldXScls[i], pOldYScls[i]);
		pOldTexLocks[i] = pFaceTex->IsTexLocked();

		AutoAlignFace(i, xOff, yOff, rotAng, xScl, yScl);

		pFaceTex->SetTInfo(xOff, yOff, rotAng, xScl, yScl);
	}

	// Manual update the QViews, skipping self.
	pQDoc->UpdateAllViews(NULL, DUAV_QVIEWS | DUAV_OBJTEXMODATTRIB,
						  (CObject *) pObjNode);

	// Commit operation.
	OpTextureBrushManip *op =
		new OpTextureBrushManip(pOldXOffs, pOldYOffs, pOldRotAngs,
								pOldXScls, pOldYScls, pOldTexLocks);
	pQMainFrame->CommitOperation(*op);

	// Clean up.
	delete pOldXOffs;
	delete pOldYOffs;
	delete pOldRotAngs;
	delete pOldXScls;
	delete pOldYScls;
	delete pOldTexLocks;
}

//===== Reset =====
void QPropPageTexture::OnPPTReset() {
	if (m_bSelectFace)
		ResetTexFace();
	else
		ResetTexBrushFaces();
}

void QPropPageTexture::ResetTexFace(void) {
	// Remember the old data.
	UpdateData(TRUE);
	oldXOff = m_iXOffset;
	oldYOff = m_iYOffset;
	oldXScl = m_fXScale;
	oldYScl = m_fYScale;
	oldRot = m_fRotAng;

	m_iTextureLock = 0;
	m_iXOffset = m_iYOffset = 0;
	m_fXScale = m_fYScale = 1.0f;
	m_fRotAng = 0.0f;
	UpdateData(FALSE);

	UpdateDoc();

	Selector *pSlctr = &(pQMainFrame->GetSelector());
	ASSERT(pSlctr->GetSelectFaceIndex() != -1);

	OpTextureFaceManip *op =
		new OpTextureFaceManip(pSlctr->GetSelectFaceIndex(),
							   oldXOff, oldYOff, oldRot,
							   oldXScl, oldYScl, (m_iTextureLock != 0));
	pQMainFrame->CommitOperation(*op);
}

void QPropPageTexture::ResetTexBrushFaces(void) {
	Selector *pSlctr = &(pQMainFrame->GetSelector());
	ASSERT(pSlctr->GetNumMSelectedObjects() == 1);

	Object *pObj = (pSlctr->GetMSelectedObjects())[0].GetPtr();
	ASSERT(pObj->HasBrush());
	Geometry *pBrush = &(pObj->GetBrush());

	int numFaces = pBrush->GetNumFaces();

	int *pOldXOffs = new int[numFaces];
	int *pOldYOffs = new int[numFaces];
	float *pOldRotAngs = new float[numFaces];
	float *pOldXScls = new float[numFaces];
	float *pOldYScls = new float[numFaces];
	bool *pOldTexLocks = new bool[numFaces];

	FaceTex *pFaceTex;
	for(int i = 0; i < numFaces; i++) {
		pFaceTex = pBrush->GetFaceTexturePtr(i);
		pFaceTex->GetTInfo(pOldXOffs[i], pOldYOffs[i], pOldRotAngs[i],
						   pOldXScls[i], pOldYScls[i]);
		pFaceTex->SetTInfo(0, 0, 0.0f, 1.0f, 1.0f);
		pFaceTex->SetTexLock(false);
	}

	// Manual update the QViews, skipping self.
	pQDoc->UpdateAllViews(NULL, DUAV_QVIEWS | DUAV_OBJTEXMODATTRIB,
						  (CObject *) pObj);

	// Commit operation
	OpTextureBrushManip *op =
		new OpTextureBrushManip(pOldXOffs, pOldYOffs, pOldRotAngs,
								pOldXScls, pOldYScls, pOldTexLocks);
	pQMainFrame->CommitOperation(*op);

	// Clean up.
	delete pOldXOffs;
	delete pOldYOffs;
	delete pOldRotAngs;
	delete pOldXScls;
	delete pOldYScls;
	delete pOldTexLocks;
}
