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

#if !defined(AFX_QPROPENT_H__B24EFAE2_62A2_11D2_8669_000000000000__INCLUDED_)
#define AFX_QPROPENT_H__B24EFAE2_62A2_11D2_8669_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// qpropent.h : header file
//

#include "QPropPg.h"
#include "QPropLst.h"

//======================================================================
// QPropPageEntity
//======================================================================

class QPropPageEntity : public QPropPage {
	DECLARE_DYNCREATE(QPropPageEntity)

// Construction
public:
	QPropPageEntity();
	~QPropPageEntity();

	virtual void OnUpdate(LPARAM lHint, Object *pScope);

	static void OnClickCallBack(int row, QPropLstInfoStruct *info);
	static CString *DlgProcCallBack(CString &sInitText);

// Dialog Data
	//{{AFX_DATA(QPropPageEntity)
	enum { IDD = IDD_PROPPAGE_ENTITY };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(QPropPageEntity)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
	CHeaderCtrl headerCtrl;
	QPropertyList propList;
	static EntInfo *pEntInfo;

	// Generated message map functions
	//{{AFX_MSG(QPropPageEntity)
	virtual BOOL OnInitDialog();
	afx_msg void OnLightButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QPROPENT_H__B24EFAE2_62A2_11D2_8669_000000000000__INCLUDED_)
