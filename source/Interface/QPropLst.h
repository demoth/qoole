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

#if !defined(AFX_QPROPLST_H__8CA1BA21_5E3B_11D2_BA3F_004005310168__INCLUDED_)
#define AFX_QPROPLST_H__8CA1BA21_5E3B_11D2_BA3F_004005310168__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// QPropLst.h : header file
//

// Call back procedure to invoke dlg box when
//  the "..." button is clicked in a property cell.
typedef CString *(*QPropLstDlgProc)(CString &);

// Structure to store information about the
//  prop cell being clicked on.
typedef struct {
	int type;
	CStringList lstItems;
	CStringList lstData;
	QPropLstDlgProc pDlgProc;
} QPropLstInfoStruct;

// Call back proc to determine type of property cell
typedef void (*QPropLstOnClickProc)(int, QPropLstInfoStruct *);

//======================================================================
// QPropertyList window
//======================================================================

class QPropertyList : public CListCtrl {
// Construction
public:
	QPropertyList();

// Attributes
public:
	int HitTestEx(CPoint &point, int *col) const;

// Operations
public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	BOOL SubclassWindow(HWND hWnd);

	void FillGrids(CStringList &propNameList, CStringList &valList);
	void RegisterProcs(QPropLstOnClickProc pOnClckProc, QPropLstDlgProc pDlgProc);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(QPropertyList)
	protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~QPropertyList();

	// Generated message map functions
protected:
	CEdit* EditSubLabel(int nItem, int nCol);
	CWnd* ShowInPlaceList(int nItem, int nCol, CStringList &lstItems);
	CWnd* ShowInPlaceBitFlagList(int nItem, int nCol,
								 CStringList &lstItems, CStringList &lstFlags);
	CWnd *ShowInPlaceDlgEdit(int nItem, int nCol);

	//{{AFX_MSG(QPropertyList)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	int rowHeight;
	CFont font;
	QPropLstOnClickProc m_pOnClickProc;
	QPropLstDlgProc m_pDlgProc;

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////
// Ctrls to support the QPropertyList class.

#define IDC_PROPLST_HEADER	1
#define IDC_PROPLST_EDIT	2
#define IDC_PROPLST_COMBO	3
#define IDC_PROPLST_BFCOMBO	4
#define IDC_PROPLST_DLGEDIT	5
#define IDC_PROPLST_DLGBTN	6

//======================================================================
// QInPlaceEdit Ctrl.
//======================================================================

class QInPlaceEdit : public CEdit {
// Construction
public:
	QInPlaceEdit(int iItem, int iSubItem, CString sInitText);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(QInPlaceEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~QInPlaceEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(QInPlaceEdit)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnNcDestroy();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	int m_iItem;
	int m_iSubItem;
	CString m_sInitText;
	BOOL    m_bESC;         // To indicate whether ESC key was pressed
};

//======================================================================
// QInPlaceChildEdit 
//======================================================================

class QInPlaceChildEdit : public CEdit {
// Construction
public:
	QInPlaceChildEdit(int nestLevel);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(QInPlaceChildEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~QInPlaceChildEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(QInPlaceChildEdit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	int nest;
};

//======================================================================
// QInPlaceCombo
//======================================================================

class QInPlaceCombo : public CWnd {
// Construction
public:
	QInPlaceCombo(int iItem, int iSubItem, CString sInitText,
				  CStringList *plstItems);

// Attributes
public:

// Operations
public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(QInPlaceCombo)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~QInPlaceCombo();

	// Generated message map functions
protected:
	//{{AFX_MSG(QInPlaceCombo)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnNcDestroy();
	//}}AFX_MSG
	
	static LRESULT CALLBACK EditSubClassWndProc(HWND hwnd, UINT iMsg,
												WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

protected:
	CComboBox	comboCtrl;
	QInPlaceChildEdit editCtrl;

	int     m_iItem;
	int     m_iSubItem;
	CString m_sInitText;
	CStringList m_lstItems;
	BOOL    m_bESC;                         // To indicate whether ESC key was pressed
};

//======================================================================
// QInPlaceBitFlagCombo window
//======================================================================

class QInPlaceBitFlagCombo : public QInPlaceCombo {
// Construction
public:
	QInPlaceBitFlagCombo(int iItem, int iSubItem, CString sInitText,
						 CStringList *plstItems, CStringList *plstFlags,
						 int itemHeight);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(QInPlaceBitFlagCombo)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~QInPlaceBitFlagCombo();

	// Generated message map functions
protected:
	//{{AFX_MSG(QInPlaceBitFlagCombo)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG

	afx_msg void OnSelChange();
	afx_msg void OnSelBitFlag();
	afx_msg void OnDropDown();
	afx_msg void OnCloseUp();

	DECLARE_MESSAGE_MAP()

protected:
	UINT *m_piFlags;
	char newFlag[16];
	int selNewFlag;
	int lbItemHeight;
};

//======================================================================
// QInPlaceDlgEdit
//======================================================================

class QInPlaceDlgEdit : public CWnd {
// Construction
public:
	QInPlaceDlgEdit(int iItem, int iSubItem,
					CString sInitText, QPropLstDlgProc pDlgProc);

// Attributes
public:

// Operations
public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(QInPlaceDlgEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~QInPlaceDlgEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(QInPlaceDlgEdit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnNcDestroy();
	afx_msg void OnClose();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	afx_msg void OnDlgBtnClicked(void);

	CButton dlgButton;
	QInPlaceChildEdit editWnd;
	QPropLstDlgProc pProc;
	int     m_iItem;
	int     m_iSubItem;
	CString m_sInitText;
	BOOL    m_bESC;                         // To indicate whether ESC key was pressed
};


/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QPROPLST_H__8CA1BA21_5E3B_11D2_BA3F_004005310168__INCLUDED_)
