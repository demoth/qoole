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
 * objects.cpp
 */

#include "stdafx.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "objects.h"

//==================== Object ====================

Object::Object(const char *className) {
  parent = NULL;
  boundRadius = 0.0f;
  iterChildrenPtr = new IterLinkList<Object>(children);
  objName[0] = '\0';
  brush = NULL;
  entityPtr = NULL;
  viewTag = false;
  itemApprx = false;
  treeAttrib = false;
  bSelected = false;

  if (className != NULL) {
    Entity *ePtr = new Entity(className);
    ASSERT(ePtr != NULL);  // Sanity Check.
    ePtr = SetEntity(ePtr);
    ASSERT(ePtr == NULL);  // Snaity.
  }
}

Object::Object(const Object &src) {
  parent = NULL;
  brush = NULL;
  entityPtr = NULL;
  iterChildrenPtr = new IterLinkList<Object>(children);
  *this = src;  
}

Object::~Object(void) {
  // Sanity Check.
  ASSERT(IsLeaf() || (!HasBrush() && !IsItemNode()));

  DelObj();
  delete iterChildrenPtr;
}

Object &
Object::operator=(const Object &src) {

  if (&src == this)
    return *this;

  // ElmNode::operator=(src);
  TransSpace::operator=(src);

  DelObj(true);

  // Sanity.
  ASSERT(parent == NULL);

  brush = (src.brush == NULL ? NULL : new Geometry(*src.brush));

  Entity *ePtr = (src.entityPtr == NULL ? NULL : new Entity(*src.entityPtr));
  if ((ePtr = SetEntity(ePtr)) != NULL)
    delete ePtr;

  IterLinkList<Object> srcChildren(src.children);
  Object *chld;

  srcChildren.Reset();
  while (!srcChildren.IsDone()) {
    chld = new Object(*srcChildren.GetNext());
    AddChild(*chld, false);
  }

  strcpy(objName, src.objName);

  boundRadius = src.boundRadius;
  viewTag = false;
  itemApprx = false;
  treeAttrib = false;
  bSelected = false;

  return *this;
}

void
Object::DelObj(bool rmFrmPrnt, bool resetBound) {
  // Remove from parent.
  if (!IsRoot() && rmFrmPrnt) {
    GetParent().RemoveChild(*this);
    parent = NULL;
  }

  // Delete name.
  objName[0] = '\0';

  // Delete brush.
  if (brush != NULL)
    delete brush;
  brush = NULL;

  // Delete all children.
  while (GetNumChildren() > 0)
    delete &(RemoveChild(GetChild(0), false));

  // Delete Entity info
  Entity *ePtr = NULL;
  if ((ePtr = SetEntity(ePtr, false)) != NULL)
    delete ePtr;

  if (resetBound)
	  SetBoundRadius();
}

//===== Object: tree traversal =====

bool
Object::IsRoot(void) const {
  return (GetParentPtr() == NULL);
}

Object &
Object::GetRoot(void) {
  return (IsRoot() ? *this : GetParent().GetRoot());
}

bool
Object::IsLeaf(void) const {
  return (GetNumChildren() == 0);
}

Object *
Object::GetParentPtr(void) const {
  return parent;
}

Object &
Object::GetParent(void) const {
  // Sanity Check.
  ASSERT(parent != NULL);
  return *parent;
}

Object &
Object::SetParentPtr(Object *parentPtr) {
  parent = parentPtr;
  return *this;
}

Object &
Object::SetParent(Object &parentObj) {
  parent = &parentObj;
  return *this;
}

int
Object::GetNumChildren(void) const {
  return children.NumOfElm();
}

Object &
Object::AddChild(Object &childNode, bool resetBound, int index) {
  // Sanity Check.
  ASSERT(!HasBrush());
  ASSERT(!HasEntity() || (IsModelNode() &&
          (strcmp(GetEntityPtr(true)->GetClassName(), "worldspawn") == 0 ||
           childNode.CountEntities() == 0)));
  ASSERT(index >= index -1 && index <= children.NumOfElm());

  childNode.SetParent(*this);
  if (index == -1)
    children.AppendNode(childNode);
  else
    children.InsertNode(childNode, index);

  if (resetBound)
    SetBoundRadius();

  return *this;
}

Object &
Object::RemoveChild(Object &childNode, bool resetBound, int *pIndex) {
  // PreCondition.
  ASSERT(IsMyChild(childNode));

  Object *rtnVal;

  if (pIndex != NULL) {
	  *pIndex = children.FindNodeIndex(&childNode);
  }

  childNode.SetParentPtr(NULL);
  rtnVal = &(children.RemoveNode(childNode));
  if (resetBound)
    SetBoundRadius();

  return *rtnVal;
}

Object &
Object::GetChild(int childNum) const {
  // PreCondition.
  ASSERT(childNum >= 0 && childNum < children.NumOfElm());

  return children[childNum];
}

int
Object::FloatChildren(bool resetBound) {
  // Sanity.
  ASSERT(!IsRoot());
  ASSERT(GetNumChildren() > 0);

  int rtnVal = GetNumChildren();

  Matrix44 trans;
  CalTransSpaceMatrix(trans.SetIdentity());

  // Reset current obj.
  Vector3d oVec(0.0f, 0.0f, 0.0f);
  TransSpace::SetPosition(oVec);

  SphrVector oriVec(0.0f, 0.0f, 0.0f);
  TransSpace::SetOrientation(oriVec);

  Vector3d sVec(1.0f, 1.0f, 1.0f);
  TransSpace::SetScale(sVec);

  Object *childPtr;
  Object *parentPtr = GetParentPtr();
  ASSERT(parentPtr != NULL);  // Sanity.

  IterLinkList<Object> *iter = &(GetIterChildren());
  iter->Reset();
  while (!iter->IsDone()) {
    childPtr = iter->GetNext();
	childPtr->Transform(trans, resetBound);
    RemoveChild(*childPtr, false);
    parentPtr->AddChild(*childPtr, false);
  }

  // Recalculate bound radius.
  if (resetBound)
    SetBoundRadius();

  // Post Condition.
  ASSERT(GetNumChildren() == 0);

  return rtnVal;
}

IterLinkList<Object> &
Object::GetIterChildren(void) {
  // Sanity.
  ASSERT(iterChildrenPtr != NULL);

  iterChildrenPtr->Reset();
  return *iterChildrenPtr;
}

bool
Object::IsMyChild(const Object &node) const {
  return (children.FindNodeIndex(&node) != -1);
}

bool
Object::IsMyAncestor(const Object &node) const {
  if (IsRoot())
    return false;
  if (GetParentPtr() == &node)
    return true;
  else
    return (GetParent().IsMyAncestor(node));
}

Object *
Object::FindCommonAncestor(Object &node) {
	if (IsMyAncestor(node) || this == &node)
		return &node;
	if (node.IsRoot())
		return NULL;
	return FindCommonAncestor(node.GetParent());
}

int
Object::CountNodes(int cType) const {
  // PreCondition
  ASSERT(cType >= 0 && cType < 4);
  // Sanity Check.
  ASSERT(IsLeaf() || (!HasBrush() && !IsItemNode()));

  int rtnVal = 0;
  Object *chld;
  IterLinkList<Object> iter(children);

  if (cType == 0) {
    rtnVal++;
  }
  else if (cType == 1) {
    // Kind of useless.
    rtnVal += (HasEntity() && GetEntityPtr()->IsItemEntity() ? 1 : 0);
  }
  else if (cType == 2) {
    rtnVal += (HasBrush() ? 1 : 0);
  }
  else {
    rtnVal += (HasEntity() ? 1 : 0);
  }

  iter.Reset();
  while (!iter.IsDone()) {
    chld = iter.GetNext();
    if (cType == 0)
      rtnVal += chld->CountNodes();
    else if (cType == 1)
      rtnVal += chld->CountItems();
    else if (cType == 2)
      rtnVal += chld->CountBrushes();
    else
      rtnVal += chld->CountEntities();
  }

  return rtnVal;
}

int
Object::PreOrderApply(bool (*func)(Object &obj)) {
  int rtnVal = 0;
  IterLinkList<Object> iter(children);
  
  if ((*func)(*this))
    rtnVal++;
  iter.Reset();
  while (!iter.IsDone()) {
    rtnVal += ((iter.GetNext())->PreOrderApply(func));
  }

  return rtnVal;
}

int
Object::PreOrderCondApply(bool (*func)(Object &obj)) {
  int rtnVal = 0;
  IterLinkList<Object> iter(children);

  if ((*func)(*this)) {
    rtnVal++;
    iter.Reset();
    while (!iter.IsDone()) {
      rtnVal += ((iter.GetNext())->PreOrderCondApply(func));
    }
  }

  return rtnVal;
}

int
Object::PostOrderApply(bool (*func)(Object &obj)) {
  int rtnVal = 0;
  IterLinkList<Object> iter(children);

  iter.Reset();
  while (!iter.IsDone()) {
    rtnVal += ((iter.GetNext())->PostOrderApply(func));
  }
  if ((*func)(*this))
    rtnVal++;

  return rtnVal;
}


//===== Object Name =====
const char *
Object::GetObjName(void) const {
  return objName;
}

void
Object::SetObjName(const char *name) {
  int i;

  if (name == NULL) {
    objName[0] = '\0';
    return;
  }

  for(; *name != '\0' && (isspace(*name) || *name == ':'); name++);
  for(i = 0;
      i < 31 && !(name[i] == '\n' || name[i] == '\r' || name[i] == '\0');
      i++) {
    objName[i] = name[i];
  }
  objName[i] = '\0';
}

//===== Brush Manipulation =====

bool
Object::HasBrush(void) const {
  return (brush != NULL);
}

Geometry &
Object::GetBrush(void) const {
  // Sanity Check.
  ASSERT(brush != NULL);

  return *brush;
}

Geometry *
Object::SetBrush(Geometry *brushPtr) {
  // PreCondition.
  ASSERT(IsLeaf() && (!HasEntity() || IsModelNode()));

  Geometry *rtnVal = brush;

  brush = brushPtr;
  SetBoundRadius();

  return rtnVal;
}

bool
Object::HollowInBrush(float delta) {
  // PreCondition.
  ASSERT(HasBrush());

  LinkList<Geometry> *brushListPtr;
  Geometry *hollowee;
  Object *newObj;
  Matrix44 trans;
  Vector3d pVec;
  bool rtnVal = false;

  if (!HasBrush())
    return false;

  hollowee = new Geometry(GetBrush());
  CalTransSpaceMatrix(trans.SetIdentity());
  hollowee->Transform(trans);
  hollowee->CenterGeometry(pVec);

  brushListPtr = hollowee->HollowInward(delta);

  if (brushListPtr == NULL || brushListPtr->NumOfElm() <= 1) {
    rtnVal = false;
  }
  else {
    Vector3d p2Vec, sVec(1.0f, 1.0f, 1.0f);
    SphrVector oriVec(0.0f, 0.0f, 0.0f);

    delete brush;
    brush = NULL;

    while (brushListPtr->NumOfElm() > 0) {
      newObj = new Object();
      newObj->brush = &(brushListPtr->RemoveNode(0));
      (newObj->brush)->CenterGeometry(p2Vec);
      newObj->SetBoundRadius(false);
      newObj->SetPosition(p2Vec);
      AddChild(*newObj, false);
    }
    SetBoundRadius(true);

    TransSpace::SetPosition(pVec);
    TransSpace::SetOrientation(oriVec);
    TransSpace::SetScale(sVec);

    rtnVal = true;
  }

  delete hollowee;

  return rtnVal;
}

bool
Object::HollowOutBrush(float delta) {
  Geometry *holdBrush = GetBrush().CreateExpandedGeom(delta);
  
  if (holdBrush == NULL)
    return false;

  holdBrush = SetBrush(holdBrush);
  bool rtnVal = HollowInBrush(delta);

  if (!rtnVal) {
    holdBrush = SetBrush(holdBrush);
    ASSERT(holdBrush != NULL);
  }
  else {
    ASSERT(brush == NULL);
  }

  delete holdBrush;

  return rtnVal;
}

//===== Entity Info =====

bool
Object::HasEntity(void) const {
  return (entityPtr != NULL);
}

Entity *
Object::GetEntityPtr(bool bubble) const {
  if (entityPtr != NULL)
    return entityPtr;
  else if (bubble == false || IsRoot())
    return NULL;
  else
    return (GetParent()).GetEntityPtr(bubble);
}

Entity *
Object::SetEntity(Entity *ePtr, bool resetBound) {
  // PreCondition.
  ASSERT(ePtr == NULL || ePtr->IsModelEntity() || (!HasBrush() && IsLeaf()));

  Entity *rtnVal = entityPtr;
  if (ePtr != NULL && objName[0] == '\0') {
	  SetObjName(ePtr->GetClassName());
  }

  entityPtr = ePtr;
  if (resetBound)
	  SetBoundRadius();
  return rtnVal;
}

bool
Object::IsItemNode(void) const {
  return (HasEntity() && (GetEntityPtr())->IsItemEntity());
}

bool
Object::IsModelNode(void) const {
  return (HasEntity() && (GetEntityPtr())->IsModelEntity());
}

const WireFrameGeom &
Object::GetItemWireFrame(void) const {
  // Sanity.
  ASSERT(IsItemNode());

  return (GetEntityPtr()->GetItemWireFrame());
}

//===== Object Dimension =====

Vector3d &
Object::AutoCenterByBound(Vector3d &centerVec) {
  IterLinkList<Object> iter(children);
  Object *objPtr;
  Vector3d minVec(0.0f, 0.0f, 0.0f);
  Vector3d maxVec(0.0f, 0.0f, 0.0f);
  Vector3d posVec;
  Matrix44 trans;

  SetTransformation(trans.SetIdentity());
  GetBoundBox(minVec, maxVec);
  centerVec.AddVector(minVec, maxVec);
  centerVec.MultVector(0.5f);

  iter.Reset();
  while (!iter.IsDone()) {
    objPtr = iter.GetNext();
    posVec.SubVector(objPtr->GetPosition(), centerVec);
    objPtr->TransSpace::SetPosition(posVec);
  }

  SetBoundRadius();

  return centerVec;
}

void
Object::GetObjDimension(Vector3d &dimVec) {
  IterLinkList<Object> iter(children);
  Vector3d minVec, maxVec;
  Matrix44 trans;

  SetTransformation(trans.SetIdentity());
  GetBoundBox(minVec, maxVec);

  // Sanity.
  ASSERT(minVec.GetX() <= maxVec.GetX());
  ASSERT(minVec.GetY() <= maxVec.GetY());
  ASSERT(minVec.GetZ() <= maxVec.GetZ());

  dimVec.SubVector(maxVec, minVec);
}

void
Object::GetBoundingVectors(Vector3d &minVec, Vector3d &maxVec) {
	Matrix44 trans;
	SetTransformation(trans.SetIdentity());
	GetBoundBox(minVec, maxVec);
}

Vector3d
Object::GetMinBoundVec(void) {
  IterLinkList<Object> iter(children);
  Vector3d minVec, maxVec;
  Matrix44 trans;

  SetTransformation(trans.SetIdentity());
  GetBoundBox(minVec, maxVec);

  // Sanity.
  ASSERT(minVec.GetX() <= maxVec.GetX());
  ASSERT(minVec.GetY() <= maxVec.GetY());
  ASSERT(minVec.GetZ() <= maxVec.GetZ());

  return minVec;
}

Vector3d
Object::GetMaxBoundVec(void) {
  IterLinkList<Object> iter(children);
  Vector3d minVec, maxVec;
  Matrix44 trans;

  SetTransformation(trans.SetIdentity());
  GetBoundBox(minVec, maxVec);

  // Sanity.
  ASSERT(minVec.GetX() <= maxVec.GetX());
  ASSERT(minVec.GetY() <= maxVec.GetY());
  ASSERT(minVec.GetZ() <= maxVec.GetZ());

  return maxVec;
}

void
Object::GetBoundBox(Vector3d &minVec, Vector3d &maxVec) {
  // Sanity.
  // ASSERT(!IsRoot());

  if (IsItemNode()) {
    Vector3d bVec(boundRadius, boundRadius, boundRadius);
    minVec.NewVector(0.0f, 0.0f, 0.0f);
    (GetTransformation()).Transform(minVec);
    maxVec.AddVector(minVec, bVec);
    minVec.SubVector(bVec);
    return;
  }

  if (HasBrush()) {
    (GetBrush()).FindTransBoundBox(GetTransformation(), minVec, maxVec);
    return;
  }

  Object *objPtr;
  Vector3d bMinVec, bMaxVec;
  IterLinkList<Object> iter(children);
  Matrix44 trans;
  bool init = false;

  iter.Reset();
  while (!iter.IsDone()) {
    objPtr = iter.GetNext();

    trans = GetTransformation();
    objPtr->CalTransSpaceMatrix(trans);
    objPtr->SetTransformation(trans);
    objPtr->GetBoundBox(bMinVec, bMaxVec);

    if (!init) {
      init = true;
      minVec = bMinVec;
      maxVec = bMaxVec;
    }
    else {
      minVec.NewVector(Min(minVec.GetX(), bMinVec.GetX()),
                       Min(minVec.GetY(), bMinVec.GetY()),
                       Min(minVec.GetZ(), bMinVec.GetZ()));
      maxVec.NewVector(Max(maxVec.GetX(), bMaxVec.GetX()),
                       Max(maxVec.GetY(), bMaxVec.GetY()),
                       Max(maxVec.GetZ(), bMaxVec.GetZ()));
    }
  }
}

//===== transformation =====
const Matrix44 &
Object::SetTransformation(const Matrix44 &transMatrix) {
  return (absTransMatrix = transMatrix);
}

const Matrix44 &
Object::SetInvTransformation(const Matrix44 &transMatrix) {
  return (invTransMatrix = transMatrix);
}

void
Object::Transform(const Matrix44 &transMatrix,
                  bool resetBound, bool mirrorEffect) {
  Matrix44 m1(transMatrix);
  Vector3d pVec, sVec;
  SphrVector oriVec;  

  CalTransSpaceMatrix(m1);

  TransSpace::SetScale(sVec.NewVector(1.0f, 1.0f, 1.0f));
  if (IsItemNode()) {
    // Sanity
    ASSERT(GetNumChildren() == 0);

    Vector3d fVec, tVec;
    m1.Transform(pVec, 0.0f, 0.0f, 0.0f);
    (m1.Transform(fVec, 0.0f, 1.0f, 0.0f)).SubVector(pVec);
    (m1.Transform(tVec, 0.0f, 0.0f, 1.0f)).SubVector(pVec);
	TransSpace::SetPosition(pVec);
	TransSpace::SetOrientation(fVec, tVec);
  }
  else if (HasBrush()) {
	// Sanity
    ASSERT(GetNumChildren() == 0);

    GetBrush().Transform(m1, mirrorEffect);
    GetBrush().CenterGeometry(pVec);
    TransSpace::SetPosition(pVec);

    SphrVector oriVec(0.0f, 0.0f, 0.0f);
	TransSpace::SetOrientation(oriVec);
  }
  else {
    // Sanity.
    ASSERT(GetNumChildren() > 0);

    Vector3d aveVec(0.0f, 0.0f, 0.0f);
    Object *childPtr;
    IterLinkList<Object> iter(children);
    iter.Reset();

    while (!(iter.IsDone())) {
      childPtr = iter.GetNext();
      childPtr->Transform(m1, resetBound, mirrorEffect);
      childPtr->GetPosition(pVec);
      aveVec.AddVector(pVec);
    }

    // Reset position based on mean of children's positions.
    aveVec.MultVector(1.0f / children.NumOfElm());
    TransSpace::SetPosition(aveVec);

    // Adjust all children's position relatively.
    iter.Reset();
    while (!iter.IsDone()) { 
      childPtr = iter.GetNext();
      (childPtr->GetPosition(pVec)).SubVector(aveVec);
      childPtr->TransSpace::SetPosition(pVec);
    }

    // Reset orientation.
    SphrVector oriVec(0.0f, 0.0f, 0.0f);
    TransSpace::SetOrientation(oriVec);
  }

  if (resetBound)
    SetBoundRadius(false);
}

void 
Object::GetTransMatrix(Object &startObj,
					   Object &endObj, Matrix44 &transMatrix) {
	const Object *pStart = &startObj;
	const Object *pEnd = &endObj;
	const Object *pRoot = &(startObj.GetRoot());
	ASSERT(&(endObj.GetRoot()) == pRoot);

	// Traver up the tree from startObj until
	//  we reach a common ancestor.
	Matrix44 m1, m2, m3;
	m1.SetIdentity();
	while (pStart != pEnd && !pEnd->IsMyAncestor(*pStart)) {
		pStart->CalTransSpaceMatrix(m3.SetIdentity());
		m1 = m2.Multiply(m3, m1);
		pStart = pStart->GetParentPtr();
		ASSERT(pStart != NULL);
	}

	m2.SetIdentity();
	while (pStart != pEnd) {
		pEnd->CalInvTransSpaceMatrix(m2);
		pEnd = pEnd->GetParentPtr();
		ASSERT(pEnd != NULL);
	}

	transMatrix.Multiply(m2, m1);
}

//===== TransSpace wrapper =====
const Vector3d &
Object::SetPosition(const Vector3d &newPos) {
  const Vector3d *rtnVal;
  rtnVal = &(TransSpace::SetPosition(newPos));
  if (!IsRoot())
    (GetParent()).SetBoundRadius();
  return *rtnVal;
}

const Vector3d &
Object::MoveRelPosition(const Vector3d &posDelta) {
  const Vector3d *rtnVal;
  rtnVal = &(TransSpace::MoveRelPosition(posDelta));
  if (!IsRoot())
    (GetParent()).SetBoundRadius();
  return *rtnVal;
}

const Vector3d &
Object::SetOrientation(const Vector3d &frontVec, const Vector3d &topVec) {
  Vector3d fv(frontVec);
  Vector3d tv(topVec);

  if (CountEntities() > 0) {
    // Hack to check for the camera intermission entity.
    if (!IsItemNode() ||
        strcmp(GetEntityPtr()->GetClassName(), "info_intermission") != 0) {
      Vector3d xAxis(1.0f, 0.0f, 0.0f);
      Vector3d yAxis(0.0f, 1.0f, 0.0f);

      if (frontVec.GetX() == 0.0 && frontVec.GetY() == 0.0)
        fv.NewVector(-xAxis.DotVector(tv), -yAxis.DotVector(tv), 0.0f);
      else
        fv.NewVector(xAxis.DotVector(fv), yAxis.DotVector(fv), 0.0f);

      tv.NewVector(0.0f, 0.0f, 1.0f);
    }
  }

  return (TransSpace::SetOrientation(fv, tv));
}

const SphrVector &
Object::SetOrientation(const SphrVector &orientVec) {
  Vector3d fv, tv;

  orientVec.GetRectVector(fv, tv);
  SetOrientation(fv, tv);
  return (TransSpace::GetOrientation());
}

const SphrVector &
Object::ChangeRelOrientation(const SphrVector &deltaVec) {
  Vector3d fv, tv;

  TransSpace::ChangeRelOrientation(deltaVec);
  TransSpace::GetOrientation(fv, tv);
  SetOrientation(fv, tv);
  return (TransSpace::GetOrientation());
}

const Vector3d &
Object::SetScale(const Vector3d &newScale) {
  const Vector3d *rtnVal;

  if (!IsItemNode()) {
    rtnVal = &(TransSpace::SetScale(newScale));
    if (!IsRoot())
      (GetParent()).SetBoundRadius();
  }
  else {
    rtnVal = &(TransSpace::GetScale());
    // Sanity.
    ASSERT(rtnVal->GetX() == 1.0 &&
           rtnVal->GetY() == 1.0 &&
           rtnVal->GetZ() == 1.0);
  }

  return *rtnVal;
}

const Vector3d &
Object::ChangeScale(const Vector3d &scaleDelta) {
  const Vector3d *rtnVal;

  if (!IsItemNode()) {
    rtnVal = &(TransSpace::ChangeScale(scaleDelta));
    if (!IsRoot())
      (GetParent()).SetBoundRadius();
  }
  else {
    rtnVal = &(TransSpace::GetScale());
    // Sanity.
    ASSERT(rtnVal->GetX() == 1.0 &&
           rtnVal->GetY() == 1.0 &&
           rtnVal->GetZ() == 1.0);
  }

  return *rtnVal;
}

//===== Hit Detection =====
bool
Object::ResetBoundRadius(Object &obj) {
  obj.SetBoundRadius(false);
  return true;
}

float
Object::SetBoundRadius(bool bubble) {

  if (HasBrush()) {
    boundRadius = (GetBrush()).GetBoundRadius();
    return boundRadius;
  }

  if (IsItemNode()) {
    boundRadius = (GetItemWireFrame()).GetBoundRadius();
    return boundRadius;
  }

  Object *child;
  Vector3d bound;
  float m1, m2, tmp;

  boundRadius = 0.0f;

  IterLinkList<Object> iter(children);

  iter.Reset();
  while (!iter.IsDone()) {
    child = iter.GetNext();
    child->GetScale(bound);
    m1 = Max(bound.GetX(), bound.GetY());
    m1 = Max(m1, bound.GetZ());
    child->GetPosition(bound);
    m2 = bound.GetMag();
    tmp = child->GetBoundRadius() * m1 + m2;
    if (tmp > boundRadius)
      boundRadius = tmp;
  }

  if (bubble && !(IsRoot()))
    (GetParent()).SetBoundRadius();

  return boundRadius;
}

bool
Object::HitObject(const Line &ray, float rangeBegin,
                  float rangeEnd, float &depth) const {
  static const Vector3d origin(0.0f, 0.0f, 0.0f);
  Vector3d intrsctPt;
  Plane perp(ray.GetSlope(), origin);
  bool intrsct;

  intrsct = perp.Intersect(ray, intrsctPt);
  ASSERT(intrsct);  // Sanity Check.
  if (intrsctPt.GetMag() > GetBoundRadius() + 0.1)
    return false;

  if (HasBrush())
    return (GetBrush().HitGeometry(ray, rangeBegin, rangeEnd, depth));

  if (IsItemNode())
    return (GetItemWireFrame().HitGeometry(ray, rangeBegin, rangeEnd, depth));

  // Check children.
  Object *chld;
  Line newRay;
  Matrix44 trans;
  // Matrix41 m;
  Vector3d p0, v0, v1;
  float zTmp;
  IterLinkList<Object> iter(children);

  intrsct = false;
  iter.Reset();
  while(!iter.IsDone()) {
    chld = iter.GetNext();
    chld->CalInvTransSpaceMatrix(trans.SetIdentity());

    // ((m.FillMatrix(ray.GetPoint())).Transform(trans)).FillVector(p0);
    p0 = ray.GetPoint();
    trans.Transform(p0);

    // (chld->GetPosition(v0)).AddVector(ray.GetSlope());
    // (m.Transform(trans, v0.GetX(), v0.GetY(), v0.GetZ())).FillVector(v1);
    v1.AddVector(chld->GetPosition(), ray.GetSlope());
    trans.Transform(v1);

    newRay.NewLine(p0, v1);
    if (chld->HitObject(newRay, rangeBegin, rangeEnd, zTmp)) {
      if (!intrsct || zTmp < depth)
        depth = zTmp;
      intrsct = true;
    }
  }
  return intrsct;
}

//===== CSG Subtraction =====
bool
Object::CSGSubtract(const LinkList<ObjectPtr> &cutters) {
#ifndef NDEBUG
  // Sanity.
  ASSERT(!IsRoot());
  ASSERT(!IsItemNode());
  ASSERT(!IsLeaf() || HasBrush());
  ASSERT(cutters.NumOfElm() > 0);
  Object *oPtr;
  for(int i = 0; i < cutters.NumOfElm(); i++) {
    oPtr = cutters[i].GetPtr();
    ASSERT(oPtr->GetParentPtr() == GetParentPtr());
	ASSERT(oPtr != this);
    // ASSERT(!oPtr->IsItemNode());
  }
#endif

  bool modified = false;

  // Set parent's matrix.
  Matrix44 t;
  GetParent().SetTransformation(t.SetIdentity());

  // Iter through cutters and perform subtraction.
  Object *cutterPtr;
  IterLinkList<ObjectPtr> iterCutters(cutters);
  iterCutters.Reset();
  while (!iterCutters.IsDone()) {
    cutterPtr = (iterCutters.GetNext())->GetPtr();

	if (cutterPtr->IsItemNode())
		continue;

    if (CSGObjNodeSubtract(*cutterPtr, true))
      modified = true;

    if (IsLeaf() && !HasBrush())  {
      // Hmm... let caller handle deletion.
      // Nothing left.... Delete self.
      // GetParent().RemoveChild(*this, false);
      // delete this;
      return modified;
	}
  }

  // Set bound radius for all nodes in tree.
  PostOrderApply(ResetBoundRadius);

  return modified;
}

bool
Object::CSGObjNodeSubtract(Object &cutter, bool resetBound) {
  // Sanity.
  ASSERT(!IsRoot());
  ASSERT(!IsItemNode());
  ASSERT(this != &cutter);
  ASSERT(!IsMyAncestor(cutter));

  Matrix44 t;
  bool modified;

  CalInvTransSpaceMatrix(t.SetIdentity());
  t.Multiply(GetParent().GetTransformation());
  
  if (GetNumChildren() > 0) {
    ASSERT(!HasBrush());

    SetTransformation(t);

    Object *objPtr;
    IterLinkList<Object> iter(children);

	modified = false;
    iter.Reset();
    while (!iter.IsDone()) {
      objPtr = iter.GetNext();

      // Skip entities.
      if (objPtr->IsItemNode())
        continue;

      if (objPtr->CSGObjNodeSubtract(cutter))
        modified = true;

      if (objPtr->IsLeaf() && !objPtr->HasBrush())
        delete &(RemoveChild(*objPtr));
	}

    return modified;
  }

  // Sanity.
  ASSERT(HasBrush());

  // Traverse the cutter tree now.
  cutter.CalTransSpaceMatrix(t);
  cutter.SetTransformation(t);
  modified = cutter.CSGTraverseCutter(*this);

  // Case 1: Brush was deleted.  Nothing left. 
  //  Do nothing. Let parent node handle deletion of current node.
  if (IsLeaf() && !HasBrush())
    return modified;

  // Case 2: One brush left.  Same obj node.
  if (HasBrush()) {
    ASSERT(IsLeaf());

    // Re-adjust position and orientation of obj.
    Vector3d posVec, sVec;
    SphrVector oriVec;

    CalTransSpaceMatrix(t.SetIdentity());
    brush->Transform(t);
    brush->CenterGeometry(posVec);
    SetPosition(posVec);
    SetOrientation(oriVec.NewVector(0.0f, 0.0f, 0.0f));
    SetScale(sVec.NewVector(1.0f, 1.0f, 1.0f));

    return modified;
  }

  // Case 3: Sub pieces are introduced.
  //  Float all the new sub pieces to the parent node
  //   if no model entity is attached.
  //  Let parent node handle deletion of current node.
  ASSERT(GetNumChildren() > 0);
  ASSERT(!HasBrush());

  if (!IsModelNode()) {
    if (resetBound)
      PostOrderApply(ResetBoundRadius);
    FloatChildren(false);
  }

  return modified;
}

bool
Object::CSGTraverseCutter(Object &cuttee) {
  // Sanity.
  ASSERT(!IsItemNode());
  ASSERT(this != &cuttee);
  ASSERT(!cuttee.IsMyAncestor(*this));
  ASSERT(cuttee.HasBrush());

  if (HasBrush()) {
    return CSGBrushSubtract(cuttee, *this);
  }

  IterLinkList<Object> iter(children);
  Object *objPtr;
  Matrix44 t;
  bool modified = false;

  iter.Reset();
  while (!iter.IsDone()) {
    objPtr = iter.GetNext();
    if (objPtr->IsItemNode())
      continue;
    t = GetTransformation();
    objPtr->CalTransSpaceMatrix(t);
    objPtr->SetTransformation(t);
    if (objPtr->CSGTraverseCutter(cuttee))
      modified = true;
    if (!cuttee.HasBrush())
      break;
  }
  return modified;
}

bool
Object::CSGBrushSubtract(Object &cuttee, Object &cutter) {
  // PreCondition.
  ASSERT(cuttee.HasBrush());
  ASSERT(cutter.HasBrush());
  ASSERT(!cuttee.IsRoot());

  Geometry cutterBrush(cutter.GetBrush());
  LinkList<Geometry> *brushListPtr;
  Object *newObj;
  Vector3d posVec;
  const Vector3d sVec(1.0f, 1.0f, 1.0f);
  const SphrVector oriVec(0.0f, 0.0f, 0.0f);

  cutterBrush.Transform(cutter.GetTransformation());
  brushListPtr = Geometry::CSGSubtract(cuttee.GetBrush(), cutterBrush);

  if (brushListPtr == NULL)  // No modification was done.
    return false;

  delete cuttee.brush;
  cuttee.brush = NULL;

  if (brushListPtr->NumOfElm() == 1) {
    cuttee.brush = &(brushListPtr->RemoveNode(0));
  }
  else {
    while (brushListPtr->NumOfElm() > 0) {
      newObj = new Object();
      newObj->brush = &(brushListPtr->RemoveNode(0));
      cuttee.AddChild(*newObj, false);
    }
  }

  return true;
}

//===== CSG Intersect =====
Object *
Object::CSGIntersect(const LinkList<ObjectPtr> &objPtrs) const {
  Object *rtnVal;

  Geometry *pCombined = CSGSelfIntersect(&objPtrs);

  if (pCombined == NULL)
    rtnVal = NULL;
  else {
    Vector3d posVec;
    pCombined->CenterGeometry(posVec);

    rtnVal = new Object;
    rtnVal->SetBrush(pCombined);
    rtnVal->SetPosition(posVec);
  }
  
  return rtnVal;
}


Geometry *
Object::CSGSelfIntersect(const LinkList<ObjectPtr> *pObjsPtrs) const {

#ifndef NDEBUG
  if (pObjsPtrs) {
    for(int k = 0; k < pObjsPtrs->NumOfElm(); k++) {
      ASSERT((*pObjsPtrs)[k].GetPtr()->GetParentPtr() == this);
    }
  }
#endif

  int numListElm = 0;
  Geometry *rtnVal = NULL;

  if (pObjsPtrs == NULL) {
    if (IsItemNode() || (IsLeaf() && !HasBrush()))
      return NULL;
    if (HasBrush())
      rtnVal = new Geometry(GetBrush());    
    else
      numListElm = GetNumChildren();
  }
  else
    numListElm = pObjsPtrs->NumOfElm();

  if (numListElm > 0) {
    // Sanity.
    ASSERT(rtnVal == NULL);

    bool failed = false;
    Geometry *pNewBrush, *pHold, *pCombined = NULL;
    Object *pObj;

    for(int i = 0; i < numListElm; i++) {
      if (pObjsPtrs != NULL)
        pObj = (*pObjsPtrs)[i].GetPtr();
      else
        pObj = &(children[i]);

      pNewBrush = pObj->CSGSelfIntersect(NULL);

      if (pNewBrush == NULL) {
        failed = true;
        break;
      }

      if (pCombined == NULL) {
        pCombined = pNewBrush;
        pNewBrush = NULL;
        continue;
      }

      pHold = Geometry::CSGIntersect(*pNewBrush, *pCombined);
      if (pHold == NULL) {
        failed = true;
        break;
      }

      delete pNewBrush;
      pNewBrush = NULL;
      delete pCombined;
      pCombined = pHold;
    }

    if (pNewBrush)
      delete pNewBrush;

    if (failed) {
      if (pCombined)
        delete pCombined;
    }
    else {
      ASSERT(pCombined != NULL);
      rtnVal = pCombined;
    }
  }

  if (rtnVal != NULL) {
    Matrix44 t;
    CalTransSpaceMatrix(t.SetIdentity());
    rtnVal->Transform(t);
  }

  return rtnVal;
}

//===== Plane Clip =====
bool
Object::PlaneClip(const Plane &clipPlane, bool bubble) {

  if (IsItemNode())
    return false;

  Matrix44 m;
  CalInvTransSpaceMatrix(m.SetIdentity());
  Plane cPlane(clipPlane);
  m.Transform(cPlane);

  if (HasBrush()) {
    Geometry *pNewBrush = GetBrush().PlaneClip(cPlane);
    if (pNewBrush == &(GetBrush()))
      return false;
    else if (pNewBrush != NULL) {
      delete brush;
      brush = pNewBrush;
      SetBoundRadius(bubble);
      return true;
    }
    else { // pNewBrush == NULL
      // Let caller handle deletion of object node.
      delete brush;
      brush = NULL;
      return true;
    }
  }

  ASSERT(GetNumChildren() > 0);

  bool rtnVal = false;

  Object *pChild;
  IterLinkList<Object> *pIter = &(GetIterChildren());
  pIter->Reset();
  while (!pIter->IsDone()) {
    pChild = pIter->GetNext();
    if (pChild->PlaneClip(cPlane, false)) {
      rtnVal = true;
      if (!pChild->IsItemNode() && !pChild->HasBrush() &&
          pChild->GetNumChildren() == 0) {
        delete &(RemoveChild(*pChild, false));
      }
    }
  }

  if (rtnVal)
    SetBoundRadius(bubble);

  return rtnVal;
}

//===== Object: load/save geometries =====

Object *
Object::CreateObjFromFile(const char *fName,
                          int (*ErrorPromptFunc)(const char *message),
                          bool (*ProgressFunc)(int percent)) {
  Object *rtnObj = new Object;

  if (!rtnObj->LoadObjFile(fName, ErrorPromptFunc, ProgressFunc)) {
    delete rtnObj;
    return NULL;
  }
  return rtnObj;
}

#define PROGRESS_BRUSH	4
#define PROGRESS_ENTITY	1
#define PROGRESS_OBJECT	1

bool
Object::LoadObjFile(const char *fName,
                    int (*ErrorPromptFunc)(const char *message),
                    bool (*ProgressFunc)(int percent)) {
  // PreCondition.
  ASSERT(fName != NULL);

  bool rtnVal;
  LFile *inFile = new LFile;
  Geometry *holdPtr;
  Entity *holdEntPtr;
  LinkList<GeometryPtr> shapes;
  LinkList<EntityPtr> entities;

  int brushNum, entNum, objNum;
  int numBrushError, numEntError;
  int pDone, pTotal;
  int i, errorVal;
  bool ignore;
  char tmpError[80];

  if (!inFile->Open(fName)) {
    // Clear self.
    DelObj(false);
    return false;
  }
  inFile->ResetLineNumber();

  brushNum = entNum = objNum = 0;
  // if (strncmp(inFile->GetNextLine(), "// QOOLE", 8) != 0)
  //   goto ERROR_LOAD_OBJFILE;
  while (sscanf(inFile->GetNextLine(), "// Brushes: %d", &brushNum) != 1) {
    if (inFile->EndOfFile())
      goto ERROR_LOAD_OBJFILE;
  }
  while (sscanf(inFile->GetNextLine(), "// Entities: %d", &entNum) != 1) {
    if (inFile->EndOfFile())
      goto ERROR_LOAD_OBJFILE;
  }
  while (sscanf(inFile->GetNextLine(), "// Objects: %d", &objNum) != 1) {
    if (inFile->EndOfFile())
      goto ERROR_LOAD_OBJFILE;
  }

  // Clean up self.
  DelObj(false);

  ignore = false;
  if (ProgressFunc) {
    pDone = 0;
    pTotal = brushNum * PROGRESS_BRUSH
			+ entNum * PROGRESS_ENTITY
			+ objNum * PROGRESS_OBJECT;
    if(!(*ProgressFunc)(pDone)) {
      return false;
	}
  }

  // Load all the brushes in first.  Store them in a list.
  numBrushError = 0;
  for(i = 0; i < brushNum; i++) {
    // Update progress meter.
    if (ProgressFunc != NULL) {
      pDone += PROGRESS_BRUSH;
      (*ProgressFunc)((int) (pDone * 100.0f / pTotal));
    }

    if ((holdPtr = Geometry::ReadBoundGeometry(inFile, i)) == NULL) {
      numBrushError++;
      if (ErrorPromptFunc != NULL && !ignore) {
        sprintf(tmpError,
                "Error loading brush before line %d.\n"
                "Ignore and continue?", inFile->GetLineNumber());
        errorVal = ErrorPromptFunc(tmpError);
        if (errorVal == -1) { // Abort Load.
          rtnVal = false;
          goto RTN_LOADOBJFILE;
        }
        else if (errorVal == 1) // Ignore Errors.
          ignore = true;
      }
    }
    shapes.AppendNode(*(new GeometryPtr(holdPtr, i)));
  }

  // Load all the entities in.  Store in list.
  numEntError = 0;
  for(i = 0; i < entNum; i++) {
    // Update progress meter.
    if (ProgressFunc != NULL) {
      pDone += PROGRESS_ENTITY;
      (*ProgressFunc)((int) (pDone * 100.0f / pTotal));
    }

    if ((holdEntPtr = Entity::ReadEntity(inFile, i)) == NULL) {
      numEntError++;
      if (ErrorPromptFunc != NULL && !ignore) {
        sprintf(tmpError,
                "Error loading entity before line %d.\n"
                "Ignore and continue?", inFile->GetLineNumber());
        errorVal = ErrorPromptFunc(tmpError);
        if (errorVal == -1) {  // Abort Load
          rtnVal = false;
          goto RTN_LOADOBJFILE;
        }
        else if (errorVal == 1) // Ingore Errors.
          ignore = true;
      }
    }
    entities.AppendNode(*(new EntityPtr(holdEntPtr, i)));
  }

  // Now load object grouping information.
  if (LoadObject(inFile, 0, shapes, entities, 
                 pDone, pTotal, ProgressFunc) != 1)
    goto ERROR_LOAD_OBJFILE;

  // Set bound radius for all nodes.
  PostOrderApply(ResetBoundRadius);

  rtnVal = true;
  goto RTN_LOADOBJFILE;

ERROR_LOAD_OBJFILE:
  LError("Parse error before line # %d\n", inFile->GetLineNumber());

  // Clear self.
  DelObj(false);

  rtnVal = false;

RTN_LOADOBJFILE:
  if (inFile != NULL) {
    inFile->Close();
    delete inFile;
  }

  // Clean up any left over geometries.
  while (shapes.NumOfElm() > 0) {
    if (shapes[0].GetPtr() != NULL)
      delete shapes[0].GetPtr();
    delete &(shapes.RemoveNode(0));
  }

  // Clean up any left over entities.
  while (entities.NumOfElm() > 0) {
    if (entities[0].GetPtr() != NULL)
      delete entities[0].GetPtr();
    delete &(entities.RemoveNode(0));
  }

  if (rtnVal && (numBrushError > 0 || numEntError > 0))
    LError("Ignored %d bad brushes and %d bad entities.",
            numBrushError, numEntError);

  return rtnVal;
}


bool
Object::SaveObjFile(const char *fName, bool append,
                    bool (*ProgressFunc)(int percent)) {
  // PreCondition.
  ASSERT(fName != NULL);

  FILE *fOutPtr;
  int brushNum, entNum, objNum;
  int brushSaved, entSaved, objSaved;
  int pDone, pTotal;

  if ((fOutPtr = fopen(fName, (append ? "at" : "wt"))) == NULL)
    return false;

  brushNum = CountBrushes();
  entNum = CountEntities();
  objNum = CountNodes();

  // fprintf(fOutPtr, "// QOOLE\n");
  fprintf(fOutPtr, "// Brushes: %d\n", brushNum);
  fprintf(fOutPtr, "// Entities: %d\n", entNum);
  fprintf(fOutPtr, "// Objects: %d\n", objNum);
  fprintf(fOutPtr, "\n");

  if (ProgressFunc) {
    pDone = 0;
    pTotal = brushNum + entNum + objNum;
    (*ProgressFunc)(pDone);
  }

  // Remember position.
  Vector3d origPosition = TransSpace::GetPosition();
  TransSpace::SetPosition(Vector3d::origVec);

  brushSaved = entSaved = objSaved = 0;
  AssignObjIds(brushSaved, entSaved, objSaved);  
  brushSaved = SaveBrushes(fOutPtr, pDone, pTotal, ProgressFunc);
  entSaved = SaveEntities(fOutPtr, pDone, pTotal, ProgressFunc);
  objSaved = SaveObject(fOutPtr, pDone, pTotal, ProgressFunc);

  // Restore position.
  TransSpace::SetPosition(origPosition);

  // Sanity Check.
  ASSERT(brushSaved == brushNum);
  ASSERT(entSaved == entNum);
  ASSERT(objSaved == objNum);

  if (fOutPtr != NULL)
    fclose(fOutPtr);

  return true;
}

int
Object::LoadObject(LFile *inFile, int objNum,
                   LinkList<GeometryPtr> &shapes,
                   LinkList<EntityPtr> &entities,
                   int &pDone, int &pTotal,
                   bool (*ProgressFunc)(int percent)) {
  int rtnVal;
  char srch[20];
  float f1, f2, f3, f4, f5, f6;
  int i1;
  const char *s;
  Vector3d inVec, inVec2;
  SphrVector inSVec1;
  Object *holdPtr;
  IterLinkList<Object> iter(children);

  if (ProgressFunc) {
    pDone += PROGRESS_OBJECT;
    (*ProgressFunc)((int) (pDone * 100.0f / pTotal));
  }

  // Clear Self; 
  DelObj(false, false);

  // Find object in file.
  sprintf(srch, "{ // Object %d", objNum);  
  if (inFile->Search(srch) == NULL)
    return false;

  s = strchr(inFile->GetLine(), ':');
  SetObjName(s);

  // Parse until the end of object section.
  while (1) {
    if (inFile->GetNextLine() == NULL)
      goto ERROR_LOAD_OBJECT;
    else if (strncmp(inFile->GetLine(), "}", 1) == 0)
      break;
    else if (strncmp(inFile->GetLine(), "Position: ", 10) == 0) {
      if (sscanf(inFile->GetLine() + 10, "(%f, %f, %f)", &f1, &f2, &f3) != 3)
        goto ERROR_LOAD_OBJECT;
      TransSpace::SetPosition(inVec.NewVector(f1, f2, f3));
    }
    else if (strncmp(inFile->GetLine(), "Orientation (deg): ", 19) == 0) {
      if (sscanf(inFile->GetLine() + 19, "(%f, %f, %f)", &f1, &f2, &f3) != 3)
        goto ERROR_LOAD_OBJECT;
      f1 = DEG2RAD(f1);
      f2 = DEG2RAD(f2);
      f3 = DEG2RAD(f3);
      TransSpace::SetOrientation(inSVec1.NewVector(f1, f2, f3));
    }
    else if (strncmp(inFile->GetLine(), "Orientation (rad): ", 19) == 0) {
      if (sscanf(inFile->GetLine() + 19, "(%f, %f, %f)", &f1, &f2, &f3) != 3)
        goto ERROR_LOAD_OBJECT;
      TransSpace::SetOrientation(inSVec1.NewVector(f1, f2, f3));
    }
    else if (strncmp(inFile->GetLine(), "Orientation (f,t):", 19) == 0) {
      if (sscanf(inFile->GetLine() + 19, "(%f, %f, %f), (%f, %f, %f)",
                 &f1, &f2, &f3, &f4, &f5, &f6) != 6)
        goto ERROR_LOAD_OBJECT;
      TransSpace::SetOrientation(inVec.NewVector(f1, f2, f3),
                                 inVec2.NewVector(f4, f5, f6));
    }
    else if (strncmp(inFile->GetLine(), "Orientation (rad):", 19) == 0) {
      if (sscanf(inFile->GetLine() + 19, "(%f, %f, %f)", &f1, &f2, &f3) != 3)
        goto ERROR_LOAD_OBJECT;
      TransSpace::SetOrientation(inSVec1.NewVector(f1, f2, f3));
    }
    else if (strncmp(inFile->GetLine(), "Scale: ", 7) == 0) {
      if (sscanf(inFile->GetLine() + 7, "(%f, %f, %f)", &f1, &f2, &f3) != 3)
        goto ERROR_LOAD_OBJECT;
      TransSpace::SetScale(inVec.NewVector(f1, f2, f3));
    }
    else if (strncmp(inFile->GetLine(), "Brush:", 6) == 0) {
      if (!IsLeaf() || HasBrush())
        goto ERROR_LOAD_OBJECT;
      if (sscanf(inFile->GetLine() + 6, "%d", &i1) != 1)
        goto ERROR_LOAD_OBJECT;
       if (i1 < 0 || shapes[0].GetRec() != i1)
        goto ERROR_LOAD_OBJECT;
      brush = shapes[0].GetPtr();
      delete &(shapes.RemoveNode(0));
      if (brush == NULL)
        return 0;
    }
    else if (strncmp(inFile->GetLine(), "Entity:", 7) == 0) {
      if (sscanf(inFile->GetLine() + 7, "%d", &i1) != 1)
        goto ERROR_LOAD_OBJECT;
      if (HasEntity() || i1 < 0 || entities[0].GetRec() != i1)
        goto ERROR_LOAD_OBJECT;
	  SetEntity(entities[0].GetPtr(), false);
      delete &(entities.RemoveNode(0));
      if (entityPtr == NULL)
        return 0;
    }
    else if (strncmp(inFile->GetLine(), "Objects:", 8) == 0) {
      if (HasBrush() || IsItemNode())
        goto ERROR_LOAD_OBJECT;
      s = inFile->GetLine() + 8;
      while (sscanf(s, "%d", &i1) == 1) {
        holdPtr = new Object;
        holdPtr->objID = i1;
        AddChild(*holdPtr, false);
        if ((s = strchr(s, ',')) == NULL)
          break;
        s++;
      }
    }
  }

  // Load in all sub objects.
  iter.Reset();
  while (!iter.IsDone()) {
    holdPtr = iter.GetNext();
    rtnVal = holdPtr->LoadObject(inFile, holdPtr->objID, shapes, entities,
                                 pDone, pTotal, ProgressFunc);
    if (rtnVal == 0) {
      RemoveChild(*holdPtr, false);
      delete holdPtr;
    }
    else if (rtnVal == -1) {
      goto ERROR_LOAD_OBJECT;
    }
  }

  // Check for empty nodes.
  if (IsLeaf() && !(HasBrush() || IsItemNode()))
    rtnVal = 0;
  else
    rtnVal = 1;

  goto RTN_LOAD_OBJECT;

ERROR_LOAD_OBJECT:
  rtnVal = -1;
  while (children.NumOfElm() > 0)
    delete &(RemoveChild(GetChild(0), false));

RTN_LOAD_OBJECT:
  return rtnVal;
}

void
Object::AssignObjIds(int &brushCnt, int &entCnt, int &objCnt) {
  // PreCondition.
  ASSERT(brushCnt >=0 && entCnt >= 0 && objCnt >= 0);

  brushID = (HasBrush() ? brushCnt++ : -1);
  entID = (HasEntity() ? entCnt++ : -1);
  objID = objCnt++;

  IterLinkList<Object> iter(children);

  // Now traverse the children.
  iter.Reset();
  while (!iter.IsDone()) {
    (iter.GetNext())->AssignObjIds(brushCnt, entCnt, objCnt);
  }
}

int
Object::SaveObject(FILE *outFile, int &pDone, int &pTotal, 
                   bool (*ProgressFunc)(int percent)) {
  // Sanity Check.
  ASSERT(IsLeaf() || (!HasBrush() && !IsItemNode()));

  int i, id, rtnVal;
  Vector3d vec1;
  SphrVector sVec1;
  IterLinkList<Object> iter(children);

  fprintf(outFile, "{ // Object %d : %s\n", objID, GetObjName());
  GetPosition(vec1);
  fprintf(outFile, "  Position: (%s,", FormatFloat(vec1.GetX()));
  fprintf(outFile, " %s,", FormatFloat(vec1.GetY()));
  fprintf(outFile, " %s)\n", FormatFloat(vec1.GetZ()));
  GetOrientation(sVec1);
  fprintf(outFile, "  Orientation (rad): (%s,", FormatFloat(sVec1.GetYaw()));
  fprintf(outFile, " %s,", FormatFloat(sVec1.GetPitch()));
  fprintf(outFile, " %s)\n", FormatFloat(sVec1.GetRoll()));
  GetScale(vec1);
  fprintf(outFile, "  Scale: (%s,", FormatFloat(vec1.GetX()));
  fprintf(outFile, " %s,", FormatFloat(vec1.GetY()));
  fprintf(outFile, " %s)\n", FormatFloat(vec1.GetZ()));

  if (GetNumChildren() > 0) {
    iter.Reset();
    for(i = 0; !iter.IsDone(); i++) {
      ASSERT(i < GetNumChildren());  // Sanity.
      id = (iter.GetNext())->objID;
      ASSERT(id >= 0);  // Sanity.
      if ((i % 10) == 0) {
        if (i != 0)
          fprintf(outFile, "\n");
        fprintf(outFile, "  Objects: %d", id);
      }
      else
        fprintf(outFile, ", %d", id);
    }
    fprintf(outFile, "\n");
  }

  if (HasBrush()) {
    ASSERT(!IsItemNode());
    ASSERT(brushID >= 0);
    fprintf(outFile, "  Brush: %d\n", brushID);
  }

  if (HasEntity()) {
    // Sanity Check.
    ASSERT(entID >= 0);
    fprintf(outFile, "  Entity: %d\n", entID);
  }

  fprintf(outFile, "} // Object %d\n", objID);
  fprintf(outFile, "\n");

  if (ProgressFunc) {
    pDone += 1;
    (*ProgressFunc)((int) (pDone * 100.0f / pTotal));
  }

  // Now traverse all children.
  rtnVal = 1;
  iter.Reset();
  while (!iter.IsDone()) {
    rtnVal += (iter.GetNext())->
        SaveObject(outFile, pDone, pTotal, ProgressFunc);
  }

  return rtnVal;
}

int
Object::SaveBrushes(FILE *outFile, int &pDone, int &pTotal,
                    bool (*ProgressFunc)(int percent)) {
  // Sanity Check
  ASSERT(IsLeaf() || (!HasBrush() && !IsItemNode()));

  IterLinkList<Object> iter(children);
  int rtnVal = 0;

  // Write out the current brush.
  if (HasBrush()) {
    Matrix44 t;
    t.SetIdentity();
    ASSERT(brushID >= 0);
    Geometry::WriteBoundGeometry(outFile, *brush, brushID, t);
    fprintf(outFile, "\n");
    rtnVal++;

    if (ProgressFunc) {
      pDone += 1;
      (*ProgressFunc)((int) (pDone * 100.0f / pTotal));
	}
  }

  // Traverse All Children.
  iter.Reset();
  while (!iter.IsDone()) {
    rtnVal += (iter.GetNext())->
        SaveBrushes(outFile, pDone, pTotal, ProgressFunc);
  }

  return rtnVal;
}

int
Object::SaveEntities(FILE *outFile, int &pDone, int &pTotal,
                     bool (*ProgressFunc)(int percent)) {
  // Sanity Check.
  ASSERT(IsLeaf() || (!HasBrush() && !IsItemNode()));

  IterLinkList<Object> iter(children);
  int rtnVal = 0;

  // Write out the current entity.
  if (HasEntity()) {
    // Sanity Check.
    ASSERT(!IsItemNode() || IsLeaf());
    ASSERT(entID >= 0);
    Entity::WriteEntity(outFile, *(GetEntityPtr()), entID);
    fprintf(outFile, "\n");
    rtnVal++;

    if (ProgressFunc) {
      pDone += 1;
      (*ProgressFunc)((int) (pDone * 100.0f / pTotal));
	}
  }

  // Now traverse the children.
  iter.Reset();
  while (!iter.IsDone()) {
    rtnVal += (iter.GetNext())->
        SaveEntities(outFile, pDone, pTotal, ProgressFunc);
  }

  return rtnVal;
}

//========== Testing ==========
#if 0
Object *
Object::LoadWFGeom(const GeomWFInfo &infoRec) {
  Object *rtnVal = NULL;
  Geometry *geomPtr = NULL;

  // geomPtr = Geometry::ReadGeomFile(fName);
  geomPtr = Geometry::NewWFGeom(infoRec);
  if (geomPtr != NULL) {
    rtnVal = new Object;
    rtnVal->geomPtr = geomPtr;
  }
  rtnVal->SetBoundRadius(false);

  return rtnVal;
}
#endif
