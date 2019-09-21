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
 * undo.cpp
 */

#include "stdafx.h"

#include "Resource.h"
#include "QooleOp.h"
#include "QMainfrm.h"

//==================== Operation ====================

Operation::Operation(bool saveAfterViews, QViewsState *qvBefore) {
	commit = true;

	if (saveAfterViews)
		viewsAfter = new QViewsState();
	else
		viewsAfter = NULL;

	viewsBefore = qvBefore;
}

Operation::~Operation(void) {
	if (viewsBefore)
		delete viewsBefore;
	if (viewsAfter)
		delete viewsAfter;
}

QooleDoc &Operation::GetDocument(void) {
	QooleDoc *pDoc = pQMainFrame->GetDeskTopDocument();
	return *pDoc;
}

void Operation::Undo(void) {
	if (viewsBefore)
		viewsBefore->RestoreQViewsState();
	else if (viewsAfter)
		viewsAfter->RestoreQViewsState();
}

void Operation::Redo(void) {
	if (viewsAfter)
		viewsAfter->RestoreQViewsState();
}

//==================== OpManager ====================

OpManager::OpManager(void) {
	bufSize = 0;
	SetUndoBufSize();
}

OpManager::~OpManager(void) {
	ResetUndoList();
}

void OpManager::RegisterOp(Operation &op) {

	// Clear redo ops.
	while (GetNumRedoOps() > 0) {
		delete &(redoList.RemoveNode(0));
	}

	// Remember the desktop's op mode.
	op.SetOpMode(pQMainFrame->GetOpMode());

	// Add the operation to the undo buffer.
	undoList.InsertNode(op, 0);

	// Check for buffer overflow.
	ASSERT(bufSize > 0);
	for(int i = GetNumUndoOps(); i > bufSize; i--) {
		delete &(undoList.RemoveNode(i - 1));
	}
}

void OpManager::ResetUndoList(void) {
	// Clear redo ops.
	while (GetNumRedoOps() > 0) {
		delete &(redoList.RemoveNode(0));
	}

	// Clear undo ops.
	while (GetNumUndoOps() > 0) {
		delete &(undoList.RemoveNode(0));
	}
}

void OpManager::SetUndoBufSize(int size) {
	// Sanity.
	ASSERT(size > 0);

	bufSize = size;

	// Check for extra ops in buffer.
	for(int i = GetNumUndoOps(); i > bufSize; i--) {
		delete &(undoList.RemoveNode(i - 1));
	}
}

void OpManager::UndoOps(int numOps) {
	// Sanity.
	ASSERT(numOps > 0 && numOps <= GetNumUndoOps());

	Operation *opPtr;

	for(; numOps > 0; numOps--) {
		opPtr = &(undoList.RemoveNode(0));
		opPtr->Undo();
		opPtr->UnDone();
		redoList.InsertNode(*opPtr, 0);
	}

	// Reset the desktop's op mode.
	pQMainFrame->SwitchOpMode(opPtr->GetOpMode());

	// Check for buffer overflow.
	int i = GetNumRedoOps();
	while (i > 0 && i + GetNumUndoOps() > bufSize) {
		delete &(redoList.RemoveNode(i - 1));
		i--;
	}
}

void OpManager::RedoOps(int numOps) {
	// Sanity.
	ASSERT(numOps > 0 && numOps <= GetNumRedoOps());

	Operation *opPtr;

	for(; numOps > 0; numOps--) {
		opPtr = &(redoList.RemoveNode(0));
		opPtr->Redo();
		opPtr->ReDone();
		undoList.InsertNode(*opPtr, 0);
	}

	// Reset the desktop's op mode.
	pQMainFrame->SwitchOpMode(opPtr->GetOpMode());

	// Check for buffer overflow.
	int i = GetNumUndoOps();
	while (i > 0 && i + GetNumRedoOps() > bufSize) {
		delete &(undoList.RemoveNode(i - 1));
		i--;
	}
}

//========== Edit Menu: Cut ==========

OpEditCut::OpEditCut(void) {
	// Move the selections to the clipboard
	Redo();
}

OpEditCut::~OpEditCut(void) {
	// Clean up the backup clipboard.
	if (commit) {
		ObjectPtr *pObjPtr;
		while (oldClipBoard.NumOfElm() > 0) {
			pObjPtr = &(oldClipBoard.RemoveNode(0));
			delete pObjPtr->GetPtr();
			delete pObjPtr;
		}
	}
	else {
		ASSERT(oldClipBoard.NumOfElm() == 0);
	}
}

void OpEditCut::Undo(void) {
	Operation::Undo();

	ASSERT((pQMainFrame->clipBoard).NumOfElm() > 0);

	// Move the objs from the clipboard back into the document
	//  and reselect them.
	QooleDoc *pDoc = &(GetDocument());
	Selector *pSlctr = &(pQMainFrame->GetSelector());

	pDoc->ObjectsAdd(pQMainFrame->clipBoard, pQMainFrame->GetScope(), false);
	pSlctr->MSelectObjects(pQMainFrame->clipBoard);

	// Set the operate center for QViews.
	Vector3d centerVec;
	pSlctr->GetMSelectedObjectsCenter(centerVec);
	QView::SetOperateCenterPos(centerVec);

	// Restore the clipboard.
	(pQMainFrame->clipBoard).DeleteAllNodes();

	while (oldClipBoard.NumOfElm() > 0) {
		(pQMainFrame->clipBoard).AppendNode(oldClipBoard.RemoveNode(0));
	}

	// Update views.
	pDoc->UpdateAllViews(NULL, DUAV_OBJSSEL | DUAV_OBJSMODSTRUCT,
		(CObject *) pSlctr->GetScopePtr());
}

void OpEditCut::Redo(void) {
	ASSERT(oldClipBoard.NumOfElm() == 0);

	Operation::Redo();

	// Backup the clipboard.
	while ((pQMainFrame->clipBoard).NumOfElm() > 0) {
		oldClipBoard.AppendNode((pQMainFrame->clipBoard).RemoveNode(0));
	}

	// Move the current selections from the document to
	//  the clipboard.  Unselect everything.
	QooleDoc *pDoc = &(GetDocument());
	Selector *pSlctr = &(pQMainFrame->GetSelector());

	pQMainFrame->clipBoard = (pQMainFrame->GetSelector()).GetMSelectedObjects();
	pDoc->ObjectsDelete(pQMainFrame->clipBoard, false, false);
	pSlctr->MUnselect();

	// Update views.
	pDoc->UpdateAllViews(NULL, DUAV_OBJSSEL| DUAV_OBJSMODSTRUCT,
		(CObject *) pSlctr->GetScopePtr());
}

//========== Edit Menu: Copy ==========
OpEditCopy::OpEditCopy(void) {
	// Copy the selected objs to the clipboard.
	Redo();
}

OpEditCopy::~OpEditCopy(void) {
	// Clean up the backup clipboard.
	if (commit) {
		ObjectPtr *pObjPtr;
		while (oldClipBoard.NumOfElm() > 0) {
			pObjPtr = &(oldClipBoard.RemoveNode(0));
			delete pObjPtr->GetPtr();
			delete pObjPtr;
		}
	}
	else {
		ASSERT(oldClipBoard.NumOfElm() == 0);
	}
}

void OpEditCopy::Undo(void) {
	Operation::Undo();

	ASSERT((pQMainFrame->clipBoard).NumOfElm() > 0);

	// Clear the clipboard of the newly created objects
	//  from the copy command.
	ObjectPtr *pObjPtr;
	while ((pQMainFrame->clipBoard).NumOfElm() > 0) {
		pObjPtr = &((pQMainFrame->clipBoard).RemoveNode(0));
		delete pObjPtr->GetPtr();
		delete pObjPtr;
	}

	// Restore the clipboard.
	while (oldClipBoard.NumOfElm() > 0) {
		(pQMainFrame->clipBoard).AppendNode(oldClipBoard.RemoveNode(0));
	}
}

void OpEditCopy::Redo(void) {
	ASSERT(oldClipBoard.NumOfElm() == 0);

	Operation::Redo();

	// Backup the clipboard.
	while ((pQMainFrame->clipBoard).NumOfElm() > 0) {
		oldClipBoard.AppendNode((pQMainFrame->clipBoard).RemoveNode(0));
	}

	// Copy the selected objs to the clipboard.
	Selector *pSlctr = &(pQMainFrame->GetSelector());
	Object *pNewObj;
	IterLinkList<ObjectPtr> iter(pSlctr->GetMSelectedObjects());
	iter.Reset();
	while (!iter.IsDone()) {
		pNewObj = new Object(*(iter.GetNext())->GetPtr());
		(pQMainFrame->clipBoard).AppendNode(*(new ObjectPtr(pNewObj)));
	}
}

//========== Edit Menu: Paste ==========

OpEditPaste::OpEditPaste(const Vector3d &pasteVec) {
	Selector *pSlctr = &(pQMainFrame->GetSelector());

	// Backup selection.
	ASSERT(oldSelection.NumOfElm() == 0);
	oldSelection = pSlctr->GetMSelectedObjects();

	ASSERT((pQMainFrame->clipBoard).NumOfElm() > 0);
	IterLinkList<ObjectPtr> iter(pQMainFrame->clipBoard);

	// Find the center pos of selected objs.
	Vector3d posVec;
	Object *pObj;
	iter.Reset();
	while (!iter.IsDone()) {
		pObj = iter.GetNext()->GetPtr();
		posVec.AddVector(pObj->GetPosition());
	}
	posVec.MultVector(1.0f / (pQMainFrame->clipBoard).NumOfElm());

	Vector3d diffVec;
	diffVec.SubVector(posVec, pasteVec);

	// Create the new objects that will be pasted into the document.
	Object *pNewObj;
	iter.Reset();
	while (!iter.IsDone()) {
		pNewObj = new Object(*(iter.GetNext()->GetPtr()));
		posVec = pNewObj->GetPosition();
		pNewObj->SetPosition(posVec.SubVector(diffVec));
		pastedObjs.AppendNode(*(new ObjectPtr(pNewObj)));
	}

	// Paste the new Objects.
	Redo();
}

OpEditPaste::~OpEditPaste(void) {
	oldSelection.DeleteAllNodes();

	ObjectPtr *pObjPtr;
	while (pastedObjs.NumOfElm() > 0) {
		pObjPtr = &(pastedObjs.RemoveNode(0));
		if (!commit) {
			delete pObjPtr->GetPtr();
		}
		delete pObjPtr;
	}
}

void OpEditPaste::Undo(void) {
	Operation::Undo();

	Selector *pSlctr = &(pQMainFrame->GetSelector());
	QooleDoc *pDoc = &(GetDocument());

	// Unselect everything.
	pSlctr->MUnselect();

	// Remove the pasted objects from the document.
	pDoc->ObjectsDelete(pastedObjs, false, false);

	// Reselect the old selection.
	pSlctr->MSelectObjects(oldSelection);

	// Update Views.
	pDoc->UpdateAllViews(NULL, DUAV_OBJSSEL| DUAV_OBJSMODSTRUCT,
		(CObject *) pSlctr->GetScopePtr());
}

void OpEditPaste::Redo(void) {
	Operation::Redo();

	Selector *pSlctr = &(pQMainFrame->GetSelector());
	QooleDoc *pDoc = &(GetDocument());

	// Reinsert the pasted objects back into the document.  
	pDoc->ObjectsAdd(pastedObjs, pQMainFrame->GetScope(), false);

	// Then reselect them.
	pSlctr->MSelectObjects(pastedObjs);

	// Set the operate center for QViews.
	Vector3d centerVec;
	pSlctr->GetMSelectedObjectsCenter(centerVec);
	QView::SetOperateCenterPos(centerVec);

	// Update Views.
	pDoc->UpdateAllViews(NULL, DUAV_OBJSSEL | DUAV_OBJSMODSTRUCT,
		(CObject *) pSlctr->GetScopePtr());
}

//========== Edit Menu: Duplicate Objects ==========
OpEditDuplicate::OpEditDuplicate(const Vector3d &dupVec) {
	Selector *pSlctr = &(pQMainFrame->GetSelector());

	// Backup selection.
	ASSERT(oldSelection.NumOfElm() == 0);
	oldSelection = pSlctr->GetMSelectedObjects();

	// Create the new objects that will be pasted into the document.
	Object *pNewObj;
	Vector3d posVec;

	IterLinkList<ObjectPtr> iter(pSlctr->GetMSelectedObjects());
	iter.Reset();
	while (!iter.IsDone()) {
		pNewObj = new Object(*(iter.GetNext()->GetPtr()));
		posVec = pNewObj->GetPosition();
		pNewObj->SetPosition(posVec.AddVector(dupVec));
		pastedObjs.AppendNode(*(new ObjectPtr(pNewObj)));
	}

	// Paste the new Objects.
	Redo();
}

OpEditDuplicate::~OpEditDuplicate(void) {
	oldSelection.DeleteAllNodes();

	ObjectPtr *pObjPtr;
	while (pastedObjs.NumOfElm() > 0) {
		pObjPtr = &(pastedObjs.RemoveNode(0));
		if (!commit) {
			delete pObjPtr->GetPtr();
		}
		delete pObjPtr;
	}
}

void OpEditDuplicate::Undo(void) {
	Operation::Undo();

	Selector *pSlctr = &(pQMainFrame->GetSelector());
	QooleDoc *pDoc = &(GetDocument());

	// Unselect everything.
	pSlctr->MUnselect();

	// Remove the pasted objects from the document.
	pDoc->ObjectsDelete(pastedObjs, false, false);

	// Reselect the old selection.
	pSlctr->MSelectObjects(oldSelection);

	// Update Views.
	pDoc->UpdateAllViews(NULL, DUAV_OBJSSEL| DUAV_OBJSMODSTRUCT,
		(CObject *) pSlctr->GetScopePtr());
}

void OpEditDuplicate::Redo(void) {
	Operation::Redo();

	Selector *pSlctr = &(pQMainFrame->GetSelector());
	QooleDoc *pDoc = &(GetDocument());

	// Reinsert the pasted objects back into the document.  
	pDoc->ObjectsAdd(pastedObjs, pQMainFrame->GetScope(), false);

	// Then reselect them.
	pSlctr->MSelectObjects(pastedObjs);

	// Set the operate center for QViews.
	Vector3d centerVec;
	pSlctr->GetMSelectedObjectsCenter(centerVec);
	QView::SetOperateCenterPos(centerVec);

	// Update Views.
	pDoc->UpdateAllViews(NULL, DUAV_OBJSSEL | DUAV_OBJSMODSTRUCT,
		(CObject *) pSlctr->GetScopePtr());
}

//========== Edit Menu: OpObjModification ==========

OpObjModification::OpObjModification(Object &mObjPtr, bool saveAfterViews,
									 QViewsState *qvBefore)
				  :Operation(saveAfterViews, qvBefore) {

	ASSERT(mObjPtr.GetParentPtr() != NULL);

	QooleDoc *pDoc = &(GetDocument());
	ASSERT(pDoc != NULL);

	objPtr = new Object(mObjPtr);
	pDoc->ObjectReplace(mObjPtr, *objPtr, false);
	oldCopyPtr = &mObjPtr;
}

OpObjModification::~OpObjModification(void) {
	delete oldCopyPtr;
}

void OpObjModification::SwapObjs(bool updateViews) {
	QooleDoc *pDoc = &(GetDocument());

	pDoc->ObjectReplace(*objPtr, *oldCopyPtr, false);

	Object *holdPtr = objPtr;
	objPtr = oldCopyPtr;
	oldCopyPtr = holdPtr;

	// Change Selection
	Selector *pSlctr = &(pQMainFrame->GetSelector());
	pSlctr->MUnselect();
	pSlctr->MSelectObject(objPtr);

	// Set the operate center for QViews.
	Vector3d centerVec;
	pSlctr->GetMSelectedObjectsCenter(centerVec);
	QView::SetOperateCenterPos(centerVec);

	// Update Views.
	if (updateViews) {
		pDoc->UpdateAllViews(NULL, DUAV_OBJSSEL | DUAV_OBJSMODSTRUCT,
							 (CObject *) pSlctr->GetScopePtr());
	}
}

// Hollow...
OpObjModification *OpObjModification::NewHollowOp(Object &brushObj, bool inward, int width) {
	Selector *pSlctr = &(pQMainFrame->GetSelector());
	QooleDoc *pDoc = &(GetDocument());

	OpObjModification *op = new OpObjModification(brushObj);

	if (pDoc->HollowBrush(*(op->objPtr), inward, (float) width, false)) { // Operation happend.
		// Select the new replacement obj
		pSlctr->MUnselect();
		pSlctr->MSelectObject(op->objPtr);
		// Update all views.
		pDoc->UpdateAllViews(NULL, DUAV_OBJSSEL | DUAV_OBJSMODSTRUCT,
							 (CObject *) pSlctr->GetScopePtr());
	}
	else {  // Operation didn't happen.
		op->SwapObjs(false);
		delete op;
		op = NULL;
	}

	return op;
}

//===== Edit: Subtract =====
OpCSGSubtract *OpCSGSubtract::NewSubtractOp(void) {
	OpCSGSubtract *op = new OpCSGSubtract;

	Selector *pSlctr = &(pQMainFrame->GetSelector());
	ASSERT(pSlctr->GetNumMSelectedObjects() > 0);
	QooleDoc *pDoc = &(GetDocument());

	Object *pScope = &(pQMainFrame->GetScope());
	Object *pObjNode, *pCutter;

	IterLinkList<Object> *iterChildren = &(pScope->GetIterChildren());
	// Make a copy of all children nodes in current scope.
	LinkList<ObjectPtr> oldChildrenList;
	iterChildren->Reset();
	while (!iterChildren->IsDone()) {
		pObjNode = iterChildren->GetNext();
		oldChildrenList.AppendNode(*(new ObjectPtr(pObjNode)));
	}

	// Now iter through and make the subtractions.
	IterLinkList<ObjectPtr> iterCutters(pSlctr->GetMSelectedObjects());
	iterChildren->Reset();
	while (!iterChildren->IsDone()) {
		pObjNode = iterChildren->GetNext();

		if (pObjNode->IsItemNode()) // Skip items.
			continue;

		// Check to see if we can skip this obj.
		bool skip = false;
		bool mayIntersect = false;
		iterCutters.Reset();
		while (!iterCutters.IsDone()) {
			pCutter = (iterCutters.GetNext())->GetPtr();

			if (pObjNode == pCutter) { // One of the cutters.
				skip = true;
				break;
			}

			// See if the cutter and the child node may not even intersect.
			Vector3d diffVec;
			diffVec.SubVector(pObjNode->GetPosition(), pCutter->GetPosition());
			if (diffVec.GetMag() <=
				pObjNode->GetBoundRadius() + pCutter->GetBoundRadius()) {
				mayIntersect = true;
			}
		}

		if (skip || !mayIntersect)
			continue;

		// Make a backup copy of the current node.
		Object *pNewObjNode = new Object(*pObjNode);
		pDoc->ObjectReplace(*pObjNode, *pNewObjNode, false, false);

		// Perform the subtraction on the new obj;
		if (!pNewObjNode->CSGSubtract(pSlctr->GetMSelectedObjects())) {
			// No changes...  undo the replacement and delete new obj.
			pDoc->ObjectReplace(*pNewObjNode, *pObjNode, false, false);
			delete pNewObjNode;
			continue;
		}

		// Just removed pObjNode cuz it changed.  Need to save it.
		op->removedObjs.AppendNode(*(new ObjectPtr(pObjNode)));

		if (pNewObjNode->IsLeaf() && !pNewObjNode->HasBrush()) {
			// Nothing left.  delete brush.
			pScope->RemoveChild(*pNewObjNode, false);
			delete pNewObjNode;
			continue;
		}
	}

	// Now iter through children list and filter for
	//  newly introduced replacement obj nodes.
	IterLinkList<ObjectPtr> iterOldChildren(oldChildrenList);
	ObjectPtr *pOldChildPtr;
	iterChildren->Reset();
	while (!iterChildren->IsDone()) {
		pObjNode = iterChildren->GetNext();
		bool found = false;
		iterOldChildren.Reset();
		while (!iterOldChildren.IsDone()) {
			pOldChildPtr = iterOldChildren.GetNext();
			if (pOldChildPtr->GetPtr() == pObjNode) {
				oldChildrenList.RemoveNode(*pOldChildPtr);
				found = true;
				break;
			}
		}
		if (!found)
			op->replaceObjs.AppendNode(*(new ObjectPtr(pObjNode)));
	}
	oldChildrenList.DeleteAllNodes();

	pScope->SetBoundRadius();

	if (op->removedObjs.NumOfElm() == 0) {
		// Sanity.
		ASSERT(op->replaceObjs.NumOfElm() == 0);
		// nothing was changed..
		delete op;
		op = NULL;
	}
	else {
		// Update all views.
		pDoc->UpdateAllViews(NULL, DUAV_OBJSMODSTRUCT, (CObject *) pScope);
	}

	return op;
}

OpCSGSubtract::OpCSGSubtract(void) {
}

OpCSGSubtract::~OpCSGSubtract(void) {
	ObjectPtr *pObjPtr;

	while (removedObjs.NumOfElm() > 0) {
		pObjPtr = &(removedObjs.RemoveNode(0));
		if (commit)
			delete pObjPtr->GetPtr();
		delete pObjPtr;
	}

	while (replaceObjs.NumOfElm() > 0) {
		pObjPtr = &(replaceObjs.RemoveNode(0));
		if (!commit)
			delete pObjPtr->GetPtr();
		delete pObjPtr;
	}
}

void OpCSGSubtract::Undo(void) {
	Selector *pSlctr = &(pQMainFrame->GetSelector());
	QooleDoc *pDoc = &(GetDocument());
	Object *pScope = &(pSlctr->GetScope());

	if (replaceObjs.NumOfElm() > 0)
		pDoc->ObjectsDelete(replaceObjs, false, false, false);
	if (removedObjs.NumOfElm() > 0)
		pDoc->ObjectsAdd(removedObjs, *pScope, false);

	// Update all views.
	pDoc->UpdateAllViews(NULL, DUAV_OBJSMODSTRUCT, (CObject *) pScope);
}

void OpCSGSubtract::Redo(void) {
	Selector *pSlctr = &(pQMainFrame->GetSelector());
	QooleDoc *pDoc = &(GetDocument());
	Object *pScope = &(pSlctr->GetScope());

	if (removedObjs.NumOfElm() > 0)
		pDoc->ObjectsDelete(removedObjs, false, false, false);
	if (replaceObjs.NumOfElm() > 0)
		pDoc->ObjectsAdd(replaceObjs, *pScope, false);

	// Update all views.
	pDoc->UpdateAllViews(NULL, DUAV_OBJSMODSTRUCT, (CObject *) pScope);
}

//===== Edit: Intersect =====

OpCSGIntersect *OpCSGIntersect::NewIntersectOp(void) {
	OpCSGIntersect *op = NULL;

	Selector *pSlctr = &(pQMainFrame->GetSelector());
	Object *pScope = &(pSlctr->GetScope());
	Object *pNewObj = pScope->CSGIntersect(pSlctr->GetMSelectedObjects());

	if (pNewObj) {
		op = new OpCSGIntersect;
		op->newIntrsctObj.AppendNode(*(new ObjectPtr(pNewObj)));
		op->constructObjs = pSlctr->GetMSelectedObjects();
		op->Redo();
	}

	return op;
}

OpCSGIntersect::OpCSGIntersect(void) {
}

OpCSGIntersect::~OpCSGIntersect(void) {
	ObjectPtr *pObjPtr;

	while (constructObjs.NumOfElm() > 0) {
		pObjPtr = &(constructObjs.RemoveNode(0));
		if (commit)
			delete pObjPtr->GetPtr();
		delete pObjPtr;
	}

	while (newIntrsctObj.NumOfElm() > 0) {
		pObjPtr = &(newIntrsctObj.RemoveNode(0));
		if (!commit)
			delete pObjPtr->GetPtr();
		delete pObjPtr;
	}
}

void OpCSGIntersect::Undo(void) {
	Selector *pSlctr = &(pQMainFrame->GetSelector());
	QooleDoc *pDoc = &(GetDocument());
	Object *pScope = &(pSlctr->GetScope());

	pSlctr->MUnselect();
	pDoc->ObjectsDelete(newIntrsctObj, false, false, false);
	pDoc->ObjectsAdd(constructObjs, *pScope, false);
	pSlctr->MSelectObjects(constructObjs);

	// Set the operate center for QViews.
	Vector3d centerVec;
	pSlctr->GetMSelectedObjectsCenter(centerVec);
	QView::SetOperateCenterPos(centerVec);

	// Update Views.
	pDoc->UpdateAllViews(NULL, DUAV_OBJSSEL | DUAV_OBJSMODSTRUCT,
						 (CObject *) pScope);
}

void OpCSGIntersect::Redo(void) {
	Selector *pSlctr = &(pQMainFrame->GetSelector());
	QooleDoc *pDoc = &(GetDocument());
	Object *pScope = &(pSlctr->GetScope());

	pSlctr->MUnselect();
	pDoc->ObjectsDelete(constructObjs, false, false, false);
	pDoc->ObjectsAdd(newIntrsctObj, *pScope, false);
	pSlctr->MSelectObjects(newIntrsctObj);

	// Set the operate center for QViews.
	Vector3d centerVec;
	pSlctr->GetMSelectedObjectsCenter(centerVec);
	QView::SetOperateCenterPos(centerVec);

	// Update Views.
	pDoc->UpdateAllViews(NULL, DUAV_OBJSSEL | DUAV_OBJSMODSTRUCT,
						 (CObject *) pScope);
}

//========== Mode Menu: Selection ==========
//===== Select New Objects =====

OpSelectNewObjs::OpSelectNewObjs(LinkList<ObjectPtr> &newSelections) {
	Selector *slctPtr = &(pQMainFrame->GetSelector());

	// Remember the old selection
	oldSlctObjs = slctPtr->GetMSelectedObjects();

	// Remember the new selection.
	while (newSelections.NumOfElm() > 0) {
		newSlctObjs.AppendNode(newSelections.RemoveNode(0));
	}

	// Select the objs in the new selections.
	Redo();
}

OpSelectNewObjs::~OpSelectNewObjs(void) {
	// Clean up.
	newSlctObjs.DeleteAllNodes();
	oldSlctObjs.DeleteAllNodes();
}

void OpSelectNewObjs::Undo(void) {
	Operation::Undo();

	Selector *slctPtr = &(pQMainFrame->GetSelector());
	Object *slctObj;
	bool in;

	// Unselect everything first.
	slctPtr->MUnselect();

	// ReSelect the old selections.
	IterLinkList<ObjectPtr> iter(oldSlctObjs);
	iter.Reset();
	while (!iter.IsDone()) {
		slctObj = (iter.GetNext())->GetPtr();
 		in = slctPtr->MSelectObject(slctObj);
		ASSERT(in);
	}

	// Set the operate center for QViews.
	Vector3d centerVec;
	slctPtr->GetMSelectedObjectsCenter(centerVec);
	QView::SetOperateCenterPos(centerVec);

	// Update Views.  Should this be done in Document class?
	GetDocument().UpdateAllViews(NULL, DUAV_OBJSSEL,
		(CObject *) slctPtr->GetScopePtr());
}

void OpSelectNewObjs::Redo(void) {
	Operation::Redo();

	Selector *slctPtr = &(pQMainFrame->GetSelector());
	Object *slctObj;
	bool in;

	// Unselect everything.
	slctPtr->MUnselect();

	// ReSelect the new selections.
	IterLinkList<ObjectPtr> iter(newSlctObjs);
	iter.Reset();
	while (!iter.IsDone()) {
		slctObj = (iter.GetNext())->GetPtr();
		in = slctPtr->MSelectObject(slctObj);
		ASSERT(in);
	}

	// Set the operate center for QViews.
	Vector3d centerVec;
	slctPtr->GetMSelectedObjectsCenter(centerVec);
	QView::SetOperateCenterPos(centerVec);

	// Update Views.  Should this be done in Document class?
	GetDocument().UpdateAllViews(NULL, DUAV_OBJSSEL,
		(CObject *) slctPtr->GetScopePtr());
}

//===== Select Additional Objects =====

OpSelectAddObjs::OpSelectAddObjs(LinkList<ObjectPtr> &addSelections) {
	ASSERT(addSelections.NumOfElm() > 0);

	Selector *slctPtr = &(pQMainFrame->GetSelector());
	ObjectPtr *pObjPtr;

	// Remember the new selections.
	while (addSelections.NumOfElm() > 0) {
		pObjPtr = &(addSelections.RemoveNode(0));
		// Removes those objs that are already selected.
		// if (slctPtr->IsObjectMSelected(pObjPtr->GetPtr()))
		// 	delete pObjPtr;
		// else
		newSlctObjs.AppendNode(*pObjPtr);
	}

	// Select the new selections.
	Redo();
}

OpSelectAddObjs::~OpSelectAddObjs(void) {
	// Clean up.
	newSlctObjs.DeleteAllNodes();
}

void OpSelectAddObjs::Undo(void) {
	Operation::Undo();

	Selector *slctPtr = &(pQMainFrame->GetSelector());
	Object *slctObj;
	bool in;
	
	// Unselect those added.
	IterLinkList<ObjectPtr> iter(newSlctObjs);
	iter.Reset();
	while (!iter.IsDone()) {
		slctObj = (iter.GetNext())->GetPtr();
		in = slctPtr->MSelectObject(slctObj);
		// ASSERT(!in);
	}

	// Set the operate center for QViews.
	Vector3d centerVec;
	slctPtr->GetMSelectedObjectsCenter(centerVec);
	QView::SetOperateCenterPos(centerVec);

	// Update Views.  Should this be done in Document class?
	GetDocument().UpdateAllViews(NULL, DUAV_OBJSSEL,
		(CObject *) slctPtr->GetScopePtr());
}

void OpSelectAddObjs::Redo(void) {
	Operation::Redo();

	Selector *slctPtr = &(pQMainFrame->GetSelector());
	Object *slctObj;
	bool in;
	
	// Select those added.
	IterLinkList<ObjectPtr> iter(newSlctObjs);
	iter.Reset();
	while (!iter.IsDone()) {
		slctObj = (iter.GetNext())->GetPtr();
		in = slctPtr->MSelectObject(slctObj);
		// ASSERT(in);
	}

	// Set the operate center for QViews.
	Vector3d centerVec;
	slctPtr->GetMSelectedObjectsCenter(centerVec);
	QView::SetOperateCenterPos(centerVec);

	// Update Views.  Should this be done in Document class?
	GetDocument().UpdateAllViews(NULL, DUAV_OBJSSEL,
		(CObject *) slctPtr->GetScopePtr());
}

//========== Mode Menu: Move Object ==========

OpSelectionMove::OpSelectionMove(const TransSpace &viewSpace,
								 const Vector3d &moveVec,
								 QViewsState *qvBefore)
				:Operation(true, qvBefore) {
	// Remember parameters.
	opSpace = viewSpace;
	mVec = moveVec;
	opCenterVec = QView::GetOperateCenterPos();
}

OpSelectionMove::~OpSelectionMove(void) {
}

void OpSelectionMove::Undo(void) {
	Operation::Undo();

	QooleDoc *pDoc = &(GetDocument());
	Selector *pSlctr = &(pQMainFrame->GetSelector());
	ASSERT(pSlctr->GetNumMSelectedObjects() > 0);

	Vector3d tVec(mVec);
	pDoc->ObjectsMove(pSlctr->GetMSelectedObjects(),
		opSpace, tVec.MultVector(-1.0f), false);

	// Set the operate center for QViews.
	Matrix44 m;
	opSpace.CalTransSpaceMatrix(m.SetIdentity());

	Vector3d oVec;
	m.Transform(oVec);
	(m.Transform(tVec)).SubVector(oVec);
	tVec.AddVector(opCenterVec);
	QView::SetOperateCenterPos(tVec);

	// Update Views.
	pDoc->UpdateAllViews(NULL, DUAV_OBJSSEL, (CObject *) pSlctr->GetScopePtr());
}

void OpSelectionMove::Redo(void) {
	Operation::Redo();

	QooleDoc *pDoc = &(GetDocument());
	Selector *pSlctr = &(pQMainFrame->GetSelector());
	ASSERT(pSlctr->GetNumMSelectedObjects() > 0);

	pDoc->ObjectsMove(pSlctr->GetMSelectedObjects(), opSpace, mVec, false);

	// Set the operate center for QViews.
	QView::SetOperateCenterPos(opCenterVec);

	// Update Views.
	pDoc->UpdateAllViews(NULL, DUAV_OBJSSEL, (CObject *) pSlctr->GetScopePtr());
}

//========== Mode Menu: Rotate Object ==========

OpSelectionRotate::OpSelectionRotate(const TransSpace &viewSpace,
									 const Vector3d &rotPt, float rotAng,
									 QViewsState *qvBefore)
				  :Operation(true, qvBefore) {
	opSpace = viewSpace;
	rPt = rotPt;
	rAng = rotAng;
	opCenterVec = QView::GetOperateCenterPos();
}

OpSelectionRotate::~OpSelectionRotate(void) {
}

void OpSelectionRotate::Undo(void) {
	Operation::Undo();

	Selector *pSlctr = &(pQMainFrame->GetSelector());
	QooleDoc *pDoc = &(GetDocument());
	ASSERT(pSlctr->GetNumMSelectedObjects() > 0);

	pDoc->ObjectsRotate(pSlctr->GetMSelectedObjects(), opSpace, rPt, -rAng, false);

	// Set the operate center for QViews.
	QView::SetOperateCenterPos(opCenterVec);

	// Update Views.
	pDoc->UpdateAllViews(NULL, DUAV_OBJSSEL, (CObject *) pSlctr->GetScopePtr());
}

void OpSelectionRotate::Redo(void) {
	Operation::Redo();

	QooleDoc *pDoc = &(GetDocument());
	Selector *pSlctr = &(pQMainFrame->GetSelector());
	ASSERT(pSlctr->GetNumMSelectedObjects() > 0);

	pDoc->ObjectsRotate(pSlctr->GetMSelectedObjects(), opSpace, rPt, rAng, false);

	// Set the operate center for QViews.
	QView::SetOperateCenterPos(opCenterVec);

	// Update Views.
	pDoc->UpdateAllViews(NULL, DUAV_OBJSSEL, (CObject *) pSlctr->GetScopePtr());
}

//========== Mode Menu: Scale Object ==========

OpSelectionScale::OpSelectionScale(const TransSpace &viewSpace,
								   const Vector3d &basisVec,
								   const Vector3d &scaleVec,
								   QViewsState *qvBefore)
				 :Operation(true, qvBefore) {
	opSpace = viewSpace;
	bVec = basisVec;
	sVec = scaleVec;
	opCenterVec = QView::GetOperateCenterPos();
}

OpSelectionScale::~OpSelectionScale(void) {
}

void OpSelectionScale::Undo(void) {
	Operation::Undo();

	QooleDoc *pDoc = &(GetDocument());
	Selector *pSlctr = &(pQMainFrame->GetSelector());
	ASSERT(pSlctr->GetNumMSelectedObjects() > 0);

	Vector3d invSVec(1.0f / sVec.GetX(),
		1.0f / sVec.GetY(), 1.0f / sVec.GetZ());

	pDoc->ObjectsScaleBegin(pSlctr->GetMSelectedObjects(), opSpace, bVec);
	pDoc->ObjectsScaleChange(pSlctr->GetMSelectedObjects(), invSVec, false);
	pDoc->ObjectsScaleEnd(pSlctr->GetMSelectedObjects(), false);

	// Set the operate center for QViews.
	QView::SetOperateCenterPos(opCenterVec);

	// Update Views.
	pDoc->UpdateAllViews(NULL, DUAV_OBJSSEL, (CObject *) pSlctr->GetScopePtr());
}

void OpSelectionScale::Redo(void) {
	Operation::Redo();

	QooleDoc *pDoc = &(GetDocument());
	Selector *pSlctr = &(pQMainFrame->GetSelector());
	ASSERT(pSlctr->GetNumMSelectedObjects() > 0);

	pDoc->ObjectsScaleBegin(pSlctr->GetMSelectedObjects(), opSpace, bVec);
	pDoc->ObjectsScaleChange(pSlctr->GetMSelectedObjects(), sVec, false);
	pDoc->ObjectsScaleEnd(pSlctr->GetMSelectedObjects(), false);

	// Set the operate center for QViews.
	QView::SetOperateCenterPos(opCenterVec);

	// Update Views.
	pDoc->UpdateAllViews(NULL, DUAV_OBJSSEL, (CObject *) pSlctr->GetScopePtr());
}

//========== Mode Menu: Brush Manipulation (Face/Edge/Vertex move) ==========
OpBrushModification::OpBrushModification(Object &manipObj, Geometry &oldBrush,
										 QViewsState *qvBefore)
					:OpObjModification(manipObj, true, qvBefore) {
	Selector *pSlctr = &(pQMainFrame->GetSelector());
	ASSERT(pSlctr->GetNumMSelectedObjects() == 1);
	Object *pManipObj = (pSlctr->GetMSelectedObjects())[0].GetPtr();
	ASSERT(pManipObj->HasBrush());
	ASSERT(pManipObj == &manipObj);

	// Change Selection.
	pSlctr->MUnselect();
	pSlctr->MSelectObject(objPtr);

	Geometry *pDupBrush = manipObj.SetBrush(&oldBrush);
	delete pDupBrush;
}

OpBrushModification::~OpBrushModification(void){
}

/*
void OpBrushModification::SwapBrushes(void) {
	QooleDoc *pDoc = &(GetDocument());
	pOldBrush = pDoc->BrushReplace(*pManipObj, *pOldBrush, false);

	pDoc->UpdateAllViews(NULL, DUAV_OBJSMODSTRUCT,
						 (CObject *) pManipObj->GetParentPtr());
}
*/

//========== Mode Menu: Plane Clip ==========
OpPlaneClip *OpPlaneClip::NewPlaneClipOp(const Plane &clipPlane) {
	OpPlaneClip *op = new OpPlaneClip;

	Selector *pSlctr = &(pQMainFrame->GetSelector());
	QooleDoc *pDoc = &(GetDocument());
	ASSERT(pSlctr->GetNumMSelectedObjects() > 0);

	op->oldSelection = pSlctr->GetMSelectedObjects();
	pSlctr->MUnselect();

	Vector3d distVec;
	Object *pObjNode;

	IterLinkList<ObjectPtr> iterCuttees(op->oldSelection);
	iterCuttees.Reset();
	while (!iterCuttees.IsDone()) {
		pObjNode = iterCuttees.GetNext()->GetPtr();

		if (pObjNode->IsItemNode()) { // Skip items.
			op->newSelection.AppendNode(*(new ObjectPtr(pObjNode)));
			continue;
		}

		// Check if we can skip this obj.
		distVec = pObjNode->GetPosition();
		if (clipPlane.DistOffPlane(distVec) > pObjNode->GetBoundRadius()) {
			if (clipPlane.PointInPlane(distVec) < 0) { // Skip.
				op->newSelection.AppendNode(*(new ObjectPtr(pObjNode)));
				continue;
			}
			else {  // Remove and save.
				LinkList<ObjectPtr> rmObjs;
				rmObjs.AppendNode(*(new ObjectPtr(pObjNode)));
				pDoc->ObjectsDelete(rmObjs, false, false, false);
				op->removedObjs.AppendNode(rmObjs.RemoveNode(0));
				continue;
			}
		}

		// Make a backup copy of the current node.
		Object *pNewObjNode = new Object(*pObjNode);
		pDoc->ObjectReplace(*pObjNode, *pNewObjNode, false, false);

		// Perform the clipping on the new obj.
		if (!pNewObjNode->PlaneClip(clipPlane, false)) {
			// No changes... undo the replacement and delete new obj.
			pDoc->ObjectReplace(*pNewObjNode, *pObjNode, false, false);
			delete pNewObjNode;

			op->newSelection.AppendNode(*(new ObjectPtr(pObjNode)));
			continue;
		}

		// Just removed pObjNode cuz it changed.  Need to save it.
		op->removedObjs.AppendNode(*(new ObjectPtr(pObjNode)));

		if (pNewObjNode->IsLeaf() && !pNewObjNode->HasBrush()) {
			// Nothing left.  delete brush.
			delete pNewObjNode;
			continue;
		}

		// A new replacement node was introduced.  Remember it.
		op->replaceObjs.AppendNode(*(new ObjectPtr(pNewObjNode)));
		op->newSelection.AppendNode(*(new ObjectPtr(pNewObjNode)));
	}

	Object *pScope = &(pQMainFrame->GetScope());
	pScope->SetBoundRadius();

	if (op->removedObjs.NumOfElm() == 0) {
		// Sanity.
		ASSERT(op->replaceObjs.NumOfElm() == 0);
		// Restore selection.
		pSlctr->MSelectObjects(op->oldSelection);
		// Nothing was changed..
		delete op;
		op = NULL;
	}
	else {
		// Update all views.
		pSlctr->MSelectObjects(op->newSelection);
		pDoc->UpdateAllViews(NULL, DUAV_OBJSMODSTRUCT | DUAV_OBJSSEL,
							 (CObject *) pScope);
	}

	return op;
}

//===== OpPlaneSplit =====
OpPlaneClip *OpPlaneClip::NewPlaneSplitOp(const Plane &clipPlane) {
	OpPlaneClip *op = new OpPlaneClip;

	Selector *pSlctr = &(pQMainFrame->GetSelector());
	QooleDoc *pDoc = &(GetDocument());
	ASSERT(pSlctr->GetNumMSelectedObjects() > 0);

	op->oldSelection = pSlctr->GetMSelectedObjects();
	pSlctr->MUnselect();

	Vector3d distVec;
	Object *pObjNode;

	IterLinkList<ObjectPtr> iterCuttees(op->oldSelection);
	iterCuttees.Reset();
	while (!iterCuttees.IsDone()) {
		pObjNode = iterCuttees.GetNext()->GetPtr();

		if (pObjNode->IsItemNode()) { // Skip items.
			op->newSelection.AppendNode(*(new ObjectPtr(pObjNode)));
			continue;
		}

		// Check if we can skip this obj.
		distVec = pObjNode->GetPosition();
		if (clipPlane.DistOffPlane(distVec) > pObjNode->GetBoundRadius()) {
			// Not touching the split plane.  Skip.
			op->newSelection.AppendNode(*(new ObjectPtr(pObjNode)));
			continue;
		}

		// Make a backup copy of the current node.
		Object *pNewObjNode = new Object(*pObjNode);
		pDoc->ObjectReplace(*pObjNode, *pNewObjNode, false, false);

		// Perform the clipping on the new obj.
		if (!pNewObjNode->PlaneClip(clipPlane, false) // Brush untouched,
			||
			(pNewObjNode->IsLeaf() && !pNewObjNode->HasBrush()) // Nothing left.
			) {
			// No changes... undo the replacement and delete new obj.
			pDoc->ObjectReplace(*pNewObjNode, *pObjNode, false, false);
			delete pNewObjNode;

			op->newSelection.AppendNode(*(new ObjectPtr(pObjNode)));
			continue;
		}

		// Just removed pObjNode cuz it changed.  Need to save it.
		op->removedObjs.AppendNode(*(new ObjectPtr(pObjNode)));

		// A new replacement node was introduced.  Remember it.
		op->replaceObjs.AppendNode(*(new ObjectPtr(pNewObjNode)));
		op->newSelection.AppendNode(*(new ObjectPtr(pNewObjNode)));

		// Now clip the other side.
		Vector3d invNorm(clipPlane.GetNorm());
		invNorm.MultVector(-1.0f);
		Plane invClipPlane(invNorm, clipPlane.GetPoint());

		pNewObjNode = new Object(*pObjNode);
		bool ok = pNewObjNode->PlaneClip(invClipPlane, false);
		ASSERT(ok && (!pNewObjNode->IsLeaf() || pNewObjNode->HasBrush()));

		// In case things dont work perfectly.
		if (!ok || (pNewObjNode->IsLeaf() && !pNewObjNode->HasBrush())) {
			delete pNewObjNode;
			continue;
		}

		// Another new replacement node was introduced
		// Add it to document.
		LinkList<ObjectPtr> tmpAddList;
		tmpAddList.AppendNode(*(new ObjectPtr(pNewObjNode)));
		pDoc->ObjectsAdd(tmpAddList, pSlctr->GetScope(), false, false);
		tmpAddList.DeleteAllNodes();

		// Remember it for the operation.
		op->replaceObjs.AppendNode(*(new ObjectPtr(pNewObjNode)));
		op->newSelection.AppendNode(*(new ObjectPtr(pNewObjNode)));
	}

	Object *pScope = &(pQMainFrame->GetScope());
	pScope->SetBoundRadius();

	if (op->removedObjs.NumOfElm() == 0) {
		// Sanity.
		ASSERT(op->replaceObjs.NumOfElm() == 0);
		// Restore selection.
		pSlctr->MSelectObjects(op->oldSelection);
		// Nothing was changed..
		delete op;
		op = NULL;
	}
	else {
		// Update all views.
		pSlctr->MSelectObjects(op->newSelection);
		pDoc->UpdateAllViews(NULL, DUAV_OBJSMODSTRUCT | DUAV_OBJSSEL,
							 (CObject *) pScope);
	}

	return op;
}

OpPlaneClip::OpPlaneClip(void) {
}

OpPlaneClip::~OpPlaneClip(void) {
	ObjectPtr *pObjPtr;

	while (removedObjs.NumOfElm() > 0) {
		pObjPtr = &(removedObjs.RemoveNode(0));
		if (commit)
			delete pObjPtr->GetPtr();
		delete pObjPtr;
	}

	while (replaceObjs.NumOfElm() > 0) {
		pObjPtr = &(replaceObjs.RemoveNode(0));
		if (!commit)
			delete pObjPtr->GetPtr();
		delete pObjPtr;
	}

	oldSelection.DeleteAllNodes();
	newSelection.DeleteAllNodes();
}

void OpPlaneClip::Undo(void) {
	Selector *pSlctr = &(pQMainFrame->GetSelector());
	QooleDoc *pDoc = &(GetDocument());
	Object *pScope = &(pSlctr->GetScope());

	pSlctr->MUnselect();
	if (replaceObjs.NumOfElm() > 0)
		pDoc->ObjectsDelete(replaceObjs, false, false, false);
	if (removedObjs.NumOfElm() > 0)
		pDoc->ObjectsAdd(removedObjs, *pScope, false);
	pSlctr->MSelectObjects(oldSelection);

	// Update all views.
	pDoc->UpdateAllViews(NULL, DUAV_OBJSMODSTRUCT | DUAV_OBJSSEL,
						 (CObject *) pScope);
}

void OpPlaneClip::Redo(void) {
	Selector *pSlctr = &(pQMainFrame->GetSelector());
	QooleDoc *pDoc = &(GetDocument());
	Object *pScope = &(pSlctr->GetScope());

	pSlctr->MUnselect();
	if (removedObjs.NumOfElm() > 0)
		pDoc->ObjectsDelete(removedObjs, false, false, false);
	if (replaceObjs.NumOfElm() > 0)
		pDoc->ObjectsAdd(replaceObjs, *pScope, false);
	pSlctr->MSelectObjects(newSelection);

	// Update all views.
	pDoc->UpdateAllViews(NULL, DUAV_OBJSMODSTRUCT | DUAV_OBJSSEL,
						 (CObject *) pScope);
}

//========== Mode Menu: Plane Flip ==========
OpPlaneFlip::OpPlaneFlip(const Plane &flipPlane)
			:fPlane(flipPlane) {
	Flip();
}

OpPlaneFlip::~OpPlaneFlip(void) {
}

void OpPlaneFlip::Flip(void) {
	Selector *pSlctr = &(pQMainFrame->GetSelector());
	QooleDoc *pDoc = &(GetDocument());

	ASSERT(pSlctr->GetMSelectedObjects().NumOfElm() > 0);
	pDoc->FlipObjects(pSlctr->GetMSelectedObjects(), fPlane);
}

//========== Mode Menu: Plane Mirror ==========
// ~(), Undo(), and Redo() are all the same as OpObjsAddNew's.
// Should combine these somehow...
OpPlaneMirror::OpPlaneMirror(const Plane &mirrorPlane) {
	Selector *pSlctr = &(pQMainFrame->GetSelector());
	QooleDoc *pDoc = &(GetDocument());

	// Remember old selection.
	oldSelection = pSlctr->GetMSelectedObjects();
	// Remember parent.
	parentPtr = pSlctr->GetScopePtr();

	// Make dups for the currently selected objs.
	ASSERT(pSlctr->GetNumMSelectedObjects() > 0);
	IterLinkList<ObjectPtr> iter(pSlctr->GetMSelectedObjects());
	Object *pObj;
	iter.Reset();
	while (!iter.IsDone()) {
		pObj = iter.GetNext()->GetPtr();
		pObj = new Object(*pObj);
		nObjs.AppendNode(*(new ObjectPtr(pObj)));
	}
	pDoc->ObjectsAdd(nObjs, *parentPtr, false, false);
	pSlctr->MSelectObjects(nObjs);
	pDoc->FlipObjects(nObjs, mirrorPlane);
}

OpPlaneMirror::~OpPlaneMirror(void) {
	ObjectPtr *pObjPtr;

	while (nObjs.NumOfElm() > 0) {
		pObjPtr = &(nObjs.RemoveNode(0));
		if (!commit) {
			ASSERT(pObjPtr->GetPtr()->GetParentPtr() == NULL);
			delete pObjPtr->GetPtr();
		}
		delete pObjPtr;
	}

	oldSelection.DeleteAllNodes();
}

void OpPlaneMirror::Undo(void) {
#ifndef NDEBUG
	for(int i = 0; i < nObjs.NumOfElm(); i++)
		ASSERT(nObjs[i].GetPtr()->GetParentPtr() == parentPtr);
#endif

	Operation::Undo();

	QooleDoc *pDoc = &(GetDocument());
	Selector *pSlctr = &(pQMainFrame->GetSelector());

	pDoc->ObjectsDelete(nObjs, false, false);
	pSlctr->MSelectObjects(oldSelection);

	// Set the operate center for QViews.
	Vector3d centerVec;
	pSlctr->GetMSelectedObjectsCenter(centerVec);
	QView::SetOperateCenterPos(centerVec);

	pDoc->UpdateAllViews(NULL, DUAV_OBJSSEL | DUAV_OBJSMODSTRUCT,
		(CObject *)	parentPtr);
}

void OpPlaneMirror::Redo(void) {
#ifndef NDEBUG
	for(int i = 0; i < nObjs.NumOfElm(); i++)
		ASSERT(nObjs[i].GetPtr()->GetParentPtr() == NULL);
#endif

	Operation::Redo();

	QooleDoc *pDoc = &(GetDocument());
	Selector *pSlctr = &(pQMainFrame->GetSelector());

	pDoc->ObjectsAdd(nObjs, *parentPtr, false);
	pSlctr->MSelectObjects(nObjs);

	// Set the operate center for QViews.
	Vector3d centerVec;
	pSlctr->GetMSelectedObjectsCenter(centerVec);
	QView::SetOperateCenterPos(centerVec);

	pDoc->UpdateAllViews(NULL, DUAV_OBJSSEL | DUAV_OBJSMODSTRUCT,
		(CObject *) parentPtr);
}

//========== Object Menu: Add Object ==========

OpObjsAddNew::OpObjsAddNew(LinkList<ObjectPtr> &newObjs,
						   const Vector3d &centerVec, Object *parent) {
	// Remember old selection.
	Selector *pSlctr = &(pQMainFrame->GetSelector());
	oldSelection = pSlctr->GetMSelectedObjects();

	ObjectPtr *pObjPtr;
	Vector3d posVec;

	// Move the new objects over.
	while (newObjs.NumOfElm() > 0) {
		pObjPtr = &(newObjs.RemoveNode(0));
		ASSERT(pObjPtr->GetPtr()->GetParentPtr() == NULL);
		posVec.AddVector(centerVec, pObjPtr->GetPtr()->GetPosition());
		pObjPtr->GetPtr()->SetPosition(posVec);
		nObjs.AppendNode(*pObjPtr);
	}

	parentPtr = parent;

	// Add the new objs.
	Redo();
}

OpObjsAddNew::~OpObjsAddNew(void) {
	ObjectPtr *pObjPtr;

	while (nObjs.NumOfElm() > 0) {
		pObjPtr = &(nObjs.RemoveNode(0));
		if (!commit) {
			ASSERT(pObjPtr->GetPtr()->GetParentPtr() == NULL);
			delete pObjPtr->GetPtr();
		}
		delete pObjPtr;
	}

	oldSelection.DeleteAllNodes();
}

void OpObjsAddNew::Undo(void) {

#ifndef NDEBUG
	for(int i = 0; i < nObjs.NumOfElm(); i++)
		ASSERT(nObjs[i].GetPtr()->GetParentPtr() == parentPtr);
#endif

	Operation::Undo();

	QooleDoc *pDoc = &(GetDocument());
	Selector *pSlctr = &(pQMainFrame->GetSelector());

	pDoc->ObjectsDelete(nObjs, false, false);
	pSlctr->MSelectObjects(oldSelection);

	// Set the operate center for QViews.
	Vector3d centerVec;
	pSlctr->GetMSelectedObjectsCenter(centerVec);
	QView::SetOperateCenterPos(centerVec);

	pDoc->UpdateAllViews(NULL, DUAV_OBJSSEL | DUAV_OBJSMODSTRUCT,
		(CObject *)	parentPtr);
}

void OpObjsAddNew::Redo(void) {

#ifndef NDEBUG
	for(int i = 0; i < nObjs.NumOfElm(); i++)
		ASSERT(nObjs[i].GetPtr()->GetParentPtr() == NULL);
#endif

	Operation::Redo();

	QooleDoc *pDoc = &(GetDocument());
	Selector *pSlctr = &(pQMainFrame->GetSelector());

	pDoc->ObjectsAdd(nObjs, *parentPtr, false);
	pSlctr->MSelectObjects(nObjs);

	// Set the operate center for QViews.
	Vector3d centerVec;
	pSlctr->GetMSelectedObjectsCenter(centerVec);
	QView::SetOperateCenterPos(centerVec);

	pDoc->UpdateAllViews(NULL, DUAV_OBJSSEL | DUAV_OBJSMODSTRUCT,
		(CObject *) parentPtr);
}

//========== Object Menu: Delete Object ==========

OpObjsDel::OpObjsDel(void) {

	Selector *pSlctr = &(pQMainFrame->GetSelector());
	delObjs = pSlctr->GetMSelectedObjects();

	// Remove the selections to buffer.
	Redo();
}

OpObjsDel::~OpObjsDel(void) {
	ObjectPtr *pObjPtr;

	while (delObjs.NumOfElm() > 0) {
		pObjPtr = &(delObjs.RemoveNode(0));
		if (commit) {
			ASSERT(pObjPtr->GetPtr()->GetParentPtr() == NULL);
			delete pObjPtr->GetPtr();
		}
		delete pObjPtr;
	}
}

void OpObjsDel::Undo(void) {
	Operation::Undo();

	QooleDoc *pDoc = &(GetDocument());
	Selector *pSlctr = &(pQMainFrame->GetSelector());

	pDoc->ObjectsAdd(delObjs, pQMainFrame->GetScope(), false);
	pSlctr->MSelectObjects(delObjs);

	// Set the operate center for QViews.
	Vector3d centerVec;
	pSlctr->GetMSelectedObjectsCenter(centerVec);
	QView::SetOperateCenterPos(centerVec);

	// Update views.
	pDoc->UpdateAllViews(NULL, DUAV_OBJSSEL | DUAV_OBJSMODSTRUCT,
		(CObject *) pSlctr->GetScopePtr());
}

void OpObjsDel::Redo(void) {
	Operation::Redo();

	QooleDoc *pDoc = &(GetDocument());
	Selector *pSlctr = &(pQMainFrame->GetSelector());

	pDoc->ObjectsDelete(delObjs, false, false);
	pSlctr->MUnselect();

	// Update views.
	pDoc->UpdateAllViews(NULL, DUAV_OBJSSEL | DUAV_OBJSMODSTRUCT,
		(CObject *) pSlctr->GetScopePtr());
}

//========== Desktop: Scope Change ==========
OpScopeChange::OpScopeChange(Object *newScope, LinkList<ObjectPtr> &selection)
			  :Operation(false) {
	Selector *slctPtr = &(pQMainFrame->GetSelector());

	// Remember the old scope and selection
	pOldScope = slctPtr->GetScopePtr();
	oldSlctObjs = slctPtr->GetMSelectedObjects();

	// Remember the new scope and selection.
	pNewScope = newScope;
	while (selection.NumOfElm() > 0)
		newSlctObjs.AppendNode(selection.RemoveNode(0));

	// Remember the before qviews.
	viewsBefore = new QViewsState();
	
	// Change to new scope and new selection.
	Redo();

	// Remember the after qviews.
	viewsAfter = new QViewsState();
}

OpScopeChange::~OpScopeChange(void) {
	// Clean up.
	newSlctObjs.DeleteAllNodes();
	oldSlctObjs.DeleteAllNodes();
}

void OpScopeChange::Undo(void) {
	QooleDoc *pDoc = &(GetDocument());
	Selector *slctPtr = &(pQMainFrame->GetSelector());
	ASSERT(pNewScope == slctPtr->GetScopePtr());

	// Unselect 
	slctPtr->MUnselect();

	// Update QTView.  Need to unselect items and collaps folders.
	pDoc->UpdateAllViews(NULL, DUAV_OBJSSEL | DUAV_QTREEVIEW, (CObject*) pNewScope);

	// Change Scope.
	slctPtr = &(pQMainFrame->ChangeEditScope(pOldScope));
	ASSERT(pOldScope == slctPtr->GetScopePtr());

	// Inform all views that scope has changed.
	pDoc->UpdateAllViews(NULL, DUAV_SCOPECHANGED, (CObject*) pNewScope);

	// Remake the old selection.
	slctPtr->MSelectObjects(oldSlctObjs);

	// Set the operate center for QViews.
	Vector3d centerVec;
	slctPtr->GetMSelectedObjectsCenter(centerVec);
	QView::SetOperateCenterPos(centerVec);

	// Restore old views info.
	Operation::Undo();

	// Update Views.
	pDoc->UpdateAllViews(NULL, DUAV_OBJSSEL, (CObject *) pOldScope);
}

void OpScopeChange::Redo(void) {
	QooleDoc *pDoc = &(GetDocument());
	Selector *slctPtr = &(pQMainFrame->GetSelector());
	ASSERT(pOldScope == slctPtr->GetScopePtr());

	// Unselect everything in the current scope.
	slctPtr->MUnselect();

	// Update QTView.  Need to unselect items and collaps folders.
	pDoc->UpdateAllViews(NULL, DUAV_OBJSSEL | DUAV_QTREEVIEW, (CObject*) pOldScope);

	// Change Scope.
	slctPtr = &(pQMainFrame->ChangeEditScope(pNewScope));
	ASSERT(pNewScope == slctPtr->GetScopePtr());

	// Inform all views that scope has changed.
	pDoc->UpdateAllViews(NULL, DUAV_SCOPECHANGED, (CObject*) pOldScope);

	// ReSelect the new selections.
	slctPtr->MSelectObjects(newSlctObjs);

	// Set the operate center for QViews.
	Vector3d centerVec;
	slctPtr->GetMSelectedObjectsCenter(centerVec);
	QView::SetOperateCenterPos(centerVec);

	// Restore new views info.
	Operation::Redo();

	// Update Views.
	pDoc->UpdateAllViews(NULL, DUAV_OBJSSEL, (CObject *) pNewScope);
}

//========== Object: Grouping ==========

OpGrouping::OpGrouping(LinkList<ObjectPtr> &objsGroup, bool skipRedo) {
	// Precondition.
	ASSERT(objsGroup.NumOfElm() > 0);

	group = true;
	pParentObj = new Object();
	while (objsGroup.NumOfElm() > 0)
		childrenObjs.AppendNode(objsGroup.RemoveNode(0));

	if(!skipRedo)
		Redo();
}

OpGrouping::OpGrouping(Object *pObjUngroup) {
	// PreCondition
	ASSERT(pObjUngroup->GetNumChildren() > 0);

	group = false;
	pParentObj = pObjUngroup;

	Object *pObj;
	IterLinkList<Object> *pIter = &(pParentObj->GetIterChildren());
	pIter->Reset();
	while (!pIter->IsDone()) {
		pObj = pIter->GetNext();
		childrenObjs.AppendNode(*(new ObjectPtr(pObj)));
	}
	
	Redo();
}

OpGrouping::~OpGrouping(void) {
	// Sanity.
	ASSERT(childrenObjs.NumOfElm() > 0);

	if (delParent) {
		ASSERT(pParentObj->GetNumChildren() == 0);
		delete pParentObj;
	}

	childrenObjs.DeleteAllNodes();
}

void OpGrouping::Undo(void) {
	Operation::Undo();

	if (group)
		UnGroupObjs();
	else
		GroupObjs();
}

void OpGrouping::Redo(void) {
	Operation::Redo();

	if (group)
		GroupObjs();
	else
		UnGroupObjs();
}

void OpGrouping::GroupObjs(bool updateViews) {
	delParent = false;

	QooleDoc *pDoc = &(GetDocument());
	pDoc->GroupObjects(*pParentObj, childrenObjs, false);

	// Update selection.
	Selector *pSlctPtr = &(pQMainFrame->GetSelector());
	pSlctPtr->MUnselect();
	pSlctPtr->MSelectObject(pParentObj);

	// Set the operate center for QViews.
	Vector3d centerVec;
	pSlctPtr->GetMSelectedObjectsCenter(centerVec);
	QView::SetOperateCenterPos(centerVec);

	// Update Views.
	Object *pGrandParent = pParentObj->GetParentPtr();
	ASSERT(pGrandParent != NULL);
	if(updateViews) {
		pDoc->UpdateAllViews(NULL, 
			DUAV_OBJSMODSTRUCT | DUAV_OBJSMODATTRIB | DUAV_OBJSSEL,
			(CObject *) pGrandParent);
	}
}

void OpGrouping::UnGroupObjs(bool updateViews) {
	delParent = true;

	Object *pGrandParent = pParentObj->GetParentPtr();
	ASSERT(pGrandParent != NULL);

	QooleDoc *pDoc = &(GetDocument());
	pDoc->UngroupObjects(*pParentObj, false);

	// Update selection.
	Selector *pSlctPtr = &(pQMainFrame->GetSelector());
	pSlctPtr->MUnselect();

	Object *pSelObj;
	IterLinkList<ObjectPtr> iter(childrenObjs);
	iter.Reset();
	while (!iter.IsDone()) {
		pSelObj = (iter.GetNext())->GetPtr();
		pSlctPtr->MSelectObject(pSelObj);
	}

	// Set the operate center for QViews.
	Vector3d centerVec;
	pSlctPtr->GetMSelectedObjectsCenter(centerVec);
	QView::SetOperateCenterPos(centerVec);

	// Update Views.
	if(updateViews) {
		pDoc->UpdateAllViews(NULL,
			DUAV_OBJSMODSTRUCT | DUAV_OBJSMODATTRIB | DUAV_OBJSSEL,
			(CObject *) pGrandParent);
	}
}

//========== TreeView: Object Name change ==========
OpObjNameChange::OpObjNameChange(Object &obj, const char *newName)
				:Operation(false) {
	ASSERT(newName != NULL);
	ASSERT(!obj.IsRoot());

	pObj = &obj;

	oldName = new char[strlen(pObj->GetObjName()) + 1];
	strcpy(oldName, pObj->GetObjName());
	pObj->SetObjName(newName);

 	// Update selection.
	(GetDocument()).UpdateAllViews(NULL, 
		DUAV_OBJSSEL | DUAV_QTREEVIEW,
		(CObject *) pObj->GetParentPtr());

	// Update text label.
	(GetDocument()).UpdateAllViews(NULL, 
		DUTV_RENAMEOBJ | DUAV_QTREEVIEW,
		(CObject *) pObj);
}

OpObjNameChange::~OpObjNameChange(void) {
	ASSERT(oldName != NULL);
	delete oldName;
}

void OpObjNameChange::SwapNames(void) {

	char *hold = new char[strlen(pObj->GetObjName()) + 1];
	strcpy(hold, pObj->GetObjName());
	pObj->SetObjName(oldName);
	delete oldName;
	oldName = hold;

	// Update selection.
	(GetDocument()).UpdateAllViews(NULL, 
		DUAV_OBJSSEL | DUAV_QTREEVIEW,
		(CObject *) pObj->GetParentPtr());

	// Update text label.
	(GetDocument()).UpdateAllViews(NULL, 
		DUTV_RENAMEOBJ | DUAV_QTREEVIEW,
		(CObject *) pObj);
}

//========== TreeView: Drag Move an object node ==========
OpTreeViewDragMove::OpTreeViewDragMove(Object *dropTarget,
									   LinkList<ObjectPtr> &dragObjPtrs,
									   LinkList<ObjectPtr> &oldSlctObjs)
				   :Operation(false) {
	// Sanity.
	ASSERT(dropTarget != NULL);
	pDropScope = dropTarget;

	ASSERT(dragObjPtrs.NumOfElm() > 0);
	while (dragObjPtrs.NumOfElm() > 0)
		movedObjPtrs.AppendNode(dragObjPtrs.RemoveNode(0));

	pOldParent = (movedObjPtrs[0].GetPtr())->GetParentPtr();
	ASSERT(pOldParent != dropTarget);

	while (oldSlctObjs.NumOfElm() > 0)
		oldSelection.AppendNode(oldSlctObjs.RemoveNode(0));

	Selector *pSlct = &(pQMainFrame->GetSelector());
	pOldEditScope = pSlct->GetScopePtr();

	// Remember qviews before.
	viewsBefore = new QViewsState();

	// Move the objs.
	Redo();

	// Remember qviews after.
	viewsAfter = new QViewsState();
}

OpTreeViewDragMove::~OpTreeViewDragMove(void) {

	movedObjPtrs.DeleteAllNodes();
	oldSelection.DeleteAllNodes();
}

void OpTreeViewDragMove::Undo(void) {
	QooleDoc *pDoc = &(GetDocument());
	Selector *pSlct = &(pQMainFrame->GetSelector());

	pSlct->MUnselect();
	pDoc->MoveObjNodesInTree(movedObjPtrs, pOldParent);

	// Update the old parent scope in Treeview.
	pDoc->UpdateAllViews(NULL, DUAV_OBJSMODSTRUCT, (CObject *) pOldParent);

	// Change scope.
	if (pDropScope != pOldEditScope) {
		pSlct = &(pQMainFrame->ChangeEditScope(pOldEditScope));
		pDoc->UpdateAllViews(NULL, DUAV_SCOPECHANGED, (CObject *) pDropScope);
	}

	// Restore selection.
	pSlct->MSelectObjects(oldSelection);

	// Restore old qviews info.
	Operation::Undo();

	// Update the new scope in all views.
	pDoc->UpdateAllViews(NULL, DUAV_OBJSSEL | DUAV_OBJSMODSTRUCT,
		(CObject *) pOldEditScope);
}

void OpTreeViewDragMove::Redo(void) {
	QooleDoc *pDoc = &(GetDocument());
	Selector *pSlct = &(pQMainFrame->GetSelector());

	pSlct->MUnselect();
	pDoc->MoveObjNodesInTree(movedObjPtrs, pDropScope);

	// Update the old parent scope in Treeview.
	pDoc->UpdateAllViews(NULL, DUAV_OBJSMODSTRUCT, (CObject *) pOldParent);

	// Change scope.
	if (pDropScope != pOldEditScope) {
		pSlct = &(pQMainFrame->ChangeEditScope(pDropScope));
		pDoc->UpdateAllViews(NULL, DUAV_SCOPECHANGED, (CObject *) pOldEditScope);
	}

	// Selection.
	pSlct->MSelectObjects(movedObjPtrs);

	// Restore new qviews info.
	Operation::Redo();

	// Update the new scope in all views.
	pDoc->UpdateAllViews(NULL, DUAV_OBJSSEL | DUAV_OBJSMODSTRUCT,
		(CObject *) pDropScope);
}

//========== TreeView: Drag Copy an object node ==========
OpTreeViewDragCopy::OpTreeViewDragCopy(Object *dropTarget,
									   const LinkList<ObjectPtr> &dragObjPtrs,
									   LinkList<ObjectPtr> &oldSlctObjs)
				   :Operation(false) {
	// Sanity.
	ASSERT(dropTarget != NULL);
	pDropScope = dropTarget;

	Object *pObj, *pNewObj;
	IterLinkList<ObjectPtr> iter(dragObjPtrs);
	iter.Reset();
	while (!iter.IsDone()) {
		pObj = (iter.GetNext())->GetPtr();
		pNewObj = new Object(*pObj);
		newObjPtrs.AppendNode(*(new ObjectPtr(pNewObj)));
	}

	while (oldSlctObjs.NumOfElm() > 0)
		oldSelection.AppendNode(oldSlctObjs.RemoveNode(0));

	Selector *pSlct = &(pQMainFrame->GetSelector());
	pOldEditScope = pSlct->GetScopePtr();

	// Remember old qviews info.
	viewsBefore = new QViewsState();

	// Copy the objs.
	Redo();

	// Remember new qviews info.
	viewsAfter = new QViewsState();
}

OpTreeViewDragCopy::~OpTreeViewDragCopy(void) {

	oldSelection.DeleteAllNodes();

	while (newObjPtrs.NumOfElm() > 0) {
		ObjectPtr *pObjPtr = &(newObjPtrs.RemoveNode(0));
		if (!commit)
			delete pObjPtr->GetPtr();
		delete pObjPtr;
	}
}

void OpTreeViewDragCopy::Undo(void) {
	QooleDoc *pDoc = &(GetDocument());
	Selector *pSlct = &(pQMainFrame->GetSelector());

	// Remove the copied objs.
	pSlct->MUnselect();
	pDoc->UpdateAllViews(NULL, DUAV_OBJSSEL , (CObject*) pDropScope);
	pDoc->ObjectsDelete(newObjPtrs, false, false);

	if (pDropScope != pOldEditScope) { 
		// Change scope.
		pSlct = &(pQMainFrame->ChangeEditScope(pOldEditScope));
		pDoc->UpdateAllViews(NULL, DUAV_SCOPECHANGED, (CObject*) pDropScope);
	}

	pSlct->MSelectObjects(oldSelection);

	// Restore old qviews info.
	Operation::Undo();

	// Update view.
	pDoc->UpdateAllViews(NULL, DUAV_OBJSSEL, (CObject *) pOldEditScope);
}

void OpTreeViewDragCopy::Redo(void) {
	QooleDoc *pDoc = &(GetDocument());
	Selector *pSlct = &(pQMainFrame->GetSelector());

	pSlct->MUnselect();
	pDoc->UpdateAllViews(NULL, DUAV_OBJSSEL, (CObject*) pOldEditScope);

	if (pDropScope != pOldEditScope) {
		// Change scope.
		pSlct = &(pQMainFrame->ChangeEditScope(pDropScope));
		pDoc->UpdateAllViews(NULL, DUAV_SCOPECHANGED, (CObject*) pOldEditScope);
	}

	// Add the new objs.
	pDoc->ObjectsAdd(newObjPtrs, *pDropScope, false);
	pSlct->MSelectObjects(newObjPtrs);

	// Restore new qviews info.
	Operation::Redo();

	// Update Views.
	pDoc->UpdateAllViews(NULL, 	DUAV_OBJSSEL | DUAV_OBJSMODSTRUCT,
		(CObject *)	pDropScope);
}

//========== Texture Apply ==========

int OpTextureApply::_faceCount = 0;
FaceList *OpTextureApply::_faceList = NULL;

OpTextureApply::OpTextureApply(char *name) {
	Selector *pSlctr = &(pQMainFrame->GetSelector());
	IterLinkList<ObjectPtr> iter(pSlctr->GetMSelectedObjects());

	texName = new char[strlen(name) + 1];
	strcpy(texName, name);

	Object *pObj;
	_faceCount = 0;

	iter.Reset();
	while(!iter.IsDone()) {
		pObj = iter.GetNext()->GetPtr();
		pObj->PreOrderApply(OpTextureApply::CountFaceList);
	}

	faceCount = _faceCount;
	faceList = new FaceList[faceCount];
	_faceCount = 0;
	_faceList = faceList;

	iter.Reset();
	while(!iter.IsDone()) {
		pObj = iter.GetNext()->GetPtr();
		pObj->PreOrderApply(OpTextureApply::BuildFaceList);
	}

	Redo();
}

OpTextureApply::~OpTextureApply() {
	delete texName;
	for(int i = 0; i < faceCount; i++)
		delete faceList[i].name;
	delete faceList;
}

bool OpTextureApply::CountFaceList(Object &obj) {
	if(obj.HasBrush())
		_faceCount += obj.GetBrush().GetNumFaces();
	return true;
}

bool OpTextureApply::BuildFaceList(Object &obj) {
	if(!obj.HasBrush())
		return true;

	char *name;
	for(int i = 0; i < obj.GetBrush().GetNumFaces(); i++) {
		_faceList[_faceCount].obj = &obj;
		_faceList[_faceCount].index = i;

		name = obj.GetBrush().GetFaceTexturePtr(i)->GetTName();
		_faceList[_faceCount].name = new char[strlen(name) + 1];
		strcpy(_faceList[_faceCount].name, name);

		_faceCount++;
	}

	return true;
}

void OpTextureApply::Undo(void) {
	QooleDoc *pDoc = &(GetDocument());
	for(int i = 0; i < faceCount; i++)
		pDoc->TextureApplyFace(*faceList[i].obj, faceList[i].index,
							   faceList[i].name, false);

	pDoc->UpdateAllViews(NULL, DUAV_NOQTREEVIEW | DUAV_OBJTEXMODATTRIB, NULL);
}

void OpTextureApply::Redo(void) {
	QooleDoc *pDoc = &(GetDocument());
	for(int i = 0; i < faceCount; i++)
		pDoc->TextureApplyFace(*faceList[i].obj, faceList[i].index,
							   texName, false);

	pDoc->UpdateAllViews(NULL, DUAV_NOQTREEVIEW | DUAV_OBJTEXMODATTRIB, NULL);
}

//========== Texture Apply Face ==========

OpTextureApplyFace::OpTextureApplyFace(int faceIndex, char *name) {
	Selector *pSlctr = &(pQMainFrame->GetSelector());

	ASSERT(pSlctr->GetNumMSelectedObjects() == 1);
	pOpObj = (pSlctr->GetMSelectedObjects())[0].GetPtr();

	ASSERT(pOpObj->HasBrush());
	Geometry *pBrush = &(pOpObj->GetBrush());

	ASSERT(faceIndex >= 0 && faceIndex < pBrush->GetNumFaces());
	opFaceIndex = faceIndex;

	newName = new char[strlen(name) + 1];
	strcpy(newName, name);

	FaceTex *pFaceTex;
	pFaceTex = pBrush->GetFaceTexturePtr(opFaceIndex);
	oldName = new char[strlen(pFaceTex->GetTName()) + 1];
	strcpy(oldName, pFaceTex->GetTName());

	Redo();
}

OpTextureApplyFace::~OpTextureApplyFace() {
	delete oldName;
	delete newName;
}

void OpTextureApplyFace::Undo(void) {
	Selector *pSlctr = &(pQMainFrame->GetSelector());
	ASSERT(pSlctr->GetNumMSelectedObjects() == 1);
	ASSERT((pSlctr->GetMSelectedObjects())[0].GetPtr() == pOpObj);

	// Make sure for the right face selection.
	if (pSlctr->GetSelectFaceIndex() != opFaceIndex)
		pSlctr->SelectFaceIndex(opFaceIndex);

	QooleDoc *pDoc = &(GetDocument());
	pDoc->TextureApplyFace(*pOpObj, opFaceIndex, oldName);
}

void OpTextureApplyFace::Redo(void) {
	Selector *pSlctr = &(pQMainFrame->GetSelector());
	ASSERT(pSlctr->GetNumMSelectedObjects() == 1);
	ASSERT((pSlctr->GetMSelectedObjects())[0].GetPtr() == pOpObj);

	// Make sure for the right face selection.
	if (pSlctr->GetSelectFaceIndex() != opFaceIndex)
		pSlctr->SelectFaceIndex(opFaceIndex);

	QooleDoc *pDoc = &(GetDocument());
	pDoc->TextureApplyFace(*pOpObj, opFaceIndex, newName);
}

//========== Texture Manipulation ==========

OpTextureFaceManip::OpTextureFaceManip(int faceIndex, int oldXOffset,
									   int oldYOffset, float oldRotAng,
									   float oldXScale, float oldYScale,
									   bool oldTextureLock) {
	Selector *pSlctr = &(pQMainFrame->GetSelector());

	ASSERT(pSlctr->GetNumMSelectedObjects() == 1);
	pOpObj = (pSlctr->GetMSelectedObjects())[0].GetPtr();

	ASSERT(pOpObj->HasBrush());
	Geometry *pBrush = &(pOpObj->GetBrush());

	ASSERT(faceIndex >= 0 && faceIndex < pBrush->GetNumFaces());
	opFaceIndex = faceIndex;

	oldXOff = oldXOffset;
	oldYOff = oldYOffset;
	oldRot = oldRotAng;
	oldXScl = oldXScale;
	oldYScl = oldYScale;
	oldTexLock = oldTextureLock;

	FaceTex *pFaceTex;
	pFaceTex = pBrush->GetFaceTexturePtr(opFaceIndex);
	pFaceTex->GetTInfo(newXOff, newYOff, newRot, newXScl, newYScl);
	newTexLock = pFaceTex->IsTexLocked();
}

OpTextureFaceManip::~OpTextureFaceManip() {
}

void OpTextureFaceManip::Undo(void) {
	Selector *pSlctr = &(pQMainFrame->GetSelector());
	ASSERT(pSlctr->GetNumMSelectedObjects() == 1);
	ASSERT((pSlctr->GetMSelectedObjects())[0].GetPtr() == pOpObj);

	// Make sure text prop page is visible.
	QPropSheet *pQPSWnd = pQMainFrame->GetPropWnd();
	if (pQPSWnd == NULL) {
		pQMainFrame->SendMessage(WM_COMMAND, ID_OBJECT_PROPERTIES);
		pQPSWnd = pQMainFrame->GetPropWnd();
	}
	if (pQPSWnd->GetPageIndex(pQPSWnd->GetActivePage()) != 3)
		pQPSWnd->SetActivePage(3);

	// Make sure for the right face selection.
	if (pSlctr->GetSelectFaceIndex() != opFaceIndex)
		pSlctr->SelectFaceIndex(opFaceIndex);

	QooleDoc *pDoc = &(GetDocument());
	pDoc->TextureManipulate(*pOpObj, opFaceIndex, oldXOff, oldYOff,
							oldRot, oldXScl, oldYScl, oldTexLock);
}

void OpTextureFaceManip::Redo(void) {
	Selector *pSlctr = &(pQMainFrame->GetSelector());
	ASSERT(pSlctr->GetNumMSelectedObjects() == 1);
	ASSERT((pSlctr->GetMSelectedObjects())[0].GetPtr() == pOpObj);

	// Make sure text prop page is visible.
	QPropSheet *pQPSWnd = pQMainFrame->GetPropWnd();
	if (pQPSWnd == NULL) {
		pQMainFrame->SendMessage(WM_COMMAND, ID_OBJECT_PROPERTIES);
		pQPSWnd = pQMainFrame->GetPropWnd();
	}
	if (pQPSWnd->GetPageIndex(pQPSWnd->GetActivePage()) != 3)
		pQPSWnd->SetActivePage(3);

	// Make sure for the right face selection.
	if (pSlctr->GetSelectFaceIndex() != opFaceIndex)
		pSlctr->SelectFaceIndex(opFaceIndex);

	QooleDoc *pDoc = &(GetDocument());
	pDoc->TextureManipulate(*pOpObj, opFaceIndex, newXOff, newYOff,
							newRot, newXScl, newYScl, newTexLock);
}

//========== Multi Faces Texture Manipulation ==========
OpTextureBrushManip::OpTextureBrushManip(int oldXOffset[], int oldYOffset[],
										 float oldRotAng[], float oldXScale[],
										 float oldYScale[], bool oldTextureLock[]) {
	Selector *pSlctr = &(pQMainFrame->GetSelector());

	ASSERT(pSlctr->GetNumMSelectedObjects() == 1);
	pOpObj = (pSlctr->GetMSelectedObjects())[0].GetPtr();

	ASSERT(pOpObj->HasBrush());
	Geometry *pBrush = &(pOpObj->GetBrush());

	numFaces = pBrush->GetNumFaces();

	pNewXOff = new int[numFaces];
	pOldXOff = new int[numFaces];
	pNewYOff = new int[numFaces];
	pOldYOff = new int[numFaces];
	pNewRot = new float[numFaces];
	pOldRot = new float[numFaces];
	pNewXScl = new float[numFaces];
	pOldXScl = new float[numFaces];
	pNewYScl = new float[numFaces];
	pOldYScl = new float[numFaces];
	pNewTexLock = new bool[numFaces];
	pOldTexLock = new bool[numFaces];

	FaceTex *pFaceTex;
	for(int i = 0; i < numFaces; i++) {
		// Copy the old info over.
		pOldXOff[i] = oldXOffset[i];
		pOldYOff[i] = oldYOffset[i];
		pOldRot[i] = oldRotAng[i];
		pOldXScl[i] = oldXScale[i];
		pOldYScl[i] = oldYScale[i];
		pOldTexLock[i] = oldTextureLock[i];

		// Set the new info.
		pFaceTex = pBrush->GetFaceTexturePtr(i);
		pFaceTex->GetTInfo(pNewXOff[i], pNewYOff[i], pNewRot[i],
						   pNewXScl[i], pNewYScl[i]);
		pNewTexLock[i] = pFaceTex->IsTexLocked();
	}
}

OpTextureBrushManip::~OpTextureBrushManip() {

	delete pNewXOff;
	delete pOldXOff;
	delete pNewYOff;
	delete pOldYOff;
	delete pNewRot;
	delete pOldRot;
	delete pNewXScl;
	delete pOldXScl;
	delete pNewYScl;
	delete pOldYScl;
	delete pNewTexLock;
	delete pOldTexLock;
}

void OpTextureBrushManip::Undo(void) {
	Selector *pSlctr = &(pQMainFrame->GetSelector());
	ASSERT(pSlctr->GetNumMSelectedObjects() == 1);
	ASSERT((pSlctr->GetMSelectedObjects())[0].GetPtr() == pOpObj);

	// Make sure text prop page is visible.
	QPropSheet *pQPSWnd = pQMainFrame->GetPropWnd();
	if (pQPSWnd == NULL) {
		pQMainFrame->SendMessage(WM_COMMAND, ID_OBJECT_PROPERTIES);
		pQPSWnd = pQMainFrame->GetPropWnd();
	}
	if (pQPSWnd->GetPageIndex(pQPSWnd->GetActivePage()) != 3)
		pQPSWnd->SetActivePage(3);

	QooleDoc *pDoc = &(GetDocument());

	for(int i = 0; i < numFaces; i++) {
		pDoc->TextureManipulate(*pOpObj, i, pOldXOff[i], pOldYOff[i],
								pOldRot[i], pOldXScl[i], pOldYScl[i],
								pOldTexLock[i], false);
	}

	pDoc->UpdateAllViews(NULL, DUAV_NOQTREEVIEW | DUAV_OBJTEXMODATTRIB,
						 (CObject *) pOpObj);
}

void OpTextureBrushManip::Redo(void) {
	Selector *pSlctr = &(pQMainFrame->GetSelector());
	ASSERT(pSlctr->GetNumMSelectedObjects() == 1);
	ASSERT((pSlctr->GetMSelectedObjects())[0].GetPtr() == pOpObj);

	// Make sure text prop page is visible.
	QPropSheet *pQPSWnd = pQMainFrame->GetPropWnd();
	if (pQPSWnd == NULL) {
		pQMainFrame->SendMessage(WM_COMMAND, ID_OBJECT_PROPERTIES);
		pQPSWnd = pQMainFrame->GetPropWnd();
	}
	if (pQPSWnd->GetPageIndex(pQPSWnd->GetActivePage()) != 3)
		pQPSWnd->SetActivePage(3);

	QooleDoc *pDoc = &(GetDocument());

	for(int i = 0; i < numFaces; i++) {
		pDoc->TextureManipulate(*pOpObj, i, pNewXOff[i], pNewYOff[i],
								pNewRot[i], pNewXScl[i], pNewYScl[i],
								pNewTexLock[i], false);
	}

	pDoc->UpdateAllViews(NULL, DUAV_NOQTREEVIEW | DUAV_OBJTEXMODATTRIB,
						 (CObject *) pOpObj);
}

//========== Surface Properties ==========

OpModSurfaceAttrib::OpModSurfaceAttrib(int faceIndex, UINT newAttrib, UINT attribMask) {
	Selector *pSlctr = &(pQMainFrame->GetSelector());
	ASSERT(pSlctr->GetNumMSelectedObjects() == 1);
	pOpObj = (pSlctr->GetMSelectedObjects())[0].GetPtr();
	ASSERT(pOpObj->HasBrush());
	Geometry *pBrush = &(pOpObj->GetBrush());

	ASSERT(faceIndex >= -1 && faceIndex < pBrush->GetNumFaces());
	opFaceIndex = faceIndex;

	mask = attribMask;
	nAttrib = newAttrib;
	nVal = 0x0;
	oVals = NULL;

	UINT a1, a3;
	FaceTex *pFaceTex;
	if (opFaceIndex != -1) {
		numFaces = 1;
		oAttribs = new UINT[1];
		pFaceTex = pBrush->GetFaceTexturePtr(opFaceIndex);
		pFaceTex->GetTAttribs(a1, oAttribs[0], a3);
	}
	else {
		numFaces = pBrush->GetNumFaces();
		oAttribs = new UINT[numFaces];
		for(int i = 0; i < numFaces; i++) {
			pFaceTex = pBrush->GetFaceTexturePtr(i);
			pFaceTex->GetTAttribs(a1, oAttribs[i], a3);
		}
	}

	Redo();
}

OpModSurfaceAttrib::OpModSurfaceAttrib(int faceIndex, UINT newValue) {
	Selector *pSlctr = &(pQMainFrame->GetSelector());
	ASSERT(pSlctr->GetNumMSelectedObjects() == 1);
	pOpObj = (pSlctr->GetMSelectedObjects())[0].GetPtr();
	ASSERT(pOpObj->HasBrush());
	Geometry *pBrush = &(pOpObj->GetBrush());

	ASSERT(faceIndex >= -1 && faceIndex < pBrush->GetNumFaces());
	opFaceIndex = faceIndex;

	mask = 0x0;
	nAttrib = 0x0;
	oAttribs = NULL;
	nVal = newValue;

	UINT a1, a2;
	FaceTex *pFaceTex;
	if (opFaceIndex != -1) {
		numFaces = 1;
		oVals = new UINT[1];
		pFaceTex = pBrush->GetFaceTexturePtr(opFaceIndex);
		pFaceTex->GetTAttribs(a1, a2, oVals[0]);
	}
	else {
		numFaces = pBrush->GetNumFaces();
		oVals = new UINT[numFaces];
		for(int i = 0; i < numFaces; i++) {
			pFaceTex = pBrush->GetFaceTexturePtr(i);
			pFaceTex->GetTAttribs(a1, a2, oVals[i]);
		}
	}

	Redo();
}


OpModSurfaceAttrib::~OpModSurfaceAttrib() {
	if (oAttribs != NULL)
		delete [] oAttribs;
	if (oVals != NULL)
		delete [] oVals;
}

void OpModSurfaceAttrib::Undo(void) {
	Selector *pSlctr = &(pQMainFrame->GetSelector());
	ASSERT(pSlctr->GetNumMSelectedObjects() == 1);
	ASSERT(pOpObj == (pSlctr->GetMSelectedObjects())[0].GetPtr());

	// Make sure text prop page is visible.
	QPropSheet *pQPSWnd = pQMainFrame->GetPropWnd();
	if (pQPSWnd == NULL) {
		pQMainFrame->SendMessage(WM_COMMAND, ID_OBJECT_PROPERTIES);
		pQPSWnd = pQMainFrame->GetPropWnd();
	}
	if (pQPSWnd->GetPageIndex(pQPSWnd->GetActivePage()) != 4)
		pQPSWnd->SetActivePage(4);

	// Make sure for the right face selection.
	if (pSlctr->GetSelectFaceIndex() != opFaceIndex)
		pSlctr->SelectFaceIndex(opFaceIndex);

	QooleDoc *pDoc = &(GetDocument());
	if (opFaceIndex != -1) {
		if (oAttribs != NULL)
			pDoc->ModifyTexSurfAttrib(*pOpObj, opFaceIndex, oAttribs[0], false);
		if (oVals != NULL)
			pDoc->ModifyTexValAttrib(*pOpObj, opFaceIndex, oVals[0], false);
	}
	else {
		for(int i = 0; i < numFaces; i++) {
			if (oAttribs != NULL)
				pDoc->ModifyTexSurfAttrib(*pOpObj, i, oAttribs[i], false);
			if (oVals != NULL)
				pDoc->ModifyTexValAttrib(*pOpObj, i, oVals[i], false);
		}
	}

	pDoc->UpdateAllViews(NULL, DUAV_NOQTREEVIEW | DUAV_OBJTEXMODATTRIB,
						 (CObject *) pOpObj);
}

void OpModSurfaceAttrib::Redo(void) {
	Selector *pSlctr = &(pQMainFrame->GetSelector());
	ASSERT(pSlctr->GetNumMSelectedObjects() == 1);
	ASSERT(pOpObj == (pSlctr->GetMSelectedObjects())[0].GetPtr());

	// Make sure text prop page is visible.
	QPropSheet *pQPSWnd = pQMainFrame->GetPropWnd();
	if (pQPSWnd == NULL) {
		pQMainFrame->SendMessage(WM_COMMAND, ID_OBJECT_PROPERTIES);
		pQPSWnd = pQMainFrame->GetPropWnd();
	}
	if (pQPSWnd->GetPageIndex(pQPSWnd->GetActivePage()) != 4)
		pQPSWnd->SetActivePage(4);

	// Make sure for the right face selection.
	if (pSlctr->GetSelectFaceIndex() != opFaceIndex)
		pSlctr->SelectFaceIndex(opFaceIndex);

	UINT a2;
	QooleDoc *pDoc = &(GetDocument());
	if (opFaceIndex != -1) {
		if (oAttribs != NULL) {
			a2 = (nAttrib & mask) | (oAttribs[0] & ~mask);
			pDoc->ModifyTexSurfAttrib(*pOpObj, opFaceIndex, a2, false);
		}
		if (oVals != NULL)
			pDoc->ModifyTexValAttrib(*pOpObj, opFaceIndex, nVal, false);
	}
	else {
		for(int i = 0; i < numFaces; i++) {
			if (oAttribs != NULL) {
				a2 = (nAttrib & mask) | (oAttribs[i] & ~mask);
				pDoc->ModifyTexSurfAttrib(*pOpObj, i, a2, false);
			}
			if (oVals != NULL)
				pDoc->ModifyTexValAttrib(*pOpObj, i, nVal, false);
		}
	}

	pDoc->UpdateAllViews(NULL, DUAV_NOQTREEVIEW | DUAV_OBJTEXMODATTRIB,
						 (CObject *) pOpObj);
}

//========== Brush Content Properties ==========
// Assume all faces have the same content properties.
// Always just reference off of the first face.
OpModContentAttrib::OpModContentAttrib(UINT newValue) {
	Selector *pSlctr = &(pQMainFrame->GetSelector());

	ASSERT(pSlctr->GetNumMSelectedObjects() == 1);
	pOpObj = (pSlctr->GetMSelectedObjects())[0].GetPtr();

	ASSERT(pOpObj->HasBrush());
	Geometry *pBrush = &(pOpObj->GetBrush());

	UINT a2, a3;
	FaceTex *pFaceTex = pBrush->GetFaceTexturePtr(0);
	pFaceTex->GetTAttribs(oldVal, a2, a3);

	newVal = newValue;

	Redo();
}

OpModContentAttrib::~OpModContentAttrib() {
}

void OpModContentAttrib::Undo(void) {
	Selector *pSlctr = &(pQMainFrame->GetSelector());
	ASSERT(pSlctr->GetNumMSelectedObjects() == 1);
	ASSERT(pOpObj == (pSlctr->GetMSelectedObjects())[0].GetPtr());

	// Make sure text prop page is visible.
	QPropSheet *pQPSWnd = pQMainFrame->GetPropWnd();
	if (pQPSWnd == NULL) {
		pQMainFrame->SendMessage(WM_COMMAND, ID_OBJECT_PROPERTIES);
		pQPSWnd = pQMainFrame->GetPropWnd();
	}
	if (pQPSWnd->GetPageIndex(pQPSWnd->GetActivePage()) != 5)
		pQPSWnd->SetActivePage(5);

	QooleDoc *pDoc = &(GetDocument());
	pDoc->ModifyContentAttrib(*pOpObj, oldVal);
}

void OpModContentAttrib::Redo(void) {
	Selector *pSlctr = &(pQMainFrame->GetSelector());
	ASSERT(pSlctr->GetNumMSelectedObjects() == 1);
	ASSERT(pOpObj == (pSlctr->GetMSelectedObjects())[0].GetPtr());

	// Make sure text prop page is visible.
	QPropSheet *pQPSWnd = pQMainFrame->GetPropWnd();
	if (pQPSWnd == NULL) {
		pQMainFrame->SendMessage(WM_COMMAND, ID_OBJECT_PROPERTIES);
		pQPSWnd = pQMainFrame->GetPropWnd();
	}
	if (pQPSWnd->GetPageIndex(pQPSWnd->GetActivePage()) != 5)
		pQPSWnd->SetActivePage(5);

	QooleDoc *pDoc = &(GetDocument());
	pDoc->ModifyContentAttrib(*pOpObj, newVal);
}

//========== Entity Properties ==========

OpEntitySetKey::OpEntitySetKey(Object *pEntObj, const char *key, const char *arg) {
	setKey = new char[strlen(key) + 1];
	strcpy(setKey, key);

	Selector *pSlctr = &(pQMainFrame->GetSelector());

	// TODO: handle multi-selection
	// only deal with single selection for now
	// ObjectPtr *pObjPtr = &((pSlctr->GetMSelectedObjects())[0]);
	// pEntity = pObjPtr->GetPtr()->GetEntityPtr();
	ASSERT(pEntObj->HasEntity());
	pEntity = pEntObj->GetEntityPtr();

	const char *keyName = pEntity->GetKey(key);
	if(keyName) {
		oldArg = new char[strlen(keyName) + 1];
		strcpy(oldArg, keyName);
	}
	else {
		oldArg = new char[1];
		strcpy(oldArg, "");
	}

	newArg = new char[strlen(arg) + 1];
	strcpy(newArg, arg);

	refresh = false;
	Redo();
	refresh = true;
}

OpEntitySetKey::~OpEntitySetKey() {
	delete [] setKey;
	delete [] oldArg;
	delete [] newArg;
}

void OpEntitySetKey::Undo(void) {
	// Make sure ent prop page is visible.
	QPropSheet *pQPSWnd = pQMainFrame->GetPropWnd();
	if (pQPSWnd == NULL) {
		pQMainFrame->SendMessage(WM_COMMAND, ID_OBJECT_PROPERTIES);
		pQPSWnd = pQMainFrame->GetPropWnd();
	}
	if (pQPSWnd->GetPageIndex(pQPSWnd->GetActivePage()) != 2)
		pQPSWnd->SetActivePage(2);

	QooleDoc *pDoc = &(GetDocument());
	pDoc->EntitySetKey(pEntity, setKey, oldArg);
}

void OpEntitySetKey::Redo(void) {
	// Make sure ent prop page is visible.
	QPropSheet *pQPSWnd = pQMainFrame->GetPropWnd();
	if (pQPSWnd == NULL) {
		pQMainFrame->SendMessage(WM_COMMAND, ID_OBJECT_PROPERTIES);
		pQPSWnd = pQMainFrame->GetPropWnd();
	}
	if (pQPSWnd->GetPageIndex(pQPSWnd->GetActivePage()) != 2)
		pQPSWnd->SetActivePage(2);

	QooleDoc *pDoc = &(GetDocument());
	pDoc->EntitySetKey(pEntity, setKey, newArg, refresh);
}

//========== Entity Apply ==========

OpEntityApply::OpEntityApply(const char *name) {
	Selector *pSlctr = &(pQMainFrame->GetSelector());
	ASSERT(pSlctr->GetNumMSelectedObjects() > 0);

	pObj = NULL;
	pOldEnt = NULL;
	if(strlen(name))
		pNewEnt = new Entity(name);
	else
		pNewEnt = NULL;
	opGroup = NULL;
	emptyName = false;

	if(pSlctr->GetNumMSelectedObjects() == 1) {
		pObj = (&((pSlctr->GetMSelectedObjects())[0]))->GetPtr();
		pOldEnt = pObj->GetEntityPtr();
		emptyName = ((pObj->GetObjName())[0] == '\0');
	}
	else {
		LinkList<ObjectPtr> objs;
		objs = pSlctr->GetMSelectedObjects();
		opGroup = new OpGrouping(objs, true);
		pObj = opGroup->GetParent();
	}

	// Make sure entity prop page is visible.
	QPropSheet *pQPSWnd = pQMainFrame->GetPropWnd();
	if (pQPSWnd == NULL) {
		pQMainFrame->SendMessage(WM_COMMAND, ID_OBJECT_PROPERTIES);
		pQPSWnd = pQMainFrame->GetPropWnd();
	}
	if (pQPSWnd->GetPageIndex(pQPSWnd->GetActivePage()) != 2)
		pQPSWnd->SetActivePage(2);

	Redo();
}

OpEntityApply::~OpEntityApply() {
	if(commit && pOldEnt)
		delete pOldEnt;
	if(!commit && pNewEnt)
		delete pNewEnt;
	if(opGroup)
		delete opGroup;
}

void OpEntityApply::Undo(void) {
	QooleDoc *pDoc = &(GetDocument());
	Object *pGrandParent = pObj->GetParentPtr();

	pDoc->EntityApply(pObj, pOldEnt, false);

	if(opGroup)
		opGroup->UnGroupObjs(false);

	if (emptyName)
		pObj->SetObjName("");
	

	pDoc->UpdateAllViews(NULL, DUAV_OBJSMODSTRUCT | DUAV_OBJSMODATTRIB |
		DUAV_OBJSSEL | DUAV_OBJENTMODATTRIB, (CObject *) pGrandParent);
}

void OpEntityApply::Redo(void) {
	QooleDoc *pDoc = &(GetDocument());
	CObject *obj = NULL;

	if(opGroup) {
		opGroup->GroupObjs(false);
		pObj = opGroup->GetParent();
		obj = (CObject *) pObj->GetParentPtr();
	}
	pDoc->EntityApply(pObj, pNewEnt, false);

	pDoc->UpdateAllViews(NULL, DUAV_OBJSMODSTRUCT | DUAV_OBJSMODATTRIB |
		DUAV_OBJSSEL | DUAV_OBJENTMODATTRIB, (CObject *) pObj->GetParentPtr());
}
