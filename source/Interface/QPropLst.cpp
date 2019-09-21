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

// QPropLst.cpp : implementation file
//

#include "stdafx.h"
#include "LCommon.h"
#include "QPropLst.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//======================================================================
// QPropertyList
//======================================================================

QPropertyList::QPropertyList() {
	m_pOnClickProc = NULL;
	m_pDlgProc = NULL;

//	font.CreatePointFont(105, "Arial");
	font.CreatePointFont(75, "MS Sans Serif");
}

QPropertyList::~QPropertyList() {
}

BEGIN_MESSAGE_MAP(QPropertyList, CListCtrl)
	//{{AFX_MSG_MAP(QPropertyList)
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndLabelEdit)
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KEYUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////
// QPropertyList message handlers

BOOL QPropertyList::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) {
	dwStyle &= ~(LVS_ICON | LVS_LIST | LVS_SMALLICON);
	dwStyle |= (LVS_REPORT | LVS_NOCOLUMNHEADER |
				LVS_NOLABELWRAP | LVS_SINGLESEL | LVS_SHOWSELALWAYS);

	if (!CListCtrl::Create(dwStyle, rect, pParentWnd, nID))
		return FALSE;

	SetFont(&font);

	// Get rowHeight.
	SetRedraw(FALSE);

	CRect rowRect;
	InsertItem(0, "A");
	GetItemRect(0, &rowRect, LVIR_BOUNDS);
	rowHeight = rowRect.bottom - rowRect.top;
	DeleteItem(0);

	SetRedraw(TRUE);

	return TRUE;
}

BOOL QPropertyList::SubclassWindow(HWND hWnd) {
	if (!CWnd::SubclassWindow(hWnd))
		return FALSE;

	SetFont(&font);

	CRect rowRect;
	InsertItem(0, "A");
	GetItemRect(0, &rowRect, LVIR_BOUNDS);
	rowHeight = rowRect.bottom - rowRect.top;
	DeleteItem(0);

	// Refit the window height to be multiple of rowHeight.
	CRect rect;
	GetClientRect(&rect);
	int height = (rect.Height() + rowHeight / 2) / rowHeight * rowHeight;
	height -= rect.Height();

	GetWindowRect(&rect);
	GetParent()->ScreenToClient(&rect);
	rect.top -= height / 2;
	rect.bottom += height - (height / 2);
	MoveWindow(&rect);

	return TRUE;
}

void QPropertyList::FillGrids(CStringList &propNameList, CStringList &valList) {
	// ASSERT(propNameList.GetCount() > 0);
	ASSERT(propNameList.GetCount() == valList.GetCount());

	SetRedraw(FALSE);

	DeleteAllItems();

	int i, num = propNameList.GetCount();
	POSITION posName = propNameList.GetHeadPosition();
	POSITION posVal = valList.GetHeadPosition();

	for(i = 0; i < num; i++) {
		InsertItem(i, propNameList.GetNext(posName));
		SetItemText(i, 1, valList.GetNext(posVal));
	}

	SetRedraw(TRUE);
}

void QPropertyList::RegisterProcs(QPropLstOnClickProc pOnClckProc, QPropLstDlgProc pDlgProc) {
	m_pOnClickProc = pOnClckProc;
	m_pDlgProc = pDlgProc;
}

void QPropertyList::OnSize(UINT nType, int cx, int cy) {
	if (GetFocus() != this)
		SetFocus();

	SetRedraw(FALSE);
	CListCtrl::OnSize(nType, cx, cy);
	// SetColumnWidth(0, cx * 3 / 10);
	// SetColumnWidth(1, cx - (cx * 3 / 10));
	int width = cx - GetColumnWidth(0);
	SetColumnWidth(1, width);
	SetRedraw(TRUE);
}

BOOL QPropertyList::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) {
	HD_NOTIFY *pHDN = (HD_NOTIFY *) lParam;

	if (pHDN->hdr.code == HDN_ITEMCHANGINGW || pHDN->hdr.code == HDN_ITEMCHANGINGA) {
		if (pHDN->iItem == 0) {
			*pResult = FALSE;
			return TRUE;
		}
	}

	return CListCtrl::OnNotify(wParam, lParam, pResult);
}

void QPropertyList::OnPaint() {
	// First let the control do its default drawing.
	const MSG *msg = GetCurrentMessage();
	DefWindowProc(msg->message, msg->wParam, msg->lParam);

	// Draw the lines only for LVS_REPORT mode
	if ((GetStyle() & LVS_TYPEMASK) != LVS_REPORT)
		return;

	// Get the number of columns
	CClientDC dc(this );
	CHeaderCtrl* pHeader = (CHeaderCtrl *) GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();

	// The bottom of the header corresponds to the top of the line 
	CRect rect;
	pHeader->GetClientRect(&rect);
	int top = rect.bottom;

	// Now get the client rect so we know the line length and when to stop.
	GetClientRect(&rect);

	// The border of the column is offset by the horz scroll
	int borderx = 0 - GetScrollPos(SB_HORZ);
	for(int i = 0; i < nColumnCount && i < 1; i++) {
		// Get the next border
		borderx += GetColumnWidth(i);

		// if next border is outside client area, break out
		if (borderx >= rect.right)
			break;
		
		// Draw the line.
		dc.MoveTo(borderx - 1, top);
		dc.LineTo(borderx - 1, rect.bottom);
	}

	// Draw the horizontal grid lines

	// First get the height
	// if (!GetItemRect(0, &rect, LVIR_BOUNDS))
	// 	return;

	// int height = rect.bottom - rect.top;
	
	GetClientRect(&rect);
	int width = rect.right;

	for(i = 1; i <= GetCountPerPage(); i++) {
		dc.MoveTo(0, top + rowHeight * i);
		dc.LineTo(width, top + rowHeight * i);
	}

	// Do not call CListCtrl::OnPaint() for painting messages
}

// HitTestEx    - Determine the row index and column index for a point
// Returns      - the row index or -1 if point is not over a row
// point        - point to be tested.
// col          - to hold the column index
int QPropertyList::HitTestEx(CPoint &point, int *col) const {
	int colnum = 0;
	int row = HitTest (point, NULL);
        
	if (col) *col = 0;

	// Make sure that the ListView is in LVS_REPORT
	if ((GetWindowLong(m_hWnd, GWL_STYLE) & LVS_TYPEMASK) != LVS_REPORT)
		return row;

	// Get the top and bottom row visible
	row = GetTopIndex();
	int bottom = row + GetCountPerPage();
	if (bottom > GetItemCount())
		bottom = GetItemCount();
        
	// Get the number of columns
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();
		
	// Loop through the visible rows
	for (;row <=bottom;row++) {
		// Get bounding rect of item and check whether point falls in it.
		CRect rect;
		GetItemRect(row, &rect, LVIR_BOUNDS);
		// if (rect.PtInRect(point)) {
		if (point.y >= rect.top && point.y <= rect.bottom) {
			// Now find the column
			for (colnum = 0; colnum < nColumnCount; colnum++) {
				int colwidth = GetColumnWidth(colnum);
				if (point.x >= rect.left 
					&& point.x <= (rect.left + colwidth)) {
					if (col) *col = colnum;
					return row;
				}
				rect.left += colwidth;
			}
		}
	}
	return -1;
}

// EditSubLabel         - Start edit of a sub item label
// Returns              - Temporary pointer to the new edit control
// nItem                - The row index of the item to edit
// nCol                 - The column of the sub item.
CEdit* QPropertyList::EditSubLabel(int nItem, int nCol) {
	// The returned pointer should not be saved

	// Make sure that the item is visible
	if (!EnsureVisible (nItem, TRUE))
		return NULL;

	// Make sure that nCol is valid
	CHeaderCtrl* pHeader = (CHeaderCtrl*) GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();
	if (nCol >= nColumnCount || GetColumnWidth(nCol) < 5)
		return NULL;

	// Get the column offset
	int offset = 0;
	for (int i = 0; i < nCol; i++)
		offset += GetColumnWidth(i);

	CRect rect;
	GetItemRect (nItem, &rect, LVIR_BOUNDS);

	// Now scroll if we need to expose the column
	CRect rcClient;
	GetClientRect (&rcClient);
	if (offset + rect.left < 0 || offset + rect.left > rcClient.right) {
		CSize size;
		size.cx = offset + rect.left;
		size.cy = 0;
		Scroll (size);
		rect.left -= size.cx;
	}

	rect.left += offset - 1; // + 4;
	rect.right = rect.left + GetColumnWidth(nCol) + 3; //  - 3 ;
	if (rect.right > rcClient.right + 3)
		rect.right = rcClient.right + 3;
	rect.bottom += 1;

	DWORD dwStyle = WS_BORDER|WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL|ES_LEFT;

	CEdit *pEdit = new QInPlaceEdit(nItem, nCol, GetItemText(nItem, nCol));
	pEdit->Create(dwStyle, rect, this, IDC_PROPLST_EDIT);

	return pEdit;
}

// ShowInPlaceList              - Creates an in-place drop down list for any 
//                              - cell in the list view control
// Returns                      - A temporary pointer to the combo-box control
// nItem                        - The row index of the cell
// nCol                         - The column index of the cell
// lstItems                     - A list of strings to populate the control with
// nSel                         - Index of the initial selection in the drop down list
CWnd* QPropertyList::ShowInPlaceList(int nItem, int nCol, CStringList &lstItems) {
	// The returned pointer should not be saved

	// Make sure that the item is visible
	if (!EnsureVisible(nItem, TRUE))
		return NULL;

	// Make sure that nCol is valid 
	CHeaderCtrl* pHeader = (CHeaderCtrl *) GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();
	if (nCol >= nColumnCount || GetColumnWidth(nCol) < 10)
		return NULL;

	// Get the column offset
	int offset = 0;
	for (int i = 0; i < nCol; i++)
		offset += GetColumnWidth(i);

	CRect rect;
	GetItemRect(nItem, &rect, LVIR_BOUNDS);

	// Now scroll if we need to expose the column
	CRect rcClient;
	GetClientRect(&rcClient);
	if (offset + rect.left < 0 || offset + rect.left > rcClient.right) {
		CSize size;
		size.cx = offset + rect.left;
		size.cy = 0;
		Scroll(size);
		rect.left -= size.cx;
	}

	rect.top += 1;
	rect.left += offset;
	rect.right = rect.left + GetColumnWidth(nCol) + 3; // - 3;
	if (rect.right > rcClient.right + 3)
		rect.right = rcClient.right + 3;

	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST;
	QInPlaceCombo *pList = (QInPlaceCombo *)
		new QInPlaceCombo(nItem, nCol, GetItemText(nItem, nCol), &lstItems);
	pList->Create(dwStyle, rect, this, IDC_PROPLST_COMBO);

	return pList;
}

CWnd* QPropertyList::ShowInPlaceBitFlagList(int nItem, int nCol,
											CStringList &lstItems,
											CStringList &lstFlags) {
	// The returned pointer should not be saved

	// Make sure that the item is visible
	if (!EnsureVisible(nItem, TRUE))
		return NULL;

	// Make sure that nCol is valid 
	CHeaderCtrl* pHeader = (CHeaderCtrl *) GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();
	if (nCol >= nColumnCount || GetColumnWidth(nCol) < 10)
		return NULL;

	// Get the column offset
	int offset = 0;
	for (int i = 0; i < nCol; i++)
		offset += GetColumnWidth(i);

	CRect rect;
	GetItemRect(nItem, &rect, LVIR_BOUNDS);

	// Now scroll if we need to expose the column
	CRect rcClient;
	GetClientRect(&rcClient);
	if (offset + rect.left < 0 || offset + rect.left > rcClient.right) {
		CSize size;
		size.cx = offset + rect.left;
		size.cy = 0;
		Scroll(size);
		rect.left -= size.cx;
	}

	rect.top += 1;
	rect.left += offset;
	rect.right = rect.left + GetColumnWidth(nCol) + 3; // - 3;
	if (rect.right > rcClient.right + 3)
		rect.right = rcClient.right + 3;

	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST;
	QInPlaceBitFlagCombo *pList = (QInPlaceBitFlagCombo *)
		new QInPlaceBitFlagCombo(nItem, nCol, GetItemText(nItem, nCol),
								 &lstItems, &lstFlags, rowHeight);
	pList->Create(dwStyle, rect, this, IDC_PROPLST_BFCOMBO);

	return pList;
}

CWnd *QPropertyList::ShowInPlaceDlgEdit(int nItem, int nCol) {
	// The returned pointer should not be saved

	// Make sure that the item is visible
	if (!EnsureVisible (nItem, TRUE))
		return NULL;

	// Make sure that nCol is valid
	CHeaderCtrl* pHeader = (CHeaderCtrl*) GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();
	if (nCol >= nColumnCount || GetColumnWidth(nCol) < 5)
		return NULL;

	// Get the column offset
	int offset = 0;
	for (int i = 0; i < nCol; i++)
		offset += GetColumnWidth(i);

	CRect rect;
	GetItemRect (nItem, &rect, LVIR_BOUNDS);

	// Now scroll if we need to expose the column
	CRect rcClient;
	GetClientRect (&rcClient);
	if (offset + rect.left < 0 || offset + rect.left > rcClient.right) {
		CSize size;
		size.cx = offset + rect.left;
		size.cy = 0;
		Scroll (size);
		rect.left -= size.cx;
	}

	rect.left += offset - 1; // + 4;
	rect.right = rect.left + GetColumnWidth(nCol) + 3; //  - 3 ;
	if (rect.right > rcClient.right + 3)
		rect.right = rcClient.right + 3;
	rect.bottom += 1;

	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_BORDER;

	// Create the edit ctrl
	QInPlaceDlgEdit *pDlgEdit =
		new QInPlaceDlgEdit(nItem, nCol, GetItemText(nItem, nCol), m_pDlgProc);
	pDlgEdit->Create(dwStyle, rect, this, IDC_PROPLST_DLGEDIT);

	return pDlgEdit;
}

void QPropertyList::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) {
	if (GetFocus() != this)
		SetFocus();
	CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}

void QPropertyList::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) {
	if (GetFocus() != this)
		SetFocus();
	CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}

void QPropertyList::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult) {
    LV_DISPINFO *plvDispInfo = (LV_DISPINFO *) pNMHDR;
	LV_ITEM *plvItem = &plvDispInfo->item;

	if (plvItem->pszText != NULL)
		SetItemText(plvItem->iItem, plvItem->iSubItem, plvItem->pszText);

	UINT flag =  LVIS_SELECTED | LVIS_FOCUSED;
	SetItemState(plvItem->iItem, flag, flag);
	
	*pResult = 0;
}

void QPropertyList::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) {
	CListCtrl::OnKeyUp(nChar, nRepCnt, nFlags);
}

void QPropertyList::OnLButtonDblClk(UINT nFlags, CPoint point) {
	int index, colnum;
	if ((index = HitTestEx(point, &colnum)) != -1 && 
		colnum == 0 && index == GetItemCount() - 1) {
		EditSubLabel(index, colnum);
		return;
	}

	OnLButtonDown(nFlags, point);
}

void QPropertyList::OnLButtonDown(UINT nFlags, CPoint point) {
	CListCtrl::OnLButtonDown(nFlags, point);

	int index, colnum;
	if ((index = HitTestEx(point, &colnum)) != -1) {
		UINT flag = LVIS_FOCUSED;

 		if ((GetItemState(index, flag) & flag) == flag &&
			colnum > 0 && index < GetItemCount() - 1) {
			QPropLstInfoStruct info;
			info.type = -1;

			if (m_pOnClickProc != NULL)
				(*m_pOnClickProc)(index, &info);

			if (m_pOnClickProc == NULL || info.type == 0) {
				// Simple in place edit
				EditSubLabel(index, colnum);
			}
			else if (info.type == 1) {
				// In place drop down box.
				ShowInPlaceList(index, colnum, info.lstItems);
			}
			else if (info.type == 2) {
				// In place drop down bit flag box.
				ShowInPlaceBitFlagList(index, colnum, info.lstItems, info.lstData);
			}
			else if (info.type == 3) {
				// Dialog box button.
				ShowInPlaceDlgEdit(index, colnum);
			}
 		}
 		else {
			SetItemState(index, LVIS_SELECTED | LVIS_FOCUSED ,
						 LVIS_SELECTED | LVIS_FOCUSED);
		}
	}

	for(int i = 0; i < GetItemCount(); i++) {
		if (GetItemState(i, LVIS_FOCUSED) && !GetItemState(i, LVIS_SELECTED)) {
			SetItemState(i, LVIS_FOCUSED | LVIS_SELECTED,  LVIS_FOCUSED | LVIS_SELECTED);
		}
	}
}

//======================================================================
// QInPlaceEdit
//======================================================================

QInPlaceEdit::QInPlaceEdit(int iItem, int iSubItem, CString sInitText)
			 :m_sInitText(sInitText) {
	m_iItem = iItem;
	m_iSubItem = iSubItem;
	m_bESC = FALSE;
}

QInPlaceEdit::~QInPlaceEdit() {
}


BEGIN_MESSAGE_MAP(QInPlaceEdit, CEdit)
	//{{AFX_MSG_MAP(QInPlaceEdit)
	ON_WM_KILLFOCUS()
	ON_WM_NCDESTROY()
	ON_WM_CHAR()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////
// QInPlaceEdit message handlers

int QInPlaceEdit::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Set the proper font
	CFont* font = GetParent()->GetFont();
	SetFont(font);
	LimitText(256);

	SetWindowText(m_sInitText);
	SetFocus();
	SetSel(0, -1);
	return 0;
}

BOOL QInPlaceEdit::PreTranslateMessage(MSG* pMsg) {
	// Pass some of the keys directly to the ctrl window
	//  by passing accelerator mapping of the main frame.
	if (pMsg->message == WM_KEYDOWN) {
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_DELETE ||
			pMsg->wParam == VK_ESCAPE || GetKeyState(VK_CONTROL)) {
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			return TRUE;                    // DO NOT process further
		}
	}

	return CEdit::PreTranslateMessage(pMsg);
}

void QInPlaceEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) {
	if (nChar == VK_ESCAPE || nChar == VK_RETURN) {
		if (nChar == VK_ESCAPE)
			m_bESC = TRUE;
		GetParent()->SetFocus();
		return;
	}

	CEdit::OnChar(nChar, nRepCnt, nFlags);
}

void QInPlaceEdit::OnKillFocus(CWnd* pNewWnd) {
	CEdit::OnKillFocus(pNewWnd);

	CString str;
	GetWindowText(str);

	// Send Notification to parent of ListView ctrl
	LV_DISPINFO dispinfo;
	dispinfo.hdr.hwndFrom = GetParent()->m_hWnd;
	dispinfo.hdr.idFrom = GetDlgCtrlID();
	dispinfo.hdr.code = LVN_ENDLABELEDIT;

	dispinfo.item.mask = LVIF_TEXT;
	dispinfo.item.iItem = m_iItem;
	dispinfo.item.iSubItem = m_iSubItem;
	dispinfo.item.pszText = m_bESC ? NULL : LPTSTR((LPCTSTR)str);
	dispinfo.item.cchTextMax = str.GetLength();

	GetParent()->GetParent()->SendMessage(WM_NOTIFY, GetParent()->GetDlgCtrlID(), 
										  (LPARAM)&dispinfo);
	
	DestroyWindow();
}

void QInPlaceEdit::OnNcDestroy() {
	CEdit::OnNcDestroy();

	delete this;
}

//======================================================================
// QInPlaceChildEdit
//======================================================================

QInPlaceChildEdit::QInPlaceChildEdit(int nestLevel) {
	nest = nestLevel;
}

QInPlaceChildEdit::~QInPlaceChildEdit() {
}

BEGIN_MESSAGE_MAP(QInPlaceChildEdit, CEdit)
	//{{AFX_MSG_MAP(QInPlaceChildEdit)
	ON_WM_CHAR()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////
// QInPlaceChildEdit message handlers

BOOL QInPlaceChildEdit::PreTranslateMessage(MSG* pMsg) {
	if (pMsg->message == WM_KEYDOWN) {
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_DELETE ||
			pMsg->wParam == VK_ESCAPE || GetKeyState(VK_CONTROL)) {
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			return TRUE;                            // DO NOT process further
		}
	}
	return CEdit::PreTranslateMessage(pMsg);
}

void QInPlaceChildEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) {
	if (nChar == VK_ESCAPE || nChar == VK_RETURN) {
		int i = nest;
		CWnd *pWnd = this;

		while (i-- > 0) {
			pWnd = pWnd->GetParent();
			ASSERT(pWnd != NULL);
		}

		pWnd->SendMessage(WM_CHAR, nChar);
		return;
	}

	CEdit::OnChar(nChar, nRepCnt, nFlags);
}

void QInPlaceChildEdit::OnKillFocus(CWnd* pNewWnd) {
	CEdit::OnKillFocus(pNewWnd);

	HWND hwnd = (pNewWnd ? pNewWnd->m_hWnd : NULL);

	int i = nest;
	CWnd *pWnd = this;

	while (i-- > 0) {
		pWnd = pWnd->GetParent();
		ASSERT(pWnd != NULL);
	}

	pWnd->SendMessage(WM_KILLFOCUS, (WPARAM) hwnd);
}

//======================================================================
// QInPlaceCombo
//======================================================================

QInPlaceCombo::QInPlaceCombo(int iItem, int iSubItem, CString sInitText,
							 CStringList *plstItems) 
			  :editCtrl(2) {
	m_iItem = iItem;
	m_iSubItem = iSubItem;
	m_sInitText = sInitText;
	m_lstItems.AddTail(plstItems);
	m_bESC = FALSE;
}

QInPlaceCombo::~QInPlaceCombo() {
}

BEGIN_MESSAGE_MAP(QInPlaceCombo, CWnd)
	//{{AFX_MSG_MAP(QInPlaceCombo)
	ON_WM_CREATE()
	ON_WM_KILLFOCUS()
	ON_WM_CHAR()
	ON_WM_NCDESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////
// QInPlaceCombo message handlers

BOOL QInPlaceCombo::PreCreateWindow(CREATESTRUCT& cs) {
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;
	cs.style &= ~(WS_HSCROLL | WS_VSCROLL);
	return TRUE;
}


BOOL QInPlaceCombo::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) {
	return CWnd::Create(NULL, "", dwStyle, rect, pParentWnd, nID);
}

int QInPlaceCombo::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_VSCROLL |
					CBS_DROPDOWN | CBS_AUTOHSCROLL;
	CRect rect(-2, -3, lpCreateStruct->cx, lpCreateStruct->cy * 7);
	comboCtrl.Create(dwStyle, rect, this, IDC_PROPLST_COMBO);
	comboCtrl.LimitText(256);

	for (POSITION pos = m_lstItems.GetHeadPosition(); pos != NULL;)
		comboCtrl.AddString((LPCTSTR)(m_lstItems.GetNext(pos)));

	// Set the proper font.
	CFont* font = GetParent()->GetFont();
	comboCtrl.SetFont(font);

	CEdit *ctrlWnd = (CEdit *) comboCtrl.GetWindow(GW_CHILD);
	ctrlWnd->ModifyStyle(0, ES_LEFT | ES_AUTOHSCROLL);
	ctrlWnd->SetFont(font);
	ctrlWnd->SetWindowText(m_sInitText);
	ctrlWnd->SetSel(0, -1);
	ctrlWnd->SetFocus();
	editCtrl.SubclassWindow(ctrlWnd->m_hWnd);

	// Set combobox's size.
	comboCtrl.SetItemHeight(-1, lpCreateStruct->cy);
	comboCtrl.SetHorizontalExtent(lpCreateStruct->cx);

	return 0;
}

void QInPlaceCombo::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) {
	if (nChar == VK_ESCAPE || nChar == VK_RETURN) {
		if (nChar == VK_ESCAPE)
			m_bESC = TRUE;
		GetParent()->SetFocus();
		return;
	}
        
	CWnd::OnChar(nChar, nRepCnt, nFlags);
}

void QInPlaceCombo::OnKillFocus(CWnd* pNewWnd) {
	CWnd::OnKillFocus(pNewWnd);
        
	CString str;
	editCtrl.GetWindowText(str);

	// Send Notification to parent of ListView ctrl
	LV_DISPINFO dispinfo;
	dispinfo.hdr.hwndFrom = GetParent()->m_hWnd;
	dispinfo.hdr.idFrom = GetDlgCtrlID();
	dispinfo.hdr.code = LVN_ENDLABELEDIT;
		
	dispinfo.item.mask = LVIF_TEXT;
	dispinfo.item.iItem = m_iItem;
	dispinfo.item.iSubItem = m_iSubItem;
	dispinfo.item.pszText = m_bESC ? NULL : LPTSTR((LPCTSTR)str);
	dispinfo.item.cchTextMax = str.GetLength();

	GetParent()->GetParent()->SendMessage(WM_NOTIFY, GetParent()->GetDlgCtrlID(), (LPARAM)&dispinfo);

	PostMessage(WM_CLOSE);
}

void QInPlaceCombo::OnNcDestroy() {
	CWnd::OnNcDestroy();
	delete this;
}

//========================================
// QInPlaceBitFlagCombo
//========================================

QInPlaceBitFlagCombo::QInPlaceBitFlagCombo(int iItem, int iSubItem, CString sInitText,
										   CStringList *plstItems, CStringList *plstFlags,
										   int itemHeight)
					 :QInPlaceCombo(iItem, iSubItem, sInitText, plstItems) {

	ASSERT(plstItems->GetCount() == plstFlags->GetCount());
	ASSERT(plstFlags->GetCount() > 0);
	m_piFlags = new UINT[plstFlags->GetCount()];
	POSITION pos = plstFlags->GetHeadPosition();
	for (int i = 0;  pos != NULL; i++) {
		bool ok = Str2Int((LPCTSTR)plstFlags->GetNext(pos), m_piFlags[i]);
		ASSERT(ok);
	}

	newFlag[0] = '\0';
	selNewFlag = -1;
	lbItemHeight = itemHeight;
}

QInPlaceBitFlagCombo::~QInPlaceBitFlagCombo() {
	delete [] m_piFlags;
}

BEGIN_MESSAGE_MAP(QInPlaceBitFlagCombo, CWnd)
	//{{AFX_MSG_MAP(QInPlaceBitFlagCombo)
	ON_WM_CREATE()
	ON_WM_MEASUREITEM()
	ON_WM_DRAWITEM()
	ON_WM_NCDESTROY()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_PROPLST_COMBO, OnSelChange)
	ON_CBN_SELENDOK(IDC_PROPLST_COMBO, OnSelBitFlag)
	ON_CBN_DROPDOWN(IDC_PROPLST_COMBO, OnDropDown)
	ON_CBN_CLOSEUP(IDC_PROPLST_COMBO, OnCloseUp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// QInPlaceBitFlagCombo message handlers

int QInPlaceBitFlagCombo::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_VSCROLL |
					CBS_DROPDOWN | CBS_AUTOHSCROLL | // ;
					CBS_OWNERDRAWFIXED | CBS_HASSTRINGS;
	CRect rect(-2, -3, lpCreateStruct->cx, lpCreateStruct->cy * 7);
	comboCtrl.Create(dwStyle, rect, this, IDC_PROPLST_COMBO);
	comboCtrl.LimitText(16);

	for (POSITION pos = m_lstItems.GetHeadPosition(); pos != NULL;)
		comboCtrl.AddString((LPCTSTR)(m_lstItems.GetNext(pos)));

	// Set the proper font.
	CFont* font = GetParent()->GetFont();
	comboCtrl.SetFont(font);

#ifdef _DEBUG
	// Make sure the text is a valid number.
	UINT textVal;
	ASSERT(Str2Int(m_sInitText, textVal));
#endif

	CEdit *ctrlWnd = (CEdit *) comboCtrl.GetWindow(GW_CHILD);
	ctrlWnd->ModifyStyle(0, ES_LEFT | ES_AUTOHSCROLL);
	ctrlWnd->SetFont(font);
	ctrlWnd->SetWindowText(m_sInitText);
	ctrlWnd->SetSel(0, -1);
	ctrlWnd->SetFocus();
	editCtrl.SubclassWindow(ctrlWnd->m_hWnd);

	// Set combobox's size.
	comboCtrl.SetItemHeight(-1, lpCreateStruct->cy);
	comboCtrl.SetHorizontalExtent(lpCreateStruct->cx);

	return 0;
}

void QInPlaceBitFlagCombo::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct) {
	lpMeasureItemStruct->itemHeight = lbItemHeight;
}

void QInPlaceBitFlagCombo::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) {
	ASSERT(nIDCtl == IDC_PROPLST_COMBO);
	ASSERT(lpDrawItemStruct->itemID < (unsigned int) m_lstItems.GetCount());

	if (lpDrawItemStruct->itemID == -1)
		return;

	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	// Draw the check box.
	int x, y;
	x = lpDrawItemStruct->rcItem.left + 1;
	y = lpDrawItemStruct->rcItem.top;

	CString textBuf;
	editCtrl.GetWindowText(textBuf);

	UINT editFlags;
	Str2Int((LPCTSTR) textBuf, editFlags);

	bool checked = ((editFlags & m_piFlags[lpDrawItemStruct->itemID]) != 0);

	CRect rect(x + 1, y + 1, x + lbItemHeight - 1, y + lbItemHeight - 1);
	pDC->DrawFrameControl(&rect, DFC_BUTTON, DFCS_BUTTONCHECK |
						  (checked ? DFCS_CHECKED : 0));

	x += lbItemHeight + 1;
	rect = lpDrawItemStruct->rcItem;
	rect.left = x;

	// Set the right color.
	COLORREF clrForeGround = pDC->SetTextColor(
		::GetSysColor(lpDrawItemStruct->itemState & ODS_SELECTED ?
					  COLOR_HIGHLIGHTTEXT : COLOR_WINDOWTEXT));
	COLORREF clrBackGround = pDC->SetBkColor(
		::GetSysColor(lpDrawItemStruct->itemState & ODS_SELECTED ?
					  COLOR_HIGHLIGHT : COLOR_WINDOW));

	// Draw the text.
	CString textStr;
	comboCtrl.GetLBText(lpDrawItemStruct->itemID, textStr);
	pDC->ExtTextOut(x, y, ETO_CLIPPED | ETO_OPAQUE,
					&rect, textStr, NULL);

	// Restore prevoius colors.
	pDC->SetTextColor(clrForeGround);
	pDC->SetBkColor(clrBackGround);

	// Draw the focus.
	if (lpDrawItemStruct->itemState & ODS_FOCUS)
		pDC->DrawFocusRect(&lpDrawItemStruct->rcItem);
}

void QInPlaceBitFlagCombo::OnSelBitFlag() {
	UINT flags;
	char textBuf[16];

	editCtrl.GetWindowText(textBuf, 16);
	if (!Str2Int(textBuf, flags))
		Str2Int(newFlag, flags);

	int selIndex = comboCtrl.GetCurSel();

	if (selIndex != -1 && comboCtrl.GetDroppedState()) {
		// Selection changed from mouse input.
		ASSERT(selIndex < m_lstItems.GetCount());

		UINT mask = flags & m_piFlags[selIndex];
		mask ^= m_piFlags[selIndex];

		flags &= ~(m_piFlags[selIndex]);
		flags |= mask;
	}

	sprintf(newFlag, "%d", flags);
	selNewFlag = m_lstItems.GetCount();
}


void QInPlaceBitFlagCombo::OnCloseUp() {
	// Content moved into OnSelChange()
	//  which will be called after OnCloseUp.
}

void QInPlaceBitFlagCombo::OnSelChange() {
	// Redisplay the right text.
	if (selNewFlag != -1) {
		// Add the new text into list box.
		int index = m_lstItems.GetCount();
		ASSERT(index == selNewFlag);

		if (comboCtrl.GetCount() > index)
			comboCtrl.DeleteString(index);
		int i = comboCtrl.AddString(newFlag);
		ASSERT(i == index);

		// Set the cur selection.
		comboCtrl.SetCurSel(index);
	}
}

void QInPlaceBitFlagCombo::OnDropDown() {
	// Clear the selection.
	if (selNewFlag != -1) {
		// Delete the previous text from list box.
		comboCtrl.DeleteString(selNewFlag);
		selNewFlag = -1;
		editCtrl.SetWindowText(newFlag);
	}
}

void QInPlaceBitFlagCombo::OnKillFocus(CWnd* pNewWnd) {
	CString textBuf;
	editCtrl.GetWindowText(textBuf);

	UINT flag;
	if (!Str2Int((LPCTSTR) textBuf, flag)) {
		// Not a valid number.
		if (!Str2Int(newFlag, flag))
			sprintf(newFlag, "%s", (LPCTSTR) m_sInitText);

		// Force edit ctrl's text back to an old valid one.
		int index = comboCtrl.AddString(newFlag);
		comboCtrl.SetCurSel(index);
	}

	QInPlaceCombo::OnKillFocus(pNewWnd);
}

//======================================================================
// QInPlaceDlgEdit
//======================================================================

QInPlaceDlgEdit::QInPlaceDlgEdit(int iItem, int iSubItem, CString sInitText,
								 QPropLstDlgProc pDlgProc)
				:editWnd(1) {
	m_iItem = iItem;
	m_iSubItem = iSubItem;
	m_sInitText = sInitText;
	m_bESC = FALSE;
	pProc = pDlgProc;
}

QInPlaceDlgEdit::~QInPlaceDlgEdit() {
}

BEGIN_MESSAGE_MAP(QInPlaceDlgEdit, CWnd)
	//{{AFX_MSG_MAP(QInPlaceDlgEdit)
	ON_WM_CHAR()
	ON_WM_NCDESTROY()
	ON_WM_CLOSE()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_PROPLST_DLGBTN, OnDlgBtnClicked)
END_MESSAGE_MAP()

////////////////////////////////////////
// QInPlaceDlgEdit message handlers

BOOL QInPlaceDlgEdit::Create(DWORD dwStyle, const RECT &rect, CWnd *pParentWnd, UINT nID) {
	
	if (!CWnd::Create(NULL, "", dwStyle, rect, pParentWnd, nID))
		return FALSE;

	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;
	DWORD style;
	CRect ctrlRect;

	// Create the edit ctrl.
	style = WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL;
	ctrlRect.SetRect(0, 0, width - height, height);
	editWnd.Create(style, ctrlRect, this, IDC_PROPLST_EDIT);
	CFont* font = GetParent()->GetFont();
	editWnd.SetFont(font);
	editWnd.LimitText(256);
	editWnd.SetWindowText(m_sInitText);
	editWnd.SetSel(0, -1);
	editWnd.SetFocus();

	// Create the button.
	ctrlRect.SetRect(width - height, 0, width - 1, height - 1);
	ctrlRect.left = ctrlRect.right - height;
	style = WS_CHILD | WS_VISIBLE | BS_NOTIFY;
	dlgButton.Create("...", style, ctrlRect, this, IDC_PROPLST_DLGBTN);
	// dlgButton.ModifyStyleEx(0, WS_EX_CLIENTEDGE | WS_EX_STATICEDGE |  WS_EX_WINDOWEDGE /* | WS_EX_DLGMODALFRAME */);
	style = dlgButton.GetButtonStyle() & ~(BS_DEFPUSHBUTTON | WS_TABSTOP);
	dlgButton.SetButtonStyle(style | BS_NOTIFY);
	dlgButton.ModifyStyle(0, BS_NOTIFY);

	return TRUE;
}

BOOL QInPlaceDlgEdit::PreTranslateMessage(MSG* pMsg) {
	if (pMsg->message == WM_KEYDOWN) {
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE) {
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			return TRUE;                            // DO NOT process further
		}
	}

	return CWnd::PreTranslateMessage(pMsg);
}

void QInPlaceDlgEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) {
	if (nChar == VK_ESCAPE || nChar == VK_RETURN) {
		if (nChar == VK_ESCAPE)
			m_bESC = TRUE;
		GetParent()->SetFocus();
		PostMessage(WM_CLOSE);
		return;
	}
        
	CWnd::OnChar(nChar, nRepCnt, nFlags);
}

BOOL QInPlaceDlgEdit::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) {
	NMHDR *pNHDR = (NMHDR *) lParam;
	if (wParam == IDC_PROPLST_DLGBTN && pNHDR->code == NM_SETFOCUS) {
		SetFocus();
		return TRUE;
	}
	
	return CWnd::OnNotify(wParam, lParam, pResult);
}

void QInPlaceDlgEdit::OnDlgBtnClicked(void) {
	UINT style = dlgButton.GetButtonStyle() & ~BS_DEFPUSHBUTTON;
	dlgButton.SetButtonStyle(style);
	editWnd.GetWindowText(m_sInitText);
	if ((*pProc)(m_sInitText) != NULL)
		editWnd.SetWindowText(m_sInitText);
	editWnd.SetFocus();
	dlgButton.SetButtonStyle(style);
}

void QInPlaceDlgEdit::OnKillFocus(CWnd* pNewWnd) {
	CWnd::OnKillFocus(pNewWnd);

	if (pNewWnd == NULL || pNewWnd->GetParent() != this)
		PostMessage(WM_CLOSE);
}

void QInPlaceDlgEdit::OnClose() {
	CString str;
	editWnd.GetWindowText(str);

	// Send Notification to parent of ListView ctrl
	LV_DISPINFO dispinfo;
	dispinfo.hdr.hwndFrom = GetParent()->m_hWnd;
	dispinfo.hdr.idFrom = GetDlgCtrlID();
	dispinfo.hdr.code = LVN_ENDLABELEDIT;
		
	dispinfo.item.mask = LVIF_TEXT;
	dispinfo.item.iItem = m_iItem;
	dispinfo.item.iSubItem = m_iSubItem;
	dispinfo.item.pszText = m_bESC ? NULL : LPTSTR((LPCTSTR)str);
	dispinfo.item.cchTextMax = str.GetLength();

	GetParent()->GetParent()->SendMessage(WM_NOTIFY, GetParent()->GetDlgCtrlID(), (LPARAM)&dispinfo);

	CWnd::OnClose();
}

void QInPlaceDlgEdit::OnNcDestroy() {
	CWnd::OnNcDestroy();
	delete this;
}

