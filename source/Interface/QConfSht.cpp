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

// QConfSht.cpp : implementation file
//

#include "stdafx.h"
#include "Qoole.h"
#include "QConfSht.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//======================================================================
// QConfigSheet

IMPLEMENT_DYNAMIC(QConfigSheet, CPropertySheet)

QConfigSheet::QConfigSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
			 :CPropertySheet(pszCaption, pParentWnd, iSelectPage) {

	// Add Pages.
	AddPage(&m_GeneralPage);
	AddPage(&m_2DViewsPage);
	AddPage(&m_3DViewPage);
	AddPage(&m_DrawPage);
	AddPage(&m_GamesPage);
}

QConfigSheet::~QConfigSheet() {
}


BEGIN_MESSAGE_MAP(QConfigSheet, CPropertySheet)
	//{{AFX_MSG_MAP(QConfigSheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// QConfigSheet message handlers
