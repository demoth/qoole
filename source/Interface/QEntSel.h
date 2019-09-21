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

#if !defined(AFX_QENTSEL_H__5296C062_75C7_11D2_A07A_00400543C1CD__INCLUDED_)
#define AFX_QENTSEL_H__5296C062_75C7_11D2_A07A_00400543C1CD__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// QEntSel.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// QEntSel dialog

#include "LCommon.h"

class QEntSel : public CDialog
{
// Construction
public:
	QEntSel(CWnd* pParent = NULL);   // standard constructor
	~QEntSel();

	void LoadEntSets(void);

	char *m_gameName;
	char *m_entName;
	char *m_palName;

// Dialog Data
	//{{AFX_DATA(QEntSel)
	enum { IDD = IDD_ENTSEL };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(QEntSel)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(QEntSel)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeCombo1();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	LConfig *cfg;
	int gameNum;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QENTSEL_H__5296C062_75C7_11D2_A07A_00400543C1CD__INCLUDED_)
