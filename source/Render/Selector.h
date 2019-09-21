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
 * selector.h
 */

#ifndef _SELECTOR_H_
#define _SELECTOR_H_

#include "LCommon.h"
#include "objects.h"

//==================== Selector ====================

class Selector : public ElmNode {
 public:
  Selector(Object &scope);
  virtual ~Selector(void);
  // fatal error.  shouldn't be called.
  Selector(const Selector &src) { *this = src; };
  Selector &operator=(const Selector &src) { return *this; LFatal(); };

  //===== Get Scope Info =====
  Object &GetScope(void) const { ASSERT(scopePtr != NULL); return *scopePtr; };
  Object *GetScopePtr(void) const { return scopePtr; };

  //===== Object Selection Testing =====
  bool IsSelectionHit(Object *sObjPtr, const Line &slctRay,
                      float rBgn, float rEnd, float &depth) const;

  //===== Single Object Selection =====
  Object *GetSSelectedObject(void) const { return sSelectedObjPtr; };

  Object *SSelectObject(Object *sObjPtr);
  Object *GetSNextSelect(const Line &slctRay, float rBgn, float rEnd) const
    { return GetSelection(sSelectedObjPtr, slctRay, rBgn, rEnd, 1); }
  Object *GetSPrevSelect(const Line &slctRay, float rBgn, float rEnd) const
    { return GetSelection(sSelectedObjPtr, slctRay, rBgn, rEnd, 0); }
  void SUnselect(void) { sSelectedObjPtr = NULL; };

  //===== Multiple Object Selection =====
  bool IsObjectMSelected(Object *sObjPtr) const;
  int GetNumMSelectedObjects(void) const
    { return mSelectedObjs.NumOfElm(); };
  const LinkList<ObjectPtr> &GetMSelectedObjects(void) const
    { return mSelectedObjs; };

  bool MSelectObject(Object *sObjPtr);
  Object *GetMNextSelect(const Line &slctRay, float rBgn, float rEnd) const;
  Object *GetMPrevSelect(const Line &slctRay, float rBgn, float rEnd) const;
  void MSelectObjects(const LinkList<ObjectPtr> &sObjPtrs);
  void MUnselect(Object *sObjPtr = NULL);

  //===== Get the average position of selected objects.
  Vector3d &GetMSelectedObjectsCenter(Vector3d &centerPos) const;

  //===== Face Selection for Brush Object =====
  void ResetSelectFace(const Vector3d &viewVec);
  int GetSelectFaceIndex(void) const { return faceSlctIndx; }
  void SelectFaceIndex(int fIndex);
  GPolygon *GetSelectFace(void) const;
  GPolygon *ForwardSelectFace(const Vector3d &viewVec)
    { return CycleSelectFace(viewVec, 1); }
  GPolygon *BackwardSelectFace(const Vector3d &viewVec)
    { return CycleSelectFace(viewVec, -1); }

  FaceTex *GetSelectedFaceTex(void);
  static bool CheckTexture(Object &obj);

 private:
  Object *GetSelection(Object *sObjPtr, const Line &slctRay,
                       float rBgn, float rEnd, int direction) const;
  Object *FindMultiSlctBaseObj(const Line &slctRay, float rBgn, float rEnd,
                               int direction) const;
  GPolygon *CycleSelectFace(const Vector3d &viewVec, int direction);

  Object *scopePtr, *sSelectedObjPtr;
  LinkList<ObjectPtr> mSelectedObjs;
  int faceSlctIndx;

  static FaceTex *checkFaceTex;
  static Texture *checkTexture;
  static bool checkFailed;
};

#endif //_SELECTOR_H_
