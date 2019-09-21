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

#if !defined(AFX_QWSBAR_H__E4A49311_E133_11D1_9F42_0040C757D406__INCLUDED_)
#define AFX_QWSBAR_H__E4A49311_E133_11D1_9F42_0040C757D406__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "QTrView.h"
#include "QTexView.h"
#include "QPrefMgr.h"

//======================================================================
// QTabCtrl
//======================================================================

class QTabCtrl : public CTabCtrl {
// Construction
public:
	QTabCtrl();

// Operations
public:
	void RegisterTabLabels(const char **pLabels) { pTabLabels = pLabels; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(QTabCtrl)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~QTabCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(QTabCtrl)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	// !!!WARNING!!!!
	// strbuf needs to have at least room for strlen + 4 chars.
	void SqueezeStr(int displayWidth, int strLen, LPSTR strBuf);
	void ResizeLabels(int displayWidth);

	CRect displayArea;
	const char **pTabLabels;

	DECLARE_MESSAGE_MAP()
};

//======================================================================
// QWorkSpaceBar window
//======================================================================

#define TAB_TREEVIEW		0
#define TAB_TEXTUREVIEW		1
#define TAB_PREFABVIEW		2

class QWorkSpaceBar : public CMRCSizeControlBar {
public:
	QWorkSpaceBar();
	virtual ~QWorkSpaceBar();

	QTreeView *GetTreeView(void) { return pTreeView; };
	QTexView *GetTextureView(void) { return pTextureView; };
	QPrefabMgr *GetPrefabView(void) { return pPrefabView; };

	CWnd *GetTabWindow(int nTab = -1);
    void ShowSelTabWindow(int nSel = -1);

	// CTabCtrl m_TabCtrl;
	QTabCtrl m_TabCtrl;
	CImageList m_ImgList;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(QWorkSpaceBar)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

protected:
	QTreeView *pTreeView;
	QTexView *pTextureView;
	QPrefabMgr *pPrefabView;
	
	// Generated message map functions
protected:
	//{{AFX_MSG(QWorkSpaceBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnTabChanged(NMHDR * pNotifyStruct, LRESULT* result);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual void OnSizedOrDocked(int cx, int cy, BOOL bFloating, int flags);
};

//======================================================================
// QOutputBar Window.
//======================================================================

#include "QProcWnd.h"

class QOutputBar : public CMRCSizeControlBar {
public:
	QOutputBar();
	virtual ~QOutputBar();

	QProcessWnd	*GetOutputWnd(void) { return pOutputWnd; };

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(QOutputBar)
	//}}AFX_VIRTUAL

protected:
	virtual void OnSizedOrDocked(int cx, int cy, BOOL bFloating, int flags);

	QProcessWnd *pOutputWnd;

	// Generated message map functions
protected:
	//{{AFX_MSG(QOutputBar)
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QWSBAR_H__E4A49311_E133_11D1_9F42_0040C757D406__INCLUDED_)
