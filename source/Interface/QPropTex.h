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

#if !defined(AFX_QPROPTEX_H__B24EFAE1_62A2_11D2_8669_000000000000__INCLUDED_)
#define AFX_QPROPTEX_H__B24EFAE1_62A2_11D2_8669_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// qproptex.h : header file
//

#include "QPropPg.h"
#include "QTexDraw.h"


//======================================================================
// QPropPageFaceSelect
//======================================================================

class QPropPageFaceSelect : public QPropPage {
	DECLARE_DYNCREATE(QPropPageFaceSelect)

// Construction
public:
	QPropPageFaceSelect();
	QPropPageFaceSelect(int nID);
	~QPropPageFaceSelect();

	virtual void OnUpdate(LPARAM lHint, Object *pScope);

// Dialog Data
	//{{AFX_DATA(QPropPageFaceSelect)
	enum { IDD = IDD_PROPPAGE_FACESLCT };
	QTexDraw	m_texWnd;
	BOOL	m_bSelectAll;
	BOOL	m_bSelectFace;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(QPropPageFaceSelect)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CButton &ctrlSelectAll()  { return *((CButton *) GetDlgItem(IDC_PPFS_SLCTALL)); }
	CButton &ctrlSelectFace() { return *((CButton *) GetDlgItem(IDC_PPFS_SLCTFACE)); }
	CButton &ctrlFacePrev()   { return *((CButton *) GetDlgItem(IDC_PPFS_PREVFACE)); }
	CButton &ctrlFaceNext()   { return *((CButton *) GetDlgItem(IDC_PPFS_NEXTFACE)); }

	void RenderFaceSlct(bool slct);
	virtual void OnSlctNewFace() {};

	// Generated message map functions
	//{{AFX_MSG(QPropPageFaceSelect)
	afx_msg void OnPPFSPrevFace();
	afx_msg void OnPPFSNextFace();
	//}}AFX_MSG
	virtual BOOL OnInitDialog();
	virtual void OnPPFSSlctAll();
	virtual void OnPPFSSlctFace();
	DECLARE_MESSAGE_MAP()
};

//======================================================================
// QPropPageTexture
//======================================================================

class QPropPageTexture : public QPropPageFaceSelect {
	DECLARE_DYNCREATE(QPropPageTexture)

// Construction
public:
	QPropPageTexture();
	~QPropPageTexture();

	virtual void OnUpdate(LPARAM lHint, Object *pScope);

// Dialog Data
	//{{AFX_DATA(QPropPageTexture)
	enum { IDD = IDD_PROPPAGE_TEXTURE };
	int		m_iTextureLock;
	int		m_iXOffset;
	int		m_iYOffset;
	float	m_fXScale;
	float	m_fYScale;
	float	m_fRotAng;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(QPropPageTexture)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CEdit	&ctrlXOffset()    { return *((CEdit *)   GetDlgItem(IDC_PPT_XOFFSET)); }
	CButton &ctrlXOffsetDec() { return *((CButton *) GetDlgItem(IDC_PPT_XOFFSETDEC)); }
	CButton &ctrlXOffsetInc() { return *((CButton *) GetDlgItem(IDC_PPT_XOFFSETINC)); }
	CEdit	&ctrlYOffset()    { return *((CEdit *)   GetDlgItem(IDC_PPT_YOFFSET)); }
	CButton &ctrlYOffsetDec() { return *((CButton *) GetDlgItem(IDC_PPT_YOFFSETDEC)); }
	CButton &ctrlYOffsetInc() { return *((CButton *) GetDlgItem(IDC_PPT_YOFFSETINC)); }
	CEdit	&ctrlXScale()     { return *((CEdit *)   GetDlgItem(IDC_PPT_XSCALE)); }
	CButton &ctrlXScaleDec()  { return *((CButton *) GetDlgItem(IDC_PPT_XSCALEDEC)); }
	CButton &ctrlXScaleInc()  { return *((CButton *) GetDlgItem(IDC_PPT_XSCALEINC)); }
	CEdit	&ctrlYScale()     { return *((CEdit *)   GetDlgItem(IDC_PPT_YSCALE)); }
	CButton &ctrlYScaleDec()  { return *((CButton *) GetDlgItem(IDC_PPT_YSCALEDEC)); }
	CButton &ctrlYScaleInc()  { return *((CButton *) GetDlgItem(IDC_PPT_YSCALEINC)); }
	CEdit	&ctrlRotAng()     { return *((CEdit *)   GetDlgItem(IDC_PPT_ROTANG)); }
	CButton &ctrlRotAngDec()  { return *((CButton *) GetDlgItem(IDC_PPT_ROTANGDEC)); }
	CButton &ctrlRotAngInc()  { return *((CButton *) GetDlgItem(IDC_PPT_ROTANGINC)); }
	CButton &ctrlTextLock()   { return *((CButton *) GetDlgItem(IDC_PPT_TEXTLOCK)); }
	CButton &ctrlAlign()      { return *((CButton *) GetDlgItem(IDC_PPT_AUTOALIGN)); }
	CButton &ctrlReset()      { return *((CButton *) GetDlgItem(IDC_PPT_RESET)); }

	void DisableCtrls();
	void OnSlctNewFace();

	void RegAttribChange();
	void UpdateDoc(bool updateViews = true);

	void TexLockFace(int faceIndx, bool lockTex,
					 int &xOff, int &yOff, float &rotAng,
					 float &xScl, float &yScl);
	void TexLockBrush(bool lockTex);

	void AutoAlignFace(int faceIndx, int &xOff, int &yOff,
					   float &rotAng, float &xScl, float &yScl);
	void AutoAlignBrush(void);

	void ResetTexFace(void);
	void ResetTexBrushFaces(void);

	HBITMAP m_hBitMapUp, m_hBitMapDown;
	HBITMAP m_hBitMapLeft, m_hBitMapRight;

	bool	ctrlsEnabled;
	int		oldXOff, oldYOff;
	float	oldXScl, oldYScl, oldRot;
	CWnd	*pInEditCtrl;

	// Generated message map functions
	//{{AFX_MSG(QPropPageTexture)
	afx_msg void OnPPFSSlctAll();
	afx_msg void OnPPFSSlctFace();
	virtual BOOL OnInitDialog();
	afx_msg void OnSetFocusPPTEditCtrls();
	afx_msg void OnChangePPTEditCtrls();
	afx_msg void OnKillFocusPPTXOffset();
	afx_msg void OnPPTXOffsetDec();
	afx_msg void OnPPTXOffsetInc();
	afx_msg void OnKillFocusPPTYOffset();
	afx_msg void OnPPTYOffsetDec();
	afx_msg void OnPPTYOffsetInc();
	afx_msg void OnKillFocusPPTXScale();
	afx_msg void OnPPTXScaleDec();
	afx_msg void OnPPTXScaleInc();
	afx_msg void OnKillFocusPPTYScale();
	afx_msg void OnPPTYScaleDec();
	afx_msg void OnPPTYScaleInc();
	afx_msg void OnKillFocusPPTRotAng();
	afx_msg void OnPPTRotAngDec();
	afx_msg void OnPPTRotAngInc();
	afx_msg void OnPPTTextureLock();
	afx_msg void OnPPTAutoAlign();
	afx_msg void OnPPTReset();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QPROPTEX_H__B24EFAE1_62A2_11D2_8669_000000000000__INCLUDED_)
