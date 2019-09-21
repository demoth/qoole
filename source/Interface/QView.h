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

// QView.h : interface of the QView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_QVIEW_H__620139CE_5AF1_11D1_A7E0_384A12000000__INCLUDED_)
#define AFX_QVIEW_H__620139CE_5AF1_11D1_A7E0_384A12000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "View.h"
#include "QooleDoc.h"
#include "Timer.h"
#include "QDraw.h"

// Forward declaration
class QViewsState;

//======================================================================
// QView
//======================================================================
#define QV_DEFAULT_ORBITDIST	256.0f

class QView : public CView {
protected: // create from serialization only
	QView();
	DECLARE_DYNCREATE(QView)

public:
	virtual ~QView();

	QooleDoc* GetDocument() { return (QooleDoc *) m_pDocument; }

	// QView Init stuff.
	void InitView(UINT nID, Object *viewScope,
				  Selector *selector, const Vector3d &posVec);
	void CreateQDraw(void);
	void SetColors(void);
	void SelColors(int vrc, int red, int grn, int blu);
	void SuspendRenderLib(bool suspend);

	// Used by QMainFrame to control changes in QViews during undo/redo.
	void GetViewState(Vector3d &posVec, SphrVector &oriVec,
					  float &zoomRate, UINT *viewType = NULL) const;
	void SetViewState(const Vector3d &posVec, const SphrVector &oriVec,
					  float zoomRate, bool updateView = false);

	// Op mode changed.
	void OnOpModeChanged(UINT opMode, UINT prevOpMode);

	// Change to fly through mode.
	void OnStartFlyThrough(void);

	// Display face selection.  Used by texture prop window.
	void RenderFaceSlct(bool faceSlct);

	// View Depth.
	void AdjustViewDepth(float incr);

	// Grids Display
	static void UpdateGridsDisplay();

	// View Object.
	View *GetViewPtr(void) { return pView; }
	int GetViewType(void) { return qvType; }

	// Op position.
	static const Vector3d &GetOperateCenterPos(void);
	static void SetOperateCenterPos(const Vector3d &opCenterPos);
	
	// Snap the object add position.
	void SnapAddObjPos(Vector3d &addVec);

	// Init and global settings.
	static void Init();
	static void Exit();

	// ReInit render library.
	static void RefreshAll(void);

	// Config stuff.
	static LConfig *cfg;
	static bool gridSnap, rotSnap;
	static UINT gridSnapVal, rotSnapVal;
	static bool invLMouse, invRMouse;
	static UINT walkSpeed, sensitivity;
	static WORD keyForward, keyBackward;
	static WORD keyLeft, keyRight, keyUp, keyDown;
	static UINT snapAlignment;
	static UINT editConstraint;

	static char draw2dWire[8];
	static char draw3dWire[8];
	static char draw3dSolid[8];
	static char draw3dTex[8];

protected:
	// Screen drawing stuff.
	void DrawOpCenter(void);
	void DrawDragBox(void);
	void DrawScaleBoundBox(void);
	void CalcScaleManipDots(void);
	void DrawOpPlane(void);
	void DrawCutSide(void);

	// Used to resize view clip regions.
	void OnZoomChanged(void);

	// Adjust the views.
	void OnScopeChanged(Object *pOldScope);

	// Conversion between device and logical coords.
	void DC2LC(const CPoint &pt, float &x, float &y) const;
	void DU2LU(const CPoint &pt, float &x, float &y) const;
	void LC2DC(float x, float y, CPoint &pt) const;
	void LC2DC(const Vector3d &vec3D, CPoint &pt) const;
	void LU2DU(float x, float y, CPoint &pt) const;

	// Context Menu.
	void OnRMBUContextMenu(UINT nFlags, CPoint point);

	// Handle the mouse interface in QViews 
	//  for the different op modes.
	void OnQViewScrollView(void);

	// Timer Render.  Need to be changed to On Idle Render.
	void SetTimerUpdateQViews(bool set);
	void OnTimerUpdateQViews(void);

	// Selection.
	void OnLMBDObjectsSelect(UINT nFlags, CPoint point);
	void OnMMObjectsSelect(UINT nFlags, CPoint point);
	void OnLMBUObjectsSelect(UINT nFlags, CPoint point);
	void OnUpdateDragSelect(void);

	// Snapping.
	void SetSnapScope(Object &snapScope);
	Vector3d &SnapVector2World(Vector3d &snapVec,
							   const Vector3d *pLockVector = NULL);

	// OpCenter.
	void FindOpCenterPos(CPoint &opCenterPos, Vector3d *pCenterVec = NULL);
	void DragOpCenterPos(CPoint point, bool updateView = true);

	// Move / Rotate / Scale
	void OnLMBDObjectsMoveRotateScale(UINT nFlags, CPoint point);

	void OnMMObjectsMove(UINT nFlags, CPoint point);
	void OnLMBUObjectsMove(UINT nFlags, CPoint point);

	void OnMMObjectsRotate(UINT nFlags, CPoint point);
	void OnLMBUObjectsRotate(UINT nFlags, CPoint point);

	void OnMMObjectsScale(UINT nFlags, CPoint point);
	void OnLMBUObjectsScale(UINT nFlags, CPoint point);

	// Brush Modify.
	void UpdateManipDispStat(void);
	void OnLMBDModifyBrush(UINT nFlags, CPoint point);
	void OnMMModifyBrush(UINT nFlags, CPoint point);
	void OnLMBUModifyBrush(UINT nFlags, CPoint point);

	// Plane Cut, Mirror / Flip
	void OnLMBDDefineOpPlane(UINT nFlags, CPoint point);
	void OnLMBUDefineOpPlane(UINT nFlags, CPoint point);
	void OnMMDefineOpPlane(UINT nFlags, CPoint point);
	void OnRMBDDefineOpPlane(UINT nFlags, CPoint point);

	Plane CalcOpPlane(void);

	// Eye movements.
	void ReCenterMousePos(CPoint &ptMousePos);
	void SetTimerUpdateLockedQViews(bool set, UINT nFlags);
	void OnTimerUpdateLockedQViews(void);

	void OnMBDEyeMoveRotateZoom(UINT nFlags, CPoint point);
	void OnMBUEyeMoveRotateZoom(UINT nFlags, CPoint point);

	void OnLMBEyeMove(UINT nFlags, CPoint point);
	void MoveEyeView(const Vector3d &moveVec);
	void OnEndViewMove(void);

	void OnRMBEyeZoom(UINT nFlags, CPoint point);
	void ZoomEyeView(float depthVal);
	void OnViewZoomIn(void);
	void OnViewZoomOut(void);

	void OnLMBEyeRotate(UINT nFlags, CPoint point);
	void OnRMBEyeRotate(UINT nFlags, CPoint point);
	
	void OnKeyCheckFlyThrough(bool forceUpdate = false);
	void OnMMFlyThrough(UINT nFlags, CPoint point);

	// Grid size display
	void OnIncrDisplayGridSize();
	void OnDecrDisplayGridSize();

	void UpdateCoordInfo(CPoint point);

protected:
	UINT qvType;
	UINT qvRender;
	View *pView;
	QDraw *pQDraw;

	float zoomVal;
	int width, height;
	int orgX, orgY;

	int bits;

	CRect dragBoxRect;

	bool useBack;
	Vector3d oldViewPos;
	SphrVector oldOrient;
	Object *oldViewObj;
	float oldZoomVal;

	LinkList<ObjectPtr> slctObjsBuf, oldSelection;
	Vector3d downClickVec, upClickVec;
	QViewsState *qvsBefore;

	CPoint	ptClick, lastMPos, scrollPos;
	bool	mouseLClick, mouseMClick, mouseRClick;
	bool	mouseLDrag, mouseMDrag, mouseRDrag;
	bool	reverseSelect;
	bool	selectPending, selectAddPending;
	bool	dragSelect, dragAddSelect;
	bool	operatePending, dragOperate, dragOpCenter;
	bool	dragScaleManipDot;
	UINT	scrollingView;

	static bool	drawScaleBoundBox;
	static bool updateSclBoundBox;
	Vector3d scaleManipDots[8];

	static Vector3d opCenterVec, oldOpCenterVec;
	static UINT modifyBrushMode;
	static const GPolygon *pManipFace;
	static const Edge3d *pManipEdge;
	static int manipVertex;
	static bool dispManipDots;

	bool	planeDefined;
	int		clipSide;   // Left = -1, Split = 0, Right = 1.
	int		dragPlanePt;
	Vector3d planeDragPt1, planeDragPt2;
	CBitmap scissors;

	Vector3d tempSnapVec, tempSnapVecTtl;
	Vector3d tempSnapBoundVec1, tempSnapBoundVec2;
	Vector3d tempScaleVec;
	Matrix44 tempSnapMatrix, tempSnapInvMatrix;
	Matrix44 tempInvViewMatrix;
	float tempSnapRotVal, tempLastSnapRotVal;

	int flyTime;

	static QView *coordView;

protected:
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(QView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual void OnDraw(CDC* pDC);
	//}}AFX_VIRTUAL

// Generated message map functions
protected:
	//{{AFX_MSG(QView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPaint();
	afx_msg void OnRcmqvPerformClip();
	afx_msg void OnRcmqvRemoveLeft();
	afx_msg void OnRcmqvRemoveRight();
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnRcmqvPerformFlip();
	afx_msg void OnRcmqvPerformMirror();
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnEditDuplicate();
	afx_msg void OnUpdateEditDuplicate(CCmdUI* pCmdUI);
	afx_msg void OnRcmqvSplitAtLine();
	//}}AFX_MSG
	afx_msg void OnEditAlignSet();
	afx_msg void OnEditAlignSelection();
	afx_msg void OnEditAlignUI(CCmdUI *pCmdUI);
	afx_msg void OnModifyBrushModes(UINT nID);
	afx_msg void OnModifyViewRender(UINT nID);
	afx_msg void OnModifyViewRenderUI(CCmdUI* pCmdUI);
	afx_msg void OnTimeRefresh();
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QVIEW_H__620139CE_5AF1_11D1_A7E0_384A12000000__INCLUDED_)
