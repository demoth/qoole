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

// MainFrm.cpp : implementation of the QMainFrame class
//

#include "stdafx.h"

#include "Qoole.h"
#include "QMainFrm.h"
#include "QConfSht.h"
#include "QCompile.h"
#include "QTexMgr.h"
#include "QTexRep.h"
#include "QVersion.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// QMainFrame

QMainFrame *pQMainFrame = NULL;

IMPLEMENT_DYNAMIC(QMainFrame, CMRCMDIFrameWndSizeDock)

BEGIN_MESSAGE_MAP(QMainFrame, CMRCMDIFrameWndSizeDock)
	//{{AFX_MSG_MAP(QMainFrame)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
	ON_COMMAND(ID_EDIT_HOLLOWIN, OnEditHollow)
	ON_UPDATE_COMMAND_UI(ID_EDIT_HOLLOWIN, OnUpdateEditHollow)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, OnUpdateObjDelete)
	ON_COMMAND(ID_OBJECT_GROUPOBJS, OnObjectGroupObjs)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_GROUPOBJS, OnUpdateObjectGroupObjs)
	ON_COMMAND(ID_OBJECT_UNGROUPOBJS, OnObjectUngroupObjs)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_UNGROUPOBJS, OnUpdateObjectUngroupObjs)
	ON_COMMAND(ID_WINDOW_DEFAULTLAYOUT, OnWindowDefaultlayout)
	ON_COMMAND(ID_WINDOW_CASCADE, OnWindowUnDefault)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_DEFAULTLAYOUT, OnUpdateWindowDefaultlayout)
	ON_WM_CONTEXTMENU()
	ON_WM_DESTROY()
	ON_COMMAND(ID_EDIT_SUBTRACT, OnEditSubtract)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SUBTRACT, OnUpdateEditSubtract)
	ON_COMMAND(ID_EDIT_INTERSECT, OnEditIntersect)
	ON_UPDATE_COMMAND_UI(ID_EDIT_INTERSECT, OnUpdateEditIntersect)
	ON_COMMAND(ID_VIEW_LOCK2DVIEWS, OnViewLock2DViews)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LOCK2DVIEWS, OnUpdateViewLock2DViews)
	ON_COMMAND(ID_VIEW_LOCK3DVIEW, OnViewLock3DView)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LOCK3DVIEW, OnUpdateViewLock3DView)
	ON_COMMAND(ID_VIEW_FOCUSSELECTION, OnViewFocusSelection)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FOCUSSELECTION, OnUpdateViewFocusSelection)
	ON_COMMAND(ID_VIEW_FOCUSORIGIN, OnViewFocusOrigin)
	ON_COMMAND(ID_EDIT_DELETE, OnObjDelete)
	ON_COMMAND(ID_OBJECT_PROPERTIES, OnObjectProperties)
	ON_COMMAND(ID_VIEW_DECVIEWDEPTH, OnViewDecViewDepth)
	ON_COMMAND(ID_VIEW_INCVIEWDEPTH, OnViewIncViewDepth)
	ON_COMMAND(ID_EDIT_PREFERENCE, OnEditPreference)
	ON_COMMAND(ID_COMPILE_EXPORTMAP, OnFileExportMap)
	ON_UPDATE_COMMAND_UI(ID_COMPILE_EXPORTMAP, OnUpdateFileExportMap)
	ON_COMMAND(ID_VIEW_DISPLAYENTITIES, OnViewDisplayEntities)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DISPLAYENTITIES, OnUpdateViewDisplayEntities)
	ON_COMMAND(ID_COMPILE_EXPORTBSP, OnFileExportBsp)
	ON_UPDATE_COMMAND_UI(ID_COMPILE_EXPORTBSP, OnUpdateFileExportBsp)
	ON_UPDATE_COMMAND_UI(ID_COMPILE_STOP, OnUpdateCompileStop)
	ON_COMMAND(ID_COMPILE_STOP, OnCompileStop)
	ON_COMMAND(ID_COMPILE_LOADLEAK, OnCompileLoadLeak)
	ON_UPDATE_COMMAND_UI(ID_COMPILE_LOADLEAK, OnUpdateCompileLoadLeak)
	ON_COMMAND(ID_EDIT_MOVEALLDIRECT, OnEditMoveAllDirect)
	ON_COMMAND(ID_EDIT_MOVEHORIZONTAL, OnEditMoveHorizontal)
	ON_COMMAND(ID_EDIT_MOVEVERTICAL, OnEditMoveVertical)
	ON_COMMAND(ID_OBJECT_SAVEPREFAB, OnObjectSavePrefab)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_SAVEPREFAB, OnUpdateObjectSavePrefab)
	ON_COMMAND(ID_OBJECT_SCOPEDOWN, OnObjectScopeDown)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_SCOPEDOWN, OnUpdateObjectScopeDown)
	ON_COMMAND(ID_OBJECT_SCOPEUP, OnObjectScopeUp)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_SCOPEUP, OnUpdateObjectScopeUp)
	ON_COMMAND(ID_OBJECT_LOADPREFAB, OnObjectLoadPrefab)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_LOADPREFAB, OnUpdateObjectLoadPrefab)
	ON_WM_ACTIVATE()
	ON_WM_ACTIVATEAPP()
	ON_COMMAND(ID_EDIT_TEXMGR, OnEditTexmgr)
	ON_COMMAND(ID_EDIT_TEXREP, OnEditTexRep)
	ON_COMMAND(ID_EDIT_HOLLOWOUT, OnEditHollow)
	ON_UPDATE_COMMAND_UI(ID_EDIT_HOLLOWOUT, OnUpdateEditHollow)
	ON_WM_SIZE()
	ON_COMMAND(ID_WINDOW_TILE_HORZ, OnWindowUnDefault)
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI_RANGE(ID_EDIT_MOVEALLDIRECT, ID_EDIT_MOVEVERTICAL, OnUpdateEditConstraintUI)
	ON_COMMAND_RANGE(ID_MODE_OBJECTSELECT, ID_MODE_FLYTHROUGH, OnOpModeSwitch)
	ON_UPDATE_COMMAND_UI_RANGE(ID_MODE_OBJECTSELECT, ID_MODE_FLYTHROUGH, OnUpdateOpModeSwitch)
	ON_COMMAND_RANGE(ID_VIEW_NEWTOP, ID_VIEW_NEWSIDE, OnWindowNewView)
	ON_UPDATE_COMMAND_UI_RANGE(ID_TBSTANDARD, ID_OUTPUTBAR, OnUpdateControlBarMenu)
	ON_COMMAND_EX_RANGE(ID_TBSTANDARD, ID_OUTPUTBAR, OnBarCheck)
	ON_COMMAND_RANGE(ID_MENUENTRY_START, ID_MENUENTRY_END, OnMenuEntry)
	ON_COMMAND(ID_OBJECT_APPLYNONE, OnApplyNone)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_GRIDSIZE, OnUpdateGridSize)
	ON_COMMAND_RANGE(ID_WINDOW_ACTIVATE_TOP, ID_WINDOW_ACTIVATE_SIDE, OnActivateFrame)
	ON_UPDATE_COMMAND_UI_RANGE(ID_WINDOW_ACTIVATE_TOP, ID_WINDOW_ACTIVATE_SIDE, OnUpdateUIActiveFrame)
	ON_COMMAND_RANGE(ID_WINDOW_ACTIVATE_TREEVIEW, ID_WINDOW_ACTIVATE_PREFABVIEW, OnActivateWSTab)
	ON_UPDATE_COMMAND_UI_RANGE(ID_WINDOW_ACTIVATE_TREEVIEW, ID_WINDOW_ACTIVATE_PREFABVIEW, OnUpdateUIActivateWSTab)
	END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// QMainFrame construction/destruction

int QMainFrame::promptHollow = 1;
int QMainFrame::hollowThickness = 8;
int QMainFrame::promptGroupName = 1;

QMainFrame::QMainFrame() {

	// Init.
	pDeskTopDoc = NULL;
	qvFrames[0] = qvFrames[1] = qvFrames[2] = qvFrames[3] = NULL;

	opMode = 0;

	editScope = NULL;
	selector = NULL;

	lock2dViews = true;
	lock3dView = false;
	displayEntities = true;

	defaultWinLayout = true;

	wndMaximized = 0;

	pPropWnd = NULL;
	propWndRect.SetRect(0, 0, 0, 0);

	pColorWnd = NULL;
	colorWndRect.SetRect(0, 0, 0, 0);

	// Config.
	cfg = new LConfig("QMainFrame");

	// Edit: Undo levels.
	undoLevels = 50;
	cfg->RegisterVar("UndoLevels", &undoLevels, LVAR_INT);
	opManager.SetUndoBufSize(undoLevels);

	// Edit: Hollow.
	cfg->RegisterVar("PromptHollow", &promptHollow, LVAR_INT);
	cfg->RegisterVar("HollowThickness", &hollowThickness, LVAR_INT);

	// Grouping.
	cfg->RegisterVar("PromptGroupName", &promptGroupName, LVAR_INT);

	// Lock views.
	cfg->RegisterVar("Lock2dViews", &lock2dViews, LVAR_INT);
	cfg->RegisterVar("Lock3dView", &lock3dView, LVAR_INT);

	cfg->RegisterVar("DisplayEntities", &displayEntities, LVAR_INT);
	View::DisplayEntities(displayEntities);

	cfg->RegisterVar("DefaultWinLayout", &defaultWinLayout, LVAR_INT);

	cfg->RegisterVar("WindowMaximized", &wndMaximized, LVAR_INT);
}

QMainFrame::~QMainFrame() {
	ASSERT(pDeskTopDoc == NULL);
	for(int i = 0; i < 4; i++)
		ASSERT(qvFrames[i] == NULL);
	ASSERT(editScope == NULL);

	ObjectPtr *pObjPtr;
	while (clipBoard.NumOfElm() > 0) {
		pObjPtr = &(clipBoard.RemoveNode(0));
		delete (pObjPtr->GetPtr());
		delete pObjPtr;
	}

	undoLevels = opManager.GetUndoBufSize();

	cfg->SaveVars();
	delete cfg;

	if(pColorWnd)
		delete pColorWnd;
}

BOOL QMainFrame::PreCreateWindow(CREATESTRUCT& cs) {
	// Turn off the MFC's default window title setting.
	cs.style &= ~FWS_ADDTOTITLE;
	return CMRCMDIFrameWndSizeDock::PreCreateWindow(cs);
}

int QMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	if (CMRCMDIFrameWndSizeDock::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// Enable the frame window for docking.
	EnableDocking(CBRS_ALIGN_ANY);
	// FrameEnableDocking(this, CBRS_ALIGN_ANY);

	if (!InitToolbars(lpCreateStruct->x, lpCreateStruct->y))
		return -1;
	if (!InitStatusBar())
		return -1;
	if (!InitWorkSpaceBar())
		return -1;
	if (!InitOutputBar())
		return -1;
	if (!InitPopupMenus())
		return -1;

	// Load states from registry.
	LoadBarState("QToolbars");
	// Doesn't work.
	// LoadSizeBarState("QSizeBars");

	// Init the global reference pointer.
	pQMainFrame = this;

	char appNameVer[256];
	sprintf(appNameVer, "%s %s", AfxGetAppName(), QVERSION);
	SetWindowText(appNameVer);

	return 0;
}

bool QMainFrame::InitToolbars(int x, int y) {

	// Toolbars Init.
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP |
					CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC;

	// Standard Toolbar.
	if (!tbStandard.Create(this, dwStyle, ID_TBSTANDARD)) {
		TRACE0("Failed to create standard toolbar\n");
		return false;      // fail to create
	}
	tbStandard.LoadToolBar(ID_TBSTANDARD);
	tbStandard.SetWindowText("Standard");
	tbStandard.EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&tbStandard, AFX_IDW_DOCKBAR_TOP);
	// ShowControlBar(&tbStandard, FALSE, FALSE);

	CPoint point(x + 100, y + 20);
	// ClientToScreen(&point);

	// File Toolbar.
	if (!tbFile.Create(this, dwStyle, ID_TBFILE)) {
		TRACE0("Failed to create file toolbar\n");
		return false;      // fail to create
	}
	tbFile.LoadToolBar(ID_TBFILE);
	tbFile.SetWindowText("File");
	tbFile.EnableDocking(CBRS_ALIGN_ANY);
	ShowControlBar(&tbFile, FALSE, FALSE);
	FloatControlBar(&tbFile, point);
	point.Offset(50, 0);
	// DockControlBar(&tbFile, AFX_IDW_DOCKBAR_TOP);

	// Edit Toolbar.
	if (!tbEdit.Create(this, dwStyle, ID_TBEDIT)) {
		TRACE0("Failed to create edit toolbar\n");
		return false;      // fail to create
	}
	tbEdit.LoadToolBar(ID_TBEDIT);
	tbEdit.SetWindowText("Edit");
	tbEdit.EnableDocking(CBRS_ALIGN_ANY);
	ShowControlBar(&tbEdit, FALSE, FALSE);
	FloatControlBar(&tbEdit, point);
	point.Offset(50, 0);
	// DockControlBar(&tbEdit, AFX_IDW_DOCKBAR_TOP);

	// Guide Toolbar.
	if (!tbGuide.Create(this, dwStyle, ID_TBGUIDE)) {
		TRACE0("Failed to create guide toolbar\n");
		return false;      // fail to create
	}
	tbGuide.LoadToolBar(ID_TBGUIDE);
	tbGuide.SetWindowText("Editing Guides");
	tbGuide.EnableDocking(CBRS_ALIGN_ANY);
	ShowControlBar(&tbGuide, FALSE, FALSE);
	FloatControlBar(&tbGuide, point);
	point.Offset(50, 0);
	// DockControlBar(&tbGuide, AFX_IDW_DOCKBAR_TOP);

	// Align Toolbar.
	if (!tbAlign.Create(this, dwStyle, ID_TBALIGN)) {
		TRACE0("Failed to create align toolbar\n");
		return false;      // fail to create
	}
	tbAlign.LoadToolBar(ID_TBALIGN);
	tbAlign.SetWindowText("Alignment");
	tbAlign.EnableDocking(CBRS_ALIGN_ANY);
	ShowControlBar(&tbAlign, FALSE, FALSE);
	FloatControlBar(&tbAlign, point);
	point.Offset(50, 0);
	// DockControlBar(&tbAlign, AFX_IDW_DOCKBAR_TOP);

	// Mode Toolbar.
	if (!tbMode.Create(this, dwStyle, ID_TBMODE)) {
		TRACE0("Failed to create mode toolbar\n");
		return false;      // fail to create
	}
	tbMode.LoadToolBar(ID_TBMODE);
	tbMode.SetWindowText("Mode");
	tbMode.EnableDocking(CBRS_ALIGN_ANY);
	ShowControlBar(&tbMode, FALSE, FALSE);
	FloatControlBar(&tbMode, point);
	point.Offset(50, 0);
	// DockControlBar(&tbMode, AFX_IDW_DOCKBAR_TOP);

	// Grouping Toolbar.
	if (!tbGroup.Create(this, dwStyle, ID_TBGROUP)) {
		TRACE0("Failed to create grouping toolbar\n");
		return false;      // fail to create
	}
	tbGroup.LoadToolBar(ID_TBGROUP);
	tbGroup.SetWindowText("Grouping");
	tbGroup.EnableDocking(CBRS_ALIGN_ANY);
	ShowControlBar(&tbGroup, FALSE, FALSE);
	FloatControlBar(&tbGroup, point);
	point.Offset(50, 0);
	// DockControlBar(&tbGroup, AFX_IDW_DOCKBAR_TOP);

	// Render Toolbar.
	if (!tbRender.Create(this, dwStyle, ID_TBRENDER)) {
		TRACE0("Failed to create render view toolbar\n");
		return false;      // fail to create
	}
	tbRender.LoadToolBar(ID_TBRENDER);
	tbRender.SetWindowText("Render View");
	tbRender.EnableDocking(CBRS_ALIGN_ANY);
	ShowControlBar(&tbRender, FALSE, FALSE);
	FloatControlBar(&tbRender, point);
	point.Offset(50, 0);
	// DockControlBar(&tbRender, AFX_IDW_DOCKBAR_TOP);

	// Compile Toolbar.
	if (!tbCompile.Create(this, dwStyle, ID_TBCOMPILE)) {
		TRACE0("Failed to create compile toolbar\n");
		return false;      // fail to create
	}
	tbCompile.LoadToolBar(ID_TBCOMPILE);
	tbCompile.SetWindowText("Compile");
	tbCompile.EnableDocking(CBRS_ALIGN_ANY);
	ShowControlBar(&tbCompile, FALSE, FALSE);
	FloatControlBar(&tbCompile, point);
	point.Offset(50, 0);
	// DockControlBar(&tbCompile, AFX_IDW_DOCKBAR_TOP);

	// Window Toolbar.
	if (!tbWindows.Create(this, dwStyle, ID_TBWINDOWS)) {
		TRACE0("Failed to create windows toolbar\n");
		return false;      // fail to create
	}
	tbWindows.LoadToolBar(ID_TBWINDOWS);
	tbWindows.SetWindowText("Windows");
	tbWindows.EnableDocking(CBRS_ALIGN_ANY);
	ShowControlBar(&tbWindows, FALSE, FALSE);
	FloatControlBar(&tbWindows, point);
	point.Offset(50, 0);
	// DockControlBar(&tbWindows, AFX_IDW_DOCKBAR_TOP);

	return true;
}

static UINT indicators[] = {
	ID_SEPARATOR,           // status line indicator
	ID_SEPARATOR
};

bool QMainFrame::InitStatusBar(void) {
	// Status Bar Init.
	if (!tbStatusBar.Create(this) ||
		!tbStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT))) {
		TRACE0("Failed to create status bar\n");
		return false;      // fail to create
	}

	tbStatusBar.SetPaneInfo(0, ID_SEPARATOR, // ID_INDICATOR_STATUS,
							SBPS_NOBORDERS | SBPS_STRETCH, 0);
	tbStatusBar.SetPaneInfo(1, ID_INDICATOR_GRIDSIZE, SBPS_NORMAL, 86);

	tbStatusBar.SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	return true;
}

bool QMainFrame::InitWorkSpaceBar(void) {

    // CREATION OF A CUSTOM control bar.
    CRect rect(0, 0, 186, 300);	
//	DWORD dwStyle = WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | CBRS_LEFT;
	DWORD dwStyle = CBRS_LEFT;
    if (!tbWorkSpaceBar.Create(this, "WorkSpace", ID_WORKSPACE, dwStyle, rect)) {
		TRACE0("Failed to create workspace dockbar\n");
		return false;
    }

	tbWorkSpaceBar.EnableDocking(CBRS_ALIGN_ANY);
    DockControlBar(&tbWorkSpaceBar);
	ShowControlBar(&tbWorkSpaceBar, TRUE, FALSE);

	return true;
}

bool QMainFrame::InitOutputBar(void) {

	// Creation of a custom control bar.
	CRect rect(0, 0, 400, 250);
//	DWORD dwStyle = WS_CHILD | WS_CLIPCHILDREN | CBRS_BOTTOM;
	DWORD dwStyle = CBRS_BOTTOM;
	if (!tbOutputBar.Create(this, "Output", ID_OUTPUTBAR, dwStyle, rect)) {
		TRACE0("Failed to create workspace dockbar\n");
		return false;
	}
	tbOutputBar.EnableDocking(CBRS_ALIGN_ANY);
	ShowControlBar(&tbFile, FALSE, FALSE);
    DockControlBar(&tbOutputBar);

	return true;
}


BOOL QMainFrame::OnCreateClient(LPCREATESTRUCT lpcs,
								CCreateContext* pContext) {
	return CMRCMDIFrameWndSizeDock::OnCreateClient(lpcs, pContext);
}

void QMainFrame::OnClose() {
	// Save states.
	wndMaximized = IsZoomed();

	// Do not save the prefab preview window.
	GetPrefabManager()->ClosePreviewWnd();

	SaveBarState("QToolbars");

	// Doesnt work.
	// SaveSizeBarState("QSizeBars");

	SetActiveView(NULL, FALSE);

	CMRCMDIFrameWndSizeDock::OnClose();
}

void QMainFrame::OnDestroy() {
	DestroyPopupMenus();

	pQMainFrame = NULL;

	CMRCMDIFrameWndSizeDock::OnDestroy();
}


/////////////////////////////////////////////////////////////////////////////
// QMainFrame diagnostics

#ifdef _DEBUG
void QMainFrame::AssertValid() const
{
	CMRCMDIFrameWndSizeDock::AssertValid();
}

void QMainFrame::Dump(CDumpContext& dc) const
{
	CMRCMDIFrameWndSizeDock::Dump(dc);
}

#endif //_DEBUG

//==================================================
// QMainFrame message handlers
//==================================================

void QMainFrame::SetDeskTopDocument(QooleDoc* pDoc) {

	if (pDoc == pDeskTopDoc)  // No change.
		return;

	// Turn off leak display.
	if (View::GetLeakObject() != NULL)
		View::SetLeakObject(NULL);

	// Clean up the compile stuff.
	if (pDeskTopDoc != NULL) {
		Game *pGame = pDeskTopDoc->GetGame();
		pGame->CleanUpCompile(pDeskTopDoc->GetDocName());
	}

	// Change TreeView.
	GetTreeView()->SetCurrentDocument(pDoc);

	// Change the document for the prop window.
	if (pPropWnd != NULL) {
		if (pDoc == NULL) // Close the prop window.
			pPropWnd->SendMessage(WM_CLOSE);
		else
			pPropWnd->RegisterDocument(pDeskTopDoc, pDoc);
	}

	// Close all QVFrames to old document
	DestroyAllQVFrames();

	// Clean up desktop.
	if (selector)
		delete selector;
	selector = NULL;
	editScope = NULL;

	SwitchOpMode(ID_MODE_OBJECTMOVE);
	// UpdateStatusBar();

	opManager.ResetUndoList();

	// Set document.
	pDeskTopDoc = pDoc;

	if(pColorWnd)
		pColorWnd->RegisterDocument(pDeskTopDoc);

	// Change window title.
	UpdateFrameTitle();

	if (pDoc == NULL)
		return;

	// Change to appropiate game
	bool ok = Game::Set(pDoc->GetGame(), pDoc->GetPalName(),
						QDraw::textureGamma);
	ASSERT(ok);

	// Change to appropiate entity list
	EntList::Set(pDoc->GetEntList());

	// Init texture list and select first item
	tbWorkSpaceBar.GetTextureView()->OnNewContents();

	// Init the desktop for new document.
	editScope = pDoc->GetRootObjectPtr();
	selector = new Selector(*editScope);
	editFocusPos.NewVector(0.0f, 0.0f, 0.0f);

	// Create new QVFrames to the new document.
	QuadViews();

	// Rebuild popup menus
	DestroyPopupMenus();
	BuildBrushMenu();
	BuildEntityMenus();

	// Show cross hair.
	View::ShowCrossHair(lock2dViews);
}

void QMainFrame::UpdateFrameTitle(void) {
	char winTitle[256];

	if (pDeskTopDoc == NULL) {
		sprintf(winTitle, "%s %s", AfxGetAppName(), QVERSION);
		SetWindowText(winTitle);
		return;
	}

	sprintf(winTitle, "%s - %s %s%s", pDeskTopDoc->GetTitle(),
			AfxGetAppName(), QVERSION,
			pDeskTopDoc->IsModified() ? " *" : "");

	SetWindowText(winTitle);
}

//===== Scoping =====

Selector &QMainFrame::ChangeEditScope(Object *newScope) {
	// Sanity.
	ASSERT(newScope && editScope && selector);

	if (newScope == editScope)
		return *selector;

	ASSERT(newScope != NULL);
	ASSERT(pDeskTopDoc->GetRootObjectPtr() == &(newScope->GetRoot()));

	// Need to transform the edit focus pos.
	Matrix44 trans;
	Object::GetTransMatrix(*editScope, *newScope, trans);
	trans.Transform(editFocusPos);

	editScope = newScope;
	delete selector;
	selector = new Selector(*editScope);
	return *selector;
}

//===== File: Export Map =====

void QMainFrame::OnFileExportMap() {
	CString szFilters = "Map Files (*.map)|*.map||";
	CString defFileName = pDeskTopDoc->GetPathName();
	if (defFileName.Right(4).Compare(".qle") == 0) {
		defFileName = defFileName.Left(defFileName.GetLength() - 4);
	}

	CFileDialog dlg(FALSE, "map", defFileName,
		OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_HIDEREADONLY,
		szFilters, this);

	if (dlg.DoModal() == IDOK) {
		CString filePathName = dlg.GetPathName();
		pDeskTopDoc->OnExportMap(filePathName);
	}
}

void QMainFrame::OnUpdateFileExportMap(CCmdUI* pCmdUI) {
	pCmdUI->Enable(pDeskTopDoc != NULL);
}

void QMainFrame::OnFileExportBsp() {
	if (pDeskTopDoc == NULL)
		return;

	if (pDeskTopDoc->GetPathName() == "") {
		// Never been saved.  Need to prompt save for a filename.
		SendMessage(WM_COMMAND, ID_FILE_SAVE);
	}

	if (pDeskTopDoc->GetPathName() == "")  // Cancelled.
		return;

	QCompile compileDlg(this);

	if (compileDlg.DoModal() == IDOK) {
		if (!tbOutputBar.IsWindowVisible())
			SendMessage(WM_COMMAND, ID_OUTPUTBAR);
		QCompile::ExecuteCompiles();
	}
}

void QMainFrame::OnUpdateFileExportBsp(CCmdUI* pCmdUI) {
	BOOL pFlag = pDeskTopDoc != NULL &&
				 !GetProcessWnd()->IsProcessRunning();
	pCmdUI->Enable(pFlag);
}

void QMainFrame::OnCompileStop() {
	QCompile::StopCompiles();
	QDraw::OutputText("Compile process stoped.\n");
}

void QMainFrame::OnUpdateCompileStop(CCmdUI* pCmdUI) {
	BOOL pFlag = GetProcessWnd()->IsProcessRunning();
	pCmdUI->Enable(pFlag);
	
}

//===== Leak Detection =====

void QMainFrame::OnCompileLoadLeak() {
	if (View::GetLeakObject() != NULL) {
		// Turn off the display.
		View::SetLeakObject(NULL);

		// Update all qviews.
		pDeskTopDoc->UpdateAllViews(NULL, DUAV_QVIEWS | DUAV_SUPDATEALL, NULL);

		return;
	}

	if (pDeskTopDoc == NULL)
		return;

	Game *pGame = pDeskTopDoc->GetGame();
	Object *pLeakObj = pGame->LoadLeakFile(pDeskTopDoc->GetDocName());
	View::SetLeakObject(pLeakObj);

	// Update all qviews.
	pDeskTopDoc->UpdateAllViews(NULL, DUAV_QVIEWS | DUAV_SUPDATEALL, NULL);
}

void QMainFrame::OnUpdateCompileLoadLeak(CCmdUI* pCmdUI) {
	if (pDeskTopDoc == NULL) {
		pCmdUI->SetCheck(0);
		pCmdUI->Enable(false);
		return;
	}

	bool check = (View::GetLeakObject() != NULL);
	pCmdUI->SetCheck(check);

	Game *pGame = pDeskTopDoc->GetGame();
	const char *leakFile = pGame->GetLeakFile(pDeskTopDoc->GetDocName());
	bool enable = LFile::Exist(leakFile);

	pCmdUI->Enable(enable || check);
}

//===== Edit: Undo and Redo =====

void QMainFrame::CommitOperation(Operation &op) {
	opManager.RegisterOp(op);
}

void QMainFrame::OnEditUndo() {
	ASSERT(opManager.GetNumUndoOps() > 0);

	if (opManager.GetNumUndoOps() > 0)
		opManager.Undo();
}

void QMainFrame::OnUpdateEditUndo(CCmdUI* pCmdUI) {
	pCmdUI->Enable(opManager.GetNumUndoOps() > 0);
}

void QMainFrame::OnEditRedo() {
	ASSERT(opManager.GetNumRedoOps() > 0);

	if (opManager.GetNumRedoOps() > 0)
		opManager.Redo();
}

void QMainFrame::OnUpdateEditRedo(CCmdUI* pCmdUI) {
	pCmdUI->Enable(opManager.GetNumRedoOps() > 0);
}

//===== Edit: Cut =====

void QMainFrame::OnEditCut() {
	ASSERT(selector != NULL);
	OpEditCut *opCut = new OpEditCut();
	opManager.RegisterOp(*opCut);
}

void QMainFrame::OnUpdateEditCut(CCmdUI* pCmdUI) {
	bool flag = (pDeskTopDoc != NULL && selector->GetNumMSelectedObjects() > 0);
	pCmdUI->Enable(flag);
}

//===== Edit: Copy =====

void QMainFrame::OnEditCopy() {
	ASSERT(selector != NULL);
	OpEditCopy *opCopy = new OpEditCopy();
	opManager.RegisterOp(*opCopy);
}

void QMainFrame::OnUpdateEditCopy(CCmdUI* pCmdUI) {
	bool flag = (pDeskTopDoc != NULL && selector->GetNumMSelectedObjects() > 0);
	pCmdUI->Enable(flag);
}

//===== Edit: Paste =====
// Handle this in QView because we need to know
//  the QView's position for pasting.
/*
void QMainFrame::OnEditPaste() {
	ASSERT(selector != NULL);
	OpEditPaste *opPaste = new OpEditPaste();
	opManager.RegisterOp(*opPaste);
}

void QMainFrame::OnUpdateEditPaste(CCmdUI* pCmdUI) {
	pCmdUI->Enable(pDeskTopDoc != NULL && clipBoard.NumOfElm() > 0);
}
*/

//===== Edit: Hollow Brush =====

void QMainFrame::OnEditHollow() {
	Object *slctObj = NULL;

	ASSERT(selector != NULL);
	ASSERT(selector->GetNumMSelectedObjects() == 1);
	slctObj = (selector->GetMSelectedObjects())[0].GetPtr();

	UINT mesg = (UINT) LOWORD(GetCurrentMessage()->wParam);
	bool inward = (mesg == ID_EDIT_HOLLOWIN);

	if (promptHollow) {
		QHollowPrompt hPrompt(hollowThickness);
		if (hPrompt.DoModal()) {
			promptHollow = (hPrompt.m_bPrompt == FALSE);
			hollowThickness = hPrompt.m_iThickness;
		}
		else {
			return;
		}
	}

	OpObjModification *opHollow = 
		OpObjModification::NewHollowOp(*slctObj, inward, hollowThickness);
	if (opHollow)
		opManager.RegisterOp(*opHollow);
	else
		MessageBeep(MB_OK);
}

void QMainFrame::OnUpdateEditHollow(CCmdUI* pCmdUI) {
	bool flag = false;

	if (pDeskTopDoc != NULL && selector->GetNumMSelectedObjects() == 1) {
		Object *slctObj = (selector->GetMSelectedObjects())[0].GetPtr();
		if (slctObj->HasBrush())
			flag = true;
	}

	pCmdUI->Enable(flag);
}

//===== Edit: CSG Subtract =====
void QMainFrame::OnEditSubtract() {
	OpCSGSubtract *op = OpCSGSubtract::NewSubtractOp();

	if (op != NULL)
		CommitOperation(*op);
	else
		MessageBeep(MB_OK);
}

void QMainFrame::OnUpdateEditSubtract(CCmdUI* pCmdUI) {
	int numBrushes = 0;
	int numEnts = 0;
	Object *pObj;

	if (selector) {
		IterLinkList<ObjectPtr> iter(selector->GetMSelectedObjects());
		iter.Reset();
		while (!iter.IsDone()) {
			pObj = iter.GetNext()->GetPtr();
			if ((numEnts = pObj->CountItems()) > 0)
				break;
			numBrushes += pObj->CountBrushes();
		}
	}

	bool flag = (numEnts == 0 && numBrushes > 0);
	pCmdUI->Enable(flag);
}

//===== Edit: CSG Intersect =====
void QMainFrame::OnEditIntersect() {
	OpCSGIntersect *op = OpCSGIntersect::NewIntersectOp();

	if (op != NULL)
		CommitOperation(*op);
	else
		MessageBeep(MB_OK);
}

void QMainFrame::OnUpdateEditIntersect(CCmdUI* pCmdUI) {
	int numBrushes = 0;
	int numEnts = 0;
	Object *pObj;

	if (selector) {
		IterLinkList<ObjectPtr> iter(selector->GetMSelectedObjects());
		iter.Reset();
		while (!iter.IsDone()) {
			pObj = iter.GetNext()->GetPtr();
			if ((numEnts = pObj->CountItems()) > 0)
				break;
			numBrushes += pObj->CountBrushes();
		}
	}

	bool flag = (numEnts == 0 && numBrushes > 1);
	pCmdUI->Enable(flag);
}

//===== Edit: Guides, editing constraints =====
void QMainFrame::OnEditMoveAllDirect() {
	if (QView::editConstraint == 0)
		return;

	QView::editConstraint = 0;
}

void QMainFrame::OnEditMoveHorizontal() {
	if (QView::editConstraint == 1)
		return;

	QView::editConstraint = 1;
}

void QMainFrame::OnEditMoveVertical() {
	if (QView::editConstraint == 2)
		return;

	QView::editConstraint = 2;
}

void QMainFrame::OnUpdateEditConstraintUI(CCmdUI* pCmdUI) {

	pCmdUI->Enable(pDeskTopDoc != NULL);

	UINT cnst = pCmdUI->m_nID - ID_EDIT_MOVEALLDIRECT;
	if (cnst == QView::editConstraint)
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}

//===== Edit: Preference =====

void QMainFrame::OnEditPreference() {
	QDraw::OutputText("Loading preferences... ");
	QConfigSheet configWnd("Qoole Configs", this);
	QDraw::OutputText("OK.\n");

	if (configWnd.DoModal() == IDOK) {
	}
}

//===== Mode: Managing Desktop Operation Modes =====

UINT QMainFrame::SwitchOpMode(UINT mode) {
	ASSERT(mode >= ID_MODE_OBJECTSELECT);
	ASSERT(mode <= ID_MODE_FLYTHROUGH);

	UINT rtnVal = opMode;
	if (mode == ID_MODE_MODIFYBRUSH)
		opMode = ID_MODE_FACEMOVE;
	else
		opMode = mode;

	// Activate the 3d frame window in we're in flythrough mode.
	if (mode == ID_MODE_FLYTHROUGH) {
		// Restore the window if needed.
		if (qvFrames[2] == NULL)
			SendMessage(WM_COMMAND, ID_VIEW_NEW3D);
		// Set focus.
		if (MDIGetActive() != qvFrames[2])
			MDIActivate(qvFrames[2]);

		// Start fly through in the 3d view.
		QView *pQView = (QView *)
			qvFrames[2]->GetDescendantWindow(AFX_IDW_PANE_FIRST, TRUE);
		pQView->OnStartFlyThrough();

		// Done with fly through.
		opMode = rtnVal;
	}

	if (pDeskTopDoc != NULL) {
		// Tell all the qviews the op mode has changed.
		CView *pView;
		POSITION pos = pDeskTopDoc->GetFirstViewPosition();
		while (pos != NULL) {
			pView = pDeskTopDoc->GetNextView(pos);
			if (!pView->IsKindOf(RUNTIME_CLASS(QView)))
				continue;
			((QView *) pView)->OnOpModeChanged(mode, rtnVal);
		}
	}

	// Update the message in status bar.
	UpdateStatusBar();

	return rtnVal;
}

void QMainFrame::OnOpModeSwitch(UINT nID) {
	// UINT nID = (UINT) LOWORD(GetCurrentMessage()->wParam);
	SwitchOpMode(nID);
}

void QMainFrame::OnUpdateOpModeSwitch(CCmdUI* pCmdUI) {
	bool flag = (pDeskTopDoc != NULL);
	bool check = (GetOpMode() == pCmdUI->m_nID);

	if (flag && pCmdUI->m_nID >= ID_MODE_FACEMOVE &&
		pCmdUI->m_nID <= ID_MODE_VERTEXMOVE) {
		ASSERT(selector != NULL);
		flag = (selector->GetNumMSelectedObjects() == 1 &&
				selector->GetMSelectedObjects()[0].GetPtr()->HasBrush());
	}
	else if (flag && (pCmdUI->m_nID == ID_MODE_PLANECLIP ||
					  pCmdUI->m_nID == ID_MODE_MIRROR)) {
		ASSERT(selector != NULL);
		flag = (selector->GetNumMSelectedObjects() > 0);
	}

	pCmdUI->Enable(flag);
	pCmdUI->SetCheck(check);
}

//===== Object: Add/Del/Group/etc...

void QMainFrame::OnObjDelete() {
	ASSERT(selector != NULL);
	ASSERT(selector->GetNumMSelectedObjects() > 0);
	OpObjsDel *opDel = new OpObjsDel();
	opManager.RegisterOp(*opDel);
}

void QMainFrame::OnUpdateObjDelete(CCmdUI* pCmdUI) {
	bool flag = (selector && selector->GetNumMSelectedObjects() > 0);
	pCmdUI->Enable(flag);
}

void QMainFrame::OnObjectGroupObjs() {
	// Sanity.
	ASSERT(selector->GetNumMSelectedObjects() > 1);

	LinkList<ObjectPtr> selObjs;
	selObjs = selector->GetMSelectedObjects();

	OpGrouping *opGroup = new OpGrouping(selObjs);
	opManager.RegisterOp(*opGroup);

	if (promptGroupName) {
		// Make sure tree view's displayed.
		tbWorkSpaceBar.ShowSelTabWindow(0);
		GetTreeView()->SendMessage(WM_COMMAND, ID_RCMTV_EDITLABEL, 0L);
	}
}

void QMainFrame::OnUpdateObjectGroupObjs(CCmdUI* pCmdUI) {
	pCmdUI->Enable(pDeskTopDoc != NULL && selector != NULL &&
				   selector->GetNumMSelectedObjects() > 1);
}

void QMainFrame::OnObjectUngroupObjs() {
	// Sanity.
	ASSERT(selector->GetNumMSelectedObjects() == 1);

	Object *selObj = (selector->GetMSelectedObjects())[0].GetPtr();
	OpGrouping *opGroup = new OpGrouping(selObj);
	opManager.RegisterOp(*opGroup);
}

void QMainFrame::OnUpdateObjectUngroupObjs(CCmdUI* pCmdUI) {
	bool group = false;

	if (pDeskTopDoc != NULL && selector != NULL &&
		selector->GetNumMSelectedObjects() == 1) {
		Object *pObj = (selector->GetMSelectedObjects())[0].GetPtr();
		if (pObj->GetNumChildren() > 0)
			group = true;
	}
		
	pCmdUI->Enable(group);
}

//===== Object: Scope up and down =====

void QMainFrame::OnObjectScopeDown() {
	ASSERT(selector->GetNumMSelectedObjects() == 1);
	Object *pNewScope = (selector->GetMSelectedObjects())[0].GetPtr();
	LinkList<ObjectPtr> slctObjs;
	
	OpScopeChange *op = new OpScopeChange(pNewScope, slctObjs);
	CommitOperation(*op);
}

void QMainFrame::OnUpdateObjectScopeDown(CCmdUI* pCmdUI) {
	BOOL flag = FALSE;

	if (pDeskTopDoc != NULL && selector != NULL &&
		selector->GetNumMSelectedObjects() == 1) {
		Object *pObj = (selector->GetMSelectedObjects())[0].GetPtr();
		if (pObj->GetNumChildren() > 0)
			flag  = TRUE;
	}

	pCmdUI->Enable(flag);
}

void QMainFrame::OnObjectScopeUp() {
	ASSERT(editScope->GetParentPtr() != NULL);
	Object *pNewScope = editScope->GetParentPtr();
	LinkList<ObjectPtr> slctObjs;

	slctObjs.AppendNode(*(new ObjectPtr(editScope)));

	OpScopeChange *op = new OpScopeChange(pNewScope, slctObjs);
	CommitOperation(*op);
}

void QMainFrame::OnUpdateObjectScopeUp(CCmdUI* pCmdUI) {
	BOOL flag = FALSE;

	if (pDeskTopDoc != NULL && selector != NULL &&
		editScope != NULL && editScope->GetParentPtr() != NULL) {
		flag  = TRUE;
	}

	pCmdUI->Enable(flag);
}

//==== Object: Load / Save Prefabs ====

void QMainFrame::OnObjectLoadPrefab() {
	CString szFilters = "Qoole Files (*.qle; *.map)|*.qle; *.map||";
	const char *prefabDir = GetPrefabManager()->GetPrefabDir();

	char origDir[256];
	GetCurrentDirectory(256, origDir);
	SetCurrentDirectory(prefabDir);

	CFileDialog dlg(TRUE, "qle", NULL,
		OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR,
		szFilters, this);

	if (dlg.DoModal() == IDOK) {
		CString filePathName = dlg.GetPathName();
		GetPrefabManager()->AddPrefab(filePathName);
	}
	SetCurrentDirectory(origDir);
}

void QMainFrame::OnUpdateObjectLoadPrefab(CCmdUI* pCmdUI) {
	if (pDeskTopDoc != NULL)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void QMainFrame::OnObjectSavePrefab() {
	ASSERT(selector->GetNumMSelectedObjects() == 1);
	Object *pObj = (selector->GetMSelectedObjects())[0].GetPtr();
	ASSERT(!pObj->IsItemNode());

	CString szFilters = "Qoole Files (*.qle)|*.qle||";
	const char *prefabDir = GetPrefabManager()->GetPrefabDir();

	char origDir[256];
	GetCurrentDirectory(256, origDir);
	SetCurrentDirectory(prefabDir);

	CFileDialog dlg(FALSE, "qle", NULL,
		OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT,
		szFilters, this);

	if (dlg.DoModal() == IDOK) {
		CString filePathName = dlg.GetPathName();
		pObj->SaveObjFile(filePathName);
	}

	SetCurrentDirectory(origDir);

 	GetPrefabManager()->RefreshView();
}

void QMainFrame::OnUpdateObjectSavePrefab(CCmdUI* pCmdUI) {
	BOOL enable = TRUE;

	if (selector->GetNumMSelectedObjects() != 1) {
		enable = FALSE;
	}
	else {
		Object *pObj = (selector->GetMSelectedObjects())[0].GetPtr();
		if (pObj->IsItemNode())
			enable = FALSE;
	}

	pCmdUI->Enable(enable);
}

//==== Object: Properties ====

void QMainFrame::OnObjectProperties() {
	if (pDeskTopDoc == NULL)
		return;

	if (pPropWnd != NULL) {
		// Window already exists.  Set focus and return.
		if (GetFocus() != pPropWnd)
			pPropWnd->SetFocus();
		return;
	}

	// Determin which property page to activate.
	int showPage = 1;
	if (selector->GetNumMSelectedObjects() == 0) {
		// Worldspawn entity.
		showPage = 2;
	}
	else if (selector->GetNumMSelectedObjects() == 1) {
		Object *pSelObj = (selector->GetMSelectedObjects())[0].GetPtr();
		if (pSelObj->HasEntity())
			showPage = 2;
		else if (pSelObj->IsLeaf())
			showPage = 3;
	}

	CRect *pRect = (propWndRect.IsRectNull() ? NULL : &propWndRect);
	pPropWnd = new QPropSheet(this, showPage, pRect);
}

void QMainFrame::OnDestroyPropWnd(CRect &wndRect) {
	ASSERT(pPropWnd != NULL);
	pPropWnd = NULL;
	propWndRect = wndRect;
}


//===== Wndow: Creating and Managing QVFrames in Desktop =====

void QMainFrame::OnWindowNewView(UINT nID) {
	int x, y, width, height;

	x = y = CW_USEDEFAULT;
	width = height = CW_USEDEFAULT;

	int i = nID - ID_VIEW_NEWTOP;
	if (defaultWinLayout && qvFrames[i] == NULL) {
		RECT mdiClientRect;
		::GetWindowRect(m_hWndMDIClient, &mdiClientRect);
		width = (mdiClientRect.right - mdiClientRect.left - 1) / 2 - 1; 
		height = (mdiClientRect.bottom - mdiClientRect.top - 1) / 2 - 1;
		x = i / 2 * width;
		y = i % 2 * height;
	}

	CreateQVFrame(nID, x, y, width, height);
}

void QMainFrame::CreateQVFrame(UINT nID, int x, int y, int width, int height) {
	// Sanity.
	ASSERT(pDeskTopDoc != NULL);

	CDocTemplate *pTemplate = pDeskTopDoc->GetDocTemplate();
	ASSERT_VALID(pTemplate);
	CFrameWnd *pFrame = pTemplate->CreateNewFrame(pDeskTopDoc, NULL);
	if (pFrame == NULL) {
		TRACE0("Warning: failed to create new frame.\n");
		return;     // command failed
	}

	// Find the new QView inside the QVFrame.
	CWnd* pWnd = pFrame->GetDescendantWindow(AFX_IDW_PANE_FIRST, TRUE);
	ASSERT(pWnd != NULL && pWnd->IsKindOf(RUNTIME_CLASS(QView)));
	QView *pQView = (QView *) pWnd;

	// Init the new QView inside the QVFrame.
	pQView->InitView(nID, editScope, selector, editFocusPos);

	// Remember the standard 4 QViews.
	int winID = nID - ID_VIEW_NEWTOP;
	if (qvFrames[winID] == NULL) 
		qvFrames[winID] = pFrame;

	// Set the QVFrame's Title.
	static const char *qvFrameWinTitles[] =	{
		"Top", "Back", "3D View", "Side"
	};

	pFrame->SetWindowText(qvFrameWinTitles[winID]);

	// Position the new window.
	if (width != CW_USEDEFAULT || height != CW_USEDEFAULT)
		pFrame->MoveWindow(x, y, width, height, TRUE);

	// Make new frame visible.
	pTemplate->InitialUpdateFrame(pFrame, pDeskTopDoc);
}

void QMainFrame::RegisterDestroyQVFrame(CMDIChildWnd *pFrame) {
	// Check against the standard views.
	for(int i = 0; i < 4; i++) {
		if (qvFrames[i] == pFrame)
			qvFrames[i] = NULL;
	}
}

void QMainFrame::DestroyAllQVFrames(void) {
	if (pDeskTopDoc == NULL)
		return;

	// Close all QVFrame windows on the desktop.
	CMDIChildWnd *pFrame;
	
	// don't destroy document while closing views
	BOOL bAutoDelete = pDeskTopDoc->m_bAutoDelete;
	pDeskTopDoc->m_bAutoDelete = FALSE;

	while ((pFrame = MDIGetActive()) != NULL)
		pFrame->MDIDestroy();

	pDeskTopDoc->m_bAutoDelete = bAutoDelete;
}

QView *QMainFrame::IsQViewValid(QView *pView) const {

	UINT nID = (UINT) pView;
	if (nID >= 0 && nID <= 3) {
		if (qvFrames[nID] == NULL)
			return NULL;

		// Find the new QView inside the QVFrame.
		CWnd* pWnd = qvFrames[nID]->GetDescendantWindow(AFX_IDW_PANE_FIRST, TRUE);
		ASSERT(pWnd != NULL && pWnd->IsKindOf(RUNTIME_CLASS(QView)));
		return (QView *) pWnd;
	}

	// Make sure pView still exists and is valid.
	POSITION pos = pDeskTopDoc->GetFirstViewPosition();
	while (pos != NULL) {
		if (pView == (QView *) pDeskTopDoc->GetNextView(pos))
			return pView;
   }   

	return NULL;
}

QView *QMainFrame::GetStdQView(UINT qvType) const {
	ASSERT (qvType >= ID_VIEW_NEWTOP && qvType <= ID_VIEW_NEWSIDE);
	qvType -= ID_VIEW_NEWTOP;

	if (qvFrames[qvType] == NULL)
		return NULL;

	CWnd* pWnd = qvFrames[qvType]->GetDescendantWindow(AFX_IDW_PANE_FIRST, TRUE);
	ASSERT(pWnd != NULL && pWnd->IsKindOf(RUNTIME_CLASS(QView)));

	return (QView *) pWnd;
}

//===== Edit Focus and Locking Views =====
bool QMainFrame::IsLockedView(QView *pView) {
	int i;
	CWnd *pWnd;

	for(i = 0; i < 4; i++) {
		if (qvFrames[i] == NULL)
			continue;

		pWnd = qvFrames[i]->GetDescendantWindow(AFX_IDW_PANE_FIRST, TRUE);
		ASSERT(pWnd != NULL && pWnd->IsKindOf(RUNTIME_CLASS(QView)));
		if ((QView *) pWnd != pView)
			continue;

		return (i == 2 ? lock3dView : lock2dViews);
	}

	return false;
}

void QMainFrame::Set2DLockedZoom(float newZoomVal, bool updateViews) {
	if (!lock2dViews)
		return;

	int i;
	CWnd *pWnd;
	QView *pView;
	Vector3d posVec;
	SphrVector oriVec;
	float zoomVal;

	for(i = 0; i < 4; i++) {
		if (i == 2 || qvFrames[i] == NULL)
			continue;

		pWnd = qvFrames[i]->GetDescendantWindow(AFX_IDW_PANE_FIRST, TRUE);
		ASSERT(pWnd != NULL && pWnd->IsKindOf(RUNTIME_CLASS(QView)));
		pView = (QView *) pWnd;

		pView->GetViewState(posVec, oriVec, zoomVal);
		pView->SetViewState(posVec, oriVec, newZoomVal, updateViews);
	}
}

void QMainFrame::SetEditFocusPos(Vector3d &focusVec, bool updateViews) {
	Vector3d oldFocus(editFocusPos);
	editFocusPos = focusVec;

	if (View::IsCrossHairShown())
		View::SetCrossHairPos(editFocusPos);

	int i;
	Vector3d posVec;
	SphrVector oriVec;
	float zoomVal;

	CWnd *pWnd;
	QView *pView;

	for(i = 0; i < 4; i++) {
		if (qvFrames[i] == NULL)
			continue;

		pWnd = qvFrames[i]->GetDescendantWindow(AFX_IDW_PANE_FIRST, TRUE);
		ASSERT(pWnd != NULL && pWnd->IsKindOf(RUNTIME_CLASS(QView)));
		pView = (QView *) pWnd;

		if (i == 2 && lock3dView) {
			pView->GetViewState(posVec, oriVec, zoomVal);
			posVec.SubVector(oldFocus);
			ASSERT(posVec.GetMag() >= 1.0f);
			posVec.NewVector(0.0f, -posVec.GetMag(), 0.0f);

			Matrix44 trans;
 			trans.SetRotate(oriVec);
			trans.Transform(posVec);

			posVec.AddVector(editFocusPos);
			pView->SetViewState(posVec, oriVec, zoomVal, updateViews);
		}
		else if (i != 2 && lock2dViews) {
			pView->GetViewState(posVec, oriVec, zoomVal);
			pView->SetViewState(editFocusPos, oriVec, zoomVal, updateViews);
		}
	}
}

void QMainFrame::GetLockedQViews(QView *qvsArray[]) {
	CWnd *pWnd;
	for(int i = 0; i < 4; i++) {
		qvsArray[i] = NULL;
		if (qvFrames[i] != NULL) {
			pWnd = qvFrames[i]->GetDescendantWindow(AFX_IDW_PANE_FIRST, TRUE);
			ASSERT(pWnd != NULL && pWnd->IsKindOf(RUNTIME_CLASS(QView)));
			if ((i == 2 && (lock3dView || lock2dViews)) || (i != 2 && lock2dViews))
				qvsArray[i] = (QView *) pWnd;
		}
	}
}

//===== View Menu Commands =====
void QMainFrame::OnViewDecViewDepth() {
	AdjustQViewDepths(-100.0f);
}

void QMainFrame::OnViewIncViewDepth() {
	AdjustQViewDepths(100.0f);
}

void QMainFrame::AdjustQViewDepths(float deltaDepth) {
	if (pDeskTopDoc == NULL)
		return;

	CView *pView;
	POSITION pos = pDeskTopDoc->GetFirstViewPosition();
	while (pos != NULL) {
		pView = pDeskTopDoc->GetNextView(pos);
		if (!pView->IsKindOf(RUNTIME_CLASS(QView)))
			continue;
		((QView *) pView)->AdjustViewDepth(deltaDepth);
	}
}

void QMainFrame::OnViewFocusOrigin() {
	if (pDeskTopDoc == NULL)
		return;

	Vector3d centerVec(0.0f, 0.0f, 0.0f);
	editFocusPos = centerVec;
	// FocusAllQViews(centerVec);

	// Hack. Fixes the scale box update problem.
	if (opMode == ID_MODE_OBJECTSCALE) {
		FocusAllQViews(centerVec, false);
		SwitchOpMode(ID_MODE_OBJECTSCALE);
	}
	else {
		FocusAllQViews(centerVec);
	}
}

void QMainFrame::OnViewFocusSelection() {
	if (pDeskTopDoc == NULL)
		return;

	if (selector->GetNumMSelectedObjects() == 0)
		return;

	Vector3d centerVec;
	selector->GetMSelectedObjectsCenter(centerVec);
	editFocusPos = centerVec;
	// FocusAllQViews(centerVec);

	// Hack. Fixes the scale box update problem.
	if (opMode == ID_MODE_OBJECTSCALE) {
		FocusAllQViews(centerVec, false);
		SwitchOpMode(ID_MODE_OBJECTSCALE);
	}
	else {
		FocusAllQViews(centerVec);
	}
}

void QMainFrame::OnUpdateViewFocusSelection(CCmdUI* pCmdUI) {
	bool flag = (selector && selector->GetNumMSelectedObjects() > 0);
	pCmdUI->Enable(flag);
}

void QMainFrame::FocusAllQViews(const Vector3d &centerVec, bool updateViews) {
	CView *pView;
	QView *pQView;
	Vector3d posVec;
	SphrVector oriVec;
	float zoom;
	UINT qvType;
	Matrix44 trans;

	// Iter through all views of doc.
	POSITION pos = pDeskTopDoc->GetFirstViewPosition();
	while (pos != NULL) {
		pView = pDeskTopDoc->GetNextView(pos);
		if (!pView->IsKindOf(RUNTIME_CLASS(QView)))
			continue;
		pQView = (QView *) pView;
		pQView->GetViewState(posVec, oriVec, zoom, &qvType);
		if (qvType != ID_VIEW_NEW3D) {
			posVec = centerVec;
		}
		else {
			posVec.NewVector(0.0f, -QV_DEFAULT_ORBITDIST, 0.0f);
			(trans.SetRotate(oriVec)).Transform(posVec);
			posVec.AddVector(centerVec);
		}
		pQView->SetViewState(posVec, oriVec, zoom, updateViews);
   }   
}

void QMainFrame::OnViewDisplayEntities() {
	displayEntities = !displayEntities;
	View::DisplayEntities(displayEntities);
	pDeskTopDoc->UpdateAllViews(NULL, DUAV_QVIEWS | DUAV_SUPDATEALL, NULL);
}

void QMainFrame::OnUpdateViewDisplayEntities(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(displayEntities);
}

void QMainFrame::Set2DViewsLock(bool lock) {
	lock2dViews = lock;
	View::ShowCrossHair(lock);
}

void QMainFrame::OnViewLock2DViews() {
	Set2DViewsLock(!lock2dViews);
}

void QMainFrame::OnUpdateViewLock2DViews(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(lock2dViews);
}

void QMainFrame::Set3DViewLock(bool lock) {
	lock3dView = lock;
}

void QMainFrame::OnViewLock3DView() {
	Set3DViewLock(!lock3dView);
}

void QMainFrame::OnUpdateViewLock3DView(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(lock3dView);
}

//====================
// Called by the MFC Framework whenever the client
//  area of the frame window is being changed.
// Force resize and reposition of the 4x QVFrames here.
void QMainFrame::RecalcLayout(BOOL bNotify) {
	static bool inRecalcLayout = false;

	// avoid being indefinately recursively called.
	if (inRecalcLayout)
		return;

	inRecalcLayout = true;
	CMRCMDIFrameWndSizeDock::RecalcLayout(bNotify);

	// Reposition the 4x QVFrames.
	if (pDeskTopDoc != NULL && defaultWinLayout) {
		QuadViews(false);
		SendMessage(WM_COMMAND, ID_WINDOW_ARRANGE, 0L);
		pDeskTopDoc->UpdateAllViews(NULL, DUAV_QVIEWS | DUAV_SUPDATEALL, NULL);
	}

	inRecalcLayout = false;
}

// Reposition the standard 4 editing
//  windows to default positions.
BOOL QMainFrame::QuadViews(bool createWnds) {
	// Avoid calling QuadViews recursively.
	static bool inQuadViews = false;

	if (inQuadViews)
		return TRUE;

	inQuadViews = true;


	ASSERT(m_hWndMDIClient != NULL);
			
	RECT mdiClientRect;
	int i, x, y, width, height;
	WINDOWPLACEMENT wndpl;

	::GetWindowRect(m_hWndMDIClient, &mdiClientRect);
	width = (mdiClientRect.right - mdiClientRect.left - 1) / 2 - 1; 
	height = (mdiClientRect.bottom - mdiClientRect.top - 1) / 2 - 1;

	// Create the 4 windows if they don't exist and 
	//  createWnds == true.  Resize and position them.
	for(i = 0; i < 4; i++) {
		x = i / 2 * width;
		y = i % 2 * height;
		if (qvFrames[i] == NULL && createWnds)
			CreateQVFrame(ID_VIEW_NEWTOP + i, x, y, width, height);
		else if (qvFrames[i] != NULL &&
			 qvFrames[i]->GetWindowPlacement(&wndpl)) {
			if (wndpl.showCmd != SW_SHOWNORMAL) {
				wndpl.showCmd = SW_SHOWNORMAL;
				qvFrames[i]->SetWindowPlacement(&wndpl);
			}
			qvFrames[i]->MoveWindow(x, y, width, height, TRUE);
		}
	}

	// Set focus to the top (first) window.
	for(i = 0; i < 4; i++) {
		if (qvFrames[i] != NULL) {
			MDIActivate(qvFrames[i]);
			break;
		}
	}

	inQuadViews = false;

	// Not checking for frame creation errors.
	// Deal with it later.
	return TRUE;
}

void QMainFrame::OnWindowDefaultlayout() {
	defaultWinLayout = !defaultWinLayout;
	if (defaultWinLayout) {
		QuadViews(true);
	}
}

void QMainFrame::OnWindowUnDefault() {
	UINT nID = (UINT) LOWORD(GetCurrentMessage()->wParam);
	CMRCMDIFrameWndSizeDock::OnMDIWindowCmd(nID);
	defaultWinLayout = false;
}

void QMainFrame::OnUpdateWindowDefaultlayout(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(defaultWinLayout);
}

void QMainFrame::OnActivateFrame(UINT nID) {
	ASSERT(pDeskTopDoc != NULL);

	nID -= ID_WINDOW_ACTIVATE_TOP;
	ASSERT(nID >= 0 && nID < 4);

	if (qvFrames[nID] == NULL)
		OnWindowNewView(nID + ID_VIEW_NEWTOP);

	ASSERT(qvFrames[nID] != NULL);
	MDIActivate(qvFrames[nID]);
}

void QMainFrame::OnUpdateUIActiveFrame(CCmdUI *pCmdUI) {
	UINT index = pCmdUI->m_nID - ID_WINDOW_ACTIVATE_TOP;
	BOOL enable, check;

	enable = (pDeskTopDoc != NULL);
	check = (enable && MDIGetActive() == qvFrames[index]);

	pCmdUI->Enable(enable);
	pCmdUI->SetCheck(check);
}

void QMainFrame::OnActivateWSTab(UINT nID) {
	nID -= ID_WINDOW_ACTIVATE_TREEVIEW;
	ASSERT(nID >= 0 && nID < 3);

	if (!tbWorkSpaceBar.IsWindowVisible())
		SendMessage(WM_COMMAND, ID_WORKSPACE);

	tbWorkSpaceBar.ShowSelTabWindow(nID);
}

void QMainFrame::OnUpdateUIActivateWSTab(CCmdUI *pCmdUI) {
	UINT index = pCmdUI->m_nID - ID_WINDOW_ACTIVATE_TREEVIEW;
	bool active = (tbWorkSpaceBar.GetTabWindow() ==
					tbWorkSpaceBar.GetTabWindow(index));
	pCmdUI->SetCheck(active);
}

//===== Suspend QViews' render library upon run game =====
void QMainFrame::SuspendRenderLib(bool suspend) {
	ASSERT(pDeskTopDoc != NULL);
	if (pDeskTopDoc == NULL)
		return;

	// Iter through all views of doc and find all qviews.
	CView *pView;
	POSITION pos = pDeskTopDoc->GetFirstViewPosition();
	while (pos != NULL) {
		pView = pDeskTopDoc->GetNextView(pos);
		if (!pView->IsKindOf(RUNTIME_CLASS(QView)))
			continue;
		((QView *) pView)->SuspendRenderLib(suspend);
   }   
}

//===== Handle right click context menu =====
void QMainFrame::OnContextMenu(CWnd* pWnd, CPoint point) {

	CRect rect;
	GetClientRect(&rect);
	ClientToScreen(&rect);

	if (!pWnd->IsKindOf(RUNTIME_CLASS(CToolBar)) &&
		!pWnd->IsKindOf(RUNTIME_CLASS(CStatusBar)) &&
		rect.PtInRect(point)) {
		CMenu rMenu;
		rMenu.LoadMenu(IDR_MAINFRAME_CONTEXT);

		CMenu *pContextMenu = rMenu.GetSubMenu(0);
		UINT nFlag = TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON;
		pContextMenu->TrackPopupMenu(nFlag, point.x, point.y, this);
		return;
	}

	CFrameWnd::OnContextMenu(pWnd, point);
}

//========== Add Brush/Entity Popup Menus ==========

#define PME_OBJLOAD		1
#define PME_ADDENT		2
#define PME_APPLYENT	3

bool QMainFrame::InitPopupMenus(void) {
	for(int i = 0; i < ID_MENUENTRY_END - ID_MENUENTRY_START; i++)
		menuEntry[i] = NULL;

	pmView.CreatePopupMenu();
	pmView.AppendMenu(MF_POPUP, 0, "Add Brush");
	pmView.AppendMenu(MF_SEPARATOR);
	pmView.AppendMenu(MF_STRING, 0, "Add Entity");
	pmView.AppendMenu(MF_STRING, 0, "Apply Entity");

	return true;
}

void QMainFrame::DestroyPopupMenus(void) {
	for(int i = 0; i < ID_MENUENTRY_END - ID_MENUENTRY_START; i++)
		DeleteMenuEntry(i);
}

void QMainFrame::AddMenuEntry(CMenu &menu, char *text, int type, char *data) {
	for(int i = 0; i < ID_MENUENTRY_END - ID_MENUENTRY_START; i++)
		if(!menuEntry[i])
			break;

	menuEntry[i] = new MenuEntry;
	menuEntry[i]->type = type;
	menuEntry[i]->data = new char[strlen(data) + 1];
	strcpy(menuEntry[i]->data, data);
	
	menu.AppendMenu(MF_STRING, i + ID_MENUENTRY_START, text);
}

void QMainFrame::DeleteMenuEntry(int nID) {
	if(!menuEntry[nID])
		return;
	delete menuEntry[nID]->data;
	delete menuEntry[nID];
	menuEntry[nID] = NULL;
}

void QMainFrame::OnMenuEntry(UINT nID) {
	MenuEntry *pEntry = menuEntry[nID - ID_MENUENTRY_START];
	ASSERT(pEntry);

	if (pEntry->type == PME_OBJLOAD)
		LoadObject(pEntry->data);
	else if (pEntry->type == PME_ADDENT)
		AddEntity(pEntry->data);
	else if (pEntry->type == PME_APPLYENT)
		ApplyEntity(pEntry->data);
}

void QMainFrame::OnApplyNone(void) {
	ApplyEntity("");
}

void QMainFrame::LoadObject(const char *name) {
	CMDIChildWnd *pFrame = MDIGetActive();
	ASSERT_VALID(pFrame);
	QView *pQView =	(QView *)
		pFrame->GetDescendantWindow(AFX_IDW_PANE_FIRST, TRUE);
	ASSERT_VALID(pQView);
	View *pView = pQView->GetViewPtr();

	Object *obj = new Object;
	if (!obj->LoadObjFile(name)) {
		LError("%s not found.", name);
		delete obj;
		return;
	}

	LinkList<ObjectPtr> objs;
	objs.AppendNode(*(new ObjectPtr(obj)));

	Selector *pSlct = &GetSelector();

	Vector3d centerVec;
	if (IsLockedView(pQView)) {
		centerVec = GetEditFocusPos();
	}
	else if (pQView->GetViewType() == ID_VIEW_NEW3D) {
		centerVec.NewVector(0.0f, 128.0f, 0.0f);
		Matrix44 trans;
		pView->CalTransSpaceMatrix(trans.SetIdentity());
		trans.Transform(centerVec);
	}
	else {
		centerVec = pView->GetPosition();
	}

	pQView->SnapAddObjPos(centerVec);

	QooleDoc *pDoc = pQView->GetDocument();
	Texture *pTexture = GetTextureView()->GetSelTexture();
	if(!pTexture)
		pTexture = pDoc->GetGame()->GetDefaultTexture();
	if(pTexture)
		pDoc->TextureApplyObjs(objs, pTexture->GetName(), false);

	OpObjsAddNew *op = new OpObjsAddNew(objs, centerVec, pSlct->GetScopePtr());
	CommitOperation(*op);
}

void QMainFrame::AddEntity(const char *name) {
	CMDIChildWnd *pFrame = MDIGetActive();
	ASSERT_VALID(pFrame);
	QView *pQView =	(QView *)
		pFrame->GetDescendantWindow(AFX_IDW_PANE_FIRST, TRUE);
	ASSERT_VALID(pQView);
	View *pView = pQView->GetViewPtr();

	Object *obj = new Object;
	Entity *ent = new Entity(name);
	obj->SetEntity(ent);

	LinkList<ObjectPtr> objs;
	objs.AppendNode(*(new ObjectPtr(obj)));

	Selector *pSlct = &GetSelector();

	Vector3d centerVec;
	if (IsLockedView(pQView)) {
		centerVec = GetEditFocusPos();
	}
	else if (pQView->GetViewType() == ID_VIEW_NEW3D) {
		centerVec.NewVector(0.0f, 128.0f, 0.0f);
		Matrix44 trans;
		pView->CalTransSpaceMatrix(trans.SetIdentity());
		trans.Transform(centerVec);
	}
	else {
		centerVec = pView->GetPosition();
	}

	pQView->SnapAddObjPos(centerVec);

	OpObjsAddNew *op = new OpObjsAddNew(objs, centerVec, pSlct->GetScopePtr());
	CommitOperation(*op);
}

void QMainFrame::ApplyEntity(const char *name) {
	Selector *pSlctr = &(pQMainFrame->GetSelector());
	if(pSlctr->GetNumMSelectedObjects() == 0)
		return;

	OpEntityApply *op = new OpEntityApply(name);
	CommitOperation(*op);
}


void QMainFrame::BuildBrushMenu(void) {
	CMenu pmSub[5];
	CMenu pmBrushMenu;

	QDraw::OutputText("Creating build brush menu... ");
	pmSub[0].CreatePopupMenu();
	AddMenuEntry(pmSub[0], "House", PME_OBJLOAD, "brushes/house.qle");
	AddMenuEntry(pmSub[0], "Plate (small)", PME_OBJLOAD, "brushes/plate2.qle");
	AddMenuEntry(pmSub[0], "Plate (large)", PME_OBJLOAD, "brushes/plate.qle");
	AddMenuEntry(pmSub[0], "Tent", PME_OBJLOAD, "brushes/tent.qle");
	AddMenuEntry(pmSub[0], "Trapezoid", PME_OBJLOAD, "brushes/trapzoid.qle");

	pmSub[1].CreatePopupMenu();
	AddMenuEntry(pmSub[1], "3 sided", PME_OBJLOAD, "brushes/column3.qle");
	AddMenuEntry(pmSub[1], "4 sided", PME_OBJLOAD, "brushes/column4.qle");
	AddMenuEntry(pmSub[1], "5 sided", PME_OBJLOAD, "brushes/column5.qle");
	AddMenuEntry(pmSub[1], "6 sided", PME_OBJLOAD, "brushes/column6.qle");
	AddMenuEntry(pmSub[1], "8 sided", PME_OBJLOAD, "brushes/column8.qle");
	AddMenuEntry(pmSub[1], "10 sided", PME_OBJLOAD, "brushes/column10.qle");
	AddMenuEntry(pmSub[1], "12 sided", PME_OBJLOAD, "brushes/column12.qle");
	AddMenuEntry(pmSub[1], "16 sided", PME_OBJLOAD, "brushes/column16.qle");

	pmSub[2].CreatePopupMenu();
	AddMenuEntry(pmSub[2], "3 sided", PME_OBJLOAD, "brushes/tripr3.qle");
	AddMenuEntry(pmSub[2], "4 sided", PME_OBJLOAD, "brushes/tripr4.qle");
	AddMenuEntry(pmSub[2], "5 sided", PME_OBJLOAD, "brushes/tripr5.qle");
	AddMenuEntry(pmSub[2], "6 sided", PME_OBJLOAD, "brushes/tripr6.qle");
	AddMenuEntry(pmSub[2], "8 sided", PME_OBJLOAD, "brushes/tripr8.qle");
	AddMenuEntry(pmSub[2], "10 sided", PME_OBJLOAD, "brushes/tripr10.qle");
	AddMenuEntry(pmSub[2], "12 sided", PME_OBJLOAD, "brushes/tripr12.qle");
	AddMenuEntry(pmSub[2], "16 sided", PME_OBJLOAD, "brushes/tripr16.qle");

	pmSub[3].CreatePopupMenu();
	AddMenuEntry(pmSub[3], "2x4", PME_OBJLOAD, "brushes/lumber/2x4.qle");
	AddMenuEntry(pmSub[3], "2x6", PME_OBJLOAD, "brushes/lumber/2x6.qle");
	AddMenuEntry(pmSub[3], "2x8", PME_OBJLOAD, "brushes/lumber/2x8.qle");
	AddMenuEntry(pmSub[3], "2x10", PME_OBJLOAD, "brushes/lumber/2x10.qle");
	AddMenuEntry(pmSub[3], "2x12", PME_OBJLOAD, "brushes/lumber/2x12.qle");
	AddMenuEntry(pmSub[3], "4x4", PME_OBJLOAD, "brushes/lumber/4x4.qle");
	AddMenuEntry(pmSub[3], "6x6", PME_OBJLOAD, "brushes/lumber/6x6.qle");
	AddMenuEntry(pmSub[3], "8x8", PME_OBJLOAD, "brushes/lumber/8x8.qle");

	pmSub[4].CreatePopupMenu();
	AddMenuEntry(pmSub[4], "14 sided", PME_OBJLOAD, "brushes/spheres/sphr14.qle");
	AddMenuEntry(pmSub[4], "18 sided", PME_OBJLOAD, "brushes/spheres/sphr18.qle");
	AddMenuEntry(pmSub[4], "20 sided", PME_OBJLOAD, "brushes/spheres/sphr20.qle");
	AddMenuEntry(pmSub[4], "26 sided A", PME_OBJLOAD, "brushes/spheres/sphr26a.qle");
	AddMenuEntry(pmSub[4], "26 sided B", PME_OBJLOAD, "brushes/spheres/sphr26b.qle");
	AddMenuEntry(pmSub[4], "26 sided C", PME_OBJLOAD, "brushes/spheres/sphr26c.qle");

	pmBrushMenu.CreatePopupMenu();
	AddMenuEntry(pmBrushMenu, "Cube (small)", PME_OBJLOAD, "brushes/cube.qle");
	AddMenuEntry(pmBrushMenu, "Cube (medium)", PME_OBJLOAD, "brushes/cube2.qle");
	AddMenuEntry(pmBrushMenu, "Cube (large)", PME_OBJLOAD, "brushes/cube3.qle");

	pmBrushMenu.AppendMenu(MF_POPUP, (UINT) pmSub[0].Detach(), "Misc");
	pmBrushMenu.AppendMenu(MF_POPUP, (UINT) pmSub[1].Detach(), "Cylinder");
	pmBrushMenu.AppendMenu(MF_POPUP, (UINT) pmSub[2].Detach(), "Pyramid");
	pmBrushMenu.AppendMenu(MF_POPUP, (UINT) pmSub[3].Detach(), "Lumber");
	pmBrushMenu.AppendMenu(MF_POPUP, (UINT) pmSub[4].Detach(), "Sphere");

	// add to right-click menu
	pmView.ModifyMenu(0, MF_BYPOSITION | MF_POPUP,
		(UINT)pmBrushMenu.m_hMenu, "Add Brush");

	// add to top menu bar
	CMenu *menu = GetMenu();
	menu = menu->GetSubMenu(3);

	menu->ModifyMenu(0, MF_BYPOSITION | MF_POPUP,
		(UINT)pmBrushMenu.m_hMenu, "Add Brush");

	QDraw::OutputText("OK.\n");
	pmBrushMenu.Detach();
}

/*
void QMainFrame::BuildAddPrefabMenu(CMenu *menu, char *dir) {
	char *name;

	LFindFiles findfiles(dir);
	while(name = findfiles.Next()) {
		char filename[256];
		sprintf(filename, "%s\\%s", dir, name);
		AddMenuEntry(*menu, name, PME_OBJLOAD, filename);
	}

	LFindDirs finddirs(dir);
	while(name = finddirs.Next()) {
		char newdir[256];
		CMenu subMenu;
		subMenu.CreatePopupMenu();

		sprintf(newdir, "%s\\%s", dir, name);
		BuildAddPrefabMenu(&subMenu, newdir);

		menu->AppendMenu(MF_POPUP, (UINT) subMenu.Detach(), name);
	}
}
*/

void QMainFrame::BuildEntityMenus(void) {
	CMenu pmAddMenu, pmApplyMenu;

	QDraw::OutputText("Creating build entities menu... ");

	pmAddMenu.CreatePopupMenu();
	pmApplyMenu.CreatePopupMenu();

	ASSERT_VALID(pDeskTopDoc);
	EntList *entList = pDeskTopDoc->GetEntList();

	pmApplyMenu.AppendMenu(MF_STRING, ID_OBJECT_APPLYNONE, "None");

	if(!entList)
		return;

	CMenu pmAdd[128], pmApply[128];
	int cnt;

	IterLinkList<EntClass> iterClass(entList->GetClassList());
	EntClass *entClass;

	cnt = 0;
	iterClass.Reset();
	while(!iterClass.IsDone()) {
		entClass = iterClass.GetNext();
		entClass->count = cnt;
		pmAdd[cnt].CreatePopupMenu();
		pmApply[cnt].CreatePopupMenu();
		cnt++;
	}		

	cnt = 0;
	IterLinkList<EntInfo> iterInfo(entList->GetInfoList());
	EntInfo *info;

	iterInfo.Reset();
	while(!iterInfo.IsDone()) {
		info = iterInfo.GetNext();
		if(!info->IsModel())
			AddMenuEntry(pmAdd[info->GetEntClass()->count],
				info->GetClassdesc(), PME_ADDENT, info->GetClassname());
		else
			AddMenuEntry(pmApply[info->GetEntClass()->count],
				info->GetClassdesc(), PME_APPLYENT, info->GetClassname());
	}

	cnt = 0;
	iterClass.Reset();
	while(!iterClass.IsDone()) {
		entClass = iterClass.GetNext();
		if(entClass->nonmodels)
			pmAddMenu.AppendMenu(MF_POPUP, 
				(UINT) pmAdd[cnt].Detach(), entClass->name);
		if(entClass->models && strcmp(entClass->name, "Null"))
			pmApplyMenu.AppendMenu(MF_POPUP, 
				(UINT) pmApply[cnt].Detach(), entClass->name);
		cnt++;
	}		

	// add to right-click menu
	pmView.ModifyMenu(2, MF_BYPOSITION | MF_POPUP,
		(UINT)pmAddMenu.m_hMenu, "Add Entity");
	pmView.ModifyMenu(3, MF_BYPOSITION | MF_POPUP,
		(UINT)pmApplyMenu.m_hMenu, "Apply Entity");

	// add to top menu bar
	CMenu *menu = GetMenu();
	menu = menu->GetSubMenu(3);

	menu->ModifyMenu(1, MF_BYPOSITION | MF_POPUP,
		(UINT)pmAddMenu.m_hMenu, "Add Entity");
	menu->ModifyMenu(2, MF_BYPOSITION | MF_POPUP,
		(UINT)pmApplyMenu.m_hMenu, "Apply Entity");

	pmAddMenu.Detach();
	pmApplyMenu.Detach();
	QDraw::OutputText("OK.\n");
}

//========== Status bar ========
void QMainFrame::UpdateStatusBar(const char *text) {
	CString mesg;

	if (text != NULL) {
		mesg = text;
	}
	else if (opMode == ID_MODE_OBJECTSELECT) {
		mesg.LoadString(ID_STATUS_OBJECTSELECT);
	}
	else if (opMode == ID_MODE_OBJECTMOVE ||
			 opMode == ID_MODE_OBJECTROTATE ||
			 opMode == ID_MODE_OBJECTSCALE) {
		mesg.LoadString(ID_STATUS_OBJECTOPS);
	}
	else if (opMode == ID_MODE_MODIFYBRUSH ||
			 opMode == ID_MODE_FACEMOVE ||
			 opMode == ID_MODE_EDGEMOVE ||
			 opMode == ID_MODE_VERTEXMOVE) {
		mesg.LoadString(ID_STATUS_MODIFYBRUSH);
	}
	else if (opMode == ID_MODE_MIRROR ||
			 opMode == ID_MODE_PLANECLIP) {
		mesg.LoadString(ID_STATUS_PLANEOPS);
	}
	else if (opMode == ID_MODE_EYEMOVE) {
		mesg.LoadString(ID_STATUS_EYEMOVE);
	}
	else if (opMode == ID_MODE_EYEROTATE) {
		mesg.LoadString(ID_STATUS_EYEROTATE);
	}
	else if (opMode == ID_MODE_EYEZOOM) {
		mesg.LoadString(ID_STATUS_EYEZOOM);
	}
	else if (opMode == ID_MODE_FLYTHROUGH) {
		mesg.LoadString(ID_STATUS_FLYTHROUGH);
	}
	else {
		mesg.LoadString(AFX_IDS_IDLEMESSAGE);
	}

	tbStatusBar.SetPaneText(0, mesg);
}

void QMainFrame::OnUpdateGridSize(CCmdUI *pCmdUI) {
	char textBuf[64];

	if (QDraw::drawGridStep1)
		sprintf(textBuf, "Grid Size: %d", QDraw::gridStep1);
	else
		sprintf(textBuf, "Grid off");

	pCmdUI->SetText(textBuf);
}

//================================================================
// Handle Frame activation
//================================================================

void QMainFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) {
	CMRCMDIFrameWndSizeDock::OnActivate(nState, pWndOther, bMinimized);

/*
	CWnd *pWnd = GetPrefabManager()->pPreviewWnd;
	if (pWnd == NULL)
		return;

	if (nState != WA_INACTIVE && pWnd == pWndOther) {
		pWnd->SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}

/*
	else if (nState == WA_INACTIVE) {
		pWnd->SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		// SetWindowPos(pWnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}
*/
}

void QMainFrame::OnActivateApp(BOOL bActive, HTASK hTask) {
	CMRCMDIFrameWndSizeDock::OnActivateApp(bActive, hTask);
/*
	CWnd *pWnd = GetPrefabManager()->pPreviewWnd;
	if (pWnd == NULL || bActive)
		return;
	
	pWnd->SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
*/
}

//======================================================================
// QViewsState
//======================================================================

QViewsState::QViewsState(void) {
	QooleDoc *pDoc = pQMainFrame->pDeskTopDoc;
	CView *pView;
	numQViews = 0;

	// Cycle through all document's views, and count num of qviews.
	POSITION pos = pDoc->GetFirstViewPosition();
	while (pos != NULL) {
		pView = pDoc->GetNextView(pos);
		if (pView->IsKindOf(RUNTIME_CLASS(QView)))
			numQViews++;
	}

	// Sanity.
	ASSERT(numQViews > 0);

	qvArray = (QView **) malloc(sizeof(QView *) * numQViews);
	posArray = (Vector3d **) malloc(sizeof(Vector3d *) * numQViews);
	orientArray = (SphrVector **) malloc(sizeof(SphrVector *) * numQViews);
	zoomArray = new float[numQViews];

	// Remember all the standard views first.
	int i;
	QView *p4Views[4];
	for(i = 0; i < 4; i++)
		p4Views[i] = pQMainFrame->IsQViewValid((QView *) i);

	int count = 0;
	QView *pQView;
	float zoomVal;
	Vector3d *pNewPos;
	SphrVector *pNewOrient;

	// Cycle through all qviews again, and save them this time.
	pos = pDoc->GetFirstViewPosition();
	while (pos != NULL) {
		pView = pDoc->GetNextView(pos);
		if (!pView->IsKindOf(RUNTIME_CLASS(QView)))
			continue;

		// Get the QView information.
		pNewPos = new Vector3d;
		pNewOrient = new SphrVector;
		pQView = (QView *) pView;
		pQView->GetViewState(*pNewPos, *pNewOrient, zoomVal);

		// Is this one of the standard 4x views?
		for(i = 0; i < 4; i++) {
			if (pQView == p4Views[i])
				break;
		}
		if (i < 4) // Yes it is.
			qvArray[count] = (QView *) i;
		else
			qvArray[count] = pQView;
		posArray[count] = pNewPos;
		orientArray[count] = pNewOrient;
		zoomArray[count] = zoomVal;
		count++;
	}
}

QViewsState::~QViewsState() {
	int i;

	for (i = 0; i < numQViews; i++) {
		delete posArray[i];
		delete orientArray[i];
	}

	delete qvArray;
	delete posArray;
	delete orientArray;
	delete zoomArray;
}

void QViewsState::RestoreQViewsState(void) {
	QView *pView;
	int i;
	bool setLockFocus = false;

	for(i = 0; i < numQViews; i++) {
		if ((pView = pQMainFrame->IsQViewValid(qvArray[i])) == NULL)
			continue;

		pView->SetViewState(*posArray[i], *orientArray[i], zoomArray[i]);

		if (!setLockFocus && pQMainFrame->IsLockedView(pView)) {
			setLockFocus = true;
			pQMainFrame->SetEditFocusPos(*posArray[i], false);
		}
	}
}

//======================================================
// QHollowPrompt
//======================================================

QHollowPrompt::QHollowPrompt(int defWidth) 
			  : CDialog(QHollowPrompt::IDD) {
	//{{AFX_DATA_INIT(QHollowPrompt)
	m_bPrompt = FALSE;
	m_iThickness = 8;
	//}}AFX_DATA_INIT
	m_iThickness = defWidth;
}

void QHollowPrompt::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(QHollowPrompt)
	DDX_Check(pDX, IDC_HOLLOW_PROMPT, m_bPrompt);
	DDX_Text(pDX, IDC_HOLLOW_THICKNESS, m_iThickness);
	DDV_MinMaxInt(pDX, m_iThickness, 1, 100);
	//}}AFX_DATA_MAP
}

BOOL QHollowPrompt::OnInitDialog() {
	CDialog::OnInitDialog();
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BEGIN_MESSAGE_MAP(QHollowPrompt, CDialog)
	//{{AFX_MSG_MAP(QHollowPrompt)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//======================================================

void QMainFrame::OnEditTexmgr() {
	QTexMgr texMgr(this);
	texMgr.DoModal();
	tbWorkSpaceBar.GetTextureView()->OnNewContents();
}

void QMainFrame::OnEditTexRep() {
	QTexRep texRep(this);
	texRep.DoModal();
}


void QMainFrame::OnEditColorSel() {
	if(pColorWnd) {
		pColorWnd->SetFocus();
		return;
	}

	pColorWnd = new QColorWnd;
	pColorWnd->Create(IDD_COLORWND);
	pColorWnd->ShowWindow(SW_SHOW);
	pColorWnd->RegisterDocument(pDeskTopDoc);

	if(!colorWndRect.IsRectNull())
		pColorWnd->MoveWindow(&colorWndRect);
}

void QMainFrame::OnDestroyColorWnd(CRect &wndRect) {
	ASSERT(pColorWnd != NULL);
	delete pColorWnd;
	pColorWnd = NULL;
	colorWndRect = wndRect;
}
