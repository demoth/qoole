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

#if !defined(AFX_QSETCOLOR_H__DFED8B97_F557_11D2_ACD6_00400543C1CD__INCLUDED_)
#define AFX_QSETCOLOR_H__DFED8B97_F557_11D2_ACD6_00400543C1CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// QColrWnd.h : header file
//

#include "QooleDoc.h"
#include "Objects.h"
#include "QDraw.h"
#include "QView.h"

/////////////////////////////////////////////////////////////////////////////
// QSetColor dialog

class QSetColor : public CDialog, public QooleView
{
// Construction
public:
	QSetColor(CWnd* pParent = NULL);   // standard constructor

	void OnUpdate(LPARAM lHint, Object *pScope);
	void SetColors(void);

// Dialog Data
	//{{AFX_DATA(QSetColor)
	enum { IDD = IDD_COLORCONFIG };
	CStatic	m_Color;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(QSetColor)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(QSetColor)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnClose();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	QDraw *pQDraw;

private:
	void OnRedraw();

	bool inUse;

	int red, grn, blu;
	bool changed;

	Object *pScope;
	Object *pObject;
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QCOLRWND_H__DFED8B97_F557_11D2_ACD6_00400543C1CD__INCLUDED_)
