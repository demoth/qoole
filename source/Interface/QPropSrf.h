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

#if !defined(AFX_QPROPSRF_H__FD3ECFAA_62C3_11D2_BA41_004005310168__INCLUDED_)
#define AFX_QPROPSRF_H__FD3ECFAA_62C3_11D2_BA41_004005310168__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// QPropSrf.h : header file
//

#include "QPropTex.h"

//======================================================================
// QPropPageSurface dialog
//======================================================================

class QPropPageSurface : public QPropPageFaceSelect {
	DECLARE_DYNCREATE(QPropPageSurface)

// Construction
public:
	QPropPageSurface();
	~QPropPageSurface();

	virtual void OnUpdate(LPARAM lHint, Object *pScope);
	BOOL UpdateData(BOOL bSaveAndValidate, UINT &attrib, UINT &value);

// Dialog Data
	//{{AFX_DATA(QPropPageSurface)
	enum { IDD = IDD_PROPPAGE_SURFACE };
	BOOL	m_bFlowing;
	BOOL	m_bHint;
	BOOL	m_bLight;
	BOOL	m_bNoDraw;
	BOOL	m_bSkip;
	BOOL	m_bSky;
	BOOL	m_bSlick;
	BOOL	m_bTrans33;
	BOOL	m_bTrans66;
	UINT	m_iValue;
	BOOL	m_bWarp;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(QPropPageSurface)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CButton &ctrlLight()   { return *((CButton *) GetDlgItem(IDC_PPS_LIGHT)); }
	CButton &ctrlSlick()   { return *((CButton *) GetDlgItem(IDC_PPS_SLICK)); }
	CButton &ctrlSky()     { return *((CButton *) GetDlgItem(IDC_PPS_SKY)); }
	CButton &ctrlWarp()    { return *((CButton *) GetDlgItem(IDC_PPS_WARP)); }
	CButton &ctrlTrans33() { return *((CButton *) GetDlgItem(IDC_PPS_TRANS33)); }
	CButton &ctrlTrans66() { return *((CButton *) GetDlgItem(IDC_PPS_TRANS66)); }
	CButton &ctrlFlowing() { return *((CButton *) GetDlgItem(IDC_PPS_FLOWING)); }
	CButton &ctrlNoDraw()  { return *((CButton *) GetDlgItem(IDC_PPS_NODRAW)); }
	CButton &ctrlHint()    { return *((CButton *) GetDlgItem(IDC_PPS_HINT)); }
	CButton &ctrlSkip()    { return *((CButton *) GetDlgItem(IDC_PPS_SKIP)); }
	CEdit   &ctrlValue()   { return *((CEdit *) GetDlgItem(IDC_PPS_VALUE)); }

	void DisableCtrls();
	void OnSlctNewFace();

	void RegAttribChange(UINT mask);

	UINT oldSurfAttrib;
	BOOL ctrlsEnabled;
	bool validValue;

	// Generated message map functions
	//{{AFX_MSG(QPropPageSurface)
	afx_msg void OnPPFSSlctAll();
	afx_msg void OnPPSHint();
	afx_msg void OnPPSLight();
	afx_msg void OnPPSNoDraw();
	afx_msg void OnPPSSkip();
	afx_msg void OnPPSSky();
	afx_msg void OnPPSSlick();
	afx_msg void OnPPSTrans33();
	afx_msg void OnPPSTrans66();
	afx_msg void OnPPSWarp();
	afx_msg void OnPPSFlowing();
	afx_msg void OnKillFocusPPSValue();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QPROPSRF_H__FD3ECFAA_62C3_11D2_BA41_004005310168__INCLUDED_)
