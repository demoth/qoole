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
 * geometry.cpp
 */

#include "stdafx.h"

#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <ctype.h>
#include <string.h>
#include "geometry.h"
#include "game.h"

//==================== Point3d ====================
Point3d::Point3d(float xPos, float yPos, float zPos) :
         Vector3d(xPos, yPos, zPos) {
}

Point3d &
Point3d::NewPos(float xPos, float yPos, float zPos) {
  NewVector(xPos, yPos, zPos);
  return *this;
}

Point3d &
Point3d::MovePoint(float dx, float dy, float dz) {
  Vector3d::NewVector(GetX() + dx, GetY() + dy, GetZ() + dz);
  return *this;
}

float
Point3d::GetDist(Point3d &pt) const {
  return (float) sqrt(GetDistSqr(pt));
}

float
Point3d::GetDistSqr(Point3d &pt) const {
  float dx = (pt.GetX() - GetX());
  float dy = (pt.GetY() - GetY());
  float dz = (pt.GetZ() - GetZ());

  return (dx * dx + dy * dy + dz * dz);
}

//==================== Points3d ====================

Points3d::Points3d(int initSize) {
  // PreConditions.
  ASSERT(initSize > 0);

  numElm = 0;
  arraySize = initSize;
  array = (Point3d **) calloc(arraySize, sizeof(Point3d *));
}

Points3d::Points3d(Points3d &src) {
  numElm = arraySize = 0;
  array = NULL;
  *this = src;
}

Points3d::~Points3d(void) {
  // Sanity Check.
  ASSERT(numElm >= 0 && numElm <= arraySize);
  ASSERT(arraySize > 0);
  ASSERT(array != NULL);

  int i;

  for(i = 0; i < arraySize; i++) {
    if (array[i] != NULL)
      delete array[i];
  }
  free(array);
}

Points3d &
Points3d::operator=(const Points3d &src) {
  int i;

  if (&src == this)
    return *this;

  if (array != NULL) {
    for(i = 0; i < arraySize; i++) {
      if (array[i] != NULL)
        delete array[i];
    }
    free(array);
  }

  numElm = src.numElm;
  arraySize = src.arraySize;
  array = (Point3d **) calloc(arraySize, sizeof(Point3d *));
  for(i = 0; i < arraySize; i++)
    array[i] = (src.array[i] == NULL ? NULL : new Point3d(*(src.array[i])));

  // PostConditions.
  ASSERT(numElm >= 0 && numElm <= arraySize);
  ASSERT(arraySize > 0);
  ASSERT(array != NULL);

  return *this;
}

int
Points3d::AddPoint(const Point3d &point) {
  // Sanity Check.
  ASSERT(array != NULL);
  ASSERT(numElm >= 0);
  ASSERT(arraySize > 0);

  int i, insertPos;

  if (numElm == arraySize) {  // Need more room.
    arraySize += 10;
    array = (Point3d **) realloc(array, sizeof(Point3d *) * arraySize);
    for(i = numElm; i < arraySize; i++)
      array[i] = NULL;

    insertPos = numElm;
  }
  else {
    for(i = 0; i < arraySize; i++)
      if (array[i] == NULL) {
        insertPos = i;
        break;
      }
  }

  ASSERT(insertPos >= 0 && insertPos < arraySize);  // Sanity check.
  array[insertPos] = new Point3d(point);
  numElm++;
  return insertPos;
}

void
Points3d::DeletePoint(int index) {
  // PreConditions
  ASSERT(index >= 0 && index < arraySize);
  ASSERT(array[index] != NULL);
  // Sanity Check.
  ASSERT(numElm > 0 && numElm <= arraySize);

  delete array[index];
  array[index] = NULL;
  numElm--;
}

Point3d &
Points3d::GetPoint(int index) const {
  // PreCondition.
  ASSERT(index >= 0 && index < arraySize && array[index] != NULL);
  return *(array[index]);
}

int
Points3d::FindPoint(float xPos, float yPos, float zPos,
                    float delta, int startIndex) const {
  // PreConditions.
  ASSERT(delta >= 0.0);
  ASSERT(startIndex >= 0 && startIndex < arraySize);
  // Sanity check.
  ASSERT(numElm >= 0 && numElm <= arraySize);
  ASSERT(arraySize > 0);
  ASSERT(array != NULL);

  int i;

  if (numElm == 0)
    return -1;

  for(i = startIndex; i < arraySize; i++) {
    if (array[i] == NULL)
      continue;
    if (xPos >= array[i]->GetX() - delta &&
        xPos <= array[i]->GetX() + delta &&
        yPos >= array[i]->GetY() - delta &&
        yPos <= array[i]->GetY() + delta &&
        zPos >= array[i]->GetZ() - delta &&
        zPos <= array[i]->GetZ() + delta) {
      return i;
    }
  }

  return -1;
}

int
Points3d::FindPoint(Point3d &pt, float delta, int startIndex) const {
  return FindPoint(pt.GetX(), pt.GetY(),
                   pt.GetZ(), delta, startIndex);
}

//==================== Edge3d ====================

Edge3d &
Edge3d::SetLine(int startPoint, int endPoint) {
  // PreConditions.
  ASSERT(startPoint >= 0);
  ASSERT(endPoint >= 0);

  p1 = startPoint;
  p2 = endPoint;
  return *this;
}

//==================== FaceTex ==========

FaceTex::FaceTex(const char *tName) {
  texture = NULL;
  SetTName(tName);
  SetTInfo(0, 0, 0.0f, 1.0f, 1.0f);
  SetTAttribs(0, 0, 0);
  lockTex = false;
  // lockTex = true;
}

FaceTex::FaceTex(const FaceTex &src) {
  texture = NULL;
  *this = src;
}

FaceTex::~FaceTex(void) {
}

FaceTex &
FaceTex::operator=(const FaceTex &src) {

  if (this == &src)
    return *this;

  SetTexture(src.texture);
  SetTInfo(src.xo, src.yo, src.ra, src.xs, src.ys);
  SetTAttribs(src.atr1, src.atr2, src.atr3);
  lockTex = src.lockTex;

  return *this;
}

char *
FaceTex::GetTName(void) const {
	if(texture)
		return texture->GetName();
	else
		return "";
}
 
void
FaceTex::GetTInfo(int &xOff, int &yOff, float &rotAng,
                  float &xScale, float &yScale) const {
  xOff = xo;
  yOff = yo;
  rotAng = ra;
  xScale = xs;
  yScale = ys;
}

void
FaceTex::GetTAttribs(unsigned int &a1, unsigned int &a2, unsigned int &a3) const {
  a1 = atr1;
  a2 = atr2;
  a3 = atr3;
}

void
FaceTex::GetTDim(int &w, int &h) const {
  ASSERT(texture != NULL);

  texture->Cache();
  w = texture->realWidth;
  h = texture->realHeight;
}

void
FaceTex::SetTName(const char *tName) {
  texture = Game::Get()->GetTexDB()->FindTexture((char *)tName);
}

void
FaceTex::SetTInfo(int xOff, int yOff, float rotAng,
                  float xScale, float yScale) {
  xo = xOff;
  yo = yOff;
  ra = rotAng;
  xs = xScale;
  ys = yScale;
}

void
FaceTex::SetTAttribs(unsigned int a1, unsigned int a2, unsigned int a3) {
  atr1 = a1;
  atr2 = a2;
  atr3 = a3;
}

Vector3d &
FaceTex::ST2XYZ(const Plane &plane, float s, float t, Vector3d &pt) const {
  // Flip y coord.	
  t = -t;

  // Offset.
  s -= xo;
  t += yo;

  // Scale it.
  if (xs != 0.0f)
    s *= xs;
  if (ys != 0.0f)
    t *= ys;

  // Rotate it.
  float rcos = (float) cos(DEG2RAD(ra));
  float rsin = (float) sin(DEG2RAD(ra));
  float s1 = s * rcos - t * rsin;
  float t1 = s * rsin + t * rcos;

  Vector3d pt0, mVec;

  int align = GetAxisAlignIndex(plane.GetNorm());

  if (align < 2) {  // Up & Down.
    // Map to XY plane.
    mVec = Vector3d::zAxisVec;
    pt0.NewVector(s1, t1, 0.0f);
  }
  else if (align < 4) { // North & South
    // Map to XZ plane.
    mVec = Vector3d::yAxisVec;
    pt0.NewVector(s1, 0.0f, t1);
  }
  else { // East & West.
    // Map to YZ Plane
    mVec = Vector3d::xAxisVec;
    pt0.NewVector(0.0f, s1, t1);
  }

  Line l(pt0, mVec);

  bool ok = plane.Intersect(l, pt);
  ASSERT(ok);

  return pt;
}

void
FaceTex::XYZ2ST(const Vector3d &norm, const Vector3d &pt,
                float &s, float &t) const {
  int align = GetAxisAlignIndex(norm);

  if (align < 2) {  // Up & Down.
    // Map to the XY plane.
    s = pt.GetX();
	t = pt.GetY();
  }
  else if (align < 4) { // North & South
    // Map to the XZ plane.
    s = pt.GetX();
    t = pt.GetZ();
  }
  else { // East & West.
    // Map to YZ plane
    s = pt.GetY();
    t = pt.GetZ();
  }

  // Rotate it.
  float rcos = (float) cos(DEG2RAD(-ra));
  float rsin = (float) sin(DEG2RAD(-ra));
  float x = s * rcos - t * rsin;
  float y = s * rsin + t * rcos;
  s = x;
  t = y;

  // Scale it.
  if (xs != 0.0f)
    s /= xs;
  if (ys != 0.0f)
    t /= ys;

  // Offset
  s += xo;
  t -= yo;

  // Flip y coord.
  t = -t;
}

int
FaceTex::GetAxisAlignIndex(const Vector3d &norm) {
  float z, zMax = 0.0f;
  int axisAlign;

  // Round the norm vector due to floating precision errors.
  // Check East & West.
  z = ROUND4(norm.GetX());
  if (z > zMax) {
    zMax = z;
    axisAlign = 4;
  }
  z = ROUND4(-norm.GetX());
  if (z > zMax) {
    zMax = z;
    axisAlign = 5;
  }

  // Check North and South
  z = ROUND4(norm.GetY());
  if (z > zMax) {
    zMax = z;
    axisAlign = 2;
  }
  z = ROUND4(-norm.GetY());
  if (z > zMax) {
    zMax = z;
    axisAlign = 3;
  }

  // Check Up and Down
  z = ROUND4(norm.GetZ());
  if (z > zMax) {
    zMax = z;
    axisAlign = 0;
  }
  z = ROUND4(-norm.GetZ());
  if (z > zMax) {
    zMax = z;
    axisAlign = 1;
  }

  return axisAlign;
}

void
FaceTex::Project2D(const Vector3d &normVec, const Vector3d &pt,
                   float &s, float &t) {
  int align = GetAxisAlignIndex(normVec);

  if (align < 2) {       // Up & Down.  Map to the XY plane.
    s = pt.GetX();
    t = pt.GetY();
  }
  else if (align < 4) {  // North & South.  Map to the XZ plane.
    s = pt.GetX();
    t = pt.GetZ();
  }
  else {                 // East & West. Map to YZ plane
    s = pt.GetY();
    t = pt.GetZ();
  }
}

void
FaceTex::DeriveCoordSys(const Vector3d &origVec, const Vector3d &sBasisVec,
                        const Vector3d &tBasisVec, const Vector3d &normVec,
                        float &xOffset, float &yOffset, float &rotAng,
                        float &xScale, float &yScale) {

  Vector3d oVec, sVec, tVec, nVec;
  int align = GetAxisAlignIndex(normVec);

  if (align < 2) {  // Up & Down.  Map to the XY plane.
    oVec.NewVector(origVec.GetX(), origVec.GetY(), 0.0f);
    sVec.NewVector(sBasisVec.GetX(), sBasisVec.GetY(), 0.0f);
    tVec.NewVector(tBasisVec.GetX(), tBasisVec.GetY(), 0.0f);
  }
  else if (align < 4) { // North & South.  Map to the XZ plane.
    oVec.NewVector(origVec.GetX(), origVec.GetZ(), 0.0f);
    sVec.NewVector(sBasisVec.GetX(), sBasisVec.GetZ(), 0.0f);
    tVec.NewVector(tBasisVec.GetX(), tBasisVec.GetZ(), 0.0f);
  }
  else { // East & West. Map to YZ plane
    oVec.NewVector(origVec.GetY(), origVec.GetZ(), 0.0f);
    sVec.NewVector(sBasisVec.GetY(), sBasisVec.GetZ(), 0.0f);
    tVec.NewVector(tBasisVec.GetY(), tBasisVec.GetZ(), 0.0f);
  }

  sVec.SubVector(oVec);
  tVec.SubVector(oVec);

  rotAng = ATan(sVec.GetY(), sVec.GetX());

  xScale = sVec.GetMag();
  yScale = tVec.GetMag();

  nVec.CrossVector(tVec, sVec);
  if (nVec.GetZ() < 0.0f)
    yScale *= -1.0f;

  float rcos = (float) cos(-rotAng);
  float rsin = (float) sin(-rotAng);

  xOffset = - (oVec.GetX() * rcos - oVec.GetY() * rsin) / xScale;
  yOffset = (oVec.GetX() * rsin + oVec.GetY() * rcos) / yScale;
}

void 
FaceTex::Transform(const Matrix44 &trans, const Plane &texPlane,
                   const Vector3d *pEndNormVec) {

  // Define the basis vectors.
  Vector3d oVec, sVec, tVec, nVec;
  ST2XYZ(texPlane, 0.0f, 0.0f, oVec);
  ST2XYZ(texPlane, 1.0f, 0.0f, sVec);
  ST2XYZ(texPlane, 0.0f, 1.0f, tVec);

  // Transform;
  trans.Transform(oVec);
  trans.Transform(sVec);
  trans.Transform(tVec);
  if (pEndNormVec == NULL)
    trans.Transform(nVec, texPlane.GetNorm());
  else
    nVec = *pEndNormVec;

  // Find the new texture settings.
  float xOffset, yOffset, rotAng, xScale, yScale;
  DeriveCoordSys(oVec, sVec, tVec, nVec,
                 xOffset, yOffset, rotAng,
                 xScale, yScale);

  // Set them.
  xo = ROUNDI(xOffset);
  yo = ROUNDI(yOffset);
  ra = RAD2DEG(rotAng);
  xs = xScale;
  ys = yScale;
}

//==================== GPolygon ====================

GPolygon::GPolygon(int pt1, int pt2, int pt3) {
  // PreConditions.
  ASSERT(pt1 >= 0 && pt2 >= 0 && pt3 >= 0);
  ASSERT(pt1 != pt2 && pt2 != pt3 && pt1 != pt3);

  edges.AppendNode(*(new Edge3d(pt1, pt2)));
  edges.AppendNode(*(new Edge3d(pt2, pt3)));
  edges.AppendNode(*(new Edge3d(pt3, pt1)));

  t = new FaceTex;
}

GPolygon::GPolygon(const GPolygon &src) {
  t = NULL;
  *this = src;
}

GPolygon::~GPolygon(void) {
  // Sanity Check.
  ASSERT(GetNumSides() >= 3);

  while (edges.NumOfElm() > 0)
    delete &(edges.RemoveNode(0));

  ASSERT(t != NULL);
  delete t;
}

GPolygon &
GPolygon::operator=(const GPolygon &src) {

  if (&src == this)
    return *this;

  // ElmNode::operator=(src);

  while (edges.NumOfElm() > 0)
    delete &(edges.RemoveNode(0));

  edges = src.edges;
  if(t)
    delete t;
  t = new FaceTex(*src.GetTexturePtr());

  return *this;
}

int
GPolygon::GetNumSides(void) const {
  return edges.NumOfElm();
}

Edge3d &
GPolygon::GetSide(int index) const {
  return edges[index];
}

int
GPolygon::GetEdgeIndex(int vIndx) const {
  IterLinkList<Edge3d> iter(edges);
  int i = 0;

  iter.Reset();
  while (!iter.IsDone()) {
    if ((iter.GetNext())->GetStartPoint() == vIndx)
      return i;
    i++;
  }
  return -1;
}

Edge3d &
GPolygon::AddVertex(int newPt, Edge3d &line) {
  // PreConditions.
  ASSERT(newPt >= 0);
  ASSERT(edges.FindNodeIndex(&line) != -1);

  int index = edges.FindNodeIndex(&line) + 1;
  Edge3d *newLine = new Edge3d(newPt, line.GetEndPoint());

  line.SetLine(line.GetStartPoint(), newPt);
  edges.InsertNode(*newLine, index);

  return line;
}

void
GPolygon::RemoveVertex(Edge3d &line1, Edge3d &line2) {
  // PreConditions.
  ASSERT(edges.FindNodeIndex(&line1) != -1);
  ASSERT(edges.FindNodeIndex(&line2) != -1);
  ASSERT(line1.GetEndPoint() == line2.GetStartPoint());
  ASSERT(edges.NumOfElm() > 3);

  line1.SetLine(line1.GetStartPoint(), line2.GetEndPoint());
  DeleteSide(line2);
}

void
GPolygon::RemoveVertex(int eIndx, int &vBefore, int &vAfter) {
  // PreCondition.
  ASSERT(GetNumSides() > 3);
  ASSERT(eIndx >= 0 && eIndx < GetNumSides());

  Edge3d *l1, *l2;

  if (eIndx > 0)
    l1 = &(edges[eIndx - 1]);
  else 
    l1 = &(edges[GetNumSides() - 1]);
  l2 = &(edges[eIndx]);

  // Sanity.
  ASSERT(l1->GetEndPoint() == l2->GetStartPoint());

  vBefore = l1->GetStartPoint();
  vAfter = l2->GetEndPoint();

  RemoveVertex(*l1, *l2);
}

void
GPolygon::DeleteSide(Edge3d &line) {
  // PreCondition.
  ASSERT(edges.FindNodeIndex(&line) != -1);
  ASSERT(edges.NumOfElm() > 3);

  delete &(edges.RemoveNode(line));
}

void
GPolygon::InvertOrientation(void) {
	LinkList<Edge3d> tmpStack;

	while (edges.NumOfElm() > 0)
		tmpStack.InsertNode(edges.RemoveNode(0), 0);

	Edge3d *pEdge;
	while (tmpStack.NumOfElm() > 0) {
		pEdge = &(tmpStack.RemoveNode(0));
		edges.AppendNode(*(new Edge3d(pEdge->GetEndPoint(), pEdge->GetStartPoint())));
		delete pEdge;
	}
}

void
GPolygon::SetTexturePtr(FaceTex *tPtr) {
  ASSERT(t != NULL);
  delete t;
  t = tPtr;
}

void
GPolygon::CopyTexture(const FaceTex *tPtr) {
  ASSERT(t != NULL);
  delete t;

  if (tPtr)
    t = new FaceTex(*tPtr);
  else
    t = new FaceTex;
}

//==================== PlaneNode ====================

PlaneNode::PlaneNode(const GPolygon &poly, const Plane &nPlane) {
  polygon = &poly;
  plane = new Plane(nPlane);
}

PlaneNode::PlaneNode(const GPolygon &poly, const Points3d &vertices) {
  int p0, p1, p2;

  polygon = &poly;
  p0 = (polygon->GetSide(0)).GetStartPoint();
  p1 = (polygon->GetSide(1)).GetStartPoint();
  p2 = (polygon->GetSide(2)).GetStartPoint();
  plane = new Plane(vertices[p0], vertices[p1], vertices[p2]);
}

PlaneNode::PlaneNode(const PlaneNode &src) {
  plane = NULL;
  *this = src;
}

PlaneNode::~PlaneNode(void) {
  // don't delete polygon.
  polygon = NULL;
  if (plane != NULL)
    delete plane;
}

PlaneNode &
PlaneNode::operator=(const PlaneNode &src) {
  if (this == &src)
    return *this;

  // ElmNode::operator=(src);

  polygon = src.polygon;
  if (plane != NULL)
    delete plane;
  plane = new Plane(*src.plane);
  return *this;
}

void
PlaneNode::SetNewPlane(const Vector3d &nVec, const Vector3d &pVec) {
  // Sanity.
  ASSERT(plane != NULL);

  plane->NewPlane(nVec, pVec);
}

//==================== Geometry ====================
#define EPSILON     1.0f
#define MAX_FACES   96

Geometry::Geometry(void) {
  boundRadius = 0.0f;
}

Geometry::~Geometry(void) {
  while (faces.NumOfElm() > 0)
    delete &(faces.RemoveNode(0));
  while (edgeList.NumOfElm() > 0)
    delete &(edgeList.RemoveNode(0));
  while (planeList.NumOfElm() > 0)
    delete &(planeList.RemoveNode(0));
}

Geometry &
Geometry::operator=(const Geometry &src) {
  if (&src == this)
    return *this;

  // ElmNode::operator=(src);

  boundRadius = src.boundRadius;
  vertices = src.vertices;
  while (faces.NumOfElm() > 0)
    delete &(faces.RemoveNode(0));
  while (edgeList.NumOfElm() > 0)
    delete &(edgeList.RemoveNode(0));
  while (planeList.NumOfElm() > 0)
    delete &(planeList.RemoveNode(0));
  edgeList = src.edgeList;
  faces = src.faces;

  // Problem....  Do not do...
  // planeList = src.planeList;
  // ConstructPlaneList();

  // Clean planelist.
  while (planeList.NumOfElm() > 0)
    delete &(planeList.RemoveNode(0));

  // Build the planes.
  int i;
  PlaneNode *pNode;
  
  ASSERT(faces.NumOfElm() == src.planeList.NumOfElm());
  for(i = 0; i < faces.NumOfElm(); i++) {
    pNode = new PlaneNode(faces[i], src.planeList[i].GetPlane());
    planeList.AppendNode(*pNode);
  }    

  return *this;
}

//===== Texture for Brush =====

FaceTex *
Geometry::GetFaceTexturePtr(int faceIndex) {
  return faces[faceIndex].GetTexturePtr();
}

void
Geometry::SetFaceTexture(int faceIndex, FaceTex *newTexturePtr) {
  faces[faceIndex].SetTexturePtr(newTexturePtr);
}

void
Geometry::NewFaceTexture(int faceIndex, const char *texture) {
  FaceTex *t = faces[faceIndex].GetTexturePtr();

  if (t == NULL) {
    t = new FaceTex(texture);
    faces[faceIndex].SetTexturePtr(t);
  }
  else {
    t->SetTName(texture);
  }

//  t->SetTInfo(0, 0, 0.0f, 1.0f, 1.0f);
}

void
Geometry::NewGeomTexture(const char *texture) {
  int i;

  for(i = 0; i < faces.NumOfElm(); i++)
    NewFaceTexture(i, texture);
}

//===== Brush Modification =====

void
Geometry::BreakPlanes(int vertex) {
  // PreCondition.
  ASSERT(vertex >= 0 && vertex < vertices.arraySize);
  ASSERT(vertices.array[vertex] != NULL);

  IterLinkList<GPolygon> iterf(faces);
  LinkList<GPolygon> fBuf;
  GPolygon *fPtr, *nPoly;
  int eIndx, v0, v1;

  iterf.Reset();
  while (!(iterf.IsDone())) {
    fPtr = iterf.GetNext();
    if (fPtr->GetNumSides() > 3 &&
        (eIndx = fPtr->GetEdgeIndex(vertex)) != -1) {
      fPtr->RemoveVertex(eIndx, v0, v1);
      nPoly = new GPolygon(v0, vertex, v1);
      nPoly->CopyTexture(fPtr->GetTexturePtr());
      // edgeList.AppendNode(*(new Edge3d(v0, v1)));
      fBuf.AppendNode(*nPoly);
    }
  }

  while (fBuf.NumOfElm() > 0)
    faces.AppendNode(fBuf.RemoveNode(0));

  // Redo the edge and plane lists.
  // ConstructEdgeList();
  ConstructPlaneList();
}

void
Geometry::BreakPlanes(const Edge3d &edge) {
  // PreCondition.
  ASSERT(edgeList.FindNodeIndex(&edge) != -1);
  ASSERT(edge.GetStartPoint() >= 0 &&
         edge.GetStartPoint() < vertices.arraySize);
  ASSERT(vertices.array[edge.GetStartPoint()] != NULL);
  ASSERT(edge.GetEndPoint() >= 0 &&
         edge.GetEndPoint() < vertices.arraySize);
  ASSERT(vertices.array[edge.GetEndPoint()] != NULL);

  IterLinkList<GPolygon> iterf(faces);
  LinkList<GPolygon> fBuf;
  GPolygon *fPtr, *nPoly;
  int eIndx, v0, v1;
  int lb = edge.GetStartPoint();
  int le = edge.GetEndPoint();

  iterf.Reset();
  while (!(iterf.IsDone())) {
    fPtr = iterf.GetNext();
    if (fPtr->GetNumSides() > 3 &&
        (eIndx = fPtr->GetEdgeIndex(lb)) != -1) {
      fPtr->RemoveVertex(eIndx, v0, v1);
      nPoly = new GPolygon(v0, lb, v1);
      nPoly->CopyTexture(fPtr->GetTexturePtr());
      // edgeList.AppendNode(*(new Edge3d(v0, v1)));
      fBuf.AppendNode(*nPoly);
    }
    if (fPtr->GetNumSides() > 3 &&
        (eIndx = fPtr->GetEdgeIndex(le)) != -1) {
      fPtr->RemoveVertex(eIndx, v0, v1);
      nPoly = new GPolygon(v0, le, v1);
      nPoly->CopyTexture(fPtr->GetTexturePtr());
      // edgeList.AppendNode(*(new Edge3d(v0, v1)));
      fBuf.AppendNode(*nPoly);
    }
  }

  while (fBuf.NumOfElm() > 0)
    faces.AppendNode(fBuf.RemoveNode(0));

  // Redo the edge and plane lists.
  // ConstructEdgeList();
  ConstructPlaneList();
}

void
Geometry::BreakPlanes(const GPolygon &face) {
  // Sanity.
  ASSERT(faces.FindNodeIndex(&face) != -1);

  IterLinkList<Edge3d> itere(face.GetEdgeList());
  IterLinkList<GPolygon> iterf(faces);
  LinkList<GPolygon> fBuf;
  GPolygon *fPtr, *nPoly;
  Edge3d *ePtr;
  int lb, eIndx, v0, v1;

  // Iterate through edges of face.
  itere.Reset();
  while (!(itere.IsDone())) {
    ePtr = itere.GetNext();
    lb = ePtr->GetStartPoint();

    // Iterate through all faces.
    iterf.Reset();
    while (!(iterf.IsDone())) {
      if ((fPtr = iterf.GetNext()) == &face)
        continue;
      if (fPtr->GetNumSides() > 3 &&
          (eIndx = fPtr->GetEdgeIndex(lb)) != -1) {
        fPtr->RemoveVertex(eIndx, v0, v1);
        nPoly = new GPolygon(v0, lb, v1);
        nPoly->CopyTexture(fPtr->GetTexturePtr());
        // edgeList.AppendNode(*(new Edge3d(v0, v1)));
        fBuf.AppendNode(*nPoly);
      }
    }
  }

  // Add new polygons.
  while (fBuf.NumOfElm() > 0)
    faces.AppendNode(fBuf.RemoveNode(0));

  // Redo the edge and plane lists.
  // ConstructEdgeList();
  ConstructPlaneList();
}

void
Geometry::MoveVertex(int vIndex, const Vector3d &deltaPos) {
  // PreCondition.
  ASSERT(vIndex >= 0 && vIndex < vertices.arraySize);
  ASSERT(vertices.array[vIndex] != NULL);

  (vertices.array[vIndex])->AddVector(deltaPos);
  boundRadius = CalBoundRadius();
}

void
Geometry::MoveEdge(const Edge3d &edge, const Vector3d &deltaPos) {
  // PreCondition.
  ASSERT(edgeList.FindNodeIndex(&edge) != -1);

  (vertices.array[edge.GetStartPoint()])->AddVector(deltaPos);
  (vertices.array[edge.GetEndPoint()])->AddVector(deltaPos);
  boundRadius = CalBoundRadius();
}

void
Geometry::MoveFace(const GPolygon &face, const Vector3d &deltaPos) {
  // PreCondition.
  ASSERT(faces.FindNodeIndex(&face) != -1);

  IterLinkList<Edge3d> iter(face.GetEdgeList());

  iter.Reset();
  while(!(iter.IsDone()))
    (vertices.array[(iter.GetNext())->GetStartPoint()])->AddVector(deltaPos);
  boundRadius = CalBoundRadius();
}

bool
Geometry::IntegrityCheck(void) {
  Plane *pBuf[MAX_FACES];
  const FaceTex *tBuf[MAX_FACES];
  Geometry *newBrush;
  int i, p, pNum;
  GPolygon *polyPtr;
  Plane *pPtr;
  bool rtnVal;

  // Sanity.
  // ASSERT(faces.NumOfElm() == planeList.NumOfElm());

  // Rebuild plane list.
  // ConstructPlaneList();
  for(i = 0; i < MAX_FACES; i++)
    pBuf[i] = NULL;

  pNum = 0;
  for(i = 0; i < faces.NumOfElm(); i++) {
    polyPtr = &(faces[i]);
    if ((pPtr = MakePlane(*polyPtr)) == NULL)
      continue;

    if (pNum >= MAX_FACES) {
      delete pPtr;
      break;
    }

    pBuf[pNum] = pPtr;
    tBuf[pNum] = polyPtr->GetTexturePtr();
    pNum++;
  }

  if (pNum < 4 || pNum > MAX_FACES) {
    rtnVal = false;
    goto RTN_INTEGRITYCHK;
  }

  // Check if all vertices are bounded by all planes.
  for(i = 0; i < vertices.arraySize; i++) {
    if (vertices.array[i] == NULL)
      continue;
    for(p = 0; p < pNum; p++) {
      if (pBuf[p]->PointInPlane(*(vertices.array[i])) > 0) {
        rtnVal = false;
        goto RTN_INTEGRITYCHK;
      }
    }
  }

  // Construct a new solid brush.
  newBrush = NewBoundGeometry(pNum,
                              (const Plane **) pBuf,
                              (const FaceTex **) tBuf);
  if (newBrush != NULL) {
    newBrush->ConstructEdgeList();
    *this = *newBrush;
    delete newBrush;
    rtnVal = true;
  }
  else {
    rtnVal = false;
  }

RTN_INTEGRITYCHK:
  for(i = 0; i < MAX_FACES; i++) {
    if (pBuf[i] != NULL)
      delete pBuf[i];
  }

  return rtnVal;
}

//===== Hit Detection =====

float
Geometry::GetBoundRadius(void) {
  if (boundRadius == 0.0f)
    boundRadius = CalBoundRadius();
  return boundRadius;
}

float
Geometry::CalBoundRadius(void) const {
  int i;
  float tmp, rMax;

  rMax = 0.0f;
  for(i = 0; i < vertices.arraySize; i++) {
    if (vertices.array[i] != NULL) {
      if ((tmp = (vertices.array[i])->GetMagSqr()) > rMax)
        rMax = tmp;
    }
  }

  rMax = (float) sqrt(rMax);
  return rMax;
}

bool
Geometry::HitGeometry(const Line &ray, float rangeBegin,
                      float rangeEnd, float &depth) const {
  int i;
  float range;
  PlaneNode *pNode;
  const Edge3d *edge;
  const GPolygon *pgon;
  Vector3d intrsctPt, v0, v1, v2;
  IterLinkList<PlaneNode> iter(planeList);
  bool rtnVal = false;

  iter.Reset();
  while (!(iter.IsDone())) {
    pNode = iter.GetNext();
    if (!(pNode->GetPlane()).Intersect(ray, intrsctPt))
      continue;

    v0.SubVector(intrsctPt, ray.GetPoint());
    range = v0.GetMag();
    if (range > 0.0f)
      range *= (v0.CosTheta(ray.GetSlope()) > 0.0f ? 1.0f : -1.0f);
    if (range < rangeBegin || range > rangeEnd)
      continue;

    pgon = &(pNode->GetPolygon());
    for(i = 0; i < pgon->GetNumSides(); i++) {
      edge = &(pgon->GetSide(i));
      v1.SubVector(intrsctPt, vertices[edge->GetStartPoint()]);
      if (v1.GetMagSqr() < 0.01f) {
        i = pgon->GetNumSides();
        break;
      }
      v2.SubVector(vertices[edge->GetEndPoint()],
                   vertices[edge->GetStartPoint()]);
      v0.CrossVector(v1, v2);
      if (EQUAL4(v0.GetMagSqr(), 0.0)) {
        i = pgon->GetNumSides();
        break;
      }
      if (v0.CosTheta((pNode->GetPlane()).GetNorm()) < 0.0)
        break;
    }
    if (i == pgon->GetNumSides() && (!rtnVal || range < depth)) {
      depth = range;
      rtnVal = true;
    }
  }
  return rtnVal;
}

//===== Geometry Transformation =====

void
Geometry::Transform(const Matrix44 &transMatrix, bool mirror) {
  int i;

  for(i = 0; i < vertices.arraySize; i++) {
    if (vertices.array[i] == NULL)
      continue;
    transMatrix.Transform(*(vertices.array[i]));
  }

  boundRadius = CalBoundRadius();

  // Remake the plane list for hit detection.
  if (!mirror) {
	  IterLinkList<PlaneNode> iter(planeList);
	  PlaneNode *pNode;

	  iter.Reset();
	  while (!iter.IsDone()) {
		pNode = iter.GetNext();
		pNode->TransformPlane(transMatrix);
	  }
  }
  else {
	  // Invert all face origination.
	  IterLinkList<GPolygon> iter(faces);
	  GPolygon *pFace;

	  iter.Reset();
	  while(!iter.IsDone()) {
		  pFace = iter.GetNext();
		  pFace->InvertOrientation();
	  }

	  ConstructPlaneList();
  }
}

Points3d &
Geometry::Transform(const Matrix44 &transMatrix, Points3d &vBuffer) const {
  int i;

  // Check if vBuffer is empty.
  if (vBuffer.numElm == 0) {
    for(i = 0; i < vBuffer.arraySize; i++)
      vBuffer.array[i] = new Point3d;
    vBuffer.numElm = vBuffer.arraySize;
  }

  // Sanity.
  ASSERT(vBuffer.numElm == vBuffer.arraySize);

  // Check if vBuffer is big enough.
  if (vBuffer.arraySize < vertices.arraySize) {
    i = vBuffer.arraySize;
    vBuffer.arraySize = vertices.arraySize;
    vBuffer.array = (Point3d **)
      realloc(vBuffer.array, sizeof(Point3d *) * vBuffer.arraySize);
    for(; i < vBuffer.arraySize; i++)
      vBuffer.array[i] = new Point3d;
    vBuffer.numElm = vBuffer.arraySize;
  }

  // Transform.
  for(i = 0; i < vertices.arraySize; i++) {
    if (vertices.array[i] == NULL)
      continue;

    // Sanity
    ASSERT(vBuffer.array[i] != NULL);
    
    // *(vBuffer.array[i]) = *(vertices.array[i]);
    // transMatrix.Transform(*(vBuffer.array[i]));
    transMatrix.Transform(*(vBuffer.array[i]), *(vertices.array[i]));
  }

  return vBuffer;
}

Points3d &
Geometry::TransformVertexPts(const Matrix44 &transMatrix,
                             Points3d &vBuffer, int vertex) const {
  // PreCondition.
  ASSERT(vertex >= -1 && vertex < GetNumVertices());

  if (vertex == -1)
    return Transform(transMatrix, vBuffer);

  // Hack....  Requires vBuffer been used before.
  // Sanity.
  ASSERT(vBuffer.numElm > 0 && vBuffer.arraySize > 0);
  ASSERT(vBuffer.array[0] != NULL);
  ASSERT(vertices.array[vertex] != NULL);

  transMatrix.Transform(*(vBuffer.array[0]), *(vertices.array[vertex]));
  return vBuffer;
}

Points3d &
Geometry::TransformEdgePts(const Matrix44 &transMatrix,
                           Points3d &vBuffer, const Edge3d *edge) const {
  int i;
  Vector3d aveVec;
  Edge3d *edgePtr;
  IterLinkList<Edge3d> iter(edgeList);

  // Check if vBuffer is empty.
  if (vBuffer.numElm == 0) {
    for(i = 0; i < vBuffer.arraySize; i++)
      vBuffer.array[i] = new Point3d;
    vBuffer.numElm = vBuffer.arraySize;
  }

  // Sanity.
  ASSERT(vBuffer.numElm == vBuffer.arraySize);

  // Check if vBuffer is big enough.
  if (vBuffer.arraySize < GetNumEdges()) {
    i = vBuffer.arraySize;
    vBuffer.arraySize = GetNumEdges();
    vBuffer.array = (Point3d **)
      realloc(vBuffer.array, sizeof(Point3d *) * vBuffer.arraySize);
    for(; i < vBuffer.arraySize; i++)
      vBuffer.array[i] = new Point3d;
    vBuffer.numElm = vBuffer.arraySize;
  }

  i = 0;
  iter.Reset();
  while (!(iter.IsDone())) {
    edgePtr = iter.GetNext();
    if (edge != NULL && edge != edgePtr)
      continue;

    aveVec.AddVector(vertices[edgePtr->GetStartPoint()],
                     vertices[edgePtr->GetEndPoint()]);
    aveVec.MultVector(0.5f);
    ASSERT(vBuffer.array[i] != NULL);
    transMatrix.Transform(*(vBuffer.array[i++]), aveVec);
    if (edge != NULL)
      break;
  }
  // Sanity.
  ASSERT(i > 0);
  return vBuffer;
}

Points3d &
Geometry::TransformFacePts(const Matrix44 &transMatrix,
                           Points3d &vBuffer, const GPolygon *face) const {
  int i, j;
  Vector3d aveVec;
  GPolygon *facePtr;
  IterLinkList<GPolygon> iter(faces);

  // Check if vBuffer is empty.
  if (vBuffer.numElm == 0) {
    for(i = 0; i < vBuffer.arraySize; i++)
      vBuffer.array[i] = new Point3d;
    vBuffer.numElm = vBuffer.arraySize;
  }

  // Sanity.
  ASSERT(vBuffer.numElm == vBuffer.arraySize);

  // Check if vBuffer is big enough.
  if (vBuffer.arraySize < GetNumFaces()) {
    i = vBuffer.arraySize;
    vBuffer.arraySize = GetNumFaces();
    vBuffer.array = (Point3d **)
      realloc(vBuffer.array, sizeof(Point3d *) * vBuffer.arraySize);
    for(; i < vBuffer.arraySize; i++)
      vBuffer.array[i] = new Point3d;
    vBuffer.numElm = vBuffer.arraySize;
  }
  
  i = 0;
  iter.Reset();
  while (!iter.IsDone()) {
    facePtr = iter.GetNext();
    if (face != NULL && face != facePtr)
      continue;
    aveVec.NewVector(0.0f, 0.0f, 0.0f);
    for(j = 0; j < facePtr->GetNumSides(); j++)
      aveVec.AddVector(vertices[(facePtr->GetSide(j)).GetStartPoint()]);
    aveVec.MultVector(1.0f / facePtr->GetNumSides());
    ASSERT(vBuffer.array[i] != NULL);
    transMatrix.Transform(*(vBuffer.array[i++]), aveVec);
    if (face != NULL)
      break;
  }
  // Sanity.
  ASSERT(i > 0);
  return vBuffer;
}

//===== Geometry Adjustment =====

Vector3d &
Geometry::CenterGeometry(Vector3d &position) {
  float xAve, yAve, zAve;
  Vector3d dVec;
  Point3d *v;
  int i;

  // Find average.
  xAve = yAve = zAve = 0.0f;
  for(i = 0; i < vertices.arraySize; i++) {
    if ((v = vertices.array[i]) == NULL)
      continue;
    xAve += v->GetX() / vertices.numElm;
    yAve += v->GetY() / vertices.numElm;
    zAve += v->GetZ() / vertices.numElm;
  }

  xAve = ROUND(xAve);
  yAve = ROUND(yAve);
  zAve = ROUND(zAve);
  dVec.NewVector(-xAve, -yAve, -zAve);

  // Adjust all vertices.
  for(i = 0; i < vertices.arraySize; i++) {
    if ((v = vertices.array[i]) == NULL)
      continue;
    v->AddVector(dVec);
  }

  boundRadius = CalBoundRadius();

  // Remake the list of planes for bounding sides.  Used for hit detection.
  // ConstructPlaneList();

  IterLinkList<PlaneNode> iter(planeList);
  PlaneNode *pNode;
  Vector3d pVec;

  iter.Reset();
  while (!iter.IsDone()) {
    pNode = iter.GetNext();
    pVec.AddVector((pNode->GetPlane()).GetPoint(), dVec);
    pNode->SetNewPlane((pNode->GetPlane()).GetNorm(), pVec);
  }

  return (position.NewVector(xAve, yAve, zAve));
}

void
Geometry::FindTransBoundBox(const Matrix44 trans,
                            Vector3d &minVec, Vector3d &maxVec) const {
  // Sanity.
  ASSERT(vertices.GetNumPoints() > 3);

  int i, init;
  Vector3d vec;

  init = 0;
  for(i = 0; i < vertices.arraySize; i++) {
    if (vertices.array[i] == NULL)
      continue;
    trans.Transform(vec, *(vertices.array[i]));
    if (init == 0) {
      minVec = maxVec = vec;
      init = 1;
    }
    else {
      minVec.NewVector(Min(minVec.GetX(), vec.GetX()),
                       Min(minVec.GetY(), vec.GetY()),
                       Min(minVec.GetZ(), vec.GetZ()));
      maxVec.NewVector(Max(maxVec.GetX(), vec.GetX()),
                       Max(maxVec.GetY(), vec.GetY()),
                       Max(maxVec.GetZ(), vec.GetZ()));
    }
  }
}

//===== Hollow =====
Geometry *
Geometry::CreateExpandedGeom(float delta) const {
  // Sanity.
  ASSERT(planeList.NumOfElm() < MAX_FACES);

  Geometry *rtnVal = NULL;
  const FaceTex *tBuf[MAX_FACES];
  Plane *pBuf[MAX_FACES];
  int i, pNum;
  const Plane *pPtr;
  Vector3d nVec, pVec;

  pNum = planeList.NumOfElm();
  for(i = 0; i < pNum; i++) {
    pPtr = &(planeList[i].GetPlane());
    nVec = pPtr->GetNorm();
    nVec.Normalize();
    nVec.MultVector(delta);
    pVec.AddVector(pPtr->GetPoint(), nVec);
    pBuf[i] = new Plane(pPtr->GetNorm(), pVec);
	// What should this be?
    // tBuf[i] = NULL;
    tBuf[i] = faces[i].GetTexturePtr();
  }

  for(; i < MAX_FACES; i++) {
    pBuf[i] = NULL;
    tBuf[i] = NULL;
  }

  rtnVal = NewBoundGeometry(pNum,
                            (const Plane **) pBuf,
                            (const FaceTex **) tBuf);

  if (rtnVal != NULL) {
    rtnVal->boundRadius = rtnVal->CalBoundRadius();
    // Not needed...
    rtnVal->ConstructEdgeList();
    // Should be made already in NewBoundGeometry().
    // rtnVal->ConstructPlaneList();
  }

  for(i = 0; i < MAX_FACES; i++) {
    if (pBuf[i] != NULL)
      delete pBuf[i];
  }

  return rtnVal;
}

LinkList<Geometry> *
Geometry::HollowInward(float delta) const {
  // Sanity.
  ASSERT(planeList.NumOfElm() + 1 < MAX_FACES);

  static LinkList<Geometry> rtnVal;
  Geometry *pNewGeom;
  const FaceTex *pTex, *tBuf[MAX_FACES];
  Plane *pBuf[MAX_FACES];
  int i, j, pNum;
  const Plane *pPtr;
  Vector3d nVec, pVec;

  // Clean up.
  rtnVal.DeleteAllNodes();

  // Init the plane array.
  pNum = planeList.NumOfElm();
  for(i = 0; i < pNum; i++) {
    pBuf[i] = (Plane *) &(planeList[i].GetPlane());
    tBuf[i] = NULL;
  }

  for(; i < MAX_FACES; i++) {
    pBuf[i] = NULL;
    tBuf[i] = NULL;
  }

  // Iter each face and create the bounding wall.
  for(i = 0; i < pNum; i++) {
    // Set the texture.
    pTex = faces[i].GetTexturePtr();
    for(j = 0; j <= pNum; j++)
      tBuf[j] = pTex;

    // Add the new inner plane.
    pPtr = &(planeList[i].GetPlane());
    nVec = pPtr->GetNorm();
    nVec.Normalize();
    nVec.MultVector(-delta);
    pVec.AddVector(pPtr->GetPoint(), nVec);
    nVec = pPtr->GetNorm();
	nVec.MultVector(-1.0f);
    pBuf[pNum] = new Plane(nVec, pVec);

    // Create the new bounding wall.
    pNewGeom = NewBoundGeometry(pNum + 1,
                                (const Plane **) pBuf,
                                (const FaceTex **) tBuf);

    // delete the inner plane.
    delete pBuf[pNum];

    ASSERT(pNewGeom != NULL);
    if (pNewGeom == NULL) {
      // Something's wrong.
      break;
    }

    pNewGeom->boundRadius = pNewGeom->CalBoundRadius();
    pNewGeom->ConstructEdgeList();

    // Should be made already in NewBoundGeometry().
    // pNewGeom->ConstructPlaneList();

	// Add it to the rtn list.
    rtnVal.AppendNode(*pNewGeom);
  }

  if (i != pNum) {
    // Something's wrong.
    rtnVal.DeleteAllNodes();
	return NULL;
  }

  return &rtnVal;
}

//===== CSG Subtraction =====

LinkList<Geometry> *
Geometry::CSGSubtract(const Geometry &cuttee, const Geometry &cutter) {
  static LinkList<Geometry> rtnVal;
  Geometry *invGeom, *geom, *newGeom;
  const Plane *pBuf[MAX_FACES * 2];
  const FaceTex *tBuf[MAX_FACES * 2];
  Vector3d n;
  int p, i, pNum;
  bool done;

  // Clean up.
  rtnVal.DeleteAllNodes();

  // Sanity Check.
  ASSERT(cuttee.faces.NumOfElm() == cuttee.planeList.NumOfElm());
  ASSERT(cutter.faces.NumOfElm() == cutter.planeList.NumOfElm());
  ASSERT(cuttee.faces.NumOfElm() <= MAX_FACES);
  ASSERT(cutter.faces.NumOfElm() <= MAX_FACES);

  // First check to see if the 2 geoms intersect at all.
  pNum = 0;
  for(i = 0; i < cuttee.faces.NumOfElm(); i++, pNum++) {
    pBuf[pNum] = &(cuttee.planeList[i].GetPlane());
    tBuf[pNum] = NULL;
  }
  for(i = 0; i < cutter.faces.NumOfElm(); i++, pNum++) {
    pBuf[pNum] = &(cutter.planeList[i].GetPlane());
    tBuf[pNum] = NULL;
  }

  invGeom = NewBoundGeometry(pNum, pBuf, tBuf);

  if (invGeom == NULL) {
    // The 2 geometries don't intersect.  return;
    // Post Condition.
    ASSERT(rtnVal.NumOfElm() == 0);
    return NULL;
  }

  // InvGeom is the intersection.
  invGeom->ConstructEdgeList();
  // Should be made in NewBoundGeometry().
  // invGeom->ConstructPlaneList();
  // Sanity.
  ASSERT(invGeom->faces.NumOfElm() == invGeom->planeList.NumOfElm());

  int tCount, maxCount;
  FaceTex *iTexture, *pTexture, *nTexture = NULL;
  Plane *invPlane = NULL;

  // Start carving.
  geom = new Geometry(cuttee);

  // Find the most used texture in cuttee.
  maxCount = 0;
  pNum = geom->faces.NumOfElm();
  for(p = 0; p < pNum; p++) {
    if ((pTexture= geom->faces[p].GetTexturePtr()) == NULL)
      continue;
    tCount = 0;
    for(i = 0; i < pNum; i++) {
      if ((i != p) &&
          ((iTexture = geom->faces[i].GetTexturePtr()) != NULL) &&
          (strcmp(iTexture->GetTName(), pTexture->GetTName()) == 0)) {
        if (i < p) // already counted this texture.
          break;
        else
          tCount++;
      }
    }
    if (i == pNum && tCount > maxCount) {
      maxCount = tCount;
      if (nTexture)
        delete nTexture;
      nTexture = new FaceTex(*pTexture);
    }
  }
  
  // Cycle through planes of invGeom.
  done = false;
  for(p = 0; p < invGeom->planeList.NumOfElm() && !done; p++) {
    // Sanity Check.
    ASSERT(geom->planeList.NumOfElm() <= MAX_FACES);

    // Make the new sub piece.
    ASSERT(invPlane == NULL);  // Sanity.
    invPlane = new Plane(invGeom->planeList[p].GetPlane()); 
    (n = invPlane->GetNorm()).MultVector(-1.0f);  // Invert the cutting plane.
    invPlane->NewPlane(n, invPlane->GetPoint());

    pNum = geom->planeList.NumOfElm();
    for(i = 0; i < pNum; i++) {
      pBuf[i] = &(geom->planeList[i].GetPlane());
      tBuf[i] = (geom->faces[i]).GetTexturePtr();
    }
    pNum++;
    pBuf[i] = invPlane;
    tBuf[i] = nTexture;

    newGeom = NewBoundGeometry(pNum, pBuf, tBuf);
    if (newGeom != NULL) {
      newGeom->ConstructEdgeList();
      // Should be made in NewBoundGeometry().
      // newGeom->ConstructPlaneList();
      rtnVal.AppendNode(*newGeom);
      newGeom = NULL;

      // Remake what's left over.
      (n = invPlane->GetNorm()).MultVector(-1.0f);
      invPlane->NewPlane(n, invPlane->GetPoint());

      newGeom = NewBoundGeometry(pNum, pBuf, tBuf);
      delete geom;
      geom = newGeom;
      newGeom = NULL;

      if (geom != NULL) {
        geom->ConstructEdgeList();
        // Should be made in NewBoundGeometry().
        // geom->ConstructPlaneList();
      }
      else {  // Nothing left.  Break the loop.
        done = true;
      }
    }

    delete invPlane;
    invPlane = NULL;

    for(i = 0; i < pNum; i++) {
      pBuf[i] = NULL;
      tBuf[i] = NULL;
    }
  }

  if (nTexture != NULL)
    delete nTexture;

  delete invGeom;
  delete geom;

  return &rtnVal;
}

//===== CSG Intersection =====
Geometry *
Geometry::CSGIntersect(const Geometry &geom1, const Geometry &geom2) {
  // Sanity.
  ASSERT(geom1.planeList.NumOfElm() == geom1.faces.NumOfElm());
  ASSERT(geom2.planeList.NumOfElm() == geom2.faces.NumOfElm());
  ASSERT(geom1.faces.NumOfElm() + geom2.faces.NumOfElm() <= 64);

  int pNum, i;
  const Plane *pBuf[MAX_FACES * 2];
  const FaceTex *tBuf[MAX_FACES * 2];

  for(pNum = i = 0; i < geom1.faces.NumOfElm(); i++, pNum++) {
    pBuf[pNum] = &(geom1.planeList[pNum].GetPlane());
	tBuf[pNum] = geom1.faces[pNum].GetTexturePtr();
  }

  for(i = 0; i < geom2.faces.NumOfElm(); i++, pNum++) {
    pBuf[pNum] = &(geom2.planeList[i].GetPlane());
	tBuf[pNum] = geom2.faces[i].GetTexturePtr();
  }

  for(i = pNum; i < MAX_FACES * 2; i++) {
    pBuf[i] = NULL;
	tBuf[i] = NULL;
  }

  Geometry *newBrush = NewBoundGeometry(pNum, pBuf, tBuf);

  if (newBrush != NULL)
    newBrush->ConstructEdgeList();

  return newBrush;
}

//===== Plane Clip =====
Geometry *
Geometry::PlaneClip(const Plane &cPlane) {
  ASSERT(faces.NumOfElm() + 1 <= MAX_FACES);

  // Find the most used texture in cuttee.
  FaceTex *iTexture, *pTexture, *nTexture = NULL;
  int p, i, tCount, maxCount = 0;
  int pNum = faces.NumOfElm();
  for(p = 0; p < pNum; p++) {
    if ((pTexture= faces[p].GetTexturePtr()) == NULL)
      continue;
    tCount = 0;
    for(i = 0; i < pNum; i++) {
      if ((i != p) &&
          ((iTexture = faces[i].GetTexturePtr()) != NULL) &&
          (strcmp(iTexture->GetTName(), pTexture->GetTName()) == 0)) {
        if (i < p) // already counted this texture.
          break;
        else
          tCount++;
      }
    }
    if (i == pNum && tCount > maxCount) {
      maxCount = tCount;
      if (nTexture)
        delete nTexture;
      nTexture = new FaceTex(*pTexture);
    }
  }

  // Copy plane ptrs over to buffer.
  ASSERT(faces.NumOfElm() == planeList.NumOfElm());

  const Plane *pBuf[MAX_FACES];
  const FaceTex *tBuf[MAX_FACES];
  IterLinkList<PlaneNode> iterPlanes(planeList);
  IterLinkList<GPolygon> iterFaces(faces);

  iterPlanes.Reset();
  iterFaces.Reset();
  for(pNum = 0; !iterFaces.IsDone(); pNum++) {
    pBuf[pNum] = &(iterPlanes.GetNext()->GetPlane());
	tBuf[pNum] = iterFaces.GetNext()->GetTexturePtr();
  }
  ASSERT(pNum == faces.NumOfElm());

  pBuf[pNum] = &cPlane;
  tBuf[pNum++] = nTexture;

  Geometry *newGeom = NewBoundGeometry(pNum, pBuf, tBuf);

  // Check if the cutting plane is part of the new geometry.
  if (newGeom != NULL && newGeom->faces.NumOfElm() == faces.NumOfElm()) {
    pNum = newGeom->planeList.NumOfElm();
    PlaneNode *pPNode = &(newGeom->planeList[pNum-1]);
/*
    const char *t1, *t2;
    t1 = pPNode->GetPolygon().GetTexturePtr()->GetTName();
    t2 = nTexture->GetTName();
    if (strcmp(t1, t2) != 0 || !pPNode->GetPlane().IsSame(cPlane)) {
*/
    if (!pPNode->GetPlane().IsSame(cPlane)) {
      // Probably the cutting plane wasn't included.
      // Cutting had no effects.
      delete newGeom;
      newGeom = this;
    }
  }

  if (newGeom != NULL && newGeom != this)
    newGeom->ConstructEdgeList();

  delete nTexture;

  return newGeom;
}

Geometry *
Geometry::MakeSegment(const Vector3d &pt1, const Vector3d &pt2, float width) {
  Geometry *rtnVal = NULL;

  int i;
  Plane *pBuf[6];
  FaceTex *tBuf[6];

  for(i = 0; i < 6; i++)
    tBuf[i] = NULL;

  Vector3d norm, tVec, pt;
  SphrVector oriVec;
  Matrix44 trans;

  // The 2 ending pts.
  norm.SubVector(pt1, pt2);
  pBuf[0] = new Plane(norm, pt1);

  norm.MultVector(-1.0f);
  pBuf[1] = new Plane(norm, pt2);

  // The 4 bounding sides.
  if (ABS(norm.CosTheta(Vector3d::xAxisVec)) <
      ABS(norm.CosTheta(Vector3d::yAxisVec)))
    tVec = Vector3d::xAxisVec;
  else
    tVec = Vector3d::yAxisVec;

  oriVec.NewVector(norm, tVec);
  trans.SetRotate(oriVec);

  trans.Transform(norm, width, 0.0f, 0.0f);
  pt.AddVector(pt1, norm);
  pBuf[2] = new Plane(norm, pt);

  trans.Transform(norm, 0.0f, 0.0f, width);
  pt.AddVector(pt1, norm);
  pBuf[3] = new Plane(norm, pt);

  trans.Transform(norm, -width, 0.0f, 0.0f);
  pt.AddVector(pt1, norm);
  pBuf[4] = new Plane(norm, pt);

  trans.Transform(norm, 0.0f, 0.0f, -width);
  pt.AddVector(pt1, norm);
  pBuf[5] = new Plane(norm, pt);

  rtnVal = NewBoundGeometry(6, (const Plane **) pBuf, (const FaceTex **)tBuf);

  ASSERT(rtnVal != NULL);
  if (rtnVal != NULL) {
    rtnVal->boundRadius = rtnVal->CalBoundRadius();
    rtnVal->ConstructEdgeList();
  }

  for(i = 0; i < 6; i++)
    delete pBuf[i];

  return rtnVal;
}

//===== Read / Write Geometry =====
Geometry *
Geometry::ReadBoundGeometry(LFile *inFile, int geomNum) {
  // PreCondition.
  ASSERT(geomNum >= -1);

  Geometry *rtnVal = NULL;
  Vector3d p1, p2, p3;
  Vector3d v1, v2;
  int i, numPlanes;
  int i1, i2;
  int a1, a2, a3;
  float f1, f2, f3;
  const char *c;
  char srch[20], t[256] = "";
  Plane *planeBuf[MAX_FACES];
  FaceTex *textureBuf[MAX_FACES];
  bool texLock;

  for(i = 0; i < MAX_FACES; i++) {
    planeBuf[i] = NULL;
    textureBuf[i] = NULL;
  }

  // Search for geometry.
  sprintf(srch, "{ // Brush %d", geomNum);

  if (geomNum != -1 && inFile->Search(srch) == NULL)
    goto ERROR_LOAD_GEOMETRY;

  numPlanes = 0;
  // Parse until end section of geometry.
  while (1) {
    char *line = inFile->GetNextLine();
    if (line == NULL)
      goto ERROR_LOAD_GEOMETRY;
    else if (strncmp(inFile->GetLine(), "}", 1) == 0)
      break;
    else if (strncmp(inFile->GetLine(), "//", 2) == 0)
      continue;
    else if (strncmp(inFile->GetLine(), "(", 1) == 0) {
      c = inFile->GetLine();
      if (sscanf(c, "( %f %f %f )", &f1, &f2, &f3) != 3)
        goto ERROR_LOAD_GEOMETRY;
      p1.NewVector(f1, f2, f3);
      if ((c = strchr(c + 1, '(')) == NULL)
        goto ERROR_LOAD_GEOMETRY;
      if (sscanf(c, "( %f %f %f )", &f1, &f2, &f3) != 3)
        goto ERROR_LOAD_GEOMETRY;
      p2.NewVector(f1, f2, f3);
      if ((c = strchr(c + 1, '(')) == NULL)
        goto ERROR_LOAD_GEOMETRY;
      if (sscanf(c, "( %f %f %f )", &f1, &f2, &f3) != 3)
        goto ERROR_LOAD_GEOMETRY;
      p3.NewVector(f1, f2, f3);

      c = strchr(c, ')') + 1;
      if (sscanf(c, "%s %d %d %f %f %f", t, &i1, &i2, &f1, &f2, &f3) == 6) {
        // skip 6 args.
        for(i = 0; i < 6; i++) {
          // Trim space from left.
          while (*c && isspace(*c))
            c++;
          // Skip all chars.
          while (*c && !isspace(*c))
            c++;
        }

        a1 = a2 = a3 = 0;
        int num = sscanf(c, "%d %d %d", &a1, &a2, &a3);

        // Hexen 2 hack.
        // -1's are appeneded in .map files for some unknown reason.
        if (num == 1 && a1 == -1)
          a1 = 0;

        texLock = ((c = strstr(c, "QLE_TexLock")) != NULL);
      }
	  else {
        i1 = i2 = 0;
        f1 = 0.0f;
        f2 = f3 = 1.0f;
        a1 = a2 = a3 = 0;
        texLock = false;
	  }

      textureBuf[numPlanes] = new FaceTex(t);
      textureBuf[numPlanes]->SetTInfo(i1, i2, f1, f2, f3);
      textureBuf[numPlanes]->SetTAttribs(a1, a2, a3);
      textureBuf[numPlanes]->SetTexLock(texLock);

      if (numPlanes > MAX_FACES)
        goto ERROR_LOAD_GEOMETRY;

      if(!(v1.SubVector(p2, p1)).IsParallel(v2.SubVector(p3, p1)))
        planeBuf[numPlanes++] = new Plane(p1, p2, p3);
    }
  }

  if (numPlanes < 4)
    goto ERROR_LOAD_GEOMETRY;

  rtnVal = NewBoundGeometry(numPlanes,
                            (const Plane **) planeBuf,
                            (const FaceTex **) textureBuf);
  if (rtnVal == NULL)
    goto ERROR_LOAD_GEOMETRY;

  // Make an edge list.  Speed up wireframe render time.
  rtnVal->ConstructEdgeList();

#if 0
  if (geomNum != -1) {
    // Make a list of planes for bounding sides.  Used for hit detection.
    // Should be done in NewBoundGeometry().
    // rtnVal->ConstructPlaneList();
  }
#endif

  goto RTN_LOAD_GEOMETRY;

ERROR_LOAD_GEOMETRY:
#if 0
  LError("Error loading brush\n");
#endif

  if (rtnVal != NULL)
    delete rtnVal;
  rtnVal = NULL;

RTN_LOAD_GEOMETRY:
  for (i = 0; i < numPlanes; i++) {
    delete planeBuf[i];
    if (textureBuf[i] != NULL)
      delete textureBuf[i];
  }

  return rtnVal;
}

typedef float vector_t	[3];
#define VECTORCOPY(a,b)				{(b)[0]=(a)[0];(b)[1]=(a)[1];(b)[2]=(a)[2];}
#define VECTORSUBTRACT(a,b,c) {(c)[0]=(a)[0]-(b)[0];(c)[1]=(a)[1]-(b)[1];(c)[2]=(a)[2]-(b)[2];}
#define VECTORADD(a,b,c) 			{(c)[0]=(a)[0]+(b)[0];(c)[1]=(a)[1]+(b)[1];(c)[2]=(a)[2]+(b)[2];}

void VectorScale(vector_t &v,float scalar)
{
	v[0] *= scalar;
	v[1] *= scalar;
	v[2] *= scalar;
}

float VectorLength(vector_t v)
{
	int		i;
	float	length;
	
	length = 0;
	for (i=0 ; i< 3 ; i++)
		length += v[i]*v[i];
	length = (float)sqrt (length);		// FIXME

	return length;
}

float VectorNormalize (vector_t &v)
{
	int		i;
	float	length;
	
	length = 0;
	for (i=0 ; i< 3 ; i++)
		length += v[i]*v[i];
	length = (float)sqrt (length);
	if (length == 0)
		return 0;
		
	for (i=0 ; i< 3 ; i++)
		v[i] /= length;	

	return length;
}

void
Geometry::WriteBoundGeometry(FILE *outFile, const Geometry &geom,
                             int geomNum, const Matrix44 &t, bool round) {
  // PreCondition.
  ASSERT(geomNum >= -1);

  int i, i1, i2;
  unsigned int a1, a2, a3;
  float f1, f2, f3;
  Vector3d pt1(1.0f, 0.0f, 0.0f);
  Vector3d pt2(0.0f, 1.0f, 0.0f);
  Vector3d pt3(0.0f, 0.0f, 1.0f);
  Plane plane(pt1, pt2, pt3);
  const Plane *pPlane;
  FaceTex *tPtr, tmpFace("");

  if (geomNum == -1)
    fprintf(outFile, " {\n");
  else
    fprintf(outFile, "{ // Brush %d\n", geomNum);

  // Sanity.
  ASSERT(geom.faces.NumOfElm() == geom.planeList.NumOfElm());

  for(i = 0; i < geom.faces.NumOfElm(); i++) {
    pPlane = &((geom.planeList[i]).GetPlane());
    plane = *pPlane;
    t.Transform(plane);
    if (round)
      plane.Round45();
    plane.Normalize(pt1, pt2, pt3);

    if (round) {
		// Jaimi
		// Scale the plane points far apart, so that the truncation error is less.
		vector_t v1,v2,v3,delta;
		v1[0] = pt1.GetX();
		v1[1] = pt1.GetY();
		v1[2] = pt1.GetZ();
		v2[0] = pt2.GetX();
		v2[1] = pt2.GetY();
		v2[2] = pt2.GetZ();
		v3[0] = pt3.GetX();
		v3[1] = pt3.GetY();
		v3[2] = pt3.GetZ();

		VECTORSUBTRACT(v1,v2,delta);
		VectorNormalize(delta);
		VectorScale(delta,4096.0f);
		VECTORADD(v1,delta,v2);

		VECTORSUBTRACT(v1,v3,delta);
		VectorNormalize(delta);
		VectorScale(delta,4096.0f);
		VECTORADD(v1,delta,v3);

		fprintf(outFile, "  ( %d %d %d ) ", ROUNDI(v1[0]), ROUNDI(v1[1]), ROUNDI(v1[2]));
		fprintf(outFile, "( %d %d %d ) ",   ROUNDI(v2[0]), ROUNDI(v2[1]), ROUNDI(v2[2]));
		fprintf(outFile, "( %d %d %d ) ",   ROUNDI(v3[0]), ROUNDI(v3[1]), ROUNDI(v3[2]));
		// End Modifications
    }
    else {
      fprintf(outFile, "  ( %s", FormatFloat(pt1.GetX()));
      fprintf(outFile, " %s", FormatFloat(pt1.GetY()));
      fprintf(outFile, " %s ) ", FormatFloat(pt1.GetZ()));
      fprintf(outFile, "( %s", FormatFloat(pt2.GetX()));
      fprintf(outFile, " %s", FormatFloat(pt2.GetY()));
      fprintf(outFile, " %s ) ", FormatFloat(pt2.GetZ()));
      fprintf(outFile, "( %s", FormatFloat(pt3.GetX()));
      fprintf(outFile, " %s", FormatFloat(pt3.GetY()));
      fprintf(outFile, " %s ) ", FormatFloat(pt3.GetZ()));
    }

    if ((tPtr = (geom.faces[i]).GetTexturePtr()) != NULL) {

      tPtr->GetTInfo(i1, i2, f1, f2, f3);
      if (geomNum == -1 && tPtr->IsTexLocked()) {
        // Transform the texture settings.
        tmpFace.SetTInfo(i1, i2, f1, f2, f3);
        tmpFace.Transform(t, *pPlane, &(plane.GetNorm()));
        tmpFace.GetTInfo(i1, i2, f1, f2, f3);
      }

	  char *texName = tPtr->GetTName();
	  if(!strlen(texName)) {
		  Texture *texture = Game::Get()->GetDefaultTexture();
		  if(texture)
			  texName = texture->GetName();
	  }
      fprintf(outFile, "%s %d %d %.4f %.4f %.4f",
              texName, i1, i2, f1, f2, f3);

      tPtr->GetTAttribs(a1, a2, a3);
      if (a1 || a2 || a3)
        fprintf(outFile, " %d %d %d", a1, a2, a3);

	  // Hexen 2 maps require this at the end...
	  if(!strcmp(Game::Get()->GetName(), "Hexen 2"))
		  fprintf(outFile, " -1");

      if (geomNum != -1 && tPtr->IsTexLocked())
        fprintf(outFile, " QLE_TexLock");
    }

    fprintf(outFile, "\n");
  }

  if (geomNum == -1)
    fprintf(outFile, " }\n");
  else
    fprintf(outFile, "} // Brush %d\n", geomNum);
}

//===== Construct Geometry =====

Plane *
Geometry::MakePlane(const GPolygon &poly) const {
  // PreCondition.
  ASSERT(faces.FindNodeIndex(&poly) != -1);

  Plane *rtnVal;
  Vector3d dVec, nVec, v0, v1;
  Vector3d aveVec(0.0f, 0.0f, 0.0f);
  const Edge3d *ePtr;
  int i, lb, le, mark;
  float len, maxLen, ang;

  mark = -1;
  maxLen = 0.0f;
  for(i = 0; i < poly.GetNumSides(); i++) {
    ePtr = &(poly.GetSide(i));
    lb = ePtr->GetStartPoint();
    le = ePtr->GetEndPoint();
    dVec.SubVector(vertices[le], vertices[lb]);
    if ((len = dVec.GetMag()) > maxLen) {
      maxLen = len;
      mark = i;
    }
    aveVec.AddVector(vertices[lb]);
  }

  if (mark == -1 || maxLen < EPSILON)
    return NULL;

  aveVec.MultVector(1.0f / poly.GetNumSides());

  ePtr = &(poly.GetSide(mark));
  v0.SubVector(vertices[ePtr->GetStartPoint()], aveVec);
  v1.SubVector(vertices[ePtr->GetEndPoint()], aveVec);

  len = EPSILON / 2.0f;
  if (v0.GetMag() < len || v1.GetMag() < len)
    return NULL;

  ang = v1.CosTheta(v0);
  if (EQUAL6(ABS(ang), 1.0f))
    return NULL;

  nVec.CrossVector(v1, v0);
  rtnVal = new Plane(nVec, aveVec);

  return rtnVal;
}

void
Geometry::ConstructEdgeList(void) {
  GPolygon *face;
  Edge3d *edge, *chkEdge;
  int f, e, i;

  // Clean.
  while (edgeList.NumOfElm() > 0)
    delete &(edgeList.RemoveNode(0));

  // Make up the edges.
  for(f = 0; f < faces.NumOfElm(); f++) {
    face = &(faces[f]);
    for(e = 0; e < face->GetNumSides(); e++) {
      edge = &(face->GetSide(e));
      // Check and see if edge already exist.
      for(i = 0; i < edgeList.NumOfElm(); i++) {
        chkEdge = &(edgeList[i]);
        if ((chkEdge->GetStartPoint() == edge->GetStartPoint() &&
             chkEdge->GetEndPoint() == edge->GetEndPoint()) ||
            (chkEdge->GetStartPoint() == edge->GetEndPoint() &&
             chkEdge->GetEndPoint() == edge->GetStartPoint())) {
          break;
        }
      }
      if (i == edgeList.NumOfElm()) {
        // Not in the list.  Add it.
        edgeList.AppendNode(*(new Edge3d(*edge)));
      }
    }
  }
}

void
Geometry::ConstructPlaneList(void) {
  IterLinkList<GPolygon> iter(faces);

  // Clean
  while (planeList.NumOfElm() > 0)
    delete &(planeList.RemoveNode(0));

  // Build the planes.
  iter.Reset();
  while (!(iter.IsDone())) {
    planeList.AppendNode(*(new PlaneNode(*(iter.GetNext()), vertices)));
  }
}

Geometry *
Geometry::NewBoundGeometry(int pNum, const Plane *pBuf[],
                           const FaceTex *tBuf[]) {
  // PreConditions.
  ASSERT(pNum <= MAX_FACES * 2 && pNum >= 4);

  Geometry *rtnVal = new Geometry();
  int p, e, i, j, k, ind;
  int sideEnds[MAX_FACES * 2];
  bool ok, dupBuf[MAX_FACES * 2];
  Line pe;
  int eBuf[256], eNum;
  Vector3d v0;
  Point3d pt;
  GPolygon *newPoly;
  float param, maxParam, minParam;
  int maxInd, minInd;
  PlaneNode *pNode;

  if (pNum < 4)
    goto ERROR_NEWGEOMETRY;

  // Check and mark duplicated planes.
  for(i = 0; i < pNum; i++)
    dupBuf[i] = false;

  for(i = 0; i < pNum; i++) {
    if (dupBuf[i])
      continue;
    for(j = i + 1; j < pNum; j++) {
      if (pBuf[i]->IsSame(*pBuf[j]))
        dupBuf[j] = true;
    }
  }

  for(p = 0; p < pNum; p++) {
    if (dupBuf[p])
      continue;

    eNum = 0;

    // Find all bounded intersections.
    for(e = 0; e < pNum; e++) {
      if (dupBuf[e] || e == p)
        continue;

      k = 0;
      for(i = 0; i < pNum; i++) {
        if (dupBuf[i] || i == p || i == e)
          continue;
        if (!pBuf[p]->Intersect(*pBuf[e], *pBuf[i], v0))
          continue;
        for(j = 0; j < pNum; j++) {
          if (dupBuf[j] || j == p || j == e || j == i)
            continue;
          if (pBuf[j]->PointInPlane(v0, 0.005f) > 0)
            break;
        }
        if (j < pNum)
          continue;

        // Found a point.
        pt.NewPos(v0.GetX(), v0.GetY(), v0.GetZ());
        if ((ind = (rtnVal->vertices).FindPoint(pt, 0.05f)) == -1)
          ind = (rtnVal->vertices).AddPoint(pt);
        for(j = 0; j < k && sideEnds[j] != ind; j++);
        if (j < k)
          continue;
        ASSERT(k < 64);  // Sanity.
        sideEnds[k++] = ind;
      }

      if (k < 2) {
        // Couldn't find the 2 bounding ends of the edge.
        // e isn't a bounding edge.
        continue;
      }

      if (k > 2) {
        // Can't form a logical polygon for face.
        // p isn't a bounding plane.
        eNum = 0;
        break;
      }

      // Found an edge.
      ok = pBuf[e]->Intersect(*pBuf[p], pe);
      // hmm.. Dont know what to do.
      if (!ok)
        continue;
      ASSERT(ok);  // Sanity.

      // Find min and max of parametrized pts.
      minInd = maxInd = sideEnds[0];
      minParam = maxParam = pe.Parametrize(rtnVal->vertices[sideEnds[0]]);
      for(j = 1; j < k; j++) {
        param = pe.Parametrize(rtnVal->vertices[sideEnds[j]]);
        if (param < minParam) {
          minParam = param;
          minInd = sideEnds[j];
        }
        else if (param > maxParam) {
          maxParam = param;
          maxInd = sideEnds[j];
        }
      }

      // Make sure same edge doesn't appear twice in eBuf.
      for(j = 0; j < eNum; j++) {
        if (eBuf[j*2] == minInd && eBuf[j*2+1] == maxInd)
          break;
      }

      if (j == eNum) {
        eBuf[eNum*2] = minInd;
        eBuf[eNum*2+1] = maxInd;
        eNum++;
      }
    }

    if (eNum < 3) {
      // Couldn't find bounding edges.
      // p is not a bounding plane.
      continue;
    }

    // New polygon.  Use dummy verticex indices.  Delete them later.
    newPoly = new GPolygon(0, 1, 2);

    k = eBuf[0];
    while (eNum > 0) {
      ok = false;
      for(i = 0; i < eNum; i++) {
        if (eBuf[i*2] != k)
          continue;
        j = k;
        k = eBuf[i*2+1];
        newPoly->AddVertex(j, newPoly->GetSide(newPoly->GetNumSides() - 1));
        eNum--;
        eBuf[i*2] = eBuf[eNum*2];
        eBuf[i*2+1] = eBuf[eNum*2+1];
        ok = true;
        break;
      }
      if (!ok) {
        // Couldn't find next link in eBuf.
        delete newPoly;
        goto ERROR_NEWGEOMETRY;
      }
    }

    // Sanity.
    ASSERT(k == (newPoly->GetSide(2)).GetEndPoint());

    // Remove the dummy points.
    newPoly->RemoveVertex(newPoly->GetSide(newPoly->GetNumSides() - 1),
                          newPoly->GetSide(0));
    newPoly->RemoveVertex(newPoly->GetSide(newPoly->GetNumSides() - 1),
                          newPoly->GetSide(0));
    newPoly->RemoveVertex(newPoly->GetSide(newPoly->GetNumSides() - 1),
                          newPoly->GetSide(0));

    newPoly->CopyTexture(tBuf[p]);
    (rtnVal->faces).AppendNode(*newPoly);
    pNode = new PlaneNode(*newPoly, *pBuf[p]);
    // pNode = new PlaneNode(*newPoly, rtnVal->vertices);
    (rtnVal->planeList).AppendNode(*pNode);
    // Sanity.
    ASSERT((rtnVal->faces).NumOfElm() == (rtnVal->planeList).NumOfElm());
  }

  if ((rtnVal->faces).NumOfElm() >= 4)
    goto RTN_NEWGEOMETRY;

ERROR_NEWGEOMETRY:
  delete rtnVal;
  rtnVal = NULL;

RTN_NEWGEOMETRY:
  return rtnVal;
}

//==================== WireFrameGeom ====================

WireFrameGeom::WireFrameGeom(int numVerts, GeomWFVertex *verts, LinkList<Edge3d> &edges) {
  numWFVertices = numVerts;
  wfVertices = verts;

  while(edges.NumOfElm())
	wfEdgeList.AppendNode(edges.RemoveNode(0));

  // Used for hit detection and wireframe approximation.
  approxBox = NULL;
  ConsApprxBox();
  boundRadius = approxBox->GetBoundRadius();

  uses = 0;
}

WireFrameGeom::WireFrameGeom(const char *filename) {
  char gem[4];
  short vertices, edges;
  gemVertex vertex[500];
  gemEdge edge[1000];
  int i;
  Edge3d *newEdge;

  // load
  LFile file;
  if(!file.Open(filename))
    LFatal("Missing gem file, %s", filename);

  file.Read(gem, 4, 1);
    if(strncmp(gem, "GEOM", 4))
      LFatal("Invalid gem file, %s", filename);

  file.Read(&vertices, sizeof(vertices), 1);
  file.Read(&edges, sizeof(edges), 1);

  file.Read(vertex, sizeof(gemVertex), vertices);
  file.Read(edge, sizeof(gemEdge), edges);

  file.Close();

  // construct
  numWFVertices = vertices;
  wfVertices = new GeomWFVertex[vertices];
  for(i = 0; i < vertices; i++) {
    wfVertices[i].x = (float)vertex[i].x / 2.0f;
    wfVertices[i].y = (float)vertex[i].y / 2.0f;
    wfVertices[i].z = (float)vertex[i].z / 2.0f;
  }

  for(i = 0; i < edges; i++) {
    newEdge = new Edge3d((int)edge[i].e1, (int)edge[i].e2);
    wfEdgeList.AppendNode(*newEdge);
  }

  // Used for hit detection and wireframe approximation.
  approxBox = NULL;
  ConsApprxBox();
  boundRadius = approxBox->GetBoundRadius();

  uses = 0;
}

WireFrameGeom::~WireFrameGeom(void) {
  while (wfEdgeList.NumOfElm() > 0)
    delete &(wfEdgeList.RemoveNode(0));

  delete wfVertices;

  // Sanity.
  ASSERT(approxBox != NULL);
  delete approxBox;
}

bool
WireFrameGeom::Uses(int diff) {
	uses += diff;
	if(uses <= 0) {
		delete this;
		return true;
	}
	return false;
}


//===== WireFrame Info =====
const LinkList<Edge3d> &
WireFrameGeom::GetWireFrameEdgeList(void) const {
  return wfEdgeList;
}

const LinkList<Edge3d> &
WireFrameGeom::GetApproxBoxEdgeList(void) const {
  // Sanity
  ASSERT(approxBox != NULL);

  return approxBox->GetEdgeList();
}

const LinkList<PlaneNode> &
WireFrameGeom::GetPlaneList(void) const {
  // Sanity
  ASSERT(approxBox != NULL);

  return approxBox->GetPlaneList();
}

//===== Hit Detection and Bounding Info: Wrappers =====

float
WireFrameGeom::GetBoundRadius(void) const {
  return boundRadius;
}

bool
WireFrameGeom::HitGeometry(const Line &ray, float rangeBegin,
                           float rangeEnd, float &depth) const {
  // Sanity.
  ASSERT(approxBox != NULL);

  return approxBox->HitGeometry(ray, rangeBegin, rangeEnd, depth);
}

void
WireFrameGeom::FindTransBoundBox(const Matrix44 trans,
                                 Vector3d &minVec, Vector3d &maxVec) const {
  // Sanity.
  ASSERT(numWFVertices > 3);

  int i;
  Vector3d vec;
  const GeomWFVertex *vertex;

  vertex = &(wfVertices[0]);
  trans.Transform(vec, vertex->x, vertex->y, vertex->z);
  minVec = maxVec = vec;

  for(i = 1; i < numWFVertices; i++) {
    vertex = &(wfVertices[i]);
    trans.Transform(vec, vertex->x, vertex->y, vertex->z);
    minVec.NewVector(Min(minVec.GetX(), vec.GetX()),
                     Min(minVec.GetY(), vec.GetY()),
                     Min(minVec.GetZ(), vec.GetZ()));
    maxVec.NewVector(Max(maxVec.GetX(), vec.GetX()),
                     Max(maxVec.GetY(), vec.GetY()),
                     Max(maxVec.GetZ(), vec.GetZ()));
  }
}

void
WireFrameGeom::FindSelfDimen(Vector3d &minVec, Vector3d &maxVec) const {
  // Sanity.
  ASSERT(numWFVertices > 3);

  int i;
  const GeomWFVertex *vertex;
  Vector3d vec;

  vertex = &(wfVertices[0]);
  vec.NewVector(vertex->x, vertex->y, vertex->z);
  minVec = maxVec = vec;

  for(i = 1; i < numWFVertices; i++) {
    vertex = &(wfVertices[i]);
    minVec.NewVector(Min(minVec.GetX(), vertex->x),
                     Min(minVec.GetY(), vertex->y),
                     Min(minVec.GetZ(), vertex->z));
    maxVec.NewVector(Max(maxVec.GetX(), vertex->x),
                     Max(maxVec.GetY(), vertex->y),
                     Max(maxVec.GetZ(), vertex->z));
  }
}

//===== WireframeGeom Transformation =====
Points3d &
WireFrameGeom::TransformWireFrame(const Matrix44 &transMatrix,
                                  Points3d &vBuffer) const {
  const GeomWFVertex *vertex;
  int i;

  // Check if vBuffer is empty.
  if (vBuffer.numElm == 0) {
    for(i = 0; i < vBuffer.arraySize; i++)
      vBuffer.array[i] = new Point3d;
    vBuffer.numElm = vBuffer.arraySize;
  }

  // Sanity.
  ASSERT(vBuffer.numElm == vBuffer.arraySize);

  // Check if vBuffer is big enough.
  if (vBuffer.arraySize < numWFVertices) {
    i = vBuffer.arraySize;
    vBuffer.arraySize = numWFVertices;
    vBuffer.array = (Point3d **)
      realloc(vBuffer.array, sizeof(Point3d *) * vBuffer.arraySize);
    for(; i < vBuffer.arraySize; i++)
      vBuffer.array[i] = new Point3d;
    vBuffer.numElm = vBuffer.arraySize;
  }

  // Transform.
  for(i = 0; i < numWFVertices; i++) {
    // Sanity.
    ASSERT(vBuffer.array[i] != NULL);

    vertex = &(wfVertices[i]);
    transMatrix.Transform(*(vBuffer.array[i]),
                          vertex->x, vertex->y, vertex->z);
  }

  return vBuffer;
}

Points3d &
WireFrameGeom::TransformApproxBox(const Matrix44 &transMatrix,
                                  Points3d &vBuffer) const {
  // Sanity.
  ASSERT(approxBox != NULL);
  return approxBox->Transform(transMatrix, vBuffer);
}

//===== Init: Approximate Bound Box =====

void
WireFrameGeom::ConsApprxBox(void) {
  int i, boxVert[8];
  float xMin, yMin, zMin;
  float xMax, yMax, zMax;
  const GeomWFVertex *vertex;
  GPolygon *newPoly;
  Point3d pt;

  // Create the geometry.
  approxBox = new Geometry();

  // Sanity.
  ASSERT(numWFVertices > 0);

  // Find bounding vectors.
  vertex = &(wfVertices[0]);
  xMin = xMax = (vertex->x);
  yMin = yMax = (vertex->y);
  zMin = zMax = (vertex->z);

  for(i = 1; i < numWFVertices; i++) {
    vertex = &(wfVertices[i]);
    xMin = Min(xMin, vertex->x);
    yMin = Min(yMin, vertex->y);
    zMin = Min(zMin, vertex->z);
    xMax = Max(xMax, vertex->x);
    yMax = Max(yMax, vertex->y);
    zMax = Max(zMax, vertex->z);
  }

  // Add the vertices to bounding box.
  boxVert[0] = approxBox->vertices.AddPoint(pt.NewPos(xMax, yMax, zMax));
  boxVert[1] = approxBox->vertices.AddPoint(pt.NewPos(xMin, yMax, zMax));
  boxVert[2] = approxBox->vertices.AddPoint(pt.NewPos(xMin, yMin, zMax));
  boxVert[3] = approxBox->vertices.AddPoint(pt.NewPos(xMax, yMin, zMax));
  boxVert[4] = approxBox->vertices.AddPoint(pt.NewPos(xMax, yMax, zMin));
  boxVert[5] = approxBox->vertices.AddPoint(pt.NewPos(xMin, yMax, zMin));
  boxVert[6] = approxBox->vertices.AddPoint(pt.NewPos(xMin, yMin, zMin));
  boxVert[7] = approxBox->vertices.AddPoint(pt.NewPos(xMax, yMin, zMin));

  // Create Bounding planes.
  newPoly = new GPolygon(boxVert[3], boxVert[2], boxVert[1]);
  newPoly->AddVertex(boxVert[0], newPoly->GetSide(2));
  approxBox->faces.AppendNode(*newPoly);

  newPoly = new GPolygon(boxVert[4], boxVert[5], boxVert[6]);
  newPoly->AddVertex(boxVert[7], newPoly->GetSide(2));
  approxBox->faces.AppendNode(*newPoly);

  newPoly = new GPolygon(boxVert[0], boxVert[1], boxVert[5]);
  newPoly->AddVertex(boxVert[4], newPoly->GetSide(2));
  approxBox->faces.AppendNode(*newPoly);

  newPoly = new GPolygon(boxVert[1], boxVert[2], boxVert[6]);
  newPoly->AddVertex(boxVert[5], newPoly->GetSide(2));
  approxBox->faces.AppendNode(*newPoly);

  newPoly = new GPolygon(boxVert[2], boxVert[3], boxVert[7]);
  newPoly->AddVertex(boxVert[6], newPoly->GetSide(2));
  approxBox->faces.AppendNode(*newPoly);

  newPoly = new GPolygon(boxVert[3], boxVert[0], boxVert[4]);
  newPoly->AddVertex(boxVert[7], newPoly->GetSide(2));
  approxBox->faces.AppendNode(*newPoly);

  // Init the geometry.
  approxBox->ConstructEdgeList();
  approxBox->ConstructPlaneList();
}

void
WireFrameGeom::GetMinMax(Vector3d &minVec, Vector3d &maxVec) const {
  int i;
  float xMin, yMin, zMin;
  float xMax, yMax, zMax;
  const GeomWFVertex *vertex;
  Point3d pt;

  // Sanity.
  ASSERT(numWFVertices > 0);

  // Find bounding vectors.
  vertex = &(wfVertices[0]);
  xMin = xMax = (vertex->x);
  yMin = yMax = (vertex->y);
  zMin = zMax = (vertex->z);

  for(i = 1; i < numWFVertices; i++) {
    vertex = &(wfVertices[i]);
    xMin = Min(xMin, vertex->x);
    yMin = Min(yMin, vertex->y);
    zMin = Min(zMin, vertex->z);
    xMax = Max(xMax, vertex->x);
    yMax = Max(yMax, vertex->y);
    zMax = Max(zMax, vertex->z);
  }

  minVec.SetX(xMin);
  minVec.SetY(yMin);
  minVec.SetZ(zMin);
  maxVec.SetX(xMax);
  maxVec.SetY(yMax);
  maxVec.SetZ(zMax);
}
