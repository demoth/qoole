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
 * selector.cpp
 */

#include "stdafx.h"

#include "selector.h"

//==================== Selector ====================

Selector::Selector(Object &scope) {
  scopePtr = &scope;
  sSelectedObjPtr = NULL;
  faceSlctIndx = -1;
}

Selector::~Selector(void) {
  SUnselect();
  MUnselect();
}

//===== Object Selection Testing =====

bool
Selector::IsSelectionHit(Object *sObjPtr, const Line &slctRay,
                         float rBgn, float rEnd, float &depth) const {
  // PreCondition.
  ASSERT(sObjPtr != NULL);
  ASSERT(sObjPtr->GetParentPtr() == scopePtr);

  Vector3d p0, v0;
  Matrix44 trans;
  Line sRay;

  // Rotate slctRay.
  sObjPtr->CalInvTransSpaceMatrix(trans.SetIdentity());
  trans.Transform(p0, slctRay.GetPoint());
  v0.AddVector(slctRay.GetSlope(), slctRay.GetPoint());
  trans.Transform(v0);
  v0.SubVector(p0);
  sRay.NewLine(p0, v0);
  return (sObjPtr->HitObject(sRay, rBgn, rEnd, depth));
}

//===== Object Selection Cycling =====

Object *
Selector::GetSelection(Object *sObjPtr, const Line &slctRay,
                       float rBgn, float rEnd, int direction) const {
  // Sanity Check.
  ASSERT(sObjPtr == NULL || sObjPtr->GetParentPtr() == scopePtr);

  int i;
  float z;
  Object *tmp, *rtnVal;
  IterLinkList<Object> *iterPtr;
  LinkList<ObjectPtr> hlObjs;
  ObjectPtr *holdPtr;

  // Add intercepted objects into hlObjs sorted by depth.
  holdPtr = NULL;
  iterPtr = &(scopePtr->GetIterChildren());
  iterPtr->Reset();
  while (!iterPtr->IsDone()) {
    tmp = iterPtr->GetNext();
    // Check line.
    if (IsSelectionHit(tmp, slctRay, rBgn, rEnd, z)) {
      // Insert in order.
      for(i = 0; i < hlObjs.NumOfElm(); i++) {
        if (z < hlObjs[i].GetRec())
          break;
      }
      hlObjs.InsertNode(*(new ObjectPtr(tmp, z)), i);
      if (tmp == sObjPtr)
        holdPtr = &(hlObjs.FindNodeByIndex(i));
    }
  }

  if (holdPtr == NULL) {
    i = (direction == 0 ? hlObjs.NumOfElm() - 1 : 0);
  }
  else {
    i = hlObjs.FindNodeIndex(holdPtr);
    ASSERT(i != -1);
    i = (direction == 0 ?
         ((i == 0 ? hlObjs.NumOfElm() : i) - 1) :
         ((i + 1) % hlObjs.NumOfElm()));
  }

  if (i >= 0 && i < hlObjs.NumOfElm())
    rtnVal = hlObjs[i].GetPtr();
  else
    rtnVal = NULL;

  // Clean up.
  while (hlObjs.NumOfElm() > 0)
    delete &(hlObjs.RemoveNode(0));

  return rtnVal;
}

//===== Single Object Selection =====

Object *
Selector::SSelectObject(Object *sObjPtr) {
  // Sanity.
  // ASSERT(sObjPtr != NULL);
  ASSERT(sObjPtr == NULL || sObjPtr->GetParentPtr() == scopePtr);

  sSelectedObjPtr = sObjPtr;
  return sSelectedObjPtr;
}

//===== Multiple Object Selection =====

bool
Selector::IsObjectMSelected(Object *sObjPtr) const {
  ObjectPtr *tmp;
  IterLinkList<ObjectPtr> iter(mSelectedObjs);

  iter.Reset();
  while(!iter.IsDone()) {
    tmp = iter.GetNext();
    if (tmp->GetPtr() == sObjPtr)
      return true;
  }

  return false;
}

bool
Selector::MSelectObject(Object *sObjPtr) {
  // Sanity.
  ASSERT(sObjPtr != NULL);
  ASSERT(sObjPtr->GetParentPtr() == scopePtr);

  // Reset face selection.
  faceSlctIndx = -1;

  int i;
  ObjectPtr *tmp;
  IterLinkList<ObjectPtr> iter(mSelectedObjs);

  for(i = 0, iter.Reset(); !iter.IsDone(); i++) {
    tmp = iter.GetNext();
    if (tmp->GetPtr() == sObjPtr) {
      delete &(mSelectedObjs.RemoveNode(i));
	  sObjPtr->SetSelected(false);
      return false;
    }
  }

  // Set object as selected
  sObjPtr->SetSelected(true);

  mSelectedObjs.AppendNode(*(new ObjectPtr(sObjPtr, 0.0f)));
  return true;
}

void
Selector::MSelectObjects(const LinkList<ObjectPtr> &sObjPtrs) {
  // Reset face selection.
  faceSlctIndx = -1;

  MUnselect();
  mSelectedObjs = sObjPtrs;

  // Set objects in selection list as selected
  IterLinkList<ObjectPtr> iter(mSelectedObjs);
  iter.Reset();
  while(!iter.IsDone())
    iter.GetNext()->GetPtr()->SetSelected(true);
}

void
Selector::MUnselect(Object *sObjPtr) {
  // Reset face selection.
  faceSlctIndx = -1;

  if (sObjPtr == NULL) {
    // Set objects in selection list as unselected
    IterLinkList<ObjectPtr> iter(mSelectedObjs);
    iter.Reset();
    while(!iter.IsDone())
      iter.GetNext()->GetPtr()->SetSelected(false);

    // Clean up the selection list.
    while (mSelectedObjs.NumOfElm() > 0)
      delete &(mSelectedObjs.RemoveNode(0));
  }
  else {
    // Set object as unselected
    sObjPtr->SetSelected(false);

    int i = mSelectedObjs.FindNodeIndex(sObjPtr);
    ASSERT(i != -1);
    delete &(mSelectedObjs.RemoveNode(i));
  }
}

Object *
Selector::GetMNextSelect(const Line &slctRay, float rBgn, float rEnd) const {
  Object *rtnVal;

  if (mSelectedObjs.NumOfElm() == 0) {
    rtnVal = GetSelection(NULL, slctRay, rBgn, rEnd, 1);
  }
  else if (mSelectedObjs.NumOfElm() == 1) {
    rtnVal = GetSelection(mSelectedObjs[0].GetPtr(), slctRay, rBgn, rEnd, 1);
  }
  else {  // Choose within the selected objs.
    rtnVal = FindMultiSlctBaseObj(slctRay, rBgn, rEnd, 1);
    if (rtnVal == NULL)
      rtnVal = GetSelection(NULL, slctRay, rBgn, rEnd, 1);
  }

  return rtnVal;
}

Object *
Selector::GetMPrevSelect(const Line &slctRay, float rBgn, float rEnd) const {
  Object *rtnVal;

  if (mSelectedObjs.NumOfElm() == 0) {
    rtnVal = GetSelection(NULL, slctRay, rBgn, rEnd, 0);
  }
  else if (mSelectedObjs.NumOfElm() == 1) {
    rtnVal = GetSelection(mSelectedObjs[0].GetPtr(), slctRay, rBgn, rEnd, 0);
  }
  else {  // Choose within the selected objs.
    rtnVal = FindMultiSlctBaseObj(slctRay, rBgn, rEnd, 0);
    if (rtnVal == NULL)
      rtnVal = GetSelection(NULL, slctRay, rBgn, rEnd, 0);
  }

  return rtnVal;
}

Object *
Selector::FindMultiSlctBaseObj(const Line &slctRay, float rBgn,
                               float rEnd, int direction) const {
  // Precondition.
  ASSERT(direction == 0 || direction == 1);

  int i;
  float z;
  Object *tmp, *rtnVal;
  LinkList<ObjectPtr> hlObjs;
  IterLinkList<ObjectPtr> iter(mSelectedObjs);

  // Add objs into hlObjs sorted by depth.
  iter.Reset();
  while (!iter.IsDone()) {
    tmp = (iter.GetNext())->GetPtr();
    // Check line.
    if (IsSelectionHit(tmp, slctRay, rBgn, rEnd, z)) {
      // Insert in order.
      for(i = 0; i < hlObjs.NumOfElm(); i++) {
        if (z < hlObjs[i].GetRec())
          break;
      }
      hlObjs.InsertNode(*(new ObjectPtr(tmp, z)), i);
    }
  }
  
  i = hlObjs.NumOfElm();
  if (i == 0)
    rtnVal = NULL;
  else if (direction == 0)
    rtnVal = (hlObjs.FindNodeByIndex(0)).GetPtr();
  else // direction == 1
    rtnVal = (hlObjs.FindNodeByIndex(i - 1)).GetPtr();

  // Clean up.
  while (hlObjs.NumOfElm() > 0)
    delete &(hlObjs.RemoveNode(0));

  return rtnVal;
}

//===== Average Multi Selected Objects' Position =====
Vector3d &
Selector::GetMSelectedObjectsCenter(Vector3d &centerPos) const {

  centerPos.NewVector(0.0f, 0.0f, 0.0f);

  Object *pObj;
  IterLinkList<ObjectPtr> iter(mSelectedObjs);
  iter.Reset();
  while (!iter.IsDone()) {
	  pObj = (iter.GetNext())->GetPtr();
	  centerPos.AddVector(pObj->GetPosition());
  }
  
  if (GetNumMSelectedObjects() > 0) {
    centerPos.MultVector(1.0f / GetNumMSelectedObjects());
  }

  return centerPos;
}

//===== Face Selection for Brush Object =====

void
Selector::ResetSelectFace(const Vector3d &viewVec) {
  faceSlctIndx = -1;

  if (mSelectedObjs.NumOfElm() != 1)
    return;

  Object *sObjPtr = mSelectedObjs[0].GetPtr();
  if (!sObjPtr->HasBrush())
    return;	  

  // Transform the view vec into brush's space.
  Matrix44 m;
  Vector3d vVec, pVec;
  sObjPtr->CalInvTransSpaceMatrix(m.SetIdentity());
  m.Transform(vVec, viewVec);
  m.Transform(pVec, 0.0f, 0.0f, 0.0f);
  vVec.SubVector(pVec);

  int i;
  float cosAng, lowAng = 2.0;
  Geometry *brush = &(sObjPtr->GetBrush());
  const LinkList<PlaneNode> *pPlaneList = &(brush->GetPlaneList());

  // Find the face with the closest normal alignment with viewVec
  for(i = 0; i < brush->GetNumFaces(); i++) {
    cosAng = vVec.CosTheta((*pPlaneList)[i].GetPlane().GetNorm());
    if (cosAng < lowAng) {
      lowAng = cosAng;
      faceSlctIndx = i;
    }
  }
}  

void
Selector::SelectFaceIndex(int fIndex) {
  if (fIndex != -1) {
    ASSERT(mSelectedObjs.NumOfElm() == 1);
    Object *sObjPtr = mSelectedObjs[0].GetPtr();
    ASSERT(sObjPtr->HasBrush());
    ASSERT(fIndex >= 0 && fIndex < sObjPtr->GetBrush().GetNumFaces());
  }

  faceSlctIndx = fIndex;
}

GPolygon *
Selector::GetSelectFace(void) const {
  if (faceSlctIndx == -1)
    return NULL;

  ASSERT(mSelectedObjs.NumOfElm() == 1);
  Object *sObjPtr = mSelectedObjs[0].GetPtr();

  ASSERT(sObjPtr->HasBrush());
  Geometry *brush = &(sObjPtr->GetBrush());

  ASSERT(faceSlctIndx >= 0 && faceSlctIndx < brush->GetNumFaces());
  return &(brush->GetFace(faceSlctIndx));
}

GPolygon *
Selector::CycleSelectFace(const Vector3d &viewVec, int direction) {
  if (faceSlctIndx == -1)
    return NULL;

  ASSERT(mSelectedObjs.NumOfElm() == 1);
  Object *sObjPtr = mSelectedObjs[0].GetPtr();

  ASSERT(sObjPtr->HasBrush());
  Geometry *brush = &(sObjPtr->GetBrush());
  int numFaces = brush->GetNumFaces();
  ASSERT(faceSlctIndx >= 0 && faceSlctIndx < numFaces);
  const LinkList<PlaneNode> *pPlaneList = &(brush->GetPlaneList());

  // Transform viewVec into brush's space.
  Matrix44 m;
  Vector3d vVec, pVec;
  sObjPtr->CalInvTransSpaceMatrix(m.SetIdentity());
  m.Transform(vVec, viewVec);
  m.Transform(pVec, 0.0f, 0.0f, 0.0f);
  vVec.SubVector(pVec);

  // Iter through and calc all all face's normal alignment with viewVec.
  int i, j, iTmp, faceArry[256];
  float valArry[256], fTmp;

  for (i = 0; i < numFaces; i++) {
    faceArry[i] = i;
    valArry[i] = vVec.CosTheta((*pPlaneList)[i].GetPlane().GetNorm());
  }

  // Sort faces based on angle alignment.
  for(i = 0; i < numFaces; i++) {
    for (j = i + 1; j < numFaces; j++) {
      if (valArry[i] > valArry[j]) {
        iTmp = faceArry[i];  faceArry[i] = faceArry[j];  faceArry[j] = iTmp;
		fTmp = valArry[i];  valArry[i] = valArry[j];  valArry[j] = fTmp;
      }
    }
  }

  // Locate the previous slcted face.
  for(i = 0; i < numFaces; i++) {
    if (faceArry[i] == faceSlctIndx)
      break;
  }
  ASSERT(i < numFaces);

  if (direction == 1)
    i = (i + 1) % numFaces;
  else if (direction == -1)
    i = (i - 1 + numFaces) % numFaces;

  faceSlctIndx = faceArry[i];

  return &(brush->GetFace(faceSlctIndx));
}

FaceTex *Selector::checkFaceTex;
Texture *Selector::checkTexture;
bool Selector::checkFailed;

// determine if there's a common FaceTex for selected objects
FaceTex *Selector::GetSelectedFaceTex(void) {
	FaceTex *pFaceTex = NULL;

	int numObjs = GetNumMSelectedObjects();
	if(!numObjs)
		return NULL;

	checkFaceTex = NULL;
	checkTexture = NULL;
	checkFailed = false;

	for(int i = 0; i < numObjs; i++) {
		Object *pSelObj = GetMSelectedObjects()[i].GetPtr();
		pSelObj->PreOrderCondApply(Selector::CheckTexture);
		if(checkFailed) {
			pFaceTex = NULL;
			break;
		}
		pFaceTex = checkFaceTex;
	}

	return pFaceTex;
}

bool Selector::CheckTexture(Object &obj) {
	if(!obj.HasBrush())
		return true;

	Geometry *pBrush = &obj.GetBrush();

	if(!checkFaceTex) {
		checkFaceTex = pBrush->GetFaceTexturePtr(0);
		checkTexture = checkFaceTex->GetTexture();
	}

	for(int i = 0; i < pBrush->GetNumFaces(); i++) {
		if(pBrush->GetFaceTexturePtr(i)->GetTexture() != checkTexture) {
			checkFailed = true;
			return false;
		}
	}

	return true;
}

