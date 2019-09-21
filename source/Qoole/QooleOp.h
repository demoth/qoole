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

/*
 * QooleOp.h
 */

#ifndef _QOOLEOP_H_
#define _QOOLEOP_H_

#include "list.h"
#include "objects.h"

#include "QooleDoc.h"
#include "QView.h"

//==================== Operation Base Class ====================
// Base class for all operations.
// Copy constructor and assignment operator are disabled.
// The constructor should perform the actual operation.
// Undo() and Redo() can then be performed by the OpManager.

// forward declaration
class OpManager;
class QViewsState;

class Operation : public ElmNode {
	friend class OpManager;

public:
	Operation(bool saveAfterViews = true, QViewsState *qvBefore = NULL);
	virtual ~Operation(void);
	Operation(const Operation &src) { *this = src; }
	Operation &operator=(const Operation &src) { ASSERT(0); return *this; }

protected:
	virtual void Undo(void);
	virtual void Redo(void);
	static QooleDoc &GetDocument(void);

	bool commit;
	QViewsState *viewsBefore, *viewsAfter;

private:
	void UnDone(void) { ASSERT(commit);  commit = false; }
	void ReDone(void) { ASSERT(!commit);  commit = true; }

	UINT GetOpMode(void) const { return opMode; }
	void SetOpMode(UINT mode) { opMode = mode; }

	UINT opMode;
};

//==================== OpManager ====================
// Implements the Undo/Redo stack.

class OpManager {
public:
	OpManager(void);
	virtual ~OpManager(void);
	OpManager(const OpManager &src) { *this = src; }
	OpManager &operator=(const OpManager &src) { ASSERT(0); return *this; }
  
	int GetNumUndoOps(void) const { return undoList.NumOfElm(); }
	int GetNumRedoOps(void) const { return redoList.NumOfElm(); }
	int GetUndoBufSize(void) const { return bufSize; }

	void RegisterOp(Operation &op);
	void ResetUndoList(void);
	void SetUndoBufSize(int size = 10);

	void Undo(void) { UndoOps(1); }
	void Redo(void) { RedoOps(1); }

	void UndoOps(int numOps);
	void RedoOps(int numOps);

private:

	LinkList<Operation> undoList;
	LinkList<Operation> redoList;

	int bufSize;
};

//============================================================
//                     Custom Operations                     =
//============================================================

//========== Edit Menu: Cut / Copy / Paste ==========
//===== Edit: Cut =====

class OpEditCut : public Operation {
public:
	OpEditCut(void);
	virtual ~OpEditCut(void);

protected:
	void Undo(void);
	void Redo(void);

private:
	LinkList<ObjectPtr> oldClipBoard;
};

//===== Edit: Copy =====

class OpEditCopy : public Operation {
public:
	OpEditCopy(void);
	virtual ~OpEditCopy(void);

protected:
	void Undo(void);
	void Redo(void);

private:
	LinkList<ObjectPtr> oldClipBoard;
};

//===== Edit: Paste =====

class OpEditPaste : public Operation {
public:
	OpEditPaste(const Vector3d &pasteVec);
	virtual ~OpEditPaste(void);

protected:
	void Undo(void);
	void Redo(void);

private:
	LinkList<ObjectPtr> oldSelection;
	LinkList<ObjectPtr> pastedObjs;
};

//===== Edit: Duplicate =====

class OpEditDuplicate : public Operation {
public:
	OpEditDuplicate(const Vector3d &dupVec);
	virtual ~OpEditDuplicate(void);

protected:
	void Undo(void);
	void Redo(void);

private:
	LinkList<ObjectPtr> oldSelection;
	LinkList<ObjectPtr> pastedObjs;
};

//========== Edit Menu: OpObjModification ==========
// Used when the object's structure is modified.

class  OpObjModification : public Operation {
public:
	OpObjModification(Object &mObjPtr, bool saveAfterViews = false,
					  QViewsState *qvBefore = NULL);
	virtual ~OpObjModification(void);

	static OpObjModification *NewHollowOp(Object &brushObj, bool inward, int width);

protected:
	virtual void Undo(void) { Operation::Undo(); SwapObjs(); };
	virtual void Redo(void) { Operation::Redo(); SwapObjs(); };
	void SwapObjs(bool updateViews = true);

	Object *oldCopyPtr, *objPtr;
};

//========== Edit: CSG Subtraction ==========

class OpCSGSubtract : public Operation {
public:
	static OpCSGSubtract *NewSubtractOp(void);
	virtual ~OpCSGSubtract(void);

protected:
	OpCSGSubtract(void);

	void Undo(void);
	void Redo(void);

	LinkList<ObjectPtr> removedObjs, replaceObjs;
};

//========== Edit:: CSG Intersection ==========

class OpCSGIntersect : public Operation {
public:
	static OpCSGIntersect *NewIntersectOp(void);
	virtual ~OpCSGIntersect(void);

protected:
	OpCSGIntersect(void);
	void Undo(void);
	void Redo(void);

	LinkList<ObjectPtr> constructObjs, newIntrsctObj;
};

//========== Mode Menu: Object Selection ==========
// Remembers the selection sequence.
// Keep a copy of selection on the undo stack.

class OpSelectNewObjs : public Operation {
public:
	OpSelectNewObjs(LinkList<ObjectPtr> &newSelections);
	virtual ~OpSelectNewObjs(void);

protected:
	void Undo(void);
	void Redo(void);

private:
	LinkList<ObjectPtr> oldSlctObjs, newSlctObjs;
};

class OpSelectAddObjs : public Operation {
public:
	OpSelectAddObjs(LinkList<ObjectPtr> &addSelections);
	virtual ~OpSelectAddObjs(void);

protected:
	void Undo(void);
	void Redo(void);

private:
	LinkList<ObjectPtr> newSlctObjs;
};

//========== Mode Menu: Object Move ==========

class OpSelectionMove : public Operation {
public:
	OpSelectionMove(const TransSpace &viewSpace,
					const Vector3d &moveVec, QViewsState *qvBefore);
	virtual ~OpSelectionMove(void);

protected:
	void Undo(void);
	void Redo(void);

private:
	TransSpace opSpace;
	Vector3d mVec, opCenterVec;
};

//========== Mode Menu: Object Rotate ==========

class OpSelectionRotate : public Operation {
public:
	OpSelectionRotate(const TransSpace &viewSpace, const Vector3d &rotPt,
					  float rotAng, QViewsState *qvBefore);
	virtual ~OpSelectionRotate(void);

protected:
	void Undo(void);
	void Redo(void);

private:
	TransSpace opSpace;
	Vector3d rPt;
	float rAng;
	Vector3d opCenterVec;
};

//========== Mode Menu: Object Scale ==========

class OpSelectionScale : public Operation {
public:
	OpSelectionScale(const TransSpace &viewSpace, const Vector3d &basisVec,
					 const Vector3d &scaleVec, QViewsState *qvBefore);
	virtual ~OpSelectionScale(void);

protected:
	void Undo(void);
	void Redo(void);

private:
	TransSpace opSpace;
	Vector3d bVec, sVec;
	Vector3d opCenterVec;
};

//========== Mode Menu: Brush Manipulation (Face/Edge/Vertex Move) ==========

class OpBrushModification : public OpObjModification {
public:
	OpBrushModification(Object &manipObj, Geometry &oldBrush,
						QViewsState *qvBefore);
	~OpBrushModification(void);

protected:
	// virtual void Undo(void) { Operation::Undo(); SwapBrushes(); };
	// virtual void Redo(void) { Operation::Redo(); SwapBrushes(); };
	// void SwapBrushes(void);

	// Object *pManipObj;
	// Geometry *pOldBrush;
};

//========== Mode Menu: Plane Clip ==========

class OpPlaneClip : public Operation {
public:
	static OpPlaneClip *NewPlaneClipOp(const Plane &clipPlane);
	static OpPlaneClip *NewPlaneSplitOp(const Plane &clipPlane);
	virtual ~OpPlaneClip(void);

protected:
	OpPlaneClip(void);

	void Undo(void);
	void Redo(void);

	LinkList<ObjectPtr> removedObjs, replaceObjs;
	LinkList<ObjectPtr> oldSelection, newSelection;
};

//========== Mode Menu: Plane Flip ==========

class OpPlaneFlip : public Operation {
public:
	OpPlaneFlip(const Plane &flipPlane);
	virtual ~OpPlaneFlip(void);

protected:
	void Undo(void) { Flip(); }
	void Redo(void) { Flip(); }

	void Flip(void);

	Plane fPlane;
};

class OpPlaneMirror : public Operation {
public:
	OpPlaneMirror(const Plane &mirrorPlane);
	virtual ~OpPlaneMirror(void);

protected:
	void Undo(void);
	void Redo(void);

private:
	LinkList<ObjectPtr> oldSelection;
	LinkList<ObjectPtr> nObjs;
	Object *parentPtr;
};

//========== Object Menu: Object Add and Delete ==========

class OpObjsAddNew : public Operation {
public:
	OpObjsAddNew(LinkList<ObjectPtr> &newObjs,
				 const Vector3d &centerVec, Object *parent);
	virtual ~OpObjsAddNew(void);

protected:
	void Undo(void);
	void Redo(void);

private:
	LinkList<ObjectPtr> oldSelection;
	LinkList<ObjectPtr> nObjs;
	Object *parentPtr;
};

class OpObjsDel : public Operation {
public:
	OpObjsDel(void);
	virtual ~OpObjsDel(void);

protected:
	void Undo(void);
	void Redo(void);

private:
	LinkList<ObjectPtr> delObjs;
};

//========== Editing Scope ==========

class OpScopeChange : public Operation {
public:
	OpScopeChange(Object *newScope, LinkList<ObjectPtr> &selection);
	virtual ~OpScopeChange(void);

protected:
	void Undo(void);
	void Redo(void);

private:
	Object *pOldScope, *pNewScope;
	LinkList<ObjectPtr> oldSlctObjs, newSlctObjs;
};


//========== Grouping ==========

class OpGrouping : public Operation {
	friend class OpEntityApply;

public:
	// Call this constructor when grouping objects.
	OpGrouping(LinkList<ObjectPtr> &objsGroup, bool skipRedo = false);
	// Call this constructor when ungrouping an object (group).
	OpGrouping(Object *pObjUngruop);
	virtual ~OpGrouping(void);

	Object *GetParent(void) { return pParentObj; }

protected:
	void Undo(void);
	void Redo(void);

	void GroupObjs(bool updateViews = true);
	void UnGroupObjs(bool updateViews = true);

private:
	bool group, delParent;
	Object *pParentObj;
	LinkList<ObjectPtr> childrenObjs;
};

//========== TreeView: Object Name Change ==========

class OpObjNameChange : public Operation {
public:
	OpObjNameChange(Object &obj, const char *newName);
	virtual ~OpObjNameChange(void);

protected:
	void Undo(void) { SwapNames(); };
	void Redo(void) { SwapNames(); };
	void SwapNames(void);

private:
	Object *pObj;
	char *oldName;
};


class OpTreeViewDragMove : public Operation {
public:
	OpTreeViewDragMove(Object *dropTarget,
					   LinkList<ObjectPtr> &dragObjPtrs,
					   LinkList<ObjectPtr> &oldSlctObjs);
	virtual ~OpTreeViewDragMove(void);

protected:
	void Undo(void);
	void Redo(void);

private:
	LinkList<ObjectPtr> movedObjPtrs;
	LinkList<ObjectPtr> oldSelection;
	Object *pDropScope, *pOldEditScope, *pOldParent;
};

class OpTreeViewDragCopy : public Operation {
public:
	OpTreeViewDragCopy(Object *dropTarget,
					   const LinkList<ObjectPtr> &dragObjPtrs,
					   LinkList<ObjectPtr> &oldSlctObjs);
	virtual ~OpTreeViewDragCopy(void);

protected:
	void Undo(void);
	void Redo(void);

private:
	LinkList<ObjectPtr> newObjPtrs;
	LinkList<ObjectPtr> oldSelection;
	Object *pDropScope, *pOldEditScope;
};


/*
//========== Texture Selection ==========
class OpTextureSelect : public Operation {
public:
	OpTextureSelect();
	virtual OpTextureSelect()

protected:
	void Undo(void);
	void Redo(void0;

private:

};
*/

//========== Texture Apply ==========

struct FaceList {
	Object *obj;
	int index;
	char *name;
};

class OpTextureApply : public Operation {
public:
	OpTextureApply(char *name);
	virtual ~OpTextureApply();

protected:
	void Undo(void);
	void Redo(void);

private:
	char *texName;
	int faceCount;
	FaceList *faceList;

	static int _faceCount;
	static FaceList *_faceList;
	static bool CountFaceList(Object &obj);
	static bool BuildFaceList(Object &obj);
};

//========== Texture Apply Face ==========
class OpTextureApplyFace : public Operation {
public:
	OpTextureApplyFace(int faceIndex, char *name);
	virtual ~OpTextureApplyFace();

protected:
	void Undo(void);
	void Redo(void);

private:
	Object *pOpObj;
	int opFaceIndex;
	char *newName;
	char *oldName;
};

//========== Texture Properties Manipulation ==========
class OpTextureFaceManip : public Operation {
public:
	OpTextureFaceManip(int faceIndex, int oldXOffset, int oldYOffset,
					   float oldRotAng, float oldXScale, float oldYScale,
					   bool oldTextureLock);
	virtual ~OpTextureFaceManip();

protected:
	void Undo(void);
	void Redo(void);

private:
	Object *pOpObj;
	int opFaceIndex;
	int newXOff, newYOff, oldXOff, oldYOff;
	float newRot, oldRot;
	float newXScl, newYScl, oldXScl, oldYScl;
	bool newTexLock, oldTexLock;
};

class OpTextureBrushManip : public Operation {
public:
	OpTextureBrushManip(int oldXOffset[], int oldYOffset[], float oldRotAng[],
						float oldXScale[], float oldYScale[], bool oldTextureLock[]);
	virtual ~OpTextureBrushManip();

protected:
	void Undo(void);
	void Redo(void);

private:
	Object *pOpObj;
	int numFaces;
	int *pNewXOff, *pNewYOff, *pOldXOff, *pOldYOff;
	float *pNewRot, *pOldRot;
	float *pNewXScl, *pNewYScl, *pOldXScl, *pOldYScl;
	bool *pNewTexLock, *pOldTexLock;
};

//========== Surface Properties ==========
class OpModSurfaceAttrib : public Operation {
public:
	OpModSurfaceAttrib(int faceIndex, UINT newAttrib, UINT attribMask);
	OpModSurfaceAttrib(int faceIndex, UINT newValue);
	~OpModSurfaceAttrib();

protected:
	void Undo(void);
	void Redo(void);

private:
	Object *pOpObj;
	int opFaceIndex, numFaces;
	UINT mask;
	UINT nAttrib, nVal;
	UINT *oAttribs, *oVals;
};

//========== Brush Content Properties ==========
class OpModContentAttrib : public Operation {
public:
	OpModContentAttrib(UINT newValue);
	~OpModContentAttrib();

protected:
	void Undo(void);
	void Redo(void);

private:
	Object *pOpObj;
	UINT newVal;
	UINT oldVal;
};

//========== Entity Properties ==========
class OpEntitySetKey : public Operation {
public:
	OpEntitySetKey(Object *pEntObj, const char *key, const char *arg);
	~OpEntitySetKey();

protected:
	void Undo(void);
	void Redo(void);

private:
	Entity *pEntity;
	char *setKey;
	char *oldArg, *newArg;
	bool refresh;
};

//========== Entity Apply ==========
class OpEntityApply : public Operation {
public:
	OpEntityApply(const char *name);
	~OpEntityApply();

protected:
	void Undo(void);
	void Redo(void);

private:
	Object *pObj;
	Entity *pOldEnt, *pNewEnt;
	OpGrouping *opGroup;
	bool emptyName;
};

#endif //_QOOLEOP_H_
