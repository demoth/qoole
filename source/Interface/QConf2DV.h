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

#if !defined(AFX_QCONF2DV_H__2B53121B_7156_11D2_BA50_004005310168__INCLUDED_)
#define AFX_QCONF2DV_H__2B53121B_7156_11D2_BA50_004005310168__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// QConf2DV.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// QConfig2DViews dialog

class QConfig2DViews : public CPropertyPage {
	DECLARE_DYNCREATE(QConfig2DViews)

// Construction
public:
	QConfig2DViews();
	~QConfig2DViews();

// Dialog Data
	//{{AFX_DATA(QConfig2DViews)
	enum { IDD = IDD_CONFIG_2DVIEWS };
	BOOL	m_bGrid;
	CString	m_sGridSize;
	BOOL	m_bGridThick;
	UINT	m_iGridThickSize;
	BOOL	m_bRotSnap;
	CString	m_sRotSnapSize;
	BOOL	m_bSnap;
	CString	m_sSnapSize;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(QConfig2DViews)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnApply();
	virtual BOOL OnKillActive();
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(QConfig2DViews)
	virtual BOOL OnInitDialog();
	afx_msg void OnConf2DGrid();
	afx_msg void OnConf2DGridThick();
	afx_msg void OnConf2DRotSnap();
	afx_msg void OnConf2DSnap();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()



};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QCONF2DV_H__2B53121B_7156_11D2_BA50_004005310168__INCLUDED_)
