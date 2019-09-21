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

#if !defined(AFX_QCONF3DV_H__F81465D3_7387_11D2_BA51_004005310168__INCLUDED_)
#define AFX_QCONF3DV_H__F81465D3_7387_11D2_BA51_004005310168__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// QConf3DV.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// QConfig3DView dialog

class QConfig3DView : public CPropertyPage
{
	DECLARE_DYNCREATE(QConfig3DView)

// Construction
public:
	QConfig3DView();
	~QConfig3DView();

// Dialog Data
	//{{AFX_DATA(QConfig3DView)
	enum { IDD = IDD_CONFIG_3DVIEW };
	CHotKeyCtrl	m_kFlyDown;
	CHotKeyCtrl	m_kFlyUp;
	CHotKeyCtrl	m_kStepRight;
	CHotKeyCtrl	m_kStepLeft;
	CHotKeyCtrl	m_kBackward;
	CHotKeyCtrl	m_kForward;
	BOOL	m_bInvLMouse;
	UINT	m_iWalkSpeed;
	UINT	m_iSensitivity;
	//}}AFX_DATA
	BOOL	m_bInvRMouse;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(QConfig3DView)
	public:
	virtual BOOL OnApply();
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(QConfig3DView)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QCONF3DV_H__F81465D3_7387_11D2_BA51_004005310168__INCLUDED_)
