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
 * View.cpp
 */

#include "stdafx.h"

#include "view.h"

bool *ent_texd;

//==================== View ====================

bool View::showEntities = true;
int View::manipVertex = -1;
const Edge3d *View::manipEdge = NULL;
const GPolygon *View::manipFace = NULL;

int View::renderColors[VRC_NUMRENDERCOLORS];

const View *View::renderView = NULL;
QDraw *View::renderQDraw = NULL;
bool View::skipRender = false;
bool View::skipSlctObjs = false;
Points3d View::vBuffer;
Point3d View::ptsBuf1[48];
Point3d View::ptsBuf2[48];
Object *View::pCrossHair = NULL;
Object *View::pCrossHairNorth = NULL;
//Object *View::pLeakGeomObj = NULL;
Object *View::pLeakObj = NULL;
bool View::aprxWFRender = false;


View::View(QDraw *pQdraw, Object &viewObject, const Selector &selector) {
  selectorPtr = &selector;

  SetWindowWidth(1);
  SetClipDepth(1000.0f);
  SetNewView(viewObject, selector);

  showEntities = true;
  displayPts = 0;

  qdraw = pQdraw;

  for(int i = 0; i < VRC_NUMRENDERCOLORS; i++)
	  renderColors[i] = i;
}

View::~View(void) {
  ShowCrossHair(false);
}

View &
View::operator=(const View &src) {
  if (this == &src)
    return *this;

  LFatal();

  // ElmNode::operator=(src);
  TransSpace::operator=(src);

  qdraw = src.qdraw;
  viewObj = src.viewObj;
  perspective = src.perspective;
  clipView = src.clipView;
  winWidth = src.winWidth;
  selectorPtr = src.selectorPtr;
  return *this;
}

//===== View Settings =====

void
View::SetNewView(Object &viewObject, const Selector &selector,
                 bool reset, Vector3d *posVec, SphrVector *orientVec,
                 bool perspectiveView, int windowWidth, float viewDepth) {
  // Precondition.
  ASSERT(&viewObject == &(selector.GetScope()));

  viewObj = &viewObject;
  selectorPtr = &selector;

  if (reset) {
    Vector3d pVec(0.0f, 0.0f, 0.0f);
    SphrVector oriVec(0.0f, 0.0f, 0.0f);

    SetPosition(posVec ? *posVec : pVec);
    SetOrientation(orientVec ? *orientVec : oriVec);
    SetPerspective(perspectiveView);
    if (windowWidth != 0)
      SetWindowWidth(windowWidth);
    if (viewDepth > 0.0f)
      SetClipDepth(viewDepth);
  }

  DisplayFaceSelect(false);
}

void
View::SetPerspective(bool perspectiveView) {
  Vector3d cMin = clipView.GetMinClipEdges();
  Vector3d cMax = clipView.GetMaxClipEdges();

  if (perspectiveView && cMin.GetY() < 1.0f)
    cMin.SetY(1.0f);

  perspective = perspectiveView;
  clipView.SetClipVolume(cMin, cMax, perspective);
}

float
View::GetClipDepth(void) const {
  // Sanity
  ASSERT(perspective ||
         (clipView.GetMaxClipEdges()).GetY() ==
          -(clipView.GetMinClipEdges()).GetY());

  return (clipView.GetMaxClipEdges()).GetY();
}

void
View::SetClipDepth(float depth) {
  // Sanity.
  ASSERT(depth >= 1.0 && depth <= 10000.0f);

  Vector3d cMin = clipView.GetMinClipEdges();
  Vector3d cMax = clipView.GetMaxClipEdges();

  cMax.SetY(depth);
  cMin.SetY(perspective ? 1.0f : -depth);
  clipView.SetClipVolume(cMin, cMax, perspective);
}

void
View::SetWindowWidth(int width) {
  Vector3d cMin = clipView.GetMinClipEdges();
  Vector3d cMax = clipView.GetMaxClipEdges();

  winWidth = (float) width;
  cMin.SetX(-winWidth);
  cMin.SetZ(-winWidth);
  cMax.SetX(winWidth);
  cMax.SetZ(winWidth);
  clipView.SetClipVolume(cMin, cMax, perspective);
}

void
View::MapViewSpaceOrigin(float &scrnX, float &scrnY) {
  // Sanity.
  ASSERT(perspective == false);

  Matrix44 trans;
  Vector3d ov(0.0f, 0.0f, 0.0f);

  CalInvTransSpaceMatrix(trans.SetIdentity());
  trans.Transform(ov);
  scrnX = ov.GetX();
  scrnY = ov.GetZ();
}

//===== Object Selection ===== 

Line &
View::SetRay(float scrnX, float scrnY, Line &ray, bool relView) const {
  Vector3d p0, v0;
  Matrix44 trans;

  if (perspective) {
    p0.NewVector(0.0f, 0.0f, 0.0f);
    v0.NewVector(scrnX / winWidth, 1.0f, scrnY / winWidth);
  }
  else {
    p0.NewVector(scrnX, 0.0f, scrnY);
    v0.NewVector(0.0f, 1.0f, 0.0f);
  }

  if (relView) {
    v0.AddVector(p0);
    TransSpace::CalTransSpaceMatrix(trans.SetIdentity());
    trans.Transform(p0);
    trans.Transform(v0);
    v0.SubVector(p0);
  }
  
  return ray.NewLine(p0, v0);
}

bool
View::IsMultiSelectionHit(float scrnX, float scrnY) const {
  Line slctRay;
  float rangeBegin, rangeEnd, z;
  bool ok;
  Object *tmp;

  if (selectorPtr->GetNumMSelectedObjects() == 0)
    return NULL;

  SetRay(scrnX, scrnY, slctRay, false);
  ok = clipView.ClipRay(slctRay, rangeBegin, rangeEnd);
  ASSERT(ok);
  SetRay(scrnX, scrnY, slctRay, true);

  IterLinkList<ObjectPtr> iter(selectorPtr->GetMSelectedObjects());
  iter.Reset();
  while (!iter.IsDone()) {
    tmp = (iter.GetNext())->GetPtr();
    if (selectorPtr->IsSelectionHit(tmp, slctRay, rangeBegin, rangeEnd, z))
		return true;		
  }
  return false;
}

Object *
View::SelectObject(float scrnX, float scrnY, int funcIndex) const {
  Line slctRay;
  float rangeBegin, rangeEnd;
  bool ok;
  Object *rtnVal = NULL;

  SetRay(scrnX, scrnY, slctRay, false);
  ok = clipView.ClipRay(slctRay, rangeBegin, rangeEnd);
  ASSERT(ok);
  SetRay(scrnX, scrnY, slctRay, true);

  if (funcIndex == 0)
    rtnVal = selectorPtr->GetSNextSelect(slctRay, rangeBegin, rangeEnd);
  else if (funcIndex == 1)
    rtnVal = selectorPtr->GetSPrevSelect(slctRay, rangeBegin, rangeEnd);
  else if (funcIndex == 2)
    rtnVal = selectorPtr->GetMNextSelect(slctRay, rangeBegin, rangeEnd);
  else if (funcIndex == 3)
    rtnVal = selectorPtr->GetMPrevSelect(slctRay, rangeBegin, rangeEnd);

  return rtnVal;
}

int
View::DragSelectMultiObjects(float scrnX1, float scrnY1,
                             float scrnX2, float scrnY2,
                             LinkList<ObjectPtr> &selectBuf) const {
  // Set up a clip volume
  Vector3d cMin, cMax;
  Clip clipVol;

  if (perspective) {
    // Hacked the Clip class.
	// Passing in bounding ratios instead of bounding vectors.
    cMin.NewVector(scrnX1 / winWidth,
                   (clipView.GetMinClipEdges()).GetY(),
                   scrnY1 / winWidth);
    cMax.NewVector(scrnX2 / winWidth,
                   (clipView.GetMaxClipEdges()).GetY(),
                   scrnY2 / winWidth);
  }
  else {
    cMin.NewVector(scrnX1, (clipView.GetMinClipEdges()).GetY(), scrnY1);
    cMax.NewVector(scrnX2, (clipView.GetMaxClipEdges()).GetY(), scrnY2);
  }
  clipVol.SetClipVolume(cMin, cMax, perspective);

  // Set up the view transformation.
  Matrix44 trans;
  CalInvTransSpaceMatrix(trans.SetIdentity());
  viewObj->SetTransformation(trans);

  // Iter through children and find those in clip vol.
  Object *pObj;
  IterLinkList<Object> *pIter = &(viewObj->GetIterChildren());
  pIter->Reset();
  while (!pIter->IsDone()) {
    pObj = pIter->GetNext();
    if (clipVol.IsObjectInVolume(*pObj))
      selectBuf.AppendNode(*(new ObjectPtr(pObj)));
  }

  return selectBuf.NumOfElm();
}

//===== Brush Manipulation Selection =====
// #define PTSEL_EPSILON 10

int
View::SelectVertex(float scrnX, float scrnY,
                   float epsilon, Vector3d *pManipDotPos) {
  // Sanity.
  ASSERT(displayPts == 1);

  int i, rtnVal;
  Matrix44 trans;
  float div, ptX, ptY, diff, depth;

  ASSERT(selectorPtr->GetNumMSelectedObjects() == 1);
  Object *objNode = (selectorPtr->GetMSelectedObjects())[0].GetPtr();

  ASSERT(objNode->HasBrush());
  Geometry *brush = &(objNode->GetBrush());

  if (clipView.BoundObject(*objNode) == -1)
    return -1;

  TransSpace::CalInvTransSpaceMatrix(trans.SetIdentity());
  objNode->CalTransSpaceMatrix(trans);
  brush->TransformVertexPts(trans, vBuffer, manipVertex);
  
  // Assume Points3d::DeletePoint() is never used.
  rtnVal = -1;
  depth = (clipView.GetMaxClipEdges()).GetY();
  for(i = 0; i < brush->GetNumVertices(); i++) {
    if (clipView.ClipPoint(vBuffer[i]))
      continue;
    if (!perspective) {
      ptX = vBuffer[i].GetX() - scrnX;
      ptY = vBuffer[i].GetZ() - scrnY;
    }
    else {
      div = winWidth / vBuffer[i].GetY();
      ptX = vBuffer[i].GetX() * div - scrnX;
      ptY = vBuffer[i].GetZ() * div - scrnY;
    }
    diff = (float) sqrt(ptX * ptX + ptY * ptY);
    if (diff <= epsilon && vBuffer[i].GetY() <= depth) {
      depth = vBuffer[i].GetY();
      rtnVal = i;
      if (pManipDotPos != NULL)
        *pManipDotPos = vBuffer[i];
    }
  }

  return rtnVal;
}

const Edge3d *
View::SelectEdge(float scrnX, float scrnY,
                 float epsilon, Vector3d *pManipDotPos) {
  // Sanity.
  ASSERT(displayPts == 2);
  
  int i, index;
  Matrix44 trans;
  float div, ptX, ptY, diff, depth;

  ASSERT(selectorPtr->GetNumMSelectedObjects() == 1);
  Object *objNode = (selectorPtr->GetMSelectedObjects())[0].GetPtr();

  ASSERT(objNode->HasBrush());
  Geometry *brush = &(objNode->GetBrush());

  if (clipView.BoundObject(*objNode) == -1)
    return NULL;

  TransSpace::CalInvTransSpaceMatrix(trans.SetIdentity());
  objNode->CalTransSpaceMatrix(trans);
  brush->TransformEdgePts(trans, vBuffer, manipEdge);
  
  index = -1;
  depth = (clipView.GetMaxClipEdges()).GetY();
  for(i = 0; i < brush->GetNumEdges(); i++) {
    if (clipView.ClipPoint(vBuffer[i]))
      continue;
    if (!perspective) {
      ptX = vBuffer[i].GetX() - scrnX;
      ptY = vBuffer[i].GetZ() - scrnY;
    }
    else {
      div = winWidth / vBuffer[i].GetY();
      ptX = vBuffer[i].GetX() * div - scrnX;
      ptY = vBuffer[i].GetZ() * div - scrnY;
    }
    diff = (float) sqrt(ptX * ptX + ptY * ptY);
    if (diff <= epsilon && vBuffer[i].GetY() <= depth) {
      depth = vBuffer[i].GetY();
      index = i;
      if (pManipDotPos != NULL)
        *pManipDotPos = vBuffer[i];
    }
  }
  // Sanity.
  ASSERT(index >= -1 || index < brush->GetNumEdges());

  return (index == -1 ? NULL : &(brush->GetEdgeList())[index]);
}

const GPolygon *
View::SelectFace(float scrnX, float scrnY,
                 float epsilon, Vector3d *pManipDotPos) {
  // Sanity.
  ASSERT(displayPts == 3);

  int i, index;
  Matrix44 trans;
  float div, ptX, ptY, diff, depth;

  ASSERT(selectorPtr->GetNumMSelectedObjects() == 1);
  Object *objNode = (selectorPtr->GetMSelectedObjects())[0].GetPtr();

  ASSERT(objNode->HasBrush());
  Geometry *brush = &(objNode->GetBrush());

  if (clipView.BoundObject(*objNode) == -1)
    return NULL;

  TransSpace::CalInvTransSpaceMatrix(trans.SetIdentity());
  objNode->CalTransSpaceMatrix(trans);
  brush->TransformFacePts(trans, vBuffer, manipFace);
  
  index = -1;
  depth = (clipView.GetMaxClipEdges()).GetY();
  for(i = 0; i < brush->GetNumFaces(); i++) {
    if (clipView.ClipPoint(vBuffer[i]))
      continue;
    if (!perspective) {
      ptX = vBuffer[i].GetX() - scrnX;
      ptY = vBuffer[i].GetZ() - scrnY;
    }
    else {
      div = winWidth / vBuffer[i].GetY();
      ptX = vBuffer[i].GetX() * div - scrnX;
      ptY = vBuffer[i].GetZ() * div - scrnY;
    }
    diff = (float) sqrt(ptX * ptX + ptY * ptY);
    if (diff <= epsilon && vBuffer[i].GetY() <= depth) {
      depth = vBuffer[i].GetY();
      index = i;
      if (pManipDotPos != NULL)
        *pManipDotPos = vBuffer[i];
    }
  }
  // Sanity.
  ASSERT(index >= -1 || index < brush->GetNumFaces());

  return (index == -1 ? NULL : &(brush->GetFace(index)));
}

void
View::DisplayNoPts(void) {
  displayPts = 0;
  manipVertex = -1;
  manipEdge = NULL;
  manipFace = NULL;
}

void
View::DisplayVertexPts(int vertex) {
  // PreCondition
  // ASSERT(selectorPtr->GetNumMSelectedObjects() == 1);
  // ASSERT((selectorPtr->GetMSelectedObjects())[0].GetPtr()->HasBrush());

  displayPts = 1;
  manipVertex = vertex;
}

void
View::DisplayEdgePts(const Edge3d *edge) {
  // PreCondition
  // ASSERT(selectorPtr->GetNumMSelectedObjects() == 1);
  // ASSERT((selectorPtr->GetMSelectedObjects())[0].GetPtr()->HasBrush());

  displayPts = 2;
  manipEdge = edge;
}

void
View::DisplayFacePts(const GPolygon *face) {
  // PreCondition
  // ASSERT(selectorPtr->GetNumMSelectedObjects() == 1);
  // ASSERT((selectorPtr->GetMSelectedObjects())[0].GetPtr()->HasBrush());

  displayPts = 3;
  manipFace = face;
}

//===== Crosshair =====

bool
View::IsCrossHairShown(void) {
  ASSERT((!pCrossHair && !pCrossHairNorth) || (pCrossHair && pCrossHairNorth));
  return (pCrossHair != NULL);
}

void
View::ShowCrossHair(bool show) {
  // Sanity.
  ASSERT((!pCrossHair && !pCrossHairNorth) || (pCrossHair && pCrossHairNorth));

  Entity *entPtr = NULL;

  if (show && pCrossHair == NULL) {
	  pCrossHair = new Object();
	  entPtr = new Entity("cross1");      
	  pCrossHair->SetEntity(entPtr);
	  pCrossHairNorth = new Object();
      entPtr = new Entity("cross2");
	  pCrossHairNorth->SetEntity(entPtr);
  }
  else if (!show && pCrossHair != NULL) {
    delete pCrossHair;
    pCrossHair = NULL;
    delete pCrossHairNorth;
    pCrossHairNorth = NULL;
  }
}

void
View::SetCrossHairPos(const Vector3d &posVec) {
  // Sanity.
  ASSERT((!pCrossHair && !pCrossHairNorth) || (pCrossHair && pCrossHairNorth));

  if (pCrossHair == NULL)
    return;

  pCrossHair->SetPosition(posVec);
  pCrossHairNorth->SetPosition(posVec);
}

Vector3d
View::GetCrossHairPos(void) {
  // PreCondition.
  ASSERT(pCrossHair && pCrossHairNorth->GetParentPtr());

  return pCrossHair->GetPosition();
}

//===== Leak Geometry =====
const Object *
View::GetLeakObject(void) {
  return pLeakObj;
}

void 
View::SetLeakObject(Object *pNewObj) {
  if (pLeakObj != NULL)
    delete pLeakObj;

  pLeakObj = pNewObj;
}

//===== Entities WireFrame Approximation =====

void
View::ApproxItems(bool approximate) {
  aprxWFRender = approximate;
}

//===== Color Settings =====
int
View::GetColor(int colorIndex) {
  // Sanity.
  ASSERT(colorIndex >= 0 && colorIndex < VRC_NUMRENDERCOLORS);

  return renderColors[colorIndex];
}

int
View::SetColor(int colorIndex, int color) {
  // Sanity.
  ASSERT(colorIndex >= 0 && colorIndex < VRC_NUMRENDERCOLORS);

  int rtnVal = renderColors[colorIndex];
  renderColors[colorIndex] = color;

  return rtnVal;
}

//===== Object Rendering =====

void
View::RenderWireFrame(bool renderWorld, bool renderSelected,
                      bool renderCrossHair) const {
  // Sanity Check.
  ASSERT(viewObj != NULL);
  ASSERT(renderView == NULL);

  renderView = this;

  int oldNormColor, oldEntColor;

  if (renderWorld) {
    // Draw the world first.  Skip selected objects.
    skipSlctObjs = true;
    viewObj->PreOrderCondApply(RenderObjWireFrame);
  }

  // Draw the crosshair.
  if (renderCrossHair && (pCrossHair || pCrossHairNorth)) {
    oldEntColor = SetColor(VRC_ENTITY, GetColor(VRC_CROSSHAIR));

    ASSERT(pCrossHair->GetParentPtr() == NULL);
    pCrossHair->SetParentPtr(viewObj);
    RenderObjWireFrame(*pCrossHair);
    pCrossHair->SetParentPtr(NULL);

    SetColor(VRC_ENTITY, GetColor(VRC_CROSSHAIRNORTH));

    ASSERT(pCrossHairNorth->GetParentPtr() == NULL);
    pCrossHairNorth->SetParentPtr(viewObj);
    RenderObjWireFrame(*pCrossHairNorth);
    pCrossHairNorth->SetParentPtr(NULL);

    SetColor(VRC_ENTITY, oldEntColor);
  }

  // Draw the additional objs. (leak geometry...)
  if (renderWorld && pLeakObj) {
    int oldBrushColor = SetColor(VRC_NORMAL, GetColor(VRC_MANIPDOT));

    ASSERT(pLeakObj->GetParentPtr() == NULL);
    pLeakObj->SetParentPtr(viewObj);
	pLeakObj->PreOrderCondApply(RenderObjWireFrame);
    pLeakObj->SetParentPtr(NULL);
	  
    SetColor(VRC_NORMAL, oldBrushColor);
  }

  // Draw the selected object.
  if (renderSelected) {
    if (!renderWorld) {
      // Need to set viewObj's trans matrix.

      Matrix44 trans;
      CalInvTransSpaceMatrix(trans.SetIdentity());
      viewObj->SetTransformation(trans);

      // What does this do?????
      skipRender = true;
      viewObj->PreOrderCondApply(RenderObjWireFrame);
      skipRender = false;
    }

    // Draw the (multi) selected objects next.
    Object *rObjPtr;
    int numSlctObjs = selectorPtr->GetNumMSelectedObjects();
    skipSlctObjs = false;

    if (numSlctObjs > 0) {
      oldNormColor = SetColor(VRC_NORMAL, GetColor(VRC_SELECT));
      oldEntColor = SetColor(VRC_ENTITY, GetColor(VRC_SELECT));

      IterLinkList<ObjectPtr> iter(selectorPtr->GetMSelectedObjects());
      iter.Reset();
      while (!iter.IsDone()) {
        rObjPtr = (iter.GetNext())->GetPtr();
        rObjPtr->PreOrderCondApply(RenderObjWireFrame);
	  }
      SetColor(VRC_NORMAL, oldNormColor);
      SetColor(VRC_ENTITY, oldEntColor);
    }

	// Draw any manipulation dots and outline faces.
    if (numSlctObjs == 1 && rObjPtr->HasBrush()) {
      if (dispFaceSlct)
        RenderSlctFaceOutline(*rObjPtr);
      if (displayPts != 0)
        DisplayManipPts();
    }

    // Draw the single (hot) selected object last.
    if ((rObjPtr = selectorPtr->GetSSelectedObject()) != NULL) {
      oldNormColor = SetColor(VRC_NORMAL, GetColor(VRC_HOTSELECT));
      oldEntColor = SetColor(VRC_ENTITY, GetColor(VRC_HOTSELECT));
      rObjPtr->PreOrderCondApply(RenderObjWireFrame);
      SetColor(VRC_NORMAL, oldNormColor);
      SetColor(VRC_ENTITY, oldEntColor);
    }
  }

  renderView = NULL;
}

bool
View::RenderObjWireFrame(Object &objNode) {
  Matrix44 trans;
  bool lineClip;
  int bound;

  ASSERT(!objNode.HasBrush() || !objNode.IsItemNode());

  // Check to see if objNode is one of the selected objects.
  if (skipSlctObjs) {
    const Selector *slctPtr = renderView->selectorPtr;

/*
    if (slctPtr->GetNumMSelectedObjects() > 0) {
      IterLinkList<ObjectPtr> oIter(slctPtr->GetMSelectedObjects());
      oIter.Reset();
      while (!oIter.IsDone()) {
        if ((oIter.GetNext())->GetPtr() == &objNode)
          return false;
      }
    }
*/
    if (objNode.IsSelected())
      return false;

    if (slctPtr->GetSSelectedObject() == &objNode)
      return false;
  }

  // Calculate the transformation matrix.
  if (&objNode == renderView->viewObj) {
    renderView->CalInvTransSpaceMatrix(trans.SetIdentity());
    objNode.SetTransformation(trans);
  }
  else if (!objNode.IsItemNode()) {
    ASSERT(!objNode.IsRoot());

    trans = (objNode.GetParent()).GetTransformation();
    objNode.CalTransSpaceMatrix(trans);
    objNode.SetTransformation(trans);
  }
  else {  // Account for item scaling problem inside other objects.
    ASSERT(!objNode.IsRoot());

    if(!renderView->showEntities)
      return false;

    Matrix44 tr;
    SphrVector oriVec;
    Vector3d ov, fv, tv;

    trans = (objNode.GetParent()).GetTransformation();
    objNode.CalTransSpaceMatrix(trans);

    trans.Transform(ov, Vector3d::origVec);
    (trans.Transform(fv, Vector3d::yAxisVec)).SubVector(ov);
    (trans.Transform(tv, Vector3d::zAxisVec)).SubVector(ov);
    tr.SetRotate(oriVec.NewVector(fv, tv));
    (trans.SetTranslation(ov)).Multiply(tr);

    objNode.SetTransformation(trans);
  }
  
  // Boundary check the object.
  bound = (renderView->clipView).BoundObject(objNode);
  if (bound == 1)
    lineClip = false;
  else if (bound == 0)
    lineClip = true;
  else // bound == -1
    return false;

  // Prepare to render wireframe.
  const LinkList<Edge3d> *listPtr;

  if (objNode.HasBrush()) {
    Geometry *brush = &(objNode.GetBrush());
    brush->Transform(objNode.GetTransformation(), vBuffer);
    listPtr = &(brush->GetEdgeList());

	renderView->qdraw->Color(GetColor(VRC_NORMAL));
  }
  else if (objNode.IsItemNode()) {
    const WireFrameGeom *wfg = &(objNode.GetItemWireFrame());
    if (!aprxWFRender || objNode.IsSelected() ||
		&objNode == pCrossHair || &objNode == pCrossHairNorth ||
		&objNode == pLeakObj) {
      wfg->TransformWireFrame(objNode.GetTransformation(), vBuffer);
      listPtr = &(wfg->GetWireFrameEdgeList());
    }
    else {
      wfg->TransformApproxBox(objNode.GetTransformation(), vBuffer);
      listPtr = &(wfg->GetApproxBoxEdgeList());
    }

	renderView->qdraw->Color(GetColor(VRC_ENTITY));
  }
  else {
    return true;
  }

  if (skipRender)
    return false;

  // Render the geometry.
  Point3d pt1, pt2;
  Edge3d *edgePtr;
  IterLinkList<Edge3d> edgeIter(*listPtr);

  for(edgeIter.Reset(); !edgeIter.IsDone(); ) {
    edgePtr = edgeIter.GetNext();
    pt1 = vBuffer[edgePtr->GetStartPoint()];
    pt2 = vBuffer[edgePtr->GetEndPoint()];

    // Clip line.
    if (lineClip && !(renderView->clipView).ClipLine(pt1, pt2))
      continue;

    if (!renderView->perspective) {
		renderView->qdraw->RenderLine(pt1.GetX(), pt1.GetZ(), pt2.GetX(), pt2.GetZ());
    }
    else {
      float div1 = renderView->winWidth / pt1.GetY();
	  float div2 = renderView->winWidth / pt2.GetY();
	  renderView->qdraw->RenderLine(pt1.GetX() * div1, pt1.GetZ() * div1,
		  pt2.GetX() * div2, pt2.GetZ() * div2);
    }
  }

  return true;
}

void
View::RenderSlctFaceOutline(Object &objNode) const {
  // Sanity.
  ASSERT(dispFaceSlct);
  ASSERT(!objNode.IsItemNode());
  ASSERT(objNode.HasBrush());
  ASSERT(GetSelectFace() != NULL);

  bool lineClip;
  int bound;

  // Boundary check the object.
  bound = clipView.BoundObject(objNode);
  if (bound == 1)
    lineClip = false;
  else if (bound == 0)
    lineClip = true;
  else // bound == -1
    return;

  Geometry *brush = &(objNode.GetBrush());
  IterLinkList<Edge3d> eIter((GetSelectFace())->GetEdgeList());
  Point3d pt1, pt2;
  Edge3d *ePtr;

  // Assume objNode has the right transformation from RenderObjWF().
  brush->Transform(objNode.GetTransformation(), vBuffer);

  qdraw->UseColor(GetColor(VRC_FACESELECT));
  eIter.Reset();
  while (!eIter.IsDone()) {
    ePtr = eIter.GetNext();
    pt1 = vBuffer[ePtr->GetStartPoint()];
    pt2 = vBuffer[ePtr->GetEndPoint()];

    // Clip line.
    if (lineClip && !clipView.ClipLine(pt1, pt2))
      continue;

    if (!perspective) {
		qdraw->RenderLine(pt1.GetX(), pt1.GetZ(), pt2.GetX(), pt2.GetZ());
    }
    else {
      float div1 = winWidth / pt1.GetY();
      float div2 = winWidth / pt2.GetY();
	  qdraw->RenderLine(pt1.GetX() * div1, pt1.GetZ() * div1,
                        pt2.GetX() * div2, pt2.GetZ() * div2);
    }
  }
}

void
View::DisplayManipPts(void) const {
  // Sanity.
  ASSERT(displayPts > 0 && displayPts <= 3);

  float div;
  int i, numPts;
  Matrix44 trans;

  ASSERT(selectorPtr->GetNumMSelectedObjects() == 1);
  Object *objNode = (selectorPtr->GetMSelectedObjects())[0].GetPtr();

  ASSERT(objNode->HasBrush());
  Geometry *brush = &(objNode->GetBrush());

  trans = objNode->GetTransformation();
  if (displayPts == 1) {
    brush->TransformVertexPts(trans, vBuffer, manipVertex);
    numPts = (manipVertex != -1 ? 1 : brush->GetNumVertices());
  }
  else if (displayPts == 2) {
    brush->TransformEdgePts(trans, vBuffer, manipEdge);
    numPts = (manipEdge != NULL ? 1 : brush->GetNumEdges());
  }
  else { // displayPts == 3
    brush->TransformFacePts(trans, vBuffer, manipFace);
    numPts = (manipFace != NULL ? 1 : brush->GetNumFaces());
  }

  qdraw->Color(GetColor(VRC_MANIPDOT));

  for(i = 0; i < numPts; i++) {
    if (clipView.ClipPoint(vBuffer[i]))
      continue;
    if (!perspective) {
      qdraw->RenderPoint(vBuffer[i].GetX(), vBuffer[i].GetZ());
    }
    else {
      div = winWidth / vBuffer[i].GetY();
      qdraw->RenderPoint(vBuffer[i].GetX() * div, vBuffer[i].GetZ() * div);
    }
  }
}

//===== Solid Rendering: Z Buffer =====

// Hack.
static renderTMap = false;
static Matrix44 svTrans;

static void
CalSVTrans(Object &objNode) {

  if (objNode.IsRoot()) {
    svTrans.SetIdentity();
  }
  else {
    CalSVTrans(objNode.GetParent());
    objNode.CalTransSpaceMatrix(svTrans);
  }
}

void
View::RenderSolid(bool tmap) const {
  // Sanity Check.
  ASSERT(viewObj != NULL);

  renderView = this;
  renderQDraw = qdraw;
  renderTMap = tmap;

  CalSVTrans(*viewObj);

  Matrix44 viewTrans;
  CalTransSpaceMatrix(viewTrans.SetIdentity());
  svTrans.Multiply(viewTrans);

  viewObj->PreOrderCondApply(RenderObjSolid);
  
  qdraw->ZBufRender();

  if (selectorPtr->GetNumMSelectedObjects() == 1) {
    Object *rObjPtr = (selectorPtr->GetMSelectedObjects())[0].GetPtr();
    if (rObjPtr->HasBrush() && dispFaceSlct)
      RenderSlctFaceOutline(*rObjPtr);
  }

  renderView = NULL;
  renderQDraw = NULL;
}

extern bool *ent_texd;

bool
View::RenderObjSolid(Object &objNode) {
  Matrix44 trans;
  bool polyClip;
  int bound;

//  if(!*ent_texd && renderTMap && objNode.IsItemNode())
//  if(renderTMap && objNode.IsItemNode())
//    return false;

  // Calculate transformation matrix.
  if (&objNode == renderView->viewObj) {
    if (renderTMap) {
      renderView->TransSpace::CalTransSpaceMatrix(trans.SetIdentity());
      objNode.SetInvTransformation(trans);
    }

    renderView->TransSpace::CalInvTransSpaceMatrix(trans.SetIdentity());
    objNode.SetTransformation(trans);
  }
  else if (!objNode.IsItemNode()) {
    if (renderTMap) {
      objNode.CalInvTransSpaceMatrix(trans.SetIdentity());
      trans.Multiply(objNode.GetParent().GetInvTransformation());
      objNode.SetInvTransformation(trans);
    }

    trans = (objNode.GetParent()).GetTransformation();
    objNode.CalTransSpaceMatrix(trans);
    objNode.SetTransformation(trans);
  }
  else {  // Account for item scaling problem inside other objects.
    // Sanity Check.
    ASSERT(!objNode.IsRoot());

    if (!renderView->showEntities)
      return false;

    Matrix44 tr;
    SphrVector oriVec;
    Vector3d ov, fv, tv;

    trans = (objNode.GetParent()).GetTransformation();
    objNode.CalTransSpaceMatrix(trans);

    trans.Transform(ov, Vector3d::origVec);
    (trans.Transform(fv, Vector3d::yAxisVec)).SubVector(ov);
    (trans.Transform(tv, Vector3d::zAxisVec)).SubVector(ov);
    tr.SetRotate(oriVec.NewVector(fv, tv));
    (trans.SetTranslation(ov)).Multiply(tr);

    objNode.SetTransformation(trans);
  }

  // Boundary check the object.
  bound = renderView->clipView.BoundObject(objNode);
  if (bound == 1)
    polyClip = false;
  else if (bound == 0)
    polyClip = true;
  else // bound == -1
    return false;

  // Prepare for rendering.
  IterLinkList<PlaneNode> *planeIter;

  if (objNode.HasBrush()) {
    const Geometry *brush = &(objNode.GetBrush());
    planeIter = new IterLinkList<PlaneNode>(brush->GetPlaneList());
    brush->Transform(trans, vBuffer);
  }
  else if (objNode.IsItemNode()) {
    const WireFrameGeom *wfg = &(objNode.GetItemWireFrame());
    planeIter = new IterLinkList<PlaneNode>(wfg->GetPlaneList());
    wfg->TransformApproxBox(trans, vBuffer);
  }
  else {
    return true;
  }

  // Solid render the brush.
  static const Vector3d oVec(0.0f, 0.0f, 0.0f);
  static Vector3d nVec(0.0f, 1.0f, 0.0f), pVec, cVec;
  static Plane facePlane(nVec, oVec);
  IterLinkList<Edge3d> *edgeIter;
  PlaneNode *pNodePtr;
  Point3d *vPtr, *fvPtr;
  int i, numPts;
  static int color = 0;
  float div;
  ZPoint zpts[3];
  bool lockTex;
  Matrix44 absTrans;

  absTrans.Multiply(svTrans, trans);
  trans.Transform(cVec, oVec);

  planeIter->Reset();
  while (!planeIter->IsDone()) {
    pNodePtr = planeIter->GetNext();
    trans.Transform(nVec, (pNodePtr->GetPlane()).GetNorm());
    nVec.SubVector(cVec);
    trans.Transform(pVec, (pNodePtr->GetPlane()).GetPoint());
    facePlane.NewPlane(nVec, pVec);
    if (facePlane.PointInPlane(oVec) <= 0.0f)
      continue;  // Back face.  Skip.

    // Set Texture/Color
    lockTex = (pNodePtr->GetPolygon()).GetTexturePtr()->IsTexLocked();

    nVec = (pNodePtr->GetPlane()).GetNorm();

    if (!lockTex) {
      absTrans.Transform(pVec, oVec);
      (absTrans.Transform(nVec)).SubVector(pVec);
    }

    bool texd =
      renderQDraw->ZBufSetup(&objNode, (pNodePtr->GetPolygon()).GetTexturePtr(), nVec);

    // Copy the vertices to local buffer.
    numPts = (pNodePtr->GetPolygon()).GetNumSides();
    ASSERT(numPts <= 32);  // Sanity.
    edgeIter = new IterLinkList<Edge3d>(pNodePtr->GetPolygon().GetEdgeList());
    edgeIter->Reset();
    for(fvPtr = ptsBuf1; !edgeIter->IsDone(); fvPtr++)
      *fvPtr = vBuffer[(edgeIter->GetNext())->GetStartPoint()];
    delete edgeIter;

    // Clip Polygon.
    fvPtr = ptsBuf1;
    if (polyClip &&
        renderView->clipView.ClipPolygon(numPts, ptsBuf1, ptsBuf2, &fvPtr) < 3)
      continue;

    ASSERT(numPts <= 48); // Sanity.

    // Break into triangles and render.
    vPtr = fvPtr;

    div = renderView->winWidth / fvPtr->GetY();
    zpts[0].x = fvPtr->GetX() * div;
    zpts[0].y = fvPtr->GetZ() * div;
    zpts[0].z = fvPtr->GetY();

    if (texd) {
      if (lockTex)
        (objNode.GetInvTransformation()).Transform(pVec, *vPtr);
      else
        svTrans.Transform(pVec, *vPtr);

      renderQDraw->ZBufCalcST(pVec.GetX(), pVec.GetY(), pVec.GetZ(), zpts[0].s, zpts[0].t);
    }
    fvPtr++;

    div = renderView->winWidth / fvPtr->GetY();
    zpts[2].x = fvPtr->GetX() * div;
    zpts[2].y = fvPtr->GetZ() * div;
    zpts[2].z = fvPtr->GetY();

    if (texd) {
      if (lockTex)
        (objNode.GetInvTransformation()).Transform(pVec, *fvPtr);
      else
        svTrans.Transform(pVec, *fvPtr);

      renderQDraw->ZBufCalcST(pVec.GetX(), pVec.GetY(), pVec.GetZ(), zpts[2].s, zpts[2].t);
    }
    fvPtr++;

    for(i = 2; i < numPts; i++, fvPtr++) {
      zpts[1].x = zpts[2].x;
      zpts[1].y = zpts[2].y;
      zpts[1].z = zpts[2].z;

      if (!fvPtr->GetY())
        continue;

      div = renderView->winWidth / fvPtr->GetY();
      zpts[2].x = fvPtr->GetX() * div;
      zpts[2].y = fvPtr->GetZ() * div;
      zpts[2].z = fvPtr->GetY();

      if (texd) {
        zpts[1].s = zpts[2].s;
        zpts[1].t = zpts[2].t;

        if (lockTex)
          (objNode.GetInvTransformation()).Transform(pVec, *fvPtr);
        else
          svTrans.Transform(pVec, *fvPtr);

        renderQDraw->ZBufCalcST(pVec.GetX(), pVec.GetY(), pVec.GetZ(), zpts[2].s, zpts[2].t);
	  }

      renderQDraw->ZBufTriangle(zpts);
    }
  }
  delete planeIter;

  return true;
}
