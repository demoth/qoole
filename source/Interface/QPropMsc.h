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

#if !defined(AFX_QPROPMSC_H__B24EFAE3_62A2_11D2_8669_000000000000__INCLUDED_)
#define AFX_QPROPMSC_H__B24EFAE3_62A2_11D2_8669_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// qpropmsc.h : header file
//

#include "QPropPg.h"

//======================================================================
// QPropPageFile
//======================================================================

class QPropPageFile : public QPropPage {
	DECLARE_DYNCREATE(QPropPageFile)

// Construction
public:
	QPropPageFile();
	~QPropPageFile();

	virtual void OnUpdate(LPARAM lHint, Object *pScope);

// Dialog Data
	//{{AFX_DATA(QPropPageFile)
	enum { IDD = IDD_PROPPAGE_FILE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(QPropPageFile)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(QPropPageFile)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//======================================================================
// QPropPageObject
//======================================================================

class QPropPageObject : public QPropPage {
	DECLARE_DYNCREATE(QPropPageObject)

// Construction
public:
	QPropPageObject();
	~QPropPageObject();

	virtual void OnUpdate(LPARAM lHint, Object *pScope);

// Dialog Data
	//{{AFX_DATA(QPropPageObject)
	enum { IDD = IDD_PROPPAGE_OBJECT };
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(QPropPageObject)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CStatic &ctlObjCount()	{ return *((CStatic *) GetDlgItem(IDC_PPO_OBJCOUNT)); }
	CStatic &ctlObjPos()	{ return *((CStatic *) GetDlgItem(IDC_PPO_OBJPOS)); }
	CStatic &ctlObjDimen()	{ return *((CStatic *) GetDlgItem(IDC_PPO_OBJDIMENSION)); }
	CStatic &ctlEntCount()	{ return *((CStatic *) GetDlgItem(IDC_PPO_ENTCOUNT)); }
	CStatic &ctlBrushCount()	{ return *((CStatic *) GetDlgItem(IDC_PPO_BRUSHCOUNT)); }

	// Generated message map functions
	//{{AFX_MSG(QPropPageObject)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QPROPMSC_H__B24EFAE3_62A2_11D2_8669_000000000000__INCLUDED_)
