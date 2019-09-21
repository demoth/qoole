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
 * objects.h
 */

#ifndef _OBJECTS_H_
#define _OBJECTS_H_

#include <stdio.h>
#include "list.h"
#include "math3d.h"
#include "geometry.h"
#include "entity.h"
#include "LCommon.h"

//==================== Object ====================

// Foward Declaration
class ObjectPtr;

class Object : public ElmNode, public TransSpace {
 public:
  Object(const char *className = NULL);
  Object(const Object &src);
  virtual ~Object(void);

  Object &operator=(const Object &src);

  //===== tree traversal =====
  bool   IsRoot(void) const;
  Object &GetRoot(void);
  bool   IsLeaf(void) const;

  Object *GetParentPtr(void) const;
  Object &GetParent(void) const;
  Object &SetParentPtr(Object *parentPtr);
  Object &SetParent(Object &parentObj);

  int    GetNumChildren(void) const;
  Object &AddChild(Object &childNode, bool resetBound = true,
				   int index = -1);
  Object &RemoveChild(Object &childNode, bool resetBound = true,
					  int *pIndex = NULL);
  Object &GetChild(int childNum) const;
  IterLinkList<Object> &GetIterChildren(void);
  int    FloatChildren(bool resetBound = true);
  bool   IsMyChild(const Object &node) const;
  bool   IsMyAncestor(const Object &node) const;
  Object *FindCommonAncestor(Object &node);

  int CountNodes(int cType = 0) const;
  int CountItems(void) const { return CountNodes(1); };
  int CountBrushes(void) const { return CountNodes(2); };
  int CountEntities(void) const { return CountNodes(3); };

  int PreOrderApply(bool (*func)(Object &obj));
  int PreOrderCondApply(bool (*func)(Object &obj));
  int PostOrderApply(bool (*func)(Object &obj));

  //===== Object Name =====
  const char *GetObjName(void) const;
  void SetObjName(const char *name);
  bool IsTreeAttribSet(void) const { return treeAttrib; };
  void SetTreeAttrib(bool tv) { treeAttrib = tv; };

  //===== Brush Manipulation =====
  bool HasBrush(void) const;
  Geometry &GetBrush(void) const;
  Geometry *SetBrush(Geometry *brushPtr);
  bool HollowInBrush(float delta = 8.0f);
  bool HollowOutBrush(float delta = 8.0f);

  //===== Entity Info =====
  bool HasEntity(void) const;
  Entity *GetEntityPtr(bool bubble = false) const;
  Entity *SetEntity(Entity *ePtr, bool resetBound = true);
  bool IsItemNode(void) const;
  bool IsModelNode(void) const;
  const WireFrameGeom &GetItemWireFrame(void) const;

  //===== Hit Detection =====
  float GetBoundRadius(void) const { return boundRadius; };
  float SetBoundRadius(bool bubble = true);
  bool  HitObject(const Line &ray, float rangeBegin,
                  float rangeEnd, float &depth) const;
  static bool ResetBoundRadius(Object &obj);

  //===== Object Dimension =====
  Vector3d &AutoCenterByBound(Vector3d &centerVec);
  void GetObjDimension(Vector3d &dimVec);
  void GetBoundingVectors(Vector3d &minVec, Vector3d &maxVec);
  Vector3d GetMinBoundVec(void);
  Vector3d GetMaxBoundVec(void);

  //===== Transformation =====
  const Matrix44 &GetTransformation(void) const { return absTransMatrix; };
  const Matrix44 &SetTransformation(const Matrix44 &transMatrix);
  const Matrix44 &GetInvTransformation(void) const { return invTransMatrix; };
  const Matrix44 &SetInvTransformation(const Matrix44 &transMatrix);
  void Transform(const Matrix44 &transMatrix,
                 bool resetBound = true, bool mirrorEffect = false);

  static void GetTransMatrix(Object &startObj,
	                         Object &endObj, Matrix44 &trans);
  
  //===== TransSpace wrapper =====
  const Vector3d &SetPosition(const Vector3d &newPos);
  const Vector3d &MoveRelPosition(const Vector3d &posDelta);
  const Vector3d &SetOrientation(const Vector3d &frontVec,
                                 const Vector3d &topVec);
  const SphrVector &SetOrientation(const SphrVector &orientVec);
  const SphrVector &ChangeRelOrientation(const SphrVector &deltaVec);
  const Vector3d &SetScale(const Vector3d &newScale);
  const Vector3d &ChangeScale(const Vector3d &scaleDelta);

  //===== CSG Operations =====
  bool CSGSubtract(const LinkList<ObjectPtr> &cutters);
  Object *CSGIntersect(const LinkList<ObjectPtr> &objPtrs) const;

  //===== Plane Clipping =====
  bool PlaneClip(const Plane &clipPlane, bool bubble = true);

  //===== Load/Save =====
  static Object *CreateObjFromFile(const char *fName,
		int (*ErrorPromptFunc)(const char *message) = NULL,
        bool (*ProgressFunc)(int percent) = NULL);

  bool LoadObjFile(const char *fName,
                   int (*ErrorPromptFunc)(const char *message) = NULL,
                   bool (*ProgressFunc)(int percent) = NULL);
  bool SaveObjFile(const char *fName, bool append = false,
                   bool (*ProgressFunc)(int percent) = NULL);

  //===== Testing.  Loads a wireframed geometry. =====
  // static Object *LoadWFGeom(const GeomWFInfo &infoRec);

  inline void SetSelected(bool bSel) { bSelected = bSel; }
  inline bool IsSelected(void) { return bSelected; }

 private:
  //===== Clear self =====
  void DelObj(bool rmFrmPrnt = true, bool resetBound = true);

  //===== Object Dimension =====
  void GetBoundBox(Vector3d &minVec, Vector3d &maxVec);

  //===== CSG Operations =====
  bool CSGObjNodeSubtract(Object &cutter, bool resetBound = false);
  bool CSGTraverseCutter(Object &cuttee);
  static bool CSGBrushSubtract(Object &cuttee, Object &cutter);
  Geometry *CSGSelfIntersect(const LinkList<ObjectPtr> *pObjsPtrs) const;


  //===== Load / Save =====
  int  LoadObject(LFile *inFile, int objNum,
                  LinkList<GeometryPtr> &shapes,
                  LinkList<EntityPtr> &entities,
                  int &pDone, int &pTotal,
                  bool (*ProgressFunc)(int percent));

  void AssignObjIds(int &brushCnt, int &entCnt, int &objCnt);
  int  SaveObject(FILE *outFile, int &pDone, int &pTotal,
                  bool (*ProgressFunc)(int percent));
  int  SaveBrushes(FILE *outFile, int &pDone, int &pTotal,
                   bool (*ProgressFunc)(int percent));
  int  SaveEntities(FILE *outFile, int &pDone, int &pTotal,
                    bool (*ProgressFunc)(int percent));

  Object *parent;
  LinkList<Object> children;
  IterLinkList<Object> *iterChildrenPtr;
  float boundRadius;

  char objName[32];
  Geometry *brush;
  Entity *entityPtr;
  bool viewTag;
  bool itemApprx;
  bool treeAttrib;

  int objID, brushID, entID;
  bool bSelected;

  Matrix44 absTransMatrix;
  Matrix44 invTransMatrix;
};

//==================== ObjectPtr ====================

class ObjectPtr : public ElmNode {
 public:
  ObjectPtr(Object *objPtr, float data = 0.0f) { ptr = objPtr; rec = data; };
  Object *GetPtr(void) const { return ptr; };
  float GetRec(void) const { return rec; };
 private:
  Object *ptr;
  float rec;
};

#endif // _OBJECTS_H_
