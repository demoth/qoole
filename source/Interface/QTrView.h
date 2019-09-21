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

#if !defined(AFX_QTRVIEW_H__3EC405E6_FE45_11D1_8E6C_004005310168__INCLUDED_)
#define AFX_QTRVIEW_H__3EC405E6_FE45_11D1_8E6C_004005310168__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// QTrView.h : header file
//

#include "QooleDoc.h"

/////////////////////////////////////////////////////////////////////////////
// QTrView window

#define DUTV_RENAMEOBJ		0x1000

class QTreeView : public CTreeView {

	DECLARE_DYNCREATE(QTreeView)

protected:
	QTreeView();           // protected constructor used by dynamic creation
	virtual ~QTreeView();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(QTreeView)
	public:
	virtual BOOL Create(CWnd* pParentWnd, RECT &rect, UINT nID);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(QTreeView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnEditLabel(void);
	afx_msg void OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemExpanding(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginDrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginRDrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocus(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetfocus(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSortListChange(UINT nID);
	//}}AFX_MSG
	void OnHandleWSBarCmds();
	DECLARE_MESSAGE_MAP()

//====================
// Qoole Specific
//====================

public: 
	void AddDocument(QooleDoc *newDoc);
	void RemoveDocument(QooleDoc *delDoc);
	void SetCurrentDocument(QooleDoc *currentDoc);

protected:
	void DeleteAllChildren(HTREEITEM tNode);
	void ExpandSubTree(HTREEITEM tNode);
	void CollapseSubTree(HTREEITEM tNode);
	void SortSubTree(HTREEITEM tNode);
	HTREEITEM ExpandObjectTree(Object *pObj);
	void RefreshSubTree(HTREEITEM tNode);
	HTREEITEM AddItemNode(HTREEITEM parentNode, Object *objPtr);

	BOOL SelectItems(HTREEITEM hFromItem, HTREEITEM hToItem);
	void ClearSelection(BOOL bMultiOnly = FALSE);
	void SelectMultiple( HTREEITEM hClickedItem, UINT nFlags );

	void DragStartHelper(LRESULT *pResult, POINT dragPt);
	void DragMoveHelper(UINT nFlags, CPoint point);
	void DragEndHelper(CPoint point);
	void OnDragMoveItem(void);
	void OnDragCopyItem(void);
	void OnDragCancel(void);
	HTREEITEM FindDropTarget(HTREEITEM hItem) const;

	HTREEITEM FindVisHItem(const Object *pObj) const;
	void UpdateScopeView(Object *pOldScope);
	void UpdateSelection(HTREEITEM hItem);

	static int CALLBACK SortObjsPos(LPARAM lParam1,
		LPARAM lParam2,	LPARAM lParamSort);

	void SetRedraw(BOOL bRedraw);

	CTreeCtrl *pTree;
	CImageList objImgList;
	LinkList<ObjectPtr> slctObjBuf, oldDragSelBuf;
	int slctObjType;
	int listSortType;

	bool		m_bClickPending;
	bool		m_bSelectPending;
	bool		m_bEditLabelPending;
	bool		m_bOnEditLabel;
	bool		m_bRightSelect;
	bool		m_bRClickedPending;
	CPoint		m_ptClick;
	HTREEITEM	m_hClickedItem;
	HTREEITEM	m_hFirstSelectedItem;
	bool		m_bInUpdateView;

	// Drag and Drop stuff.
	CImageList	*m_pDragImage, *m_pItemImage, *m_pNoImage;
    BOOL		m_bLDragging, m_bRDragging;
	HTREEITEM	m_hItemDrag, m_hItemDrop, m_hItemDragExpand;

	// Flick-free redraw.
	int m_redrawcount;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QTRVIEW_H__3EC405E6_FE45_11D1_8E6C_004005310168__INCLUDED_)
