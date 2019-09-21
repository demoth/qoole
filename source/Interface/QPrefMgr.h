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

#if !defined(AFX_QPREFMGR_H__4345927D_84DD_11D2_BA65_004005310168__INCLUDED_)
#define AFX_QPREFMGR_H__4345927D_84DD_11D2_BA65_004005310168__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// QPrefMgr.h : header file
//

#include "LCommon.h"
#include "QPrefbVw.h"

//================================================================
// QPrefabMgr window
//================================================================

class QPrefabMgr : public CTreeCtrl {
	DECLARE_DYNCREATE(QPrefabMgr)

	// Construction
public:
	QPrefabMgr();

// Attributes
public:
	QPrefabFrame *pPreviewWnd;

// Operations
public:
	bool AddPrefab(const char *filePath);
	void ClosePreviewWnd(void);

	static bool LockPrefabTexture(Object &objNode);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(QPrefabMgr)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~QPrefabMgr();

	const char *GetPrefabDir(void) const { return rootDir; }
	void RefreshView(void);

	// Generated message map functions
protected:
	//{{AFX_MSG(QPrefabMgr)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnFolderExpanding(NMHDR *pnmh, LRESULT *pResult);
	afx_msg void OnFolderCollapsed(NMHDR *pnmh, LRESULT *pResult);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	afx_msg void OnAddPrefab();
	afx_msg void OnRefreshDir();
	afx_msg void OnPreviewPrefab();
	DECLARE_MESSAGE_MAP()

	void AddFolder(const char *rootPath, const char *dirName, HTREEITEM hParent);
	void OpenFolder(const char *dirName);
	void CloseFolder(const char *dirName);
	void RemoveFolder(const char *dirName);

	void FindNodesFullPath(HTREEITEM hItem, char *dirPath);

protected:
	// CTreeCtrl dirTree;
	CImageList imgList;

	// LConfig *cfg;
	char rootDir[256];
	CRect pvWndRect;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QPREFMGR_H__4345927D_84DD_11D2_BA65_004005310168__INCLUDED_)
