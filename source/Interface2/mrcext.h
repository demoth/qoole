//  MRCEXT: Micro Focus Extension DLL for MFC 2.1+
// Copyright (C)1994-5  Micro Focus Inc, 2465 East Bayshore Rd, Palo Alto, CA 94303.
// 
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation. In addition, you may also charge for any
//  application using MRCEXT, and are under no obligation to supply source
//  code. You must accredit Micro Focus Inc in the "About Box", or banner
//  of your application. 
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should also have received a copy of the GNU General Public License with this
//  software, also indicating additional rights you have when using MRCEXT.  
//
//
/////////////////////////////////////////////////////////////////////////////////////////
// MRCEXT.H
// Header file for MRCEXT.DLL.
// (C)1995 Micro Focus Ltd.
/////////////////////////////////////////////////////////////////////////////////////////
// $Date:   12 Jan 1998 17:33:34  $
// $Revision:   1.11  $
// $Author:   MRC  $
//
// The following macros affect the compilation of this file.
// _MRC_NOFORCE_LIBS   = if defined, .libs are not automatically included
//
// if you've not included AFXCMN.H, the sizeable toolbar controls are not available.

#ifndef __MRCEXT_H__
#define __MRCEXT_H__

#define MRCEXT_EXT_CLASS

/*
// #define _AFX_NO_OCC_SUPPORT

// #ifndef MRCEXT_EXT_CLASS
// #define MRCEXT_EXT_CLASS AFX_CLASS_IMPORT

// #ifndef _MRCEXT_NOFORCELIBS                     // set when part of MRCEXT
//#undef AFX_DATA
//#define AFX_DATA AFX_DATA_IMPORT

#ifdef _DEBUG
#pragma comment(lib, "mrcext4d.lib")
#else
#pragma comment(lib, "mrcext4.lib")
#endif          //_DEBUG
#endif          // _MRC_NOFORCE_LIBS
#endif                  // MRCEXT_EXT_CLASS
*/
// some constants that don't always seem to be in commctrl.h
#ifndef TPM_RETURNCMD
#define TPM_RETURNCMD 0x0100L
#endif 



// Window arangement positions
#define CBRS_ARRANGE_TOPLEFT    1
#define CBRS_ARRANGE_TOPRIGHT   2
#define CBRS_ARRANGE_BOTTOMLEFT 4
#define CBRS_ARRANGE_BOTTOMRIGHT 8

#define CBRS_ARRANGE_LEFT       ( CBRS_ARRANGE_TOPLEFT  | CBRS_ARRANGE_BOTTOMLEFT )
#define CBRS_ARRANGE_TOP    ( CBRS_ARRANGE_TOPRIGHT | CBRS_ARRANGE_TOPLEFT )


#define WM_ADDCONTEXTMENUITEMS  (WM_USER + 1)   // Add to context menu message
#define WM_USER_AFTERFLOAT_MSG  (WM_USER + 2)

// forward references
class CMRCSizeControlBar;
class CSplitterRect;
class CDragDockContext;
class CMRCFrameWndSizeDock;
class CMRCMDIFrameWndSizeDock;

//////////////////////////////////////////////////////////////////////////////////////////////
// cut-down of MFC's AUX_DATA structure
struct MRC_AUX_DATA
{
	BOOL    bWin4;
    int         cxBorder2, cyBorder2;
    COLORREF clrBtnFace, clrBtnShadow, clrBtnHilite;
    COLORREF clrBtnText, clrWindowFrame;
    HCURSOR hcurWait, hcurArrow, hcurSizeNS, hcurSizeWE;
    HBRUSH  hbrBtnHilite, hbrBtnShadow;

     MRC_AUX_DATA();                 // constructor does the initialization automatically
    ~MRC_AUX_DATA();
	void MRC_AUX_DATA::UpdateSysColors();
};

#define CX_SPLIT        6               // dimensions of splitter bars
#define CY_SPLIT        6
#define CX_BORDER       1
#define CY_BORDER       1

//-------------------------------------------------------------------
inline BOOL SetWindowSize(CWnd * pWnd, CSize size)
// in-line function for setting window size, without changing Z-order, etc
//-------------------------------------------------------------------
{
	return pWnd->SetWindowPos(NULL, 0, 0, size.cx, size.cy, SWP_NOMOVE /* | SWP_NOREDRAW */ | SWP_NOZORDER);
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Registry functions
//////////////////////////////////////////////////////////////////////////////////////////////
LPVOID MRCGetProfileBinary(LPCTSTR lpszSection, LPCTSTR lpszEntry, int * pBytesRead = NULL);
BOOL MRCGetProfileBinary(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPVOID Buffer, DWORD nBufferSize);
BOOL MRCWriteProfileBinary(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPVOID Buffer, DWORD nBufferSize);


//////////////////////////////////////////////////////////////////////////////////////////////
// Resizing all child windows within a parent - used by sizeable dialog bar 
typedef void * GADGETRESIZEHANDLE;
GADGETRESIZEHANDLE MRCEXT_EXT_CLASS CreateGadgetResizeHandle(CWnd *pWnd);
void MRCEXT_EXT_CLASS DestroyGadgetResizeHandle(GADGETRESIZEHANDLE Handle);
void MRCEXT_EXT_CLASS ResizeGadgetsOnWindow(GADGETRESIZEHANDLE Handle, int cx, int cy);


//////////////////////////////////////////////////////////////////////////////////////////////
// Sizeable control bar - derive from this
//////////////////////////////////////////////////////////////////////////////////////////////

class MRCEXT_EXT_CLASS CMRCSizeControlBar : public CControlBar
{
// Attributes
private:
	static CObArray * m_parrAllocBars;              // array of dynamically allocated bars
public:

	CSize           m_FloatSize;           // size when floating
	CSize           m_HorzDockSize;                 // size when docked horizontal
	CSize           m_VertDockSize;                 // size when docked vertical
	//  - size reflects the current docked size of the window (I do nothing clever for stretched)
	//  - height is generally shared across the rows

    CPoint          m_FloatingPosition;             // floating position
	DWORD           m_dwAllowDockingState;  // saved enable style for allow docking on/off
    int             m_Style;                // style flags
    CSize           m_PrevSize;
    BOOL            m_bPrevFloating;

#define SZBARF_DESTROY_ON_CLOSE     1       // closing the floating window closes the control bar
#define SZBARF_AUTOTIDY             2               // keeps window in an array, so it can be deleted
										

#define SZBARF_STDMOUSECLICKS           4               // standard mouse handling for the menu
#define SZBARF_DLGAUTOSIZE                      8               // auto-size dialog bars
#define SZBARF_ALLOW_MDI_FLOAT     16           // allow bar to float in an MDI window


#define CBRS_MOVED_BY_USER       0x0001L    // set if dragged by user 
// Construction
public:
	DECLARE_DYNAMIC(CMRCSizeControlBar)
    CMRCSizeControlBar(int nStyle = SZBARF_STDMOUSECLICKS);

// Operations
public:
	static void TidyUp();
	BOOL Create(CWnd * pParent, LPCTSTR lpszTitle, UINT nID = 1,
			 DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_BOTTOM,
			   const RECT & rect = CFrameWnd::rectDefault);
	// CWnd-style create
	virtual BOOL Create( LPCTSTR lpszClassName, LPCTSTR lpszWindowName,
							DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	void SetSizeDockStyle(DWORD dwStyle);
    void EnableDocking(DWORD dwDockStyle);
		
	BOOL IsProbablyFloating();

	// command handlers for standard menus
	void OnHide();
	void OnToggleAllowDocking();
	void OnFloatAsMDI();
	afx_msg LONG OnAfterFloatMessage(UINT, LONG);
	
	// Overrides
	// ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CMRCSizeControlBar)
    //}}AFX_VIRTUAL

// Implementation
public:
    virtual ~CMRCSizeControlBar();

    virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);
    virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);

    // Generated message map functions

protected:
    // Overridable functions
    virtual void OnSizedOrDocked(int cx, int cy, BOOL bFloating, int flags);
	virtual LONG OnAddContextMenuItems(UINT wParam, LPARAM lParam);
	
    //{{AFX_MSG(CMRCSizeControlBar)
    afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnSetFocus( CWnd* pOldWnd );
    //}}AFX_MSG
	afx_msg LONG OnSetMessageString(WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP()
};

void ArrangeWindowsInWindow (CWnd * pParentWnd, CObArray & arrWnd, DWORD dwOrient);
void MRCEXT_EXT_CLASS MiniDockToClient(CRect & rect, BOOL bConvertToClient);

/////////////////////////////////////////////////////////////////////////////
// CMRCFrameWndSizeDock frame

class MRCEXT_EXT_CLASS  CMRCFrameWndSizeDock : public CFrameWnd
{
    DECLARE_DYNCREATE(CMRCFrameWndSizeDock)
protected:
    CMRCFrameWndSizeDock();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif
    void EnableDocking(DWORD dwDockStyle);
    void TileDockedBars(DWORD dwDockStyle = CBRS_ALIGN_ANY);
	void ArrangeFloatingBars(DWORD dwOrient);
	void ArrangeWindows(CObArray & arrWnd, DWORD dwOrient);
	void GetFloatingBars(CObArray & arrWnd);
	
	// save/restore bar state       
	void LoadSizeBarState(LPCTSTR pszProfileName);
	void SaveSizeBarState(LPCTSTR pszProfileName);
    void LoadBarState(LPCTSTR pszProfileName);
	void DestroyDynamicBars();
	void FloatControlBar(CControlBar* pBar, CPoint point, DWORD dwStyle = CBRS_ALIGN_TOP);
    void DockControlBarLeftOf(CControlBar* pBar, CControlBar* pLeftOf);

protected:        
	void SaveBarSizes(LPCTSTR pszSection, BOOL bSave);

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CMRCFrameWndSizeDock)
    public:
    //}}AFX_VIRTUAL

// Implementation
protected:
    virtual ~CMRCFrameWndSizeDock();

    // Generated message map functions
    //{{AFX_MSG(CMRCFrameWndSizeDock)
	afx_msg void OnSysColorChange();
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()

public:
};



// The CMRCMDIFrameWndSizeDock class is supplied for convenience. The major functions are simply
// accomplished by in-line casts to CMRCFrameWndSizeDock

class MRCEXT_EXT_CLASS  CMRCMDIFrameWndSizeDock : public CMDIFrameWnd
{
    DECLARE_DYNCREATE(CMRCMDIFrameWndSizeDock)

protected:
    CMRCMDIFrameWndSizeDock();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif
    void EnableDocking(DWORD dwDockStyle)
		{ ((CMRCFrameWndSizeDock *)this)->EnableDocking(dwDockStyle); };
    void TileDockedBars(DWORD dwDockStyle = CBRS_ALIGN_ANY)
		{ ((CMRCFrameWndSizeDock *)this)->TileDockedBars(dwDockStyle); };

	void ArrangeFloatingBars(DWORD dwOrient);
	void ArrangeWindows(CObArray & arrWnd, DWORD dwOrient);
		
	void GetFloatingBars(CObArray & arrWnd)
		{ ((CMRCFrameWndSizeDock *)this)->GetFloatingBars(arrWnd); };

	void LoadSizeBarState(LPCTSTR pszProfileName)
				{ ((CMRCFrameWndSizeDock *)this)->LoadSizeBarState(pszProfileName); };
			
	void SaveSizeBarState(LPCTSTR pszProfileName)
				{ ((CMRCFrameWndSizeDock *)this)->SaveSizeBarState(pszProfileName); };

	void FloatControlBarInMDIChild(CControlBar* pBar, CPoint point, DWORD dwStyle = CBRS_ALIGN_TOP);
	void UnFloatInMDIChild(CControlBar* pBar, CPoint point, DWORD dwStyle = CBRS_ALIGN_TOP);
	
	void FloatControlBar(CControlBar* pBar, CPoint point, DWORD dwStyle = CBRS_ALIGN_TOP)
				{ ((CMRCFrameWndSizeDock *)this)->FloatControlBar(pBar, point, dwStyle); };

    void DockControlBarLeftOf(CControlBar* pBar, CControlBar* pLeftOf)
				{ ((CMRCFrameWndSizeDock *)this)->DockControlBarLeftOf(pBar, pLeftOf); };

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CMRCMDIFrameWndSizeDock)
    public:
    //}}AFX_VIRTUAL

// Implementation
protected:
    virtual ~CMRCMDIFrameWndSizeDock();
	    // Generated message map functions
	//{{AFX_MSG(CMRCMDIFrameWndSizeDock)
	afx_msg void OnSysColorChange();
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()

public:
};



class MRCEXT_EXT_CLASS  CMRCMDIChildWndSizeDock : public CMDIChildWnd
{
    DECLARE_DYNCREATE(CMRCMDIChildWndSizeDock)

protected:
    CMRCMDIChildWndSizeDock();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:
    void EnableDocking(DWORD dwDockStyle)
		{ ((CMRCFrameWndSizeDock *)this)->EnableDocking(dwDockStyle); };
    void TileDockedBars(DWORD dwDockStyle = CBRS_ALIGN_ANY)
		{ ((CMRCFrameWndSizeDock *)this)->TileDockedBars(dwDockStyle); };

	void GetFloatingBars(CObArray & arrWnd)
		{ ((CMRCFrameWndSizeDock *)this)->GetFloatingBars(arrWnd); };

	void DestroyDynamicBars()
		{ ((CMRCFrameWndSizeDock *)this)->DestroyDynamicBars(); };

	void LoadSizeBarState(LPCTSTR pszProfileName)
				{ ((CMRCFrameWndSizeDock *)this)->LoadSizeBarState(pszProfileName); };
			
	void SaveSizeBarState(LPCTSTR pszProfileName)
				{ ((CMRCFrameWndSizeDock *)this)->SaveSizeBarState(pszProfileName); };

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CMRCMDIChildWndSizeDock)
    public:
    //}}AFX_VIRTUAL

// Implementation
protected:
    virtual ~CMRCMDIChildWndSizeDock();

    // Generated message map functions
    //{{AFX_MSG(CMRCMDIChildWndSizeDock)
	    // NOTE - the ClassWizard will add and remove member functions here.
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};



/////////////////////////////////////////////////////////////////////////////
// toolbar docking support



/////////////////////////////////////////////////////////////////////////////
// Subclassed CRectTracker. Used for resizing splitter windows, and CMRCSizeControlBars, too 
class MRCEXT_EXT_CLASS  CMRCRectTracker : public CRectTracker
{
public:
	CRect   m_LimitRect;                            // limiting rectangle - all movement must be within this.
	CRect   m_OrigRect;                                     // original position at start of track
	
	CMRCRectTracker();
	virtual void AdjustRect(int nHandle, LPRECT);
	virtual void DrawTrackerRect(LPCRECT lpRect, CWnd* pWndClipTo, CDC* pDC, CWnd* pWnd);
	BOOL TrackFromHitTest(int nHitTest, CWnd* pWnd, CPoint point,
				CWnd* pWndClipTo = NULL, BOOL bAllowInvert = FALSE);

// extra tracker styles...
#define RectTracker_OnlyMoveHorz                0x0100L                 // only move horizontally
#define RectTracker_OnlyMoveVert        0x0200L                 // only move vertically
};


// get number of system colors
int MRCEXT_EXT_CLASS GetNumberSystemColors();    
// standard function to center one window relative to another...
void MRCEXT_EXT_CLASS CenterWindowWithinParent(CWnd * pWnd, CWnd * pAlternate = NULL);
// border drawing functions
void MRCEXT_EXT_CLASS DrawBorderRaisedOuter(CDC *pDC, RECT * prect);
void MRCEXT_EXT_CLASS DrawBorderRaisedInner(CDC *pDC, RECT * prect);
void MRCEXT_EXT_CLASS DrawBorderSunkenOuter(CDC *pDC, RECT * prect);
void MRCEXT_EXT_CLASS DrawBorderSunkenInner(CDC *pDC, RECT * prect);
void MRCEXT_EXT_CLASS DrawRectBorder(CDC *pDC, RECT *prect, COLORREF crTopLeft, COLORREF crBottomRighta);

struct BITMAPCOLORMAP
{
	// use DWORD instead of RGBQUAD so we can compare two RGBQUADs easily
	DWORD rgbqFrom;         // use RGB_TO_RGBQUAD macro to initialize this
	int iSysColorTo;
};

void MRCEXT_EXT_CLASS ForceLayoutAdjust(CControlBar * pBar);

#undef AFX_DATA
#define AFX_DATA

#endif
