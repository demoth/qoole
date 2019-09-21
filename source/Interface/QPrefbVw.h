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

#if !defined(AFX_QPREFBVW_H__4C872CCE_BBF9_11D2_BAAB_004005310168__INCLUDED_)
#define AFX_QPREFBVW_H__4C872CCE_BBF9_11D2_BAAB_004005310168__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// QPrefbVw.h : header file
//

#include "View.h"
#include "QDraw.h"

//================================================================
// QPrefabView window

// class QPrefabView : public CFrameWnd {
class QPrefabView : public CWnd {
// Construction
public:
	QPrefabView();

// Attributes
public:

// Operations
public:
	bool LoadPrefab(const char *pathName);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(QPrefabView)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~QPrefabView();

	void UpdateWnd(void);

protected:
	void CreateQDraw(void);
	void ReCenterMousePos(CPoint &ptMousePos);
	void OnEyeRotate(UINT nFlags, CPoint point);
	void OnEyeZoom(UINT nFlags, CPoint point);

	Object *pRootObj;
	Selector *pSlctr;
	View *pView;
	QDraw *pQDraw;

	int width, height;
	bool mouseLClick, mouseRClick, mouseLDrag, mouseRDrag;
	CPoint	ptClick, lastMPos;

	// Generated message map functions
protected:
	//{{AFX_MSG(QPrefabView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// QPrefbFrame window

class QPrefabFrame : public CControlBar {

// Construction
public:
	QPrefabFrame();

// Attributes
public:
	QPrefabView prefabView;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(QPrefabFrame)
	//}}AFX_VIRTUAL
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	virtual CSize CalcDynamicLayout(int nLength, DWORD dwMode);

// Implementation
public:
	virtual ~QPrefabFrame();

	// Generated message map functions
protected:
	//{{AFX_MSG(QPrefabFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNcDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QPREFBVW_H__4C872CCE_BBF9_11D2_BAAB_004005310168__INCLUDED_)
