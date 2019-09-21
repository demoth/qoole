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

// QTrView.cpp : implementation file
//

#include "stdafx.h"
#include "Qoole.h"
#include "QTrView.h"
#include "QMainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// QTreeView

#define TVICON_NUM			9

#define TVICON_LEV			0
#define TVICON_QLE			1
#define TVICON_GROUP		2
#define TVICON_GROUPENT		3
#define TVICON_BRUSH		4
#define TVICON_BRUSHENT		5
#define TVICON_ENT			6
#define TVICON_DUMMY		7
#define TVICON_NO			8


IMPLEMENT_DYNCREATE(QTreeView, CTreeView)

// Def's taken from mrc's resource.h
#define ID_MRC_ALLOWDOCKING             2
#define ID_MRC_HIDE                     3
#define ID_MRC_MDIFLOAT                 4

BEGIN_MESSAGE_MAP(QTreeView, CTreeView)
	//{{AFX_MSG_MAP(QTreeView)
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
	ON_WM_CONTEXTMENU()
	ON_NOTIFY_REFLECT(NM_CLICK, OnClick)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRClick)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, OnItemExpanding)
	ON_NOTIFY_REFLECT(TVN_BEGINLABELEDIT, OnBeginLabelEdit)
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, OnEndLabelEdit)
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBeginDrag)
	ON_NOTIFY_REFLECT(TVN_BEGINRDRAG, OnBeginRDrag)
	ON_NOTIFY_REFLECT(NM_KILLFOCUS, OnKillfocus)
	ON_NOTIFY_REFLECT(NM_SETFOCUS, OnSetfocus)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_RCMTV_EDITLABEL, OnEditLabel)
	ON_COMMAND_RANGE(ID_RCMTV_SORTNAME, ID_RCMTV_SORTZPOS, OnSortListChange)
	ON_COMMAND(ID_MRC_HIDE, OnHandleWSBarCmds)
	ON_COMMAND(ID_MRC_ALLOWDOCKING, OnHandleWSBarCmds)
	ON_COMMAND(ID_RCMTV_DRAGMOVE, OnDragMoveItem)
	ON_COMMAND(ID_RCMTV_DRAGCOPY, OnDragCopyItem)
	ON_COMMAND(ID_RCMTV_DRAGCANCEL, OnDragCancel)
END_MESSAGE_MAP()

//==============================
// QTreeView Init.
//==============================

QTreeView::QTreeView() {
	pTree = NULL;
	m_bClickPending = false;
	m_bSelectPending = false;
	m_bEditLabelPending = false;
	m_bOnEditLabel = false;
	m_bInUpdateView = false;
	m_bRightSelect = false;
	m_bRClickedPending = false;
	m_hClickedItem = NULL;
	m_hFirstSelectedItem = NULL;
	slctObjType	= 0;
	listSortType = 0;
	m_bLDragging = FALSE;
	m_bRDragging = FALSE;
	m_pDragImage = NULL;
	m_pItemImage = NULL;
	m_pNoImage = NULL;
	m_hItemDrag = NULL;
	m_hItemDrop = NULL;
	m_hItemDragExpand = NULL;
	m_redrawcount = 0;
}

QTreeView::~QTreeView() {
	// Cleanup
	while (slctObjBuf.NumOfElm() > 0)
		delete &(slctObjBuf.RemoveNode(0));

	// Drag and drop stuff.
	ASSERT(oldDragSelBuf.NumOfElm() == 0);
	ASSERT(m_pDragImage == NULL);
	ASSERT(m_pItemImage == NULL);
	ASSERT(m_pNoImage != NULL);
	delete m_pNoImage;
}

BOOL QTreeView::PreCreateWindow(CREATESTRUCT& cs) {
	if (!CTreeView::PreCreateWindow(cs))
		return FALSE;

/*
	// Register the new window class.
	WNDCLASS wndclass;
	HINSTANCE hInst = AfxGetInstanceHandle();

	if (!(::GetClassInfo(hInst, "QTreeView", &wndclass))) {
		if (::GetClassInfo(hInst, cs.lpszClass, &wndclass)) {
			wndclass.lpszClassName = "QTreeView";
			wndclass.hbrBackground = NULL;
			if (!AfxRegisterClass(&wndclass))
				AfxThrowResourceException();
		}
		else
			AfxThrowResourceException();
	}
	cs.lpszClass = "QTreeView";
*/
	cs.style |= TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;
	cs.style |= TVS_EDITLABELS | TVS_SHOWSELALWAYS;

	return TRUE; 
}

BOOL QTreeView::Create(CWnd* pParentWnd, RECT &rect, UINT nID) {

	if (!CWnd::Create(NULL, m_strClass, m_dwDefaultStyle | WS_VISIBLE,
					  rect, pParentWnd, nID, NULL)) {
		return FALSE;
	}	

	return TRUE;
}

int QTreeView::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	if (CTreeView::OnCreate(lpCreateStruct) == -1)
		return -1;

	pTree = &GetTreeCtrl();
	ASSERT(pTree != NULL);
	ASSERT(pTree->GetCount() == 0);
	
	// Set the image list.
	objImgList.Create(IDB_TVIEWICO, 16, 1, RGB(192, 192, 192));
	pTree->SetImageList(&objImgList, TVSIL_NORMAL);

	// Create an 'No' image for dragging.
	HTREEITEM hItem = pTree->InsertItem("", TVICON_NO, TVICON_NO);
	m_pNoImage = pTree->CreateDragImage(hItem);
	pTree->DeleteItem(hItem);

	return 0;
}

void QTreeView::AddDocument(QooleDoc *newDoc) {
	// Make the root node.
	Object *rootObj = newDoc->GetRootObjectPtr();
	ASSERT(rootObj != NULL);

	// Decide which icon to use for this document.
	UINT iFlag = TVICON_QLE;
	if (rootObj->HasEntity() && rootObj->GetEntityPtr()->IsWorldSpawn())
		iFlag = TVICON_LEV;

	// Make the root item.
	HTREEITEM tNode = pTree->InsertItem(rootObj->GetObjName(), iFlag, iFlag);
	ASSERT(tNode != NULL);
	pTree->SetItemData(tNode, (DWORD) rootObj);
	pTree->SetItemText(tNode, newDoc->GetTitle());

	// Add a dummy node;
	pTree->InsertItem("", TVICON_DUMMY, TVICON_DUMMY, tNode);

	// Switch to the new document
	SetCurrentDocument(newDoc);
}

void QTreeView::RemoveDocument(QooleDoc *delDoc) {
	ASSERT(delDoc != NULL);

	CDocument *currentDoc = GetDocument();
	if (currentDoc == delDoc)
		SetCurrentDocument(NULL);

	// Find delDoc's sub tree.
	HTREEITEM delItem;
	DWORD rootObj = (DWORD) delDoc->GetRootObjectPtr();
	for (delItem = pTree->GetNextItem(NULL, TVGN_CHILD);
		 delItem != NULL && pTree->GetItemData(delItem) != rootObj;
		 delItem = pTree->GetNextItem(delItem, TVGN_NEXTVISIBLE));
	ASSERT(delItem != NULL);

	pTree->DeleteItem(delItem);
}

void QTreeView::SetCurrentDocument(QooleDoc *currentDoc) {
	QooleDoc *oldDoc = (QooleDoc *) GetDocument();

	if (currentDoc == oldDoc)
		return;

	HTREEITEM hItem;

	// Remove view from the old document.
	if (oldDoc != NULL) {
		oldDoc->RemoveView(this);
		ASSERT(m_pDocument == NULL);

		// Collapse the old document folder.
		hItem = FindVisHItem(oldDoc->GetRootObjectPtr());
		if (hItem) {
			// Possible problem with TVE_COLLAPSE
			CollapseSubTree(hItem);
			pTree->Expand(hItem, TVE_COLLAPSE);
		}

		// De-hilight the document node.
		pTree->SelectItem(NULL);
		pTree->SetItemState(hItem, 0, TVIS_SELECTED);
	}

	// Attach self to the new document.
	if (currentDoc != NULL) {
		currentDoc->AddView(this);
		ASSERT(m_pDocument != NULL);

		// Expand the new root level, if not already so.
		hItem = FindVisHItem(currentDoc->GetRootObjectPtr());
		if (hItem && 
			!(pTree->GetItemState(hItem, TVIS_EXPANDED) & TVIS_EXPANDED)) {
			ExpandSubTree(hItem);
			pTree->Expand(hItem, TVE_EXPAND);
		}

		// Hi-light the document node.
		pTree->SelectItem(hItem);
		pTree->SetItemState(hItem, TVIS_SELECTED, TVIS_SELECTED);
	}
}

//====================
// Tree Operations
//====================

void QTreeView::DeleteAllChildren(HTREEITEM tNode) {
	ASSERT(tNode);

	// Delete any children of tNode.
	HTREEITEM hNext;
	HTREEITEM hItem = pTree->GetChildItem(tNode);
	while (hItem != NULL) {
		hNext = pTree->GetNextSiblingItem(hItem);
		if (pTree->ItemHasChildren(hItem))
			DeleteAllChildren(hItem);
		pTree->DeleteItem(hItem);
		hItem = hNext;
	}
}

// Note: Do not use ExpandSubTree() to refresh
//  the contents of an expanded folder, especially
//  if the folder contains many sub items.

void QTreeView::ExpandSubTree(HTREEITEM tNode) {
	ASSERT(tNode);

	// Delete children of tNode.
	DeleteAllChildren(tNode);

	// Add the children nodes.
	Object *objPtr = (Object *) pTree->GetItemData(tNode);
	ASSERT(objPtr != NULL);

	// Iterate the children.
	IterLinkList<Object> *iterPtr = &(objPtr->GetIterChildren());
	iterPtr->Reset();
	while (!iterPtr->IsDone()) {
		objPtr = iterPtr->GetNext();
		AddItemNode(tNode, objPtr);
	}

	SortSubTree(tNode);
}

void QTreeView::CollapseSubTree(HTREEITEM tNode) {
	ASSERT(tNode);

	// Make sure tNode is valid and expanded.
	// ASSERT(pTree->GetItemState(tNode, TVIS_EXPANDED));
	if (!pTree->GetItemState(tNode, TVIS_EXPANDED))
		return;

	// Delete children of tNode.
	DeleteAllChildren(tNode);

	// Get Object.
	Object *objPtr = (Object *) pTree->GetItemData(tNode);
	ASSERT(objPtr != NULL);
	if (objPtr->GetNumChildren() > 0) {
		// Add a dummy node for the + line.
		pTree->InsertItem("", TVICON_DUMMY, TVICON_DUMMY, tNode);
	}
}

HTREEITEM QTreeView::ExpandObjectTree(Object *pObj) {
	HTREEITEM hItem;

	if (!pObj->IsRoot())
		hItem = ExpandObjectTree(pObj->GetParentPtr());
	else
		hItem = TVGN_ROOT;

	for(hItem = pTree->GetChildItem(hItem);
		hItem != NULL && (Object *) pTree->GetItemData(hItem) != pObj;
		hItem = pTree->GetNextSiblingItem(hItem));

	ASSERT(hItem);

	if (!(pTree->GetItemState(hItem, TVIS_EXPANDED) & TVIS_EXPANDED)) {
		ExpandSubTree(hItem);
		pTree->Expand(hItem, TVE_EXPAND);
	}

	return hItem;
}

void QTreeView::SortSubTree(HTREEITEM tNode) {
	HTREEITEM hItem;

	for(hItem = pTree->GetChildItem(tNode);
		hItem != NULL;
		hItem = pTree->GetNextSiblingItem(hItem)) {
		if (pTree->GetItemState(hItem, TVIS_EXPANDED))
			SortSubTree(hItem);
	}

	if (listSortType == 1) {
		pTree->SortChildren(tNode);
	}
	else if (listSortType > 1 && listSortType < 5) {
		TV_SORTCB sortInfo;
		sortInfo.hParent = tNode;
		sortInfo.lpfnCompare = SortObjsPos;
		sortInfo.lParam = listSortType;
		pTree->SortChildrenCB(&sortInfo);
	}
}

int CALLBACK QTreeView::SortObjsPos(LPARAM lParam1, LPARAM lParam2,
								  LPARAM lParamSort) {
	float val1, val2;

	if (lParamSort == 2) {
		val1 = ((Object *) lParam1)->GetPosition().GetX();
		val2 = ((Object *) lParam2)->GetPosition().GetX();
	}
	else if (lParamSort == 3) {
		val1 = ((Object *) lParam1)->GetPosition().GetY();
		val2 = ((Object *) lParam2)->GetPosition().GetY();
	}
	else if (lParamSort == 4) {
		val1 = ((Object *) lParam1)->GetPosition().GetZ();
		val2 = ((Object *) lParam2)->GetPosition().GetZ();
	}
	else {
		val1 = 0.0f;  val2 = 1.0f;
	}

	if(val1 == val2)
		return 0;
	else
		return (val1 < val2 ? 1 : -1);
}

void QTreeView::RefreshSubTree(HTREEITEM tNode) {
	ASSERT(tNode);

	// Prepare the compare list.
	Object *pObj = (Object *) pTree->GetItemData(tNode);
	LinkList<ObjectPtr> objPtrList;

	IterLinkList<Object> *iterPtr = &(pObj->GetIterChildren());
	iterPtr->Reset();
	while (!iterPtr->IsDone()) {
		pObj = iterPtr->GetNext();
		objPtrList.AppendNode(*(new ObjectPtr(pObj)));
	}

	ObjectPtr *pObjPtr;
	IterLinkList<ObjectPtr> iterPtrList(objPtrList);

	// Iterate through the tree nodes and 
	//  eliminate any non-existing items.
	HTREEITEM hItem = pTree->GetChildItem(tNode);
	while (hItem != NULL) {
		pObj = (Object *) pTree->GetItemData(hItem);

		// Iter through compare list and check for match.
		bool bMatch = false;
		iterPtrList.Reset();
		while (!iterPtrList.IsDone()) {
			pObjPtr = iterPtrList.GetNext();
			if (pObjPtr->GetPtr() == pObj) {
				bMatch = true;
				// Remove it from compare list.
				objPtrList.RemoveNode(*pObjPtr);
				delete pObjPtr;
				break;
			}
		}

		// Get the next item first.
		HTREEITEM hNextItem = pTree->GetNextSiblingItem(hItem);

		if (!bMatch) {	// No match.
			// Item in tree is old.  Remove it.
			pTree->DeleteItem(hItem);
		}

		hItem = hNextItem;
	}

	// Add any left over objects to the tree view list.
	while (objPtrList.NumOfElm() > 0) {
		pObjPtr = &(objPtrList.RemoveNode(0));
		AddItemNode(tNode, pObjPtr->GetPtr());
		delete pObjPtr;
	}

	// Sort it for display.
	SortSubTree(tNode);
}

HTREEITEM QTreeView::AddItemNode(HTREEITEM parentNode, Object *objPtr) {
	HTREEITEM rtnVal;

	ASSERT(parentNode);
	ASSERT(objPtr != NULL);

	// Find the right icon to represent the object node.
	int image;
	if (objPtr->GetNumChildren() > 0)
		image = (!objPtr->IsModelNode() ? TVICON_GROUP : TVICON_GROUPENT);
	else if (objPtr->HasBrush())
		image = (!objPtr->IsModelNode() ? TVICON_BRUSH : TVICON_BRUSHENT);
	else if (objPtr->IsItemNode())
		image = TVICON_ENT;
	else
		image = TVICON_DUMMY;

	// Insert the tree node.
	rtnVal = pTree->InsertItem(objPtr->GetObjName(), image, image, parentNode);
	pTree->SetItemData(rtnVal, (DWORD) objPtr);

	// Add a dummy child node to create the + line if obj has children.
	if (objPtr->GetNumChildren() > 0)
		pTree->InsertItem("", TVICON_DUMMY, TVICON_DUMMY, rtnVal);

	return rtnVal;
}

//========================================
// QTreeView message handlers
//========================================

//===== MultiSelection =====

void QTreeView::OnLButtonDown(UINT nFlags, CPoint point) {
	// Disable left clicks during right dragging.
	if (m_bRDragging)
	 	return;

	UINT nHitFlags = 0;
	HTREEITEM hClickedItem = pTree->HitTest(point, &nHitFlags);

	if (!(nHitFlags & LVHT_ONITEM) || m_bOnEditLabel) {
		// Default behavior.
		CTreeView::OnLButtonDown(nFlags, point);
		return;
	}

	SetFocus();
	m_hClickedItem = hClickedItem;
	m_bClickPending = true;
	m_ptClick = point;

	// Must invoke label editing explicitly. The base class
	//  OnLButtonDown would normally do this, but we can't
	//  call it here because of the multiple selection...
	if ((nHitFlags & LVHT_ONITEMLABEL) && !(nFlags & MK_CONTROL)) {
		if (hClickedItem == pTree->GetSelectedItem()) {
			m_bEditLabelPending = true;
			return;
		}
	}

	// Is the clicked item already selected ?
	BOOL bIsClickedItemSelected =
		pTree->GetItemState(hClickedItem, TVIS_SELECTED) & TVIS_SELECTED;

	if (bIsClickedItemSelected)	{
		// Maybe user wants to drag/drop multiple items!
		// So, wait until OnLButtonUp() to do the selection stuff. 
		m_bSelectPending = true;
	}
	else {
		SelectMultiple(hClickedItem, nFlags);
		m_bSelectPending = false;
	}
}

void QTreeView::OnLButtonDblClk(UINT nFlags, CPoint point) {
	m_bClickPending = false;
	CTreeView::OnLButtonDblClk(nFlags, point);
}

void QTreeView::OnLButtonUp(UINT nFlags, CPoint point) {

	if (m_bSelectPending) {
		// A select has been waiting to be performed here
		SelectMultiple(m_hClickedItem, nFlags);
		m_bSelectPending = false;
	}

	if (m_bClickPending && !m_bEditLabelPending) {
		// Send self a notify message that a button was single clicked.
		NMHDR nm;
		nm.hwndFrom = pTree->GetSafeHwnd();
		nm.idFrom = GetWindowLong(pTree->GetSafeHwnd(), GWL_ID);
		nm.code = NM_CLICK;
		SendMessage(WM_NOTIFY, nm.idFrom, (LPARAM) &nm);
		m_bClickPending = false;
	}

	if (m_bEditLabelPending) {
		pTree->EditLabel(pTree->GetSelectedItem());
		m_bEditLabelPending = false;
	}

	m_hClickedItem = NULL;

	CTreeView::OnLButtonUp(nFlags, point);

	if (m_bLDragging) {
		DragEndHelper(point);
		m_bLDragging = FALSE;
	}
}

void QTreeView::OnRButtonDown(UINT nFlags, CPoint point) {
	// Disable right clicks during left dragging.
	if (m_bLDragging)
		return;

	UINT nHitFlags = 0;
	HTREEITEM hClickedItem = pTree->HitTest(point, &nHitFlags);

	SetFocus();
	m_ptClick = point;
	m_bRClickedPending = true;
	m_hClickedItem = hClickedItem;

	if ((nHitFlags & LVHT_ONITEM) &&
		(!pTree->GetItemState(hClickedItem, TVIS_SELECTED))) {
		// Select it.
		m_bRightSelect = true;
		pTree->SelectItem(NULL);
		SelectMultiple(hClickedItem, 0);
	}
}

void QTreeView::OnRButtonUp(UINT nFlags, CPoint point) {

	if (m_bRClickedPending) {
		// Send self a notification message.
		NMHDR nm;
		nm.hwndFrom = pTree->GetSafeHwnd();
		nm.idFrom = GetWindowLong(pTree->GetSafeHwnd(), GWL_ID);
		nm.code = NM_RCLICK;
		SendMessage(WM_NOTIFY, nm.idFrom, (LPARAM) &nm);
		// m_bRClickedPending = false;
	}

	m_hClickedItem = NULL;

	CTreeView::OnRButtonUp(nFlags, point);

	if (m_bRDragging) {
		DragEndHelper(point);
		m_bRDragging = FALSE;
	}
}

void QTreeView::OnMouseMove(UINT nFlags, CPoint point) {
	// If there is a select pending, check if cursor has moved so much away from the 
	// down-click point that we should cancel the pending select and initiate
	// a drag/drop operation instead!

	if (m_hClickedItem) {
		CSize sizeMoved = m_ptClick - point;

		if (abs(sizeMoved.cx) > GetSystemMetrics(SM_CXDRAG) ||
			abs(sizeMoved.cy) > GetSystemMetrics(SM_CYDRAG)) {
			m_bClickPending = false;
			m_bSelectPending = false;
			m_bEditLabelPending = false;
			m_bRightSelect = false;
			m_bRClickedPending = false;

			// Notify parent that he may begin drag operation
			// Since we have taken over OnLButtonDown(), the default handler doesn't
			// do the normal work when clicking an item, so we must provide our own
			// TVN_BEGINDRAG notification for the parent!

			NM_TREEVIEW tv;
			tv.hdr.hwndFrom = pTree->GetSafeHwnd();
			tv.hdr.idFrom = GetWindowLong(pTree->GetSafeHwnd(), GWL_ID);
			tv.hdr.code = (nFlags & MK_LBUTTON ? TVN_BEGINDRAG : TVN_BEGINRDRAG);
			tv.itemNew.hItem = m_hClickedItem;
			tv.itemNew.state = pTree->GetItemState(m_hClickedItem, 0xffffffff);
			tv.itemNew.lParam = pTree->GetItemData(m_hClickedItem);
			tv.ptDrag.x = point.x;
			tv.ptDrag.y = point.y;
			SendMessage(WM_NOTIFY, tv.hdr.idFrom, (LPARAM) &tv);
			m_hClickedItem = NULL;
		}
	}

	if (m_bLDragging || m_bRDragging) {
		DragMoveHelper(nFlags, point);
	}

	CTreeView::OnMouseMove(nFlags, point);
}

void QTreeView::SelectMultiple(HTREEITEM hClickedItem, UINT nFlags) {
	Object *pObj = (Object *) pTree->GetItemData(hClickedItem);

	// Action depends on whether the user holds down the Shift or Ctrl key
	if (nFlags & MK_SHIFT) {
		// Do nothing if user shift-clicked the root node.
		if (pTree->GetParentItem(hClickedItem) == NULL)
			return;

		// Select from first selected item to the clicked item
		if (!m_hFirstSelectedItem)
			m_hFirstSelectedItem = pTree->GetSelectedItem();

		SelectItems(m_hFirstSelectedItem, hClickedItem);
	}
	else if (nFlags & MK_CONTROL) {
		// Do nothing if user control-clicked the root node.
		if (pTree->GetParentItem(hClickedItem) == NULL)
			return;

		// Find which item is currently selected
		HTREEITEM hSelectedItem = pTree->GetSelectedItem();

		// Is the clicked item already selected ?
		BOOL bIsClickedItemSelected =
			pTree->GetItemState(hClickedItem, TVIS_SELECTED) & TVIS_SELECTED;
		BOOL bIsSelectedItemSelected = (hSelectedItem == NULL ? FALSE :
			pTree->GetItemState(hSelectedItem, TVIS_SELECTED) & TVIS_SELECTED);

		// Select the clicked item (this will also deselect the previous one!)
		pTree->SelectItem(hClickedItem);

		// If the previously selected item was selected, re-select it
		if (hSelectedItem && bIsSelectedItemSelected)
			pTree->SetItemState(hSelectedItem, TVIS_SELECTED, TVIS_SELECTED);

		// We want the newly selected item to toggle its selected state,
		// so unselect now if it was already selected before
		if (bIsClickedItemSelected) {
			pTree->SetItemState(hClickedItem, 0, TVIS_SELECTED);
			// pTree->SelectItem(NULL);
		}
		else {
			pTree->SetItemState(hClickedItem, TVIS_SELECTED, TVIS_SELECTED);
		}

		// Store as first selected item (if not already stored)
		if (m_hFirstSelectedItem == NULL)
			m_hFirstSelectedItem = hClickedItem;

		// Remember selected obj.
		slctObjType	= 1;
		slctObjBuf.AppendNode(*(new ObjectPtr(pObj)));
	}
	else {
		// Clear selection of all "multiple selected" items first
		ClearSelection();

		// Then select the clicked item
		pTree->SelectItem(hClickedItem);
		pTree->SetItemState(hClickedItem, TVIS_SELECTED, TVIS_SELECTED);

		// Store as first selected item
		m_hFirstSelectedItem = hClickedItem;
		
		while (slctObjBuf.NumOfElm() > 0)
			delete &(slctObjBuf.RemoveNode(0));

		Selector *pSlct = &(pQMainFrame->GetSelector());
		Object *pObj = (Object *) pTree->GetItemData(hClickedItem);
		if (pSlct->GetNumMSelectedObjects() != 1 ||
			(pSlct->GetMSelectedObjects())[0].GetPtr() != pObj) {
			if (pTree->GetParentItem(hClickedItem) == NULL) {
				// Clear selection when root node is selected.
				slctObjType = 2;
			}
			else {
				// Remember selected obj.
				slctObjType	= 0;
				slctObjBuf.AppendNode(*(new ObjectPtr(pObj)));
			}
		}
	}
}

void QTreeView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {

	if (nChar == VK_UP || nChar == VK_DOWN ||
		nChar == VK_LEFT || nChar == VK_RIGHT) {
		if (!(GetKeyState(VK_CONTROL) & 0x8000))
			return;
	}
	CTreeView::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL QTreeView::SelectItems(HTREEITEM hFromItem, HTREEITEM hToItem) {
	// Determine direction of selection 
	// (see what item comes first in the tree)
	HTREEITEM hItem = pTree->GetRootItem();
	HTREEITEM hNewParent = pTree->GetParentItem(hToItem);

	while (hItem && hItem != hFromItem && hItem != hToItem)
		hItem = pTree->GetNextVisibleItem(hItem);

	if (!hItem)
		return FALSE;	// Items not visible in tree

	BOOL bReverse = (hItem == hToItem);

	// "Really" select the 'to' item (which will deselect 
	// the previously selected item)
	pTree->SelectItem(hToItem);

	// Go through all visible items again and select/unselect
	hItem = pTree->GetRootItem();
	BOOL bSelect = FALSE;

	// Clear the slct obj buf.
	Object *pObj;
	slctObjType = 0;
	while (slctObjBuf.NumOfElm() > 0)
		delete &(slctObjBuf.RemoveNode(0));

	while (hItem) {
		if (hItem == (bReverse ? hToItem : hFromItem))
			bSelect = TRUE;

		if (bSelect && pTree->GetParentItem(hItem) == hNewParent) {
			if (!(pTree->GetItemState(hItem, TVIS_SELECTED) & TVIS_SELECTED))
				pTree->SetItemState(hItem, TVIS_SELECTED, TVIS_SELECTED);
			pObj = (Object *) pTree->GetItemData(hItem);
			slctObjBuf.AppendNode(*(new ObjectPtr(pObj)));
		}
		else {
			if (pTree->GetItemState(hItem, TVIS_SELECTED) & TVIS_SELECTED)
				pTree->SetItemState(hItem, 0, TVIS_SELECTED);
		}

		if (hItem == (bReverse ? hFromItem : hToItem))
			bSelect = FALSE;

		hItem = pTree->GetNextVisibleItem(hItem);
	}

	return TRUE;
}

void QTreeView::ClearSelection(BOOL bMultiOnly) {
	if (!bMultiOnly)
		pTree->SelectItem(NULL);

	for(HTREEITEM hItem = pTree->GetRootItem();
		hItem != NULL;
		hItem = pTree->GetNextVisibleItem(hItem)) {
		if (pTree->GetItemState(hItem, TVIS_SELECTED) & TVIS_SELECTED)
			pTree->SetItemState(hItem, 0, TVIS_SELECTED);
	}
}

void QTreeView::OnKillfocus(NMHDR* pNMHDR, LRESULT* pResult) {
	pTree->Invalidate();
	*pResult = 0;
}

void QTreeView::OnSetfocus(NMHDR* pNMHDR, LRESULT* pResult) {
	pTree->Invalidate();
	*pResult = 0;
}

//===== Left Mouse Click: Selection and Changing Scopes =====

void QTreeView::OnClick(NMHDR* pNMHDR, LRESULT* pResult) {
	*pResult = 0;

	if (!m_hClickedItem)
		return;

	// Select object, maybe switch document, and change scope.
	Object *pObj = (Object *) pTree->GetItemData(m_hClickedItem);
	Object *pScope = &(pObj->GetRoot());
 	Object *pRootObj = ((QooleDoc *) GetDocument())->GetRootObjectPtr();
	Operation *op = NULL;

	if (pScope != pRootObj) {
		// Need to switch the current desktop document.
		QooleDoc *pNewDoc =
			((QDocTemplate *) GetDocument()->GetDocTemplate())->
			GetObjsDocPtr(pScope);
		ASSERT(pNewDoc != NULL);
		pQMainFrame->SetDeskTopDocument(pNewDoc);
	}
	
	pScope = (pObj->IsRoot() ? pObj : pObj->GetParentPtr());
	if (pObj->IsRoot()) {
		// Just selected the file.  Deselect everything.
		while (slctObjBuf.NumOfElm() > 0)
			delete &(slctObjBuf.RemoveNode(0));
	}

	if (pScope != &(pQMainFrame->GetScope())) {
		// Need to switch the editing scope.
		for(int i = 0; i < slctObjBuf.NumOfElm(); i++) {
			if (slctObjBuf[i].GetPtr()->GetParentPtr() != pScope)
				delete &(slctObjBuf.RemoveNode(i--));
		}
		op = new OpScopeChange(pScope, slctObjBuf);
	}
	else if (slctObjBuf.NumOfElm() > 0) {
		if (slctObjType == 0)  // Select new objs.
			op = new OpSelectNewObjs(slctObjBuf);
		else if (slctObjType == 1) // Select the addional new objs.
			op = new OpSelectAddObjs(slctObjBuf);
	}
	else if (slctObjType == 2) // clear the current sel objs.
		op = new OpSelectNewObjs(slctObjBuf);

	if (op)
		pQMainFrame->CommitOperation(*op);
}

void QTreeView::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) {
	// Scope into or out of.
	
	*pResult = 0;
}

void QTreeView::OnRClick(NMHDR* pNMHDR, LRESULT* pResult) {

	if (m_bRightSelect) {
		m_bRightSelect = false;
		OnClick(pNMHDR, pResult);
	}

	*pResult = 0;
}

//===== Right Click Menu =====

void QTreeView::OnContextMenu(CWnd* pWnd, CPoint point) {
	CMenu rMenu;
	rMenu.CreatePopupMenu();

	if (!m_bRClickedPending) {
		// Right Drag operation.
		// Dealt with in OnRButtonUp();
		return;
	}

	// No movement.
	m_bRClickedPending = false;

	UINT nFlags = 0;
	HTREEITEM hItem = pTree->HitTest(m_ptClick, &nFlags);
	Object *pObj = (hItem ? (Object *) pTree->GetItemData(hItem) : NULL);
	Selector *pSlctr = &(pQMainFrame->GetSelector());

	// Construct the context menu.
	if (!(nFlags & TVHT_ONITEM)) { // No hit on tree items.
		rMenu.AppendMenu(MF_STRING, ID_FILE_NEWQLE , "New Prefab");
		rMenu.AppendMenu(MF_STRING, ID_FILE_NEW, "New Level");
		rMenu.AppendMenu(MF_SEPARATOR);
		CWnd *pQWSBar = GetParent();
		pQWSBar->SendMessage(WM_ADDCONTEXTMENUITEMS, 0, (LPARAM) rMenu.m_hMenu);
	}
	else if (pObj && pObj->IsRoot()) {  // Add the file menu stuff.
		rMenu.AppendMenu(MF_STRING, ID_COMPILE_EXPORTBSP, "Export BSP");
		rMenu.AppendMenu(MF_STRING, ID_FILE_SAVE, "Save File");
		rMenu.AppendMenu(MF_STRING, ID_FILE_CLOSE, "Close File");
		rMenu.AppendMenu(MF_SEPARATOR);
	}
	else {
		if (pSlctr->GetNumMSelectedObjects() == 1) {
			UINT menuID = (!pObj->HasEntity() ?
						   ID_RCMTV_EDITLABEL : ID_RCMTV_EDITLABEL_GRAY);
			rMenu.AppendMenu(MF_STRING, menuID, "Rename");
			rMenu.AppendMenu(MF_SEPARATOR);
		}

		if (pSlctr->GetNumMSelectedObjects() > 0) {
			rMenu.AppendMenu(MF_STRING, ID_EDIT_CUT, "Cut");
			rMenu.AppendMenu(MF_STRING, ID_EDIT_COPY, "Copy");
			rMenu.AppendMenu(MF_STRING, ID_EDIT_DELETE, "Delete");
			rMenu.AppendMenu(MF_SEPARATOR);
		}

		if (pSlctr->GetNumMSelectedObjects() > 1) {
			rMenu.AppendMenu(MF_STRING, ID_OBJECT_GROUPOBJS, "Group Objs");
		}

		if (pSlctr->GetNumMSelectedObjects() == 1 &&
			pObj->GetNumChildren() > 0) {
			rMenu.AppendMenu(MF_STRING, ID_OBJECT_UNGROUPOBJS, "Ungroup Objs");
		}
	}

	if (nFlags & TVHT_ONITEM) {
		CMenu sMenu;
		sMenu.CreatePopupMenu();
		sMenu.AppendMenu(MF_STRING | (listSortType == 1 ? MF_CHECKED : 0),
			ID_RCMTV_SORTNAME, "By Name");
		sMenu.AppendMenu(MF_STRING | (listSortType == 2 ? MF_CHECKED : 0),
			ID_RCMTV_SORTXPOS, "By X Pos");
		sMenu.AppendMenu(MF_STRING | (listSortType == 3 ? MF_CHECKED : 0),
			ID_RCMTV_SORTYPOS, "By Y Pos");
		sMenu.AppendMenu(MF_STRING | (listSortType == 4 ? MF_CHECKED : 0), 
			ID_RCMTV_SORTZPOS, "By Z Pos");
		rMenu.AppendMenu(MF_POPUP, (UINT) sMenu.Detach(), "Sort Listing");
		rMenu.AppendMenu(MF_SEPARATOR);

		rMenu.AppendMenu(MF_STRING, ID_OBJECT_PROPERTIES, "Properties...");
	}
	else {
	}

	UINT nStyle = TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON;
	CPoint pt(m_ptClick);
	ClientToScreen(&pt);

	rMenu.TrackPopupMenu(nStyle, pt.x, pt.y, pQMainFrame);
}

void QTreeView::OnHandleWSBarCmds() {
	// Pass the commands into the workspace's control bar.
	const MSG *pMesg = GetCurrentMessage();

	// CWnd *pWSBar = GetParent()->GetParent();
	CWnd *pWSBar = GetParent();
	pWSBar->SendMessage(WM_COMMAND, pMesg->wParam, NULL);
}

//===== Folder Exanding and Collapsing =====

// NOTE: CTreeCtrl:Expand() doesn't send the parent window
//  TVN_ITEMEXPANDING notify message.  Hence the function
//  won't be called.

void QTreeView::OnItemExpanding(NMHDR* pNMHDR, LRESULT* pResult) {
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	*pResult = 0;

	if (pNMTreeView->action == TVE_EXPAND) {
		ExpandSubTree(hItem);
	}
	else { // pnmtv->action == TVE_COLLAPSE
		// Never allow collapsing of node containing current scope.
		Selector *pSlct = &(pQMainFrame->GetSelector());
		Object *pObj = (Object *) pTree->GetItemData(hItem);
		if (pSlct->GetScopePtr() == pObj ||
			pSlct->GetScopePtr()->IsMyAncestor(*pObj)) {
			*pResult = TRUE;
		}
		else
			CollapseSubTree(hItem);
	}
}

//===== Editing Labels =====

BOOL QTreeView::PreTranslateMessage(MSG* pMsg) {

	if (pMsg->message == WM_KEYDOWN) {
		// When an item is being edited make sure the edit control
		// receives certain important key strokes
		if (GetTreeCtrl().GetEditControl()) { //  &&
			// (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_DELETE ||
			//  pMsg->wParam == VK_ESCAPE || GetKeyState(VK_CONTROL)) ||
			//  (pMsg->wParam >= 0x30 && pMsg->wParam <= 0x39)) {
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			return TRUE;                            // DO NOT process further
		}
	}
	
	return CTreeView::PreTranslateMessage(pMsg);
}

void QTreeView::OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult) {
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	CEdit *pEditCtrl = pTree->GetEditControl();
	ASSERT(pEditCtrl != NULL);

	Object *pObj = (Object *) pTree->GetItemData(pTVDispInfo->item.hItem);
	ASSERT(pObj != NULL);

	if (pObj->HasEntity()) {
		*pResult = 1;
		return;
	}

	pEditCtrl->LimitText(16);
	m_bOnEditLabel = true;
	*pResult = 0;
}

void QTreeView::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult) {
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*) pNMHDR;
	if (pTVDispInfo->item.pszText == NULL) {
		m_bOnEditLabel = false;
		*pResult = 0;
		return;
	}

	Object *pObj = (Object *) pTree->GetItemData(pTVDispInfo->item.hItem);
	ASSERT(pObj != NULL);

	OpObjNameChange *op =
		new OpObjNameChange(*pObj, pTVDispInfo->item.pszText);
	pQMainFrame->CommitOperation(*op);

	m_bOnEditLabel = false;
	*pResult = 1;

	// This is so fucked up.
	// Returning 1 causes the app to crash with some heap error.
	// *pResult = 1;
}

void QTreeView::OnEditLabel(void) {
	// Show the treeview window.
	// ((QMainFrame *) AfxGetMainWnd())->Get
	// ShowSelTabWindow(0);

	pTree->EditLabel(pTree->GetSelectedItem());
	m_bEditLabelPending = false;
}

//===== Drag and Drop =====

void QTreeView::OnBeginDrag(NMHDR* pNMHDR, LRESULT* pResult) {
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*) pNMHDR;
	*pResult = FALSE;

	// Check for button logic.
	if (m_bRDragging) {
		*pResult = TRUE;
		return;
	}

	m_hItemDrag = pNMTreeView->itemNew.hItem;
	m_hItemDrop = NULL;

	DragStartHelper(pResult, pNMTreeView->ptDrag);
	if (*pResult == TRUE)
		return;

	m_bLDragging = TRUE;
}

void QTreeView::OnBeginRDrag(NMHDR* pNMHDR, LRESULT* pResult) {
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	*pResult = FALSE;

	// Check for button logic.
	if (m_bLDragging) {
		*pResult = TRUE;
		return;
	}

	m_hItemDrag = pNMTreeView->itemNew.hItem;
	m_hItemDrop = NULL;

	DragStartHelper(pResult, pNMTreeView->ptDrag);
	if (*pResult == TRUE)
		return;

	m_bRDragging = TRUE;
}

void QTreeView::DragStartHelper(LRESULT *pResult, POINT dragPt) {

	// prevent the root item be dragged around.
	Object *pObj = (Object *) pTree->GetItemData(m_hItemDrag);
	if (pObj->IsRoot()) {
		*pResult = TRUE;
		return;
	}

	// Remember the old selection.
	ASSERT(oldDragSelBuf.NumOfElm() == 0);
	Selector *pSlct = &(pQMainFrame->GetSelector());
	oldDragSelBuf = pSlct->GetMSelectedObjects();

	if (!pSlct->IsObjectMSelected(pObj)) {
		// Need to clear the selector now in order for 
		//  TreeView to refresh properly during drag and drop.
		pSlct->MUnselect();
	}

	// Need to clear off the multi selection buf.
	while (slctObjBuf.NumOfElm() > 0)
		delete &(slctObjBuf.RemoveNode(0));

	// Get the image lists for dragging
	// CreateDragImage() returns NULL if no image list
	//   associated with the tree view control
	if ((m_pItemImage = pTree->CreateDragImage(m_hItemDrag)) == NULL) {
		*pResult = TRUE;
		return;
	}

	m_pDragImage = m_pNoImage;
	m_pDragImage->BeginDrag(0, CPoint(-15, -15));
	ClientToScreen(&dragPt);
	m_pDragImage->DragEnter(NULL, dragPt);
	SetCapture();
}

void QTreeView::DragMoveHelper(UINT nFlags, CPoint point) {

	POINT pt = point;
	ClientToScreen(&pt);
	m_pDragImage->DragMove(pt);
	POINT hidePt = {-100L, -100L};

	// Scroll the view if mouse is near boarder
	CRect rect;
	GetClientRect(&rect);
	ClientToScreen(&rect);
	if (rect.PtInRect(pt)) {
		if (pt.y <= rect.top + 20L) {  // Scroll up.
			m_pDragImage->DragShowNolock(FALSE);
			SendMessage(WM_VSCROLL, SB_LINEUP, 0L);
			m_pDragImage->DragShowNolock(TRUE);
		}
		if (pt.y >= rect.bottom - 20L) { // Scroll down.
			m_pDragImage->DragShowNolock(FALSE);
			SendMessage(WM_VSCROLL, SB_LINEDOWN, 0L);
			m_pDragImage->DragShowNolock(TRUE);
		}
	}

	UINT flags;
	HTREEITEM hItem = pTree->HitTest(point, &flags);
	Selector *pSlct = &(pQMainFrame->GetSelector());

#if 0
	// Check for dynamic tree expansion.
	if (hItem && hItem != m_hItemDragExpand &&
		(flags & TVHT_ONITEMBUTTON) && pTree->ItemHasChildren(hItem)) {

		// Make sure expand target doesnt contain the current scope.
		Object *pObj = (Object *) pTree->GetItemData(hItem);
		if (pSlct->GetScopePtr() != pObj &&
			!(pSlct->GetScopePtr())->IsMyAncestor(*pObj)) {

			// m_pDragImage->DragShowNolock(FALSE);
			m_pDragImage->DragMove(hidePt);
			m_pDragImage->DragLeave(NULL);

			if (pTree->GetItemState(hItem, TVIS_EXPANDED)) {
				CollapseSubTree(hItem);
				BOOL suc = pTree->Expand(hItem, TVE_COLLAPSE);
				UINT val = pTree->GetItemState(hItem, TVIS_EXPANDED);
				TRACE2("Collapsing tree result %d-%X\n", suc, val);
			}
			else {
				ExpandSubTree(hItem);
				BOOL suc = pTree->Expand(hItem, TVE_EXPAND);
				UINT val = pTree->GetItemState(hItem, TVIS_EXPANDED);
				TRACE2("Expanding tree result %d-%X\n", suc, val);
			}

			// m_pDragImage->DragShowNolock(TRUE);
			m_pDragImage->DragEnter(NULL, hidePt);
		}
	}
	m_hItemDragExpand = hItem;
#endif

	// Determine the drop target.
	hItem = FindDropTarget(hItem);
	if (hItem != m_hItemDrop) {
		// Hide the drag image.
		m_pDragImage->DragMove(hidePt);
		m_pDragImage->DragLeave(NULL);

		// Select the new drop target.
		if (hItem) {
			pTree->SetItemState(hItem, 0xFFFF , TVIS_SELECTED);
		}
		else {  // Change drag image.
			m_pDragImage->EndDrag();
			m_pDragImage = m_pNoImage;
			m_pDragImage->BeginDrag(0, CPoint(-15, -15));
		}

		// If previous drop target was selected, re-hilite it again.
		if (m_hItemDrop) {
			Object *pObj = (Object *) pTree->GetItemData(m_hItemDrop);
				
			flags = (pSlct->IsObjectMSelected(pObj) ? TVIS_SELECTED : 0);
			pTree->SetItemState(m_hItemDrop, flags, TVIS_SELECTED);
		}
		else {  // Change drag image.
			m_pDragImage->EndDrag();
			m_pDragImage = m_pItemImage;
			m_pDragImage->BeginDrag(0, CPoint(-15, -15));
		}

		m_hItemDrop = hItem;
		m_pDragImage->DragEnter(NULL, hidePt);
	}
}

void QTreeView::DragEndHelper(CPoint point) {
	// Sanity.
	ASSERT(m_bLDragging || m_bRDragging);
	ASSERT(!m_bLDragging || !m_bRDragging);

	m_pDragImage->DragLeave(NULL);
	m_pDragImage->EndDrag();
	ReleaseCapture();

	ASSERT(m_pItemImage);
	delete m_pItemImage;
	m_pItemImage = m_pDragImage = NULL;

	Selector *pSlct = &(pQMainFrame->GetSelector());

	// Remove drop target highlighting
	if (m_hItemDrop) {
		Object *pObj = (Object *) pTree->GetItemData(m_hItemDrop);
		UINT flags = (pSlct->IsObjectMSelected(pObj) ? TVIS_SELECTED : 0);
		pTree->SetItemState(m_hItemDrop, flags, TVIS_SELECTED);
	}

	// Sanity.
	ASSERT(m_hItemDrag != NULL);
	if (m_hItemDrop == NULL ||
		pTree->GetParentItem(m_hItemDrag) == m_hItemDrop) {
		// Cancel drag and drop.  Restore previous selection.
		SendMessage(WM_COMMAND, ID_RCMTV_DRAGCANCEL, 0L);
	}
	else if (m_bLDragging) {
		// Drag move the node.
		SendMessage(WM_COMMAND, ID_RCMTV_DRAGMOVE, 0L);
	}
	else { // m_bRDragging
		// Pop up a context menu.
		CMenu rMenu;
		rMenu.CreatePopupMenu();
		rMenu.AppendMenu(MF_STRING, ID_RCMTV_DRAGMOVE, "Move");
		rMenu.AppendMenu(MF_STRING, ID_RCMTV_DRAGCOPY, "Copy");
		rMenu.AppendMenu(MF_SEPARATOR);
		rMenu.AppendMenu(MF_STRING, ID_RCMTV_DRAGCANCEL, "Cancel");

		// Note: TrackPopupmenu returns before the WM_COMMAND
		//  message is sent and processed.
		ClientToScreen(&point);
		rMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON,
							 point.x, point.y, pQMainFrame);
	}

}

void QTreeView::OnDragMoveItem(void) {
	// Sanity.
	ASSERT(m_hItemDrag && m_hItemDrop);

	Object *pDragObj = (Object *) pTree->GetItemData(m_hItemDrag);
	Object *pDropObj = (Object *) pTree->GetItemData(m_hItemDrop);
	ASSERT(pDragObj && pDropObj);

	LinkList<ObjectPtr> dragObjs;
	Selector *pSlct = &(pQMainFrame->GetSelector());
	if (pSlct->IsObjectMSelected(pDragObj))
		dragObjs = pSlct->GetMSelectedObjects();
	else
		dragObjs.AppendNode(*(new ObjectPtr(pDragObj)));

	OpTreeViewDragMove *opDragMove;
	opDragMove = new OpTreeViewDragMove(pDropObj, dragObjs, oldDragSelBuf);
	pQMainFrame->CommitOperation(*opDragMove);

	m_hItemDrag = m_hItemDrop = NULL;
}

// What to do with the case where we drag and copy into the same scope???
void QTreeView::OnDragCopyItem(void) {
	// Sanity.
	ASSERT(m_hItemDrag && m_hItemDrop);

	Object *pDragObj = (Object *) pTree->GetItemData(m_hItemDrag);
	Object *pDropObj = (Object *) pTree->GetItemData(m_hItemDrop);
	ASSERT(pDragObj && pDropObj);

	OpTreeViewDragCopy *opDragCopy;

	Selector *pSlct = &(pQMainFrame->GetSelector());
	if (pSlct->IsObjectMSelected(pDragObj)) {
		opDragCopy = new OpTreeViewDragCopy(pDropObj, 
			pSlct->GetMSelectedObjects(), oldDragSelBuf);
	}
	else {
		LinkList<ObjectPtr> lstDragObj;
		lstDragObj.AppendNode(*(new ObjectPtr(pDragObj)));

		opDragCopy = new OpTreeViewDragCopy(pDropObj,
			lstDragObj, oldDragSelBuf);

		delete &(lstDragObj.RemoveNode(0));
	}

	pQMainFrame->CommitOperation(*opDragCopy);

	m_hItemDrag = m_hItemDrop = NULL;
}

void QTreeView::OnDragCancel(void)  {
	Selector *pSlct = &(pQMainFrame->GetSelector());

	pSlct->MSelectObjects(oldDragSelBuf);
	// Refresh view.
	OnUpdate(NULL, DUAV_OBJSSEL, (CObject *) pSlct->GetScopePtr());

	while (oldDragSelBuf.NumOfElm() > 0)
		delete &(oldDragSelBuf.RemoveNode(0));

	m_hItemDrag = m_hItemDrop = NULL;
}

HTREEITEM QTreeView::FindDropTarget(HTREEITEM hItem) const {
	if (hItem == NULL)
		return NULL;

	// Check if the dragged item is from the same doc.
	ASSERT(m_hItemDrag != NULL);
	Object *pObj = (Object *) pTree->GetItemData(m_hItemDrag);
	if (&(pObj->GetRoot()) != pQMainFrame->GetDeskTopDocument()->GetRootObjectPtr())
		return NULL;

	pObj = (Object *) pTree->GetItemData(hItem);
	if (&(pObj->GetRoot()) != pQMainFrame->GetDeskTopDocument()->GetRootObjectPtr())
		return NULL;

	if (pObj->IsItemNode() || pObj->HasBrush())
		hItem = pTree->GetParentItem(hItem);

	// Sanity.
	ASSERT(m_hItemDrag != NULL);
	Object *pDragObj = (Object *) pTree->GetItemData(m_hItemDrag);
	Object *pDropObj = (Object *) pTree->GetItemData(hItem);
	if (pDragObj == NULL || pDropObj == NULL || pDragObj == pDropObj ||
		// pDragObj->GetParentPtr() == pDropObj ||
		pDropObj->IsMyAncestor(*pDragObj))
		return NULL;

	return hItem;
}

//===== Sort Object Listings =====
void QTreeView::OnSortListChange(UINT nID) {
	ASSERT(nID >= ID_RCMTV_SORTNAME && nID <= ID_RCMTV_SORTZPOS);
	nID -= ID_RCMTV_SORTNAME - 1;
	listSortType = ((UINT) listSortType != nID ? nID : 0);

	Object *pRoot = ((QooleDoc *) GetDocument())->GetRootObjectPtr();
	SortSubTree(FindVisHItem(pRoot));
}

//===== View Update =====

HTREEITEM QTreeView::FindVisHItem(const Object *pObj) const {

	if (pObj == NULL)
		return NULL;

	HTREEITEM hItem = pTree->GetRootItem();
	while (hItem)	{
		if (pObj == (Object *) pTree->GetItemData(hItem))
			return hItem;
		hItem = pTree->GetNextVisibleItem(hItem);
	}
	return NULL;
}

void QTreeView::UpdateSelection(HTREEITEM hItem) {
	Selector *pSlctor = &(pQMainFrame->GetSelector());
	Object *pObj;
	UINT flag;
	HTREEITEM hCursor = NULL;
	QooleDoc *pDoc = pQMainFrame->GetDeskTopDocument();
	HTREEITEM hRoot = FindVisHItem(pDoc->GetRootObjectPtr());

	if (pSlctor->GetNumMSelectedObjects() == 0) {
		pTree->SelectItem(hRoot);
		pTree->SetItemState(hRoot, TVIS_SELECTED, TVIS_SELECTED);
	}
	else {
		pTree->SetItemState(hRoot, 0, TVIS_SELECTED);

		if (pSlctor->GetNumMSelectedObjects() != 1)
			pTree->SelectItem(NULL);
	}

	for(hItem = pTree->GetChildItem(hItem);
		hItem != NULL;
		hItem = pTree->GetNextSiblingItem(hItem)) {
		pObj = (Object *) pTree->GetItemData(hItem);
		flag = (pSlctor->IsObjectMSelected(pObj) ? TVIS_SELECTED : 0);
		if (flag) {
			hCursor = hItem;
		}
		pTree->SetItemState(hItem, flag, TVIS_SELECTED);
	}

	if (hCursor)
		pTree->EnsureVisible(hCursor);

	if (pSlctor->GetNumMSelectedObjects() == 1 && hCursor) {
		// pTree->SetItemState(hItem, 0, TVIS_SELECTED);
		pTree->SelectItem(hCursor);
	}
}

void QTreeView::UpdateScopeView(Object *pOldScope) {
	ASSERT(pOldScope != NULL);

	Selector *pSlctr = &(pQMainFrame->GetSelector());
	Object *pNewScope = pSlctr->GetScopePtr();

	HTREEITEM hItem = FindVisHItem(pOldScope);
	ASSERT(hItem);

	if (!pOldScope->IsRoot()) {
		// Need to find the top most unshared scope.
		for(pOldScope = pOldScope->GetParentPtr();
			!pOldScope->IsRoot() && pOldScope != pNewScope &&
			!pNewScope->IsMyAncestor(*pOldScope);
			pOldScope = pOldScope->GetParentPtr(),
			hItem = pTree->GetParentItem(hItem));
		// Collapse the old scope folder.
		CollapseSubTree(hItem);
		pTree->Expand(hItem, TVE_COLLAPSE);
	}

	// Need to expand the new scope folder.
	hItem = ExpandObjectTree(pNewScope);
	ASSERT(hItem);
}

void QTreeView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) {

	if ((lHint & DUAV_NOQTREEVIEW) || pHint == NULL)
		return;

	m_bInUpdateView = true;
	SetRedraw(FALSE);

	Object *pScope = (Object *) pHint;
	HTREEITEM hItem = FindVisHItem(pScope);
	if (hItem == NULL)
		hItem = ExpandObjectTree(pScope);

	if (lHint & (DUAV_OBJSMODSTRUCT | DUAV_OBJSMODATTRIB)) {
		// Need to properly refresh the folder.
		RefreshSubTree(hItem);

		pTree->EnsureVisible(hItem);
		lHint |= DUAV_OBJSSEL;
	}

	if (lHint & DUAV_OBJSSEL) {
		UpdateSelection(hItem);
	}

	if (lHint & DUAV_SCOPECHANGED) {
		UpdateScopeView(pScope);
		// pTree->EnsureVisible(hItem);
		m_hFirstSelectedItem = NULL;
	}

	if (lHint & DUTV_RENAMEOBJ) {
		pTree->SetItemText(hItem, pScope->GetObjName());
	}

	SetRedraw(TRUE);
	m_bInUpdateView = false;
}

// Fixes flicker problem.
void QTreeView::SetRedraw(BOOL bRedraw) {
	if (! bRedraw) {
		if (m_redrawcount++ <= 0) {
			CTreeView::SetRedraw(FALSE);
		}
	} else {
		if (--m_redrawcount <= 0) {
			CTreeView::SetRedraw(TRUE);
			m_redrawcount = 0;
			Invalidate();
		}
	}
}
