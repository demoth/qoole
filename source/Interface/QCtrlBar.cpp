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

// qwsbar.cpp : implementation file
//

#include "stdafx.h"

#include "Qoole.h"
#include "QCtrlBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern MRC_AUX_DATA afxData;

//======================================================================
// QWorkSpaceBar
//======================================================================

QWorkSpaceBar::QWorkSpaceBar() {
	pTreeView = NULL;
	pTextureView = NULL;
	pPrefabView = NULL;
}

QWorkSpaceBar::~QWorkSpaceBar() {
	ASSERT(pTreeView == NULL);
	ASSERT(pTextureView == NULL);
	ASSERT(pPrefabView == NULL);
}

BEGIN_MESSAGE_MAP(QWorkSpaceBar, CMRCSizeControlBar)
	//{{AFX_MSG_MAP(QWorkSpaceBar)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_NOTIFY(TCN_SELCHANGE, ID_WORKSPACE_TAB, OnTabChanged)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//===== QWorkSpaceBar message handlers =====

BOOL QWorkSpaceBar::PreCreateWindow(CREATESTRUCT& cs) {
	if (!CMRCSizeControlBar::PreCreateWindow(cs))
		return FALSE;

	cs.style |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	return TRUE;
}

int QWorkSpaceBar::OnCreate(LPCREATESTRUCT lpCreateStruct) {

	if (CMRCSizeControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// Recorrect the default vertical docked width.
	m_VertDockSize.cx = m_FloatSize.cx;

    // create a tab control;
    CRect rect(0,0,0,0);
    if (!m_TabCtrl.Create(TCS_BOTTOM, rect, this, ID_WORKSPACE_TAB)) {
        TRACE("Failed to create Tab control\n");
        return -1;
    }
	
	m_ImgList.Create(ID_WORKSPACE_TAB, 16, 1, RGB(192, 192, 192));

    m_TabCtrl.SetFont(CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT))); 
    m_TabCtrl.SetImageList(&m_ImgList);

	static char *tabNames[] = {"TreeView", "Textures", "Prefabs"};

	// Create the TreeView Window
	pTreeView = (QTreeView *) RUNTIME_CLASS(QTreeView)->CreateObject();
	// if (pTreeView == NULL || !pTreeView->Create(&m_TabCtrl, rect, TAB_TREEVIEW)) {
	if (pTreeView == NULL || !pTreeView->Create(this, rect, TAB_TREEVIEW)) {
		TRACE0("Error: Can't create Tree View.");
		return -1;
	}
	pTreeView->ModifyStyleEx(0, WS_EX_CLIENTEDGE);

	TC_ITEM TCI;
	TCI.mask = TCIF_TEXT | TCIF_PARAM | TCIF_IMAGE;
	TCI.pszText = tabNames[TAB_TREEVIEW];
	TCI.lParam = (LPARAM) pTreeView;
	TCI.iImage = TAB_TREEVIEW;
	m_TabCtrl.InsertItem(TAB_TREEVIEW, &TCI);

	// Create the Texture Window.
	pTextureView = (QTexView *) RUNTIME_CLASS(QTexView)->CreateObject();
	if (pTextureView == NULL ||
		!pTextureView->Create(NULL, "", WS_CHILD | WS_VISIBLE,
							   rect, this, TAB_TEXTUREVIEW)) {
		TRACE0("Error: Can't create Texture View.");
		return -1;
	}
	pTextureView->ModifyStyleEx(0, WS_EX_CLIENTEDGE);

	TCI.pszText = tabNames[TAB_TEXTUREVIEW];
	TCI.lParam = (LPARAM) pTextureView;
	TCI.iImage = TAB_TEXTUREVIEW;
	m_TabCtrl.InsertItem(TAB_TEXTUREVIEW, &TCI);

	// Create the Prefab Window.
	pPrefabView = (QPrefabMgr *) RUNTIME_CLASS(QPrefabMgr)->CreateObject();
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | 
					TVS_LINESATROOT | TVS_HASLINES |
					TVS_HASBUTTONS | TVS_DISABLEDRAGDROP;
	if (pPrefabView == NULL ||
		!pPrefabView->Create(dwStyle, rect, this, TAB_PREFABVIEW)) {
		TRACE0("Error: Can't create Prefab View.");
		return -1;
	}
	pPrefabView->ModifyStyleEx(0, WS_EX_CLIENTEDGE);

	TCI.pszText = tabNames[TAB_PREFABVIEW];
	TCI.lParam = (LPARAM) pPrefabView;
	TCI.iImage = TAB_PREFABVIEW;
	m_TabCtrl.InsertItem(TAB_PREFABVIEW, &TCI);

	m_TabCtrl.RegisterTabLabels((const char **)tabNames);

	ShowSelTabWindow();
	m_TabCtrl.SetWindowPos(&wndBottom, 0, 0, 0, 0,
						   SWP_NOMOVE | SWP_NOSIZE | SWP_NOREDRAW);
	return 0;
}

void QWorkSpaceBar::OnSizedOrDocked(int cx, int cy, BOOL bFloating, int flags) {
	// Hack.  Fixes the flickering when resizing as float bar.
	GetParent()->ModifyStyle(0, WS_CLIPCHILDREN);

    CRect rect(0, 0, cx, cy);

    if (IsProbablyFloating()) {
        rect.InflateRect(-4, -4);
	}
    else {
        rect.InflateRect(-8, -8);
		rect.right += 2;
	}
	rect.bottom -=4;

    // Reposition the tab control.
	m_TabCtrl.MoveWindow(&rect, TRUE);

	// Reposition all the views.
	CRect cRect(rect);
	m_TabCtrl.AdjustRect(FALSE, &cRect);
	for(int i = 0; i < m_TabCtrl.GetItemCount(); i++)
		GetTabWindow(i)->MoveWindow(&cRect, TRUE);
}

void QWorkSpaceBar::OnTabChanged( NMHDR * pNM, LRESULT * pResult ) {
	// hide all the windows - except the one with the currently selected tab
	ShowSelTabWindow();
	*pResult = TRUE;
}


void QWorkSpaceBar::ShowSelTabWindow(int nSel) {    
	if (nSel == -1)
		nSel = m_TabCtrl.GetCurSel();   
	ASSERT(nSel >= 0 && nSel < m_TabCtrl.GetItemCount());
    
	bool show;
    for (int i = 0; i < m_TabCtrl.GetItemCount(); i++) {
		show = (i == nSel);
		CWnd *pWnd = GetTabWindow(i);
		pWnd->ShowWindow(show ? SW_SHOW : SW_HIDE);
		if (show && m_TabCtrl.GetCurSel() != i)
			m_TabCtrl.SetCurSel(i);
	}

}

CWnd *QWorkSpaceBar::GetTabWindow(int nTab) {
    TC_ITEM TCI;
    TCI.mask = TCIF_PARAM; 

	if (nTab == -1)
		nTab = m_TabCtrl.GetCurSel();
    m_TabCtrl.GetItem(nTab, &TCI);
    CWnd * pWnd = (CWnd *) TCI.lParam;
    ASSERT(pWnd != NULL && pWnd->IsKindOf(RUNTIME_CLASS(CWnd)));
    return pWnd;
}

void QWorkSpaceBar::OnPaint() {
	CPaintDC dc(this); // device context for painting

	// Do the painting of the background here.
	CRect rect;
	GetClientRect(&rect);
	// rect.DeflateRect(2, 2);
//	dc.FillSolidRect(&rect, afxData.clrBtnFace); 
	// dc.FillSolidRect(&rect, 0x00ff0000);
}
void QWorkSpaceBar::OnDestroy() {
    // Delete the CWnd object belonging to the child window
	for(int i = 0; i < m_TabCtrl.GetItemCount(); i++) {
		delete GetTabWindow(i);
	}

	pTreeView = NULL;
	pPrefabView = NULL;
	pTextureView = NULL;

    CMRCSizeControlBar::OnDestroy();
}

//======================================================================
// QOutputBar
//======================================================================

QOutputBar::QOutputBar() {
	pOutputWnd = NULL;
}

QOutputBar::~QOutputBar() {
	ASSERT(pOutputWnd == NULL);
}

BEGIN_MESSAGE_MAP(QOutputBar, CMRCSizeControlBar)
	//{{AFX_MSG_MAP(QOutputBar)
	ON_WM_DESTROY()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//===== QOutputBar message handlers =====

int QOutputBar::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	if (CMRCSizeControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	pOutputWnd = new QProcessWnd;

	CRect rect(0, 0, 0, 0);
	DWORD style = WS_CHILD | WS_VISIBLE | WS_BORDER | WS_EX_OVERLAPPEDWINDOW;
	pOutputWnd->Create(style, rect, this, 1);

	return 0;
}

void QOutputBar::OnDestroy() {

	delete pOutputWnd;
	pOutputWnd = NULL;

	CMRCSizeControlBar::OnDestroy();
}

void QOutputBar::OnSizedOrDocked(int cx, int cy, BOOL bFloating, int flags) {
    CRect rect(0, 0, cx, cy);

    if (IsProbablyFloating())
        rect.InflateRect(-8, -8);       // give space around controls.
    else
        rect.InflateRect(-8, -8);       // give space around controls.
	rect.bottom -=4;

	pOutputWnd->MoveWindow(&rect);
}

//======================================================================
// QTabCtrl
//======================================================================

QTabCtrl::QTabCtrl() {
}

QTabCtrl::~QTabCtrl() {
}

BOOL QTabCtrl::PreCreateWindow(CREATESTRUCT& cs) {
	cs.style |= WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS ;
	cs.style |= TCS_FIXEDWIDTH | TCS_FOCUSNEVER;
	cs.style &= ~TCS_MULTILINE;
	
	return CTabCtrl::PreCreateWindow(cs);
}

BEGIN_MESSAGE_MAP(QTabCtrl, CTabCtrl)
	//{{AFX_MSG_MAP(QTabCtrl)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// QTabCtrl message handlers

BOOL QTabCtrl::OnEraseBkgnd(CDC* pDC) {
	// return CTabCtrl::OnEraseBkgnd(pDC);

	// Calculate the region for erasing background
	//  excluding the display area and the tab buttons.
	CRect rect;
	GetClientRect(&rect);

	CRgn rgn1;
	rgn1.CreateRectRgnIndirect(&rect);

	CRgn rgn2;
	rgn2.CreateRectRgnIndirect(displayArea);

	CRgn rgn3;
	rgn3.CreateRectRgn(0, 0, 0, 0);
	rgn3.CombineRgn(&rgn1, &rgn2, RGN_DIFF);

/*  
	// iter through all tab buttons and exclude area.
	for(int i = 0; i < GetItemCount(); i++) {
		rgn1.CopyRgn(&rgn3);

		// GetTabWindow(i)->GetWindowRect(&rect);
		// ScreenToClient(&rect);
		GetItemRect(i, &rect);
		rgn2.SetRectRgn(&rect);

		rgn3.CombineRgn(&rgn1, &rgn2, RGN_DIFF);
	}
*/

	CBrush brush;
	brush.CreateStockObject(LTGRAY_BRUSH);

	pDC->FillRgn(&rgn3, &brush);
	return TRUE;
}

void QTabCtrl::OnSize(UINT nType, int cx, int cy) {
	CTabCtrl::OnSize(nType, cx, cy);

	if (nType == SIZE_MINIMIZED || nType == SIZE_MAXHIDE)
		return;

	// Calculate and remember the client area.
	displayArea.SetRect(0, 0, cx, cy);
	AdjustRect(FALSE, &displayArea);

	// Adjust the tabs' size.
	int num;
	if (GetRowCount() == 0 || (num = GetItemCount()) == 0)
		return;

	CRect rect;
	GetItemRect(0, &rect);

	CSize size;
	size.cx = (cx - 10) / num;
	size.cx = Max(1, size.cx);
	size.cy = rect.Height();

	SetItemSize(size);

	int labelWidth = size.cx - 30;
	labelWidth = Max(0, labelWidth);
	ResizeLabels(labelWidth);

	size.cx = 4;
	size.cy = 4;
	SetPadding(size);
}

void QTabCtrl::ResizeLabels(int displayWidth) {
	char buf[64];
	TC_ITEM tc_item;
	tc_item.mask = TCIF_TEXT;
	tc_item.pszText = buf;

	int i, num = GetItemCount();
	for(i = 0; i < num; i++) {
		strcpy(buf, pTabLabels[i]);
		SqueezeStr(displayWidth, strlen(buf), buf);
		SetItem(i, &tc_item);
	}
}

void QTabCtrl::SqueezeStr(int displayWidth, int strlen, LPSTR strBuf) {
	HDC hdc = ::GetDC(m_hWnd);
	SIZE size;

	GetTextExtentPoint32(hdc, strBuf, strlen, &size);
	if (size.cx <= displayWidth) {
		::ReleaseDC(m_hWnd, hdc);
		return;
	}
	
	while (strlen > 1) {
		strlen--;
		strBuf[strlen] = '\0';
		strcat(strBuf, "...");

		GetTextExtentPoint32(hdc, strBuf, strlen, &size);
		if (size.cx <= displayWidth) {
			::ReleaseDC(m_hWnd, hdc);
			return;
		}
	}

	strlen += 3;
	while (strlen > 0) {
		strlen--;
		strBuf[strlen] = '\0';

		GetTextExtentPoint32(hdc, strBuf, strlen, &size);
		if (size.cx <= displayWidth) {
			::ReleaseDC(m_hWnd, hdc);
			return;
		}
	}

	::ReleaseDC(m_hWnd, hdc);
	return;
}
