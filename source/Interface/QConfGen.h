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

#if !defined(AFX_QCONFGEN_H__813272F1_B348_11D2_BA9D_004005310168__INCLUDED_)
#define AFX_QCONFGEN_H__813272F1_B348_11D2_BA9D_004005310168__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// QConfGen.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// QConfGen dialog

class QConfGen : public CPropertyPage {
	DECLARE_DYNCREATE(QConfGen)

// Construction
public:
	QConfGen();
	~QConfGen();

// Dialog Data
	//{{AFX_DATA(QConfGen)
	enum { IDD = IDD_CONFIG_GENERAL };
	BOOL	m_bHollowPrompt;
	int		m_iHollowWidth;
	int		m_iUndoLevels;
	BOOL	m_bGroupPrompt;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(QConfGen)
	public:
	virtual BOOL OnKillActive();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(QConfGen)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QCONFGEN_H__813272F1_B348_11D2_BA9D_004005310168__INCLUDED_)
