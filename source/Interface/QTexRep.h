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

#if !defined(AFX_QTEXREP_H__A16EE542_EC57_11D2_ACCE_00400543C1CD__INCLUDED_)
#define AFX_QTEXREP_H__A16EE542_EC57_11D2_ACCE_00400543C1CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// QTexRep.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// QTexRep dialog

#include "Objects.h"

class QTexRep : public CDialog
{
// Construction
public:
	QTexRep(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(QTexRep)
	enum { IDD = IDD_TEXREP };
	CEdit	m_Old;
	CEdit	m_New;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(QTexRep)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(QTexRep)
	afx_msg void OnApply();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	static char oldTex[64], newTex[64];
	static bool Apply(Object &obj);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QTEXREP_H__A16EE542_EC57_11D2_ACCE_00400543C1CD__INCLUDED_)
