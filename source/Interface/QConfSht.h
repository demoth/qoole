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

#if !defined(AFX_QCONFSHT_H__1A2006B3_6D67_11D2_BA4C_004005310168__INCLUDED_)
#define AFX_QCONFSHT_H__1A2006B3_6D67_11D2_BA4C_004005310168__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// QConfSht.h : header file
//

#include "QConfGen.h"
#include "QConf2DV.h"
#include "QConf3DV.h"
#include "QConfGms.h"
#include "QConfDrw.h"

/////////////////////////////////////////////////////////////////////////////
// QConfigSheet

class QConfigSheet : public CPropertySheet {
	DECLARE_DYNAMIC(QConfigSheet)

// Construction
public:
	QConfigSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:
	QConfGen			m_GeneralPage;
	QConfig2DViews		m_2DViewsPage;
	QConfig3DView		m_3DViewPage;
	QConfigGames		m_GamesPage;
	QConfigDraw			m_DrawPage;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(QConfigSheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~QConfigSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(QConfigSheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QCONFSHT_H__1A2006B3_6D67_11D2_BA4C_004005310168__INCLUDED_)
