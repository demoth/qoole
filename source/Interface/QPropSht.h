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

#if !defined(AFX_QPROPSHEET_H__E0B0EDFB_5719_11D2_BA3A_004005310168__INCLUDED_)
#define AFX_QPROPSHEET_H__E0B0EDFB_5719_11D2_BA3A_004005310168__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// QPropSheet.h : header file
//

#include "QPropMsc.h"
#include "QPropEnt.h"
#include "QPropTex.h"
#include "QPropSrf.h"
#include "QPropCnt.h"

#define PROPPAGE_ADDSURFACE		0x0001
#define PROPPAGE_ADDCONTENT		0x0002

//======================================================================
// QPropSheet
//======================================================================

class QPropSheet : public CPropertySheet {
	DECLARE_DYNAMIC(QPropSheet)

// Construction
public:
	QPropSheet(CWnd *pParentWnd = NULL, UINT iSelectPage =0, CRect *pRect = NULL);

// Attributes
public:
	// Standard pages.
	QPropPageFile		m_FilePage;
	QPropPageObject		m_ObjectPage;
	QPropPageEntity		m_EntityPage;
	QPropPageTexture	m_TexturePage;
	// Game Specific pages.
	QPropPageSurface	m_SurfacePage;
	QPropPageContent	m_ContentPage;

// Operations
public:
	void RegisterDocument(QooleDoc *pOldDoc, QooleDoc *pNewDoc);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(QPropSheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~QPropSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(QPropSheet)
	afx_msg void OnNcDestroy();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QPROPSHEET_H__E0B0EDFB_5719_11D2_BA3A_004005310168__INCLUDED_)
