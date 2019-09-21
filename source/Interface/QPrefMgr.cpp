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

// QPrefMgr.cpp : implementation file
//

#include "stdafx.h"
#include "Qoole.h"
#include "QPrefMgr.h"
#include "QMainFrm.h"
#include "MapIO.h"
#include "Texture.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//======================================================================
// QPrefabMgr
//======================================================================

#define PVICON_NUM			4

#define PVICON_FOLDERCLOSE	0
#define PVICON_FOLDEROPEN	1
#define PVICON_PREFAB		2
#define PVICON_DUMMY		3

QPrefabMgr::QPrefabMgr() {
	// Find the prefab root directory.
	GetCurrentDirectory(256, rootDir);
	strcat(rootDir, "\\prefabs");

	// cfg = new LConfig("Prefab Manager");
	// cfg->RegisterVar("Dir", rootDir, LVAR_STR);

	if (!LFile::ExistDir(rootDir)) {
		// Create the prefabs directory.
		CreateDirectory(rootDir, NULL);
	}

	pPreviewWnd = NULL;
}

QPrefabMgr::~QPrefabMgr() {
	// cfg->SaveVars();
	// delete cfg;

	ASSERT(pPreviewWnd == NULL);
}

IMPLEMENT_DYNCREATE(QPrefabMgr, CTreeCtrl)

BEGIN_MESSAGE_MAP(QPrefabMgr, CTreeCtrl)
	//{{AFX_MSG_MAP(QPrefabMgr)
	ON_WM_CREATE()
	ON_WM_CONTEXTMENU()
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, OnFolderExpanding)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnFolderCollapsed)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_RCMPV_ADDPREFAB, OnAddPrefab)
	ON_COMMAND(ID_RCMPV_REFRESHDIR, OnRefreshDir)
	ON_COMMAND(ID_RCMPV_VIEWPREFAB, OnPreviewPrefab)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// QPrefabMgr message handlers

BOOL QPrefabMgr::PreCreateWindow(CREATESTRUCT& cs) {

	return CTreeCtrl::PreCreateWindow(cs);
}

int QPrefabMgr::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	if (CTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	// DWORD style = WS_CHILD | WS_VISIBLE | 
	//				  TVS_LINESATROOT | TVS_HASLINES |
	//				  TVS_HASBUTTONS | TVS_DISABLEDRAGDROP;

	// CRect rect(0, 0, 0, 0);
	// Create the tree ctrl.
	// if  (!dirTree.Create(style, rect, this, 1)) {
	// 	return -1;
	//}

	// Create the image list.
	imgList.Create(IDB_PVIEWICO, 16, 1, RGB(192, 192, 192));
	SetImageList(&imgList, TVSIL_NORMAL);

	// Display them.
	RefreshView();

	return 0;
}

// Close the preview window.
void QPrefabMgr::ClosePreviewWnd(void) {

	if (pPreviewWnd != NULL) {
		pPreviewWnd->DestroyWindow();
		pPreviewWnd = NULL;
	}
}

void QPrefabMgr::FindNodesFullPath(HTREEITEM hItem, char *dirPath) {

	if (hItem == NULL || hItem == TVI_ROOT) {
		strcpy(dirPath, rootDir);
		return;
	}

	HTREEITEM hParent = GetParentItem(hItem);
	FindNodesFullPath(hParent, dirPath);

	CString dirName = GetItemText(hItem);
	strcat(dirPath, "\\");
	strcat(dirPath, (LPCTSTR) dirName);
}

void QPrefabMgr::AddFolder(const char *rootPath, const char *dirName,
						   HTREEITEM hParent) {
	char dirPath[256];
	sprintf(dirPath, "%s\\%s", rootPath, dirName);
	ASSERT(LFile::ExistDir(dirPath));

	HTREEITEM hNode = InsertItem(dirName, PVICON_FOLDERCLOSE,
										 PVICON_FOLDERCLOSE, hParent);

	char *c;
	int count = 0;

	LFindFiles findFiles(dirPath);
	while ((c = findFiles.Next()) != NULL)
		count++;

	LFindDirs findDirs(dirPath);
	while ((c = findDirs.Next()) != NULL) {
		if (strcmp(c, ".") != 0)
			count++;
	}

	// Check if folder is not empty.
	if (count > 0)
		InsertItem("", PVICON_DUMMY, PVICON_DUMMY, hNode);
}

//==== Folder Expansion ====
void QPrefabMgr::OnFolderExpanding(NMHDR *pnmh, LRESULT *pResult) {
	NM_TREEVIEW *pnmtv = (NM_TREEVIEW *) pnmh;
	HTREEITEM hItem = pnmtv->itemNew.hItem;

	*pResult = FALSE;

	if (pnmtv->action != TVE_EXPAND)
		return;

	// Set hour glass cursor.
	HCURSOR hOldCursor = LoadCursor(NULL, MAKEINTRESOURCE(IDC_WAIT));
	hOldCursor = ::SetCursor(hOldCursor);

	// Delete the dummy node, if any.
	HTREEITEM hDelItem = GetChildItem(hItem);
	HTREEITEM hNext;
	while (hDelItem != NULL) {
		hNext = GetNextSiblingItem(hDelItem);
		DeleteItem(hDelItem);
		hDelItem = hNext;
	}

	// Find the folder's path.
	char dirPath[256];
	FindNodesFullPath(hItem, dirPath);

	// Add the sub folders.
	const char *c;
	LFindDirs findDirs(dirPath);
	while ((c = findDirs.Next()) != NULL) {
		if (strcmp(c, ".") != 0)
			AddFolder(dirPath, c, hItem);
	}

	// Add the prefabs.
	LFindFiles findQleFiles(dirPath, "*.qle");
	while ((c = findQleFiles.Next()) != NULL)
		InsertItem(c, PVICON_PREFAB, PVICON_PREFAB, hItem);

	// Add the .map files.
	LFindFiles findMapFiles(dirPath, "*.map");
	while ((c = findMapFiles.Next()) != NULL)
		InsertItem(c, PVICON_PREFAB, PVICON_PREFAB, hItem);

	// Change icon.
	SetItemImage(hItem, PVICON_FOLDEROPEN, PVICON_FOLDEROPEN);

	// Change cursor back.
	::SetCursor(hOldCursor);
}

void QPrefabMgr::OnFolderCollapsed(NMHDR *pnmh, LRESULT *pResult) {
	NM_TREEVIEW *pnmtv = (NM_TREEVIEW *) pnmh;
	HTREEITEM hItem = pnmtv->itemNew.hItem;

	if (pnmtv->action != TVE_COLLAPSE)
		return;

	// Set hour glass cursor.
	HCURSOR hOldCursor = LoadCursor(NULL, MAKEINTRESOURCE(IDC_WAIT));
	hOldCursor = ::SetCursor(hOldCursor);

	// Delete all the nodes.
	HTREEITEM hDelItem = GetChildItem(hItem);
	HTREEITEM hNext;
	while (hDelItem != NULL) {
		hNext = GetNextSiblingItem(hDelItem);
		DeleteItem(hDelItem);
		hDelItem = hNext;
	}

	// Find the folder's path.
	char dirPath[256];
	FindNodesFullPath(hItem, dirPath);

	// Add the dummy node, if folder not empty.
	char *c;
	int count = 0;

	LFindFiles findFiles(dirPath);
	while ((c = findFiles.Next()) != NULL)
		count++;

	LFindDirs findDirs(dirPath);
	while ((c = findDirs.Next()) != NULL) {
		if (strcmp(c, ".") != 0)
			count++;
	}

	// Check if folder is not empty.
	if (count > 0)
		InsertItem("", PVICON_DUMMY, PVICON_DUMMY, hItem);

	// Change icon.
	SetItemImage(hItem, PVICON_FOLDERCLOSE, PVICON_FOLDERCLOSE);

	// Change cursor back.
	::SetCursor(hOldCursor);
}

//==== Selection ====
void QPrefabMgr::OnLButtonDown(UINT nFlags, CPoint point) {
	HTREEITEM hOldSel = GetSelectedItem();

	CTreeCtrl::OnLButtonDown(nFlags, point);

	UINT nHitFlags = 0;
	HTREEITEM hClickedItem = HitTest(point, &nHitFlags);
	
	if (hClickedItem == NULL)
		return;

	int i1, i2;
	GetItemImage(hClickedItem, i1, i2);

	if (i1 != PVICON_PREFAB || hOldSel == hClickedItem)
		return;

	if ((nHitFlags & LVHT_ONITEM) == 0)
		return;

	// Preview it.
	PostMessage(WM_COMMAND, ID_RCMPV_VIEWPREFAB);
}

void QPrefabMgr::OnLButtonDblClk(UINT nFlags, CPoint point) {
	CTreeCtrl::OnLButtonDblClk(nFlags, point);

	UINT nHitFlags = 0;
	HTREEITEM hClickedItem = HitTest(point, &nHitFlags);

	int i1, i2;
	GetItemImage(hClickedItem, i1, i2);

	if (i1 != PVICON_PREFAB)
		return;

	if ((nHitFlags & LVHT_ONITEM) == 0)
		return;

	// Add it.
	PostMessage(WM_COMMAND, ID_RCMPV_ADDPREFAB);
}

void QPrefabMgr::OnRButtonDown(UINT nFlags, CPoint point) {
	// Dont call default CTreeCtrl::OnRButtonDown()
	// It'll send parent window a WM_CONTEXTMENU message.
	// CTreeCtrl::OnRButtonDown(nFlags, point);

	// No drag and drop.

	ClientToScreen(&point);

	WPARAM wParam = (WPARAM) GetSafeHwnd();
	LPARAM lParam = (point.y << 16) | point.x;
	SendMessage(WM_CONTEXTMENU, wParam, lParam);
}

void QPrefabMgr::OnRButtonUp(UINT nFlags, CPoint point) {
	// Dont call default CTreeCtrl::OnRButtonUp()
	// CTreeCtrl::OnRButtonUp(nFlags, point);
}

//==== Right Click Menu ====
void QPrefabMgr::OnContextMenu(CWnd* pWnd, CPoint point) {
	CMenu rMenu;
	rMenu.CreatePopupMenu();
	
	CPoint pt(point);
	ScreenToClient(&pt);
	
	UINT nFlags = 0;
	HTREEITEM hItem = HitTest(pt, &nFlags);

	if (hItem != NULL && (nFlags & TVHT_ONITEM)) {
		// Select the new item.
		if (GetSelectedItem() != hItem)
			SelectItem(hItem);

		if (ItemHasChildren(hItem)) {
			rMenu.AppendMenu(MF_STRING, ID_RCMPV_REFRESHDIR, "Refresh");

			nFlags = (GetItemState(hItem, TVIS_EXPANDED) & TVIS_EXPANDED ?
					  MF_ENABLED : MF_GRAYED);
			rMenu.EnableMenuItem(ID_RCMPV_REFRESHDIR, MF_BYCOMMAND | nFlags);
		}
		else {
			nFlags = (pQMainFrame->GetDeskTopDocument() != NULL ?
					  MF_ENABLED : MF_GRAYED);

			rMenu.AppendMenu(MF_STRING, ID_RCMPV_ADDPREFAB , "Add");
			rMenu.EnableMenuItem(ID_RCMPV_ADDPREFAB, MF_BYCOMMAND | nFlags);
			rMenu.AppendMenu(MF_SEPARATOR);
			rMenu.AppendMenu(MF_STRING, ID_RCMPV_VIEWPREFAB, "Preview");
			rMenu.EnableMenuItem(ID_RCMPV_VIEWPREFAB, MF_BYCOMMAND | nFlags);
		}

		// rMenu.AppendMenu(MF_SEPARATOR);
	}
	else {
		CWnd *pQWSBar = GetParent();
		pQWSBar->SendMessage(WM_ADDCONTEXTMENUITEMS, 0, (LPARAM) rMenu.m_hMenu);
	}

	UINT nStyle = TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON;
	// rMenu.TrackPopupMenu(nStyle, point.x, point.y, pQMainFrame);
	rMenu.TrackPopupMenu(nStyle, point.x, point.y, this);
}

//==== Add Prefab ====
void QPrefabMgr::OnAddPrefab() {
	QooleDoc *pDoc = pQMainFrame->GetDeskTopDocument();
	if (pDoc == NULL)
		return;

	HTREEITEM hNode = GetSelectedItem();
	ASSERT(hNode != NULL);

	char filePath[256];
	FindNodesFullPath(hNode, filePath);

	AddPrefab(filePath);
}

bool QPrefabMgr::AddPrefab(const char *filePath) {
	ObjectPtr *objPtr;
	Object *pPrefab, *pScope = &(pQMainFrame->GetScope());
	LinkList<ObjectPtr> addLst;
	OpObjsAddNew *op;

	CMDIChildWnd *pFrame;
	QView *pQView;
	View *pView;
	Vector3d addPosVec;
	HCURSOR oldCur;
	const char *c;
	bool ok;

	TexDB *texDB;

	if (!LFile::Exist(filePath))
		goto ERROR_ADDPREFAB;
	
	// Load the prefab.

	oldCur = AfxGetApp()->LoadCursor(IDC_WAIT);

	// hax0r
	texDB = pQMainFrame->GetDeskTopDocument()->GetGame()->GetTexDB();
	texDB->SetTryExtract(false);

	c = strrchr(filePath, '.');
	if (stricmp(c, ".qle") == 0) {
		pPrefab = new Object;
		ok = pPrefab->LoadObjFile(filePath);
	}
	else if (stricmp(c, ".map") == 0) {
		pPrefab = MapIO::ReadMap(filePath);
		ok = (pPrefab != NULL);
	}
	else {
		ok = false;
	}

	// un-hax0r
	texDB->SetTryExtract(true);

	::SetCursor(oldCur);

	if (!ok)
		goto ERROR_ADDPREFAB;

	// Backword compatibility for a bug.
	if (pPrefab->HasEntity() &&
		stricmp(pPrefab->GetEntityPtr()->GetClassName(), "worldspawn") == 0) {
		// Remove the worldspawn entity.
		Entity *pEnt = pPrefab->SetEntity(NULL);
		delete pEnt;

		// Remove the redundant object level.
		if (pPrefab->GetNumChildren() == 1) {
			Object *pChild = &(pPrefab->RemoveChild(pPrefab->GetChild(0)));
			delete pPrefab;
			pPrefab = pChild;
		}
	}

	// Texture lock on the entire prefab.
	pPrefab->PreOrderApply(LockPrefabTexture);

	// Set the prefab name.
	if (strcmp(pPrefab->GetObjName(), "") == 0) {
		CString fileName = filePath;
		int index = fileName.ReverseFind('\\');
		if (index != -1)
			fileName = fileName.Mid(index + 1);

		index = fileName.Find('.');
		if (index != -1)
			fileName = fileName.Left(index);

		pPrefab->SetObjName((LPCTSTR) fileName);
	}

	// Find the add position.
	pFrame = pQMainFrame->MDIGetActive();
	ASSERT_VALID(pFrame);
	pQView = (QView *) pFrame->GetDescendantWindow(AFX_IDW_PANE_FIRST, TRUE);
	ASSERT_VALID(pQView);

	if (pQMainFrame->IsLockedView(pQView)) {
		addPosVec = pQMainFrame->GetEditFocusPos();
	}
	else {
		pView = pQView->GetViewPtr();
		if (pQView->GetViewType() == ID_VIEW_NEW3D) {
			addPosVec.NewVector(0.0f, 128.0f, 0.0f);
			Matrix44 trans;
			pView->CalTransSpaceMatrix(trans.SetIdentity());
			trans.Transform(addPosVec);
		}
		else {
			addPosVec = pView->GetPosition();
		}
	}

	pQView->SnapAddObjPos(addPosVec);

	// Set up the operation.
	objPtr = new ObjectPtr(pPrefab);
	addLst.AppendNode(*objPtr);

	// Commit.
	op = new OpObjsAddNew(addLst, addPosVec, pScope);
	pQMainFrame->CommitOperation(*op);

	return true;

ERROR_ADDPREFAB:
	char errorMsg[256];
	sprintf(errorMsg, "Failed to load the prefab files:\n%s", filePath);

	MessageBox(errorMsg, NULL, MB_ICONWARNING | MB_OK);

	if (pPrefab != NULL)
		delete pPrefab;

	return false;
}

bool QPrefabMgr::LockPrefabTexture(Object &objNode) {
	if (objNode.HasBrush()) {
		Geometry *pBrush = &(objNode.GetBrush());
		FaceTex	*pTex;
		for(int i = 0; i < pBrush->GetNumFaces(); i++) {
			if ((pTex = pBrush->GetFaceTexturePtr(i)) != NULL)
				pTex->SetTexLock(true);
		}
	}

	return true;
}

//==== Preview ====
void QPrefabMgr::OnPreviewPrefab(void) {
	if (pQMainFrame->GetDeskTopDocument() == NULL)
		return;

	if (pPreviewWnd == NULL) {
		// Create it.
		pPreviewWnd = new QPrefabFrame();

		// Let it auto delete itself upon window destroy.
		pPreviewWnd->m_bAutoDelete = TRUE;

		// Find a good place to put the preview window.
		CRect pvWndRect(0, 0, 200, 200);

		CRect rect;
		GetClientRect(&rect);
		ClientToScreen(&rect);
		CPoint pt(rect.right + 20, rect.top);
		GetDesktopWindow()->GetClientRect(&rect);
		if (pt.x >= rect.right - 200)
			pt.x = rect.right - 200;
		if (pt.y >= rect.bottom - 200)
			pt.y = rect.bottom - 200;

		pvWndRect.OffsetRect(pt);

		// Create the prefab frame window.
		DWORD dwStyle = WS_VISIBLE | CBRS_SIZE_DYNAMIC;
		CString szTitle = "Preview: " + GetItemText(GetSelectedItem());
		pPreviewWnd->Create(NULL, szTitle, dwStyle,
							pvWndRect, pQMainFrame, ID_PREFABPREVIEW);

		// Diable docking.
		pPreviewWnd->EnableDocking(0);

		// Float it.
		CPoint cPt(pvWndRect.left, pvWndRect.top);
		pQMainFrame->FloatControlBar(pPreviewWnd, cPt, 0);
	}
	else {
		// Make sure it's visible.
		// pPreviewWnd->ModifyStyle(0, WS_VISIBLE, 0);
		pQMainFrame->ShowControlBar(pPreviewWnd, TRUE, FALSE);
	}

	// Get the filename.
	HTREEITEM hNode = GetSelectedItem();
	ASSERT(hNode != NULL);

	char fileName[256];
	FindNodesFullPath(hNode, fileName);

	pPreviewWnd->prefabView.LoadPrefab(fileName);
	CWnd *pFrame = pPreviewWnd->GetParent();
	CString szTitle = "Preview: " + GetItemText(hNode);
	pFrame->SetWindowText(szTitle);
}

//==== Refresh dir content ====
void QPrefabMgr::OnRefreshDir() {
 	HTREEITEM hNode = GetSelectedItem();
	ASSERT(hNode == NULL);

	char filePath[256];
	FindNodesFullPath(hNode, filePath);
 
	if (!LFile::ExistDir(filePath))
		return;

	if (!(GetItemState(hNode, TVIS_EXPANDED) & TVIS_EXPANDED))
		return;

	// Delete all the nodes.

	// Reconstruct it.

/*
	// Refresh dir listing.
	NM_TREEVIEW tv;
	tv.hdr.hwndFrom = GetSafeHwnd();
 	tv.hdr.idFrom = GetWindowLong(GetSafeHwnd(), GWL_ID);
	tv.itemNew.hItem = hNode;
	tv.itemNew.state = GetItemState(hNode, 0xffffffff);
	tv.itemNew.lParam = GetItemData(hNode);

	tv.hdr.code =  TVN_ITEMEXPANDED;
	tv.action = TVE_COLLAPSE;
	SendMessage(WM_NOTIFY, tv.hdr.idFrom, (LPARAM) &tv);

	tv.hdr.code =  TVN_ITEMEXPANDING;
	tv.action = TVE_EXPAND;
	SendMessage(WM_NOTIFY, tv.hdr.idFrom, (LPARAM) &tv);
*/
}

void QPrefabMgr::RefreshView(void) {
	// Turn off display.
	SetRedraw(FALSE);

	DeleteAllItems();

	// Add the root level folders.
	LFindDirs dirFind(rootDir);
	char *c;
	while ((c = dirFind.Next()) != NULL) {
		if (strcmp(c, ".") == 0)
			continue;
		AddFolder(rootDir, c, TVI_ROOT);
	}

	// Add the prefabs.
	LFindFiles findQleFiles(rootDir, "*.qle");
	while ((c = findQleFiles.Next()) != NULL)
		InsertItem(c, PVICON_PREFAB, PVICON_PREFAB, TVI_ROOT);

	// Add the .map files.
	LFindFiles findMapFiles(rootDir, "*.map");
	while ((c = findMapFiles.Next()) != NULL)
		InsertItem(c, PVICON_PREFAB, PVICON_PREFAB, TVI_ROOT);

	SetRedraw(TRUE);

	// Update the window.
	QWorkSpaceBar *pWkSpce = (QWorkSpaceBar *) GetParent();

	if (pWkSpce->GetTabWindow() == this)
		Invalidate();
}
