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

// MainFrm.h : interface of the QMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__8B1BCCF6_FCC8_11D1_8E6B_004005310168__INCLUDED_)
#define AFX_MAINFRM_H__8B1BCCF6_FCC8_11D1_8E6B_004005310168__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Objects.h"
#include "Selector.h"

#include "QooleDoc.h"
#include "QooleOp.h"

#include "QView.h"

#include "QCtrlBar.h"
#include "QTrView.h"
#include "QProcWnd.h"

#include "QPropSht.h"

#include "QColrWnd.h"

// Forward declaration.
class QViewsState;
struct MenuEntry;

class QMainFrame : public CMRCMDIFrameWndSizeDock {
	DECLARE_DYNAMIC(QMainFrame)

friend class QViewsState;

public:
	QMainFrame();
	virtual ~QMainFrame();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(QMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

#ifdef _DEBUG
public:
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	//{{AFX_MSG(QMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnEditRedo();
	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
	afx_msg void OnEditHollow();
	afx_msg void OnUpdateEditHollow(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnUpdateObjDelete(CCmdUI* pCmdUI);
	afx_msg void OnObjectGroupObjs();
	afx_msg void OnUpdateObjectGroupObjs(CCmdUI* pCmdUI);
	afx_msg void OnObjectUngroupObjs();
	afx_msg void OnUpdateObjectUngroupObjs(CCmdUI* pCmdUI);
	afx_msg void OnWindowDefaultlayout();
	afx_msg void OnWindowUnDefault();
	afx_msg void OnUpdateWindowDefaultlayout(CCmdUI* pCmdUI);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnEditSubtract();
	afx_msg void OnUpdateEditSubtract(CCmdUI* pCmdUI);
	afx_msg void OnEditIntersect();
	afx_msg void OnUpdateEditIntersect(CCmdUI* pCmdUI);
	afx_msg void OnViewLock2DViews();
	afx_msg void OnUpdateViewLock2DViews(CCmdUI* pCmdUI);
	afx_msg void OnViewLock3DView();
	afx_msg void OnUpdateViewLock3DView(CCmdUI* pCmdUI);
	afx_msg void OnViewFocusSelection();
	afx_msg void OnUpdateViewFocusSelection(CCmdUI* pCmdUI);
	afx_msg void OnViewFocusOrigin();
	afx_msg void OnObjDelete();
	afx_msg void OnObjectProperties();
	afx_msg void OnViewDecViewDepth();
	afx_msg void OnViewIncViewDepth();
	afx_msg void OnEditPreference();
	afx_msg void OnFileExportMap();
	afx_msg void OnUpdateFileExportMap(CCmdUI* pCmdUI);
	afx_msg void OnViewDisplayEntities();
	afx_msg void OnUpdateViewDisplayEntities(CCmdUI* pCmdUI);
	afx_msg void OnFileExportBsp();
	afx_msg void OnUpdateFileExportBsp(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCompileStop(CCmdUI* pCmdUI);
	afx_msg void OnCompileStop();
	afx_msg void OnCompileLoadLeak();
	afx_msg void OnUpdateCompileLoadLeak(CCmdUI* pCmdUI);
	afx_msg void OnEditMoveAllDirect();
	afx_msg void OnEditMoveHorizontal();
	afx_msg void OnEditMoveVertical();
	afx_msg void OnObjectSavePrefab();
	afx_msg void OnUpdateObjectSavePrefab(CCmdUI* pCmdUI);
	afx_msg void OnObjectScopeDown();
	afx_msg void OnUpdateObjectScopeDown(CCmdUI* pCmdUI);
	afx_msg void OnObjectScopeUp();
	afx_msg void OnUpdateObjectScopeUp(CCmdUI* pCmdUI);
	afx_msg void OnObjectLoadPrefab();
	afx_msg void OnUpdateObjectLoadPrefab(CCmdUI* pCmdUI);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnActivateApp(BOOL bActive, HTASK hTask);
	afx_msg void OnEditTexmgr();
	afx_msg void OnEditTexRep();
	//}}AFX_MSG
	afx_msg void OnUpdateEditConstraintUI(CCmdUI* pCmdUI);
	afx_msg void OnOpModeSwitch(UINT nID);
	afx_msg void OnUpdateOpModeSwitch(CCmdUI* pCmdUI);
	afx_msg void OnWindowNewView(UINT nID);
	afx_msg void OnCustomToolbarDraw(NMHDR *pNotify, LRESULT *pResult);
	afx_msg void OnMenuEntry(UINT nID);
	afx_msg void OnApplyNone(void);
	afx_msg void OnUpdateGridSize(CCmdUI *pCmdUI);
	afx_msg void OnActivateFrame(UINT nID);
	afx_msg void OnUpdateUIActiveFrame(CCmdUI *pCmdUI);
	afx_msg void OnActivateWSTab(UINT nID);
	afx_msg void OnUpdateUIActivateWSTab(CCmdUI *pCmdUI);
	DECLARE_MESSAGE_MAP()

//==================================================
// Qoole Specific Stuff
//==================================================

public:
	//===== Desktop Documents =====
	QooleDoc *GetDeskTopDocument(void) const { return pDeskTopDoc; }
	void SetDeskTopDocument(QooleDoc *pDoc);
	void UpdateFrameTitle(void);

	//===== Used for Undo/Redo =====
	int GetUndoLevels(void) const { return opManager.GetUndoBufSize(); }
	void SetUndoLevels(int levels) { opManager.SetUndoBufSize(levels); }
	void CommitOperation(Operation &op);

	//==== ClipBoard =====
	LinkList<ObjectPtr> clipBoard;

	//===== Edit Focus & Locking Views =====
	bool IsLockedView(QView *pView);
	const Vector3d &GetEditFocusPos(void) const { return editFocusPos; }

	void Set2DViewsLock(bool lock);
	void Set3DViewLock(bool lock);
	void Set2DLockedZoom(float newZoomVal, bool updateViews = true);
	void SetEditFocusPos(Vector3d &focusVec, bool updateViews = true);

	void GetLockedQViews(QView *qvsArray[]);

	//===== View Depth =====
	void AdjustQViewDepths(float deltaDepth);

	//===== Scoping =====
	Object &GetScope(void) { return *editScope; }
	Selector &ChangeEditScope(Object *newScope);

	//===== Operation Modes =====
	// Valid op mode values: ID_MODE_OBJECTSELECT ... ID_MODE_WALKTHROUGH
	UINT GetOpMode(void) const { return opMode; };
	UINT SwitchOpMode(UINT mode);

	//===== Object Selection =====
	Selector &GetSelector(void) const { return *selector; }

	//===== Desktop components: QVFrames =====
    // Valid nID param values: ID_WINDOW_NEWTOP, ID_WINDOW_NEWBACK,
    //                         ID_WINDOW_NEW3D and ID_WINDOW_NEWSIDE
    // Params x, y, width and height specify the new frame's dimension.
    // Set width and height = 0 to use Windows defaults.
	void CreateQVFrame(UINT nID, int x, int y, int width, int height);
	void RegisterDestroyQVFrame(CMDIChildWnd *pFrame);
	void DestroyAllQVFrames(void);

	QView *IsQViewValid(QView *pView) const;
	QView *GetStdQView(UINT qvType) const;

	//===== Desktop Default Layout =====
	BOOL QuadViews(bool createWnds = true);
	// Catch resizing of the MDIClient Window.
	virtual void RecalcLayout(BOOL bNotify = TRUE);

	//===== QViews' render library.
	void SuspendRenderLib(bool suspend);
	
	//===== Desktop components =====
	QTreeView *GetTreeView(void) { return tbWorkSpaceBar.GetTreeView(); }
	QTexView *GetTextureView(void) { return tbWorkSpaceBar.GetTextureView(); }
	QPrefabMgr *GetPrefabManager(void) { return tbWorkSpaceBar.GetPrefabView(); }
	
	QProcessWnd *GetProcessWnd(void) { return tbOutputBar.GetOutputWnd(); }

	QPropSheet *GetPropWnd(void) { return pPropWnd; };
	void OnDestroyPropWnd(CRect &wndRect);

	void OnDestroyColorWnd(CRect &wndRect);

	//===== Status bar =====
	void UpdateStatusBar(const char *text = NULL);

	//===== Popup Menus =====
	CMenu &GetAddObjectMenu(void) { return pmView; }
	void BuildEntityMenus(void);

	void OnEditColorSel(void);

	//===== Main Frame Window State =====
	int wndMaximized;

	//===== Config states =====
	static int promptHollow;
	static int hollowThickness;
	static int promptGroupName;

protected:
	// Initialization
	bool InitToolbars(int x, int y);
	bool InitStatusBar(void);
	bool InitWorkSpaceBar(void);
	bool InitOutputBar(void);

	//===== Popup Menus =====
	bool InitPopupMenus(void);
	void DestroyPopupMenus(void);
	void AddMenuEntry(CMenu &menu, char *text, int type, char *data);
	void DeleteMenuEntry(int nID);
	void BuildBrushMenu(void);

	//===== Object add funcs =====
	void LoadObject(const char *name);
	void AddEntity(const char *name);
	void ApplyEntity(const char *name);

	//===== Desktop Components: Control Bars =====
	CStatusBar		tbStatusBar;
	CToolBarEx		tbStandard, tbFile, tbEdit,	tbGuide,
					tbAlign, tbMode, tbRender, tbCompile,
					tbGroup, tbWindows;
	QWorkSpaceBar	tbWorkSpaceBar;
	QOutputBar		tbOutputBar;

	QPropSheet		*pPropWnd;
	CRect			propWndRect;

	QColorWnd		*pColorWnd;
	CRect			colorWndRect;

	//===== Desktop =====
	QooleDoc* pDeskTopDoc;
	CFrameWnd *qvFrames[4];
	bool defaultWinLayout;

	//===== Locked Views and Edit Focus.
	void FocusAllQViews(const Vector3d &centerVec, bool updateViews = true);

	Vector3d editFocusPos;
	bool lock2dViews, lock3dView;

	//===== View Menu Stuff =====
	bool displayEntities;

	//===== Scope and Object Selector =====
	Object *editScope;
	Selector *selector;

	//===== Modes and Operations =====
	UINT opMode;
	OpManager opManager;

	//===== Popup Menus =====
	CMenu pmView;
	MenuEntry *menuEntry[ID_MENUENTRY_END - ID_MENUENTRY_START];

	//===== Config =====
	LConfig *cfg;
	int undoLevels;
};

// Global reference pointer.
extern QMainFrame *pQMainFrame;

//========== MenuEntry ==========
// Used to handle popup menu entries

struct MenuEntry {
	int type;
	char *data;
};

//========== QViewsState ==========
// Used to remember the state of the QViews in QMainFrame.

class QViewsState {
public:
	QViewsState(void);
	virtual ~QViewsState();

	void RestoreQViewsState(void);

private:
	int numQViews;
	QView **qvArray;
	Vector3d **posArray;
	SphrVector **orientArray;
	float *zoomArray;
};

//================ QHollowPrompt ================

class QHollowPrompt : public CDialog {
public:
	QHollowPrompt(int defWidth);

// Dialog Data
	//{{AFX_DATA(QHollowPrompt)
	enum { IDD = IDD_HOLLOW_PROMPT };
	BOOL	m_bPrompt;
	int		m_iThickness;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(QHollowPrompt)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(QHollowPrompt)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__8B1BCCF6_FCC8_11D1_8E6B_004005310168__INCLUDED_)
