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

#if !defined(AFX_QCOMPILE_H__490D9695_8066_11D2_BA62_004005310168__INCLUDED_)
#define AFX_QCOMPILE_H__490D9695_8066_11D2_BA62_004005310168__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// QCompile.h : header file
//

//======================================================================
// QCompile
//======================================================================

class QCompile : public CDialog {
// Construction
public:
	QCompile(CWnd* pParent = NULL);   // standard constructor

	static void ExecuteCompiles(void);    // Go do it.
	static void StopCompiles(void);
	static void PrintText(LPCTSTR text);

// Dialog Data
	//{{AFX_DATA(QCompile)
	enum { IDD = IDD_COMPILE };
	CString	m_sParam;
	//}}AFX_DATA
	static BOOL		m_bUtils[MAXUTILS];
	static CString	m_sParams[MAXUTILS];
	static int		m_iSetParam;
	static int		cmpStage;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(QCompile)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(QCompile)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	afx_msg void OnUtilSelChanged();

	static bool renderSuspend;

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QCOMPILE_H__490D9695_8066_11D2_BA62_004005310168__INCLUDED_)
