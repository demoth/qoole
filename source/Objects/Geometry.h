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
 * geometry.h
 */

#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

#include <stdio.h>
#include "list.h"
#include "math3d.h"
#include "LCommon.h"
#include "Texture.h"

//==================== Point3d ====================

class Point3d : public Vector3d {
 public:
  Point3d(float xPos = 0.0, float yPos = 0.0, float zPos = 0.0);
  Point3d(const Point3d &src) { *this = src; };
  // ~Point3d(void);
  // Point3d &operator=(const Point3d &src);

  Point3d &NewPos(float xPos, float yPos, float zPos);
  Point3d &MovePoint(float dx, float dy, float dz);

  inline float GetX(void) const { return Vector3d::GetX(); };
  inline float GetY(void) const { return Vector3d::GetY(); };
  inline float GetZ(void) const { return Vector3d::GetZ(); };

  inline void SetX(float val) { Vector3d::SetX(val); };
  inline void SetY(float val) { Vector3d::SetY(val); };
  inline void SetZ(float val) { Vector3d::SetZ(val); };

  float GetDist(Point3d &pt) const;
  float GetDistSqr(Point3d &pt) const;
 private:
};

//==================== Points3d ====================

class Points3d {
  friend class Geometry;
  friend class WireFrameGeom;

 public:
  Points3d(int initSize = 10);
  Points3d(Points3d &src);
  virtual ~Points3d(void);

  Points3d &operator=(const Points3d &src);

  inline int GetNumPoints(void) const { return numElm; };
  inline int GetArraySize(void) const { return arraySize; };

  int     AddPoint(const Point3d &point);
  void    DeletePoint(int index);

  Point3d &GetPoint(int index) const;
  Point3d &operator[](int index) const { return GetPoint(index); };

  int FindPoint(float xPos, float yPos, float zPos,
                float delta, int startIndex = 0) const;
  int FindPoint(Point3d &pt, float delta, int startIndex = 0) const;

 private:
  int numElm;
  int arraySize;
  Point3d **array;
};

//==================== Edge3d ====================

class Edge3d : public ElmNode {
 public:
  Edge3d(int startPoint, int endPoint) { SetLine(startPoint, endPoint); };
  Edge3d(Edge3d &src) { *this = src; };
  // ~Edge3d(void);
  // Edge3d &operator=(const Edge3d &src);

  inline int GetStartPoint(void) const { return p1; };
  inline int GetEndPoint(void) const { return p2; };
  Edge3d &SetLine(int startPoint, int endPoint);

 private:
  int p1, p2;
};

//==================== FaceTex ==========

class Texture;

class FaceTex {
 public:
  FaceTex(const char *tName = NULL);
  FaceTex(const FaceTex &src);
  virtual ~FaceTex(void);
  FaceTex &operator=(const FaceTex &src);

  Texture *GetTexture(void) const { return texture; }
  char *GetTName(void) const;
  void GetTInfo(int &xOff, int &yOff, float &rotAng,
                float &xScale, float &yScale) const;
  void GetTAttribs(unsigned int &a1, unsigned int &a2, unsigned int &a3) const;
  void GetTDim(int &width, int &height) const;

  void SetTexture(Texture *_texture) { texture = _texture; }
  void SetTName(const char *tName);
  void SetTInfo(int xOff, int yOff, float rotAng, float xScale, float yScale);
  void SetTAttribs(unsigned int a1, unsigned int a2, unsigned int a3);
  bool IsTexLocked(void) const { return lockTex; }
  void SetTexLock(bool lock) { lockTex = lock; }

  Vector3d &ST2XYZ(const Plane &plane, float s, float t, Vector3d &pt) const;
  void XYZ2ST(const Vector3d &norm, const Vector3d &pt, float &s, float &t) const;
  void Transform(const Matrix44 &trans, const Plane &texPlane,
                 const Vector3d *pEndNormVec = NULL);

  static int GetAxisAlignIndex(const Vector3d &norm);
  static void Project2D(const Vector3d &normVec,
                        const Vector3d &pt, float &s, float &t);

  static void DeriveCoordSys(const Vector3d &origVec, const Vector3d &sBasisVec,
                             const Vector3d &tBasisVec, const Vector3d &normVec,
                             float &xOffset, float &yOffset, float &rotAng,
                             float &xScale, float &yScale);
 private:
  Texture *texture;
  int xo, yo;
  float ra, xs, ys;
  unsigned int atr1, atr2, atr3;
  bool lockTex;
};

//==================== GPolygon ====================

class GPolygon : public ElmNode {
 public:
  GPolygon(int pt1, int pt2, int pt3);
  GPolygon(const GPolygon &src);
  virtual ~GPolygon(void);
  GPolygon &operator=(const GPolygon &src);

  int GetNumSides(void) const;
  Edge3d &GetSide(int index) const;
  int GetEdgeIndex(int vIndx) const;
  const LinkList<Edge3d> &GetEdgeList(void) const { return edges; };

  Edge3d &AddVertex(int newPt, Edge3d &line);
  void RemoveVertex(Edge3d &line1, Edge3d &line2);
  void RemoveVertex(int eIndx, int &vBefore, int &vAfter);
  void DeleteSide(Edge3d &linePtr);

  void InvertOrientation(void);

  FaceTex *GetTexturePtr(void) const { ASSERT(t != NULL); return t; };
  void SetTexturePtr(FaceTex *tPtr);
  void CopyTexture(const FaceTex *tPtr);

 private:
  LinkList<Edge3d> edges;
  FaceTex *t;
};

//==================== PlaneNode ====================

class PlaneNode : public ElmNode {
 public:
  PlaneNode(const GPolygon &poly, const Plane &planeNorm);
  PlaneNode(const GPolygon &poly, const Points3d &vertices);
  PlaneNode(const PlaneNode &src);
  virtual ~PlaneNode(void);
  PlaneNode &operator=(const PlaneNode &src);

  const GPolygon &GetPolygon(void) const { return *polygon; }
  const Plane &GetPlane(void) const { return *plane; };
  void TransformPlane(const Matrix44 &trans) { trans.Transform(*plane); };
  void SetNewPlane(const Vector3d &nVec, const Vector3d &pVec);

 private:
  const GPolygon *polygon;
  Plane *plane;
};

//==================== Geometry ====================

class Geometry : public ElmNode {
 friend class WireFrameGeom;

 public:
  Geometry(void);
  Geometry(const Geometry &src) { *this = src; };
  virtual ~Geometry(void);

  Geometry &operator=(const Geometry &src);

  //===== Geometry Info =====
  int GetNumVertices(void) const { return vertices.GetNumPoints(); };
  const Vector3d &GetVertex(int vIndex) const { return vertices.GetPoint(vIndex); };

  int GetNumEdges(void) const { return edgeList.NumOfElm(); };
  const LinkList<Edge3d> &GetEdgeList(void) const { return edgeList; };

  int GetNumFaces(void) const { return faces.NumOfElm(); };
  GPolygon &GetFace(int index) const { return faces[index]; };
  const LinkList<GPolygon> &GetFaceList(void) const { return faces; };
  const LinkList<PlaneNode> &GetPlaneList(void) const { return planeList; };

  //===== Texture for Brush =====
  FaceTex *GetFaceTexturePtr(int faceIndex);
  void SetFaceTexture(int faceIndex, FaceTex *newTexturePtr);
  void NewFaceTexture(int faceIndex, const char *texture);
  void NewGeomTexture(const char *texture);

  //===== Geometry Modification =====
  void BreakPlanes(int vertex);
  void BreakPlanes(const Edge3d &edge);
  void BreakPlanes(const GPolygon &face);

  void MoveVertex(int vIndex, const Vector3d &deltaPos);
  void MoveEdge(const Edge3d &edge, const Vector3d &deltaPos);
  void MoveFace(const GPolygon &face, const Vector3d &deltaPos);

  bool IntegrityCheck(void);

  //===== Hit Detection and Bounding Info =====
  float GetBoundRadius(void);
  bool  HitGeometry(const Line &ray, float rangeBegin,
                    float rangeEnd, float &depth) const;
  void FindTransBoundBox(const Matrix44 trans,
                         Vector3d &minVec, Vector3d &maxVec) const;
  void FindSelfDimen(Vector3d &minVec, Vector3d &maxVec) const;

  //===== Geometry Transformation =====
  void Transform(const Matrix44 &transMatrix, bool mirror = false);
  Points3d &Transform(const Matrix44 &transMatrix,
                      Points3d &vBuffer) const;
  Points3d &TransformVertexPts(const Matrix44 &transMatrix,
                               Points3d &vBuffer, int vertex) const;
  Points3d &TransformEdgePts(const Matrix44 &transMatrix,
                      Points3d &vBuffer, const Edge3d *edge) const;
  Points3d &TransformFacePts(const Matrix44 &transMatrix,
                      Points3d &vBuffer, const GPolygon *face) const;

  //===== Geometry Adjustment =====
  Vector3d &CenterGeometry(Vector3d &position);

  //===== Hollow =====
  Geometry *CreateExpandedGeom(float delta) const;
  LinkList<Geometry> *HollowInward(float delta) const;

  //===== CSG Operations =====
  static LinkList<Geometry> *CSGSubtract(const Geometry &cuttee,
                                         const Geometry &cutter);
  static Geometry *CSGIntersect(const Geometry &geom1,
                                const Geometry &geom2);

  //===== Plane Clip =====
  Geometry *PlaneClip(const Plane &clipPlane);

  //===== Used for leak brush generation =====
  static Geometry *MakeSegment(const Vector3d &pt1,
                               const Vector3d &pt2, float width = 2.0);

  //===== Read / Write Geometry =====
  static Geometry *ReadBoundGeometry(LFile *inFile, int geomNum = -1);
  static void WriteBoundGeometry(FILE *outFile, const Geometry &geom,
                                 int geomNum, const Matrix44 &t,
                                 bool round = false);

 private:
  //===== Hit Detection =====
  float CalBoundRadius(void) const;

  //===== Construct Geometry =====
  Plane *MakePlane(const GPolygon &poly) const;
  void ConstructEdgeList(void);
  void ConstructPlaneList(void);

  static Geometry *NewBoundGeometry(int pNum, const Plane *pBuf[],
                                    const FaceTex *tBuf[]);

  float boundRadius;
  Points3d vertices;
  LinkList<Edge3d> edgeList;
  LinkList<GPolygon> faces;
  LinkList<PlaneNode> planeList;
};

//==================== GeometryPtr ====================

class GeometryPtr : public ElmNode {
 public:
  GeometryPtr(Geometry *geoPtr, int data) { ptr = geoPtr;  rec = data; };
  Geometry *GetPtr(void) const { return ptr; };
  int GetRec(void) const { return rec; };

 private:
  Geometry *ptr;
  int rec;
};


//==================== WireFrameGeometry ====================

typedef struct {
  float x, y, z;
} GeomWFVertex;

struct gemVertex {
  char x, y, z;
};

struct gemEdge {
  unsigned char e1, e2;
};

class WireFrameGeom {
 public:
  WireFrameGeom(int numVerts, GeomWFVertex *verts, LinkList<Edge3d> &edges);
  WireFrameGeom(const char *filename);
  virtual ~WireFrameGeom(void);

  //== Shouldn't be called ==
  WireFrameGeom(const WireFrameGeom &src)  { *this = src; };
  WireFrameGeom &operator=(const WireFrameGeom &src)
     { LFatal(); return *this; };

  //===== WireFrame Info =====
  const LinkList<Edge3d> &GetWireFrameEdgeList(void) const;
  const LinkList<Edge3d> &GetApproxBoxEdgeList(void) const;

  //===== Solid Plane Info =====
  const LinkList<PlaneNode> &GetPlaneList(void) const;

  //===== Hit Detection and Bounding Info: Wrappers =====
  float GetBoundRadius(void) const;
  bool  HitGeometry(const Line &ray, float rangeBegin,
                    float rangeEnd, float &depth) const;
  void FindTransBoundBox(const Matrix44 trans,
                         Vector3d &minVec, Vector3d &maxVec) const;
  void FindSelfDimen(Vector3d &minVec, Vector3d &maxVec) const;

  //===== WireframeGeom Transformation =====
  Points3d &TransformWireFrame(const Matrix44 &transMatrix,
                               Points3d &vBuffer) const;
  Points3d &TransformApproxBox(const Matrix44 &transMatrix,
                               Points3d &vBuffer) const;

  void GetMinMax(Vector3d &minVec, Vector3d &maxVec) const;

  bool Uses(int diff);

 private:
  void ConsApprxBox(void);

  int numWFVertices;
  GeomWFVertex *wfVertices;
  LinkList<Edge3d> wfEdgeList;
  Geometry *approxBox;
  float boundRadius;

  int uses;
};

#endif // _GEOMETRY_H_
