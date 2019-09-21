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

// Qoole.h : main header file for the QOOLE application
//

#if !defined(AFX_QOOLE_H__8B1BCCF2_FCC8_11D1_8E6B_004005310168__INCLUDED_)
#define AFX_QOOLE_H__8B1BCCF2_FCC8_11D1_8E6B_004005310168__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "CreditStatic.h"

//#define _SHAREWARE_DEMO_

/////////////////////////////////////////////////////////////////////////////
// QooleApp:
// See Qoole.cpp for the implementation of this class
//

class QooleApp : public CWinApp
{
public:
	QooleApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(QooleApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(QooleApp)
	afx_msg void OnAppAbout();
	afx_msg void OnAppRegister();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QOOLE_H__8B1BCCF2_FCC8_11D1_8E6B_004005310168__INCLUDED_)
