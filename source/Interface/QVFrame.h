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

#if !defined(AFX_QVFRAME_H__B0125AC1_176E_11D2_8E80_004005310168__INCLUDED_)
#define AFX_QVFRAME_H__B0125AC1_176E_11D2_8E80_004005310168__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// QVFrame.h : header file
//

//======================================================================
// QVFrame
//======================================================================

class QVFrame : public CMDIChildWnd {
	DECLARE_DYNCREATE(QVFrame)
public:
	QVFrame();
	virtual ~QVFrame();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(QVFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL


// Generated message map functions
protected:
	//{{AFX_MSG(QVFrame)
	afx_msg void OnClose();
	//}}AFX_MSG

	// 
	DECLARE_MESSAGE_MAP()

	// Register the deletion with desktop.
	afx_msg void OnDestroy();

private:

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QVFRAME_H__B0125AC1_176E_11D2_8E80_004005310168__INCLUDED_)
