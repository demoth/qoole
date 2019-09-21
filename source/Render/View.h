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
 *  View.h
 */

#ifndef _VIEW_H_
#define _VIEW_H_

#include "QDraw.h"
#include "math.h"
#include "clip.h"
#include "objects.h"
#include "selector.h"

//==================== View ====================
// Renders Objects.

// Used for setting rendering colors

#define VRC_VIEW            0
#define VRC_NORMAL          1
#define VRC_ENTITY          2
#define VRC_SELECT          3
#define VRC_HOTSELECT       4
#define VRC_MANIPDOT        5
#define VRC_FACESELECT      6
#define VRC_CROSSHAIR       7
#define VRC_CROSSHAIRNORTH  8
#define VRC_OPCENTER		9
#define VRC_LEAKGEOM        10

#define VRC_NUMRENDERCOLORS	11

class View : public ElmNode, public TransSpace {
 public:
  View(QDraw *qdraw, Object &viewObject, const Selector &selector);
  View(const View &src) { *this = src; };
  virtual ~View(void);
  View &operator=(const View &src);
       
  // Info
  Object *GetViewObjPtr(void) { return viewObj; };

  void SetQDraw(QDraw *pQDraw) { qdraw = pQDraw; }

  //===== View Settings =====
  void SetNewView(Object &viewObject, const Selector &selector,
                  bool reset = true, Vector3d *posVec = NULL,
                  SphrVector *orientVec = NULL, bool perspectiveView = false,
                  int windowWidth = 0, float viewDepth = 0.0f);

  void SetPerspective(bool perspectiveView);
  void SetWindowWidth(int width);
  float GetClipDepth(void) const;
  void SetClipDepth(float depth);

  // Graphics routines.
  void MapViewSpaceOrigin(float &scrnX, float &scrnY);

  //===== Object Selection =====
  bool IsMultiSelectionHit(float scrnX, float scrnY) const;

  Object *SingleForwardSelect(float scrnX, float scrnY) const
    { return SelectObject(scrnX, scrnY, 0); };
  Object *SingleReverseSelect(float scrnX, float scrnY) const
    { return SelectObject(scrnX, scrnY, 1); };
  Object *MultiForwardSelect(float scrnX, float scrnY) const
    { return SelectObject(scrnX, scrnY, 2); };
  Object *MultiReverseSelect(float scrnX, float scrnY) const
    { return SelectObject(scrnX, scrnY, 3); };

  int DragSelectMultiObjects(float scrnX1, float scrnY1,
                             float scrnX2, float scrnY2,
                             LinkList<ObjectPtr> &selectBuf) const;

  //===== Brush Manipulation Selection =====
  int SelectVertex(float scrnX, float scrnY, float epsilon,
				   Vector3d *pManipDotPos = NULL);
  const Edge3d *SelectEdge(float scrnX, float scrnY, float epsilon,
						   Vector3d *pManipDotPos = NULL);
  const GPolygon *SelectFace(float scrnX, float scrnY, float epsilon,
							 Vector3d *pManipDotPos = NULL);

  void DisplayNoPts(void);
  void DisplayVertexPts(int vertex = -1);
  void DisplayEdgePts(const Edge3d *edge = NULL);
  void DisplayFacePts(const GPolygon *face = NULL);

  //===== Face Selection for Texture Manip =====
  GPolygon *GetSelectFace(void) const
    { return selectorPtr->GetSelectFace(); };

  void DisplayFaceSelect(bool display) { dispFaceSlct = display; };

  //===== Crosshair =====
  static bool IsCrossHairShown(void);
  static void ShowCrossHair(bool show);
  static void SetCrossHairPos(const Vector3d &posVec);
  static Vector3d GetCrossHairPos(void);

  //===== Leak Geometry =====
  static const Object *GetLeakObject(void);
  static void SetLeakObject(Object *pNewObj);

  //===== Entities =====
  static bool EntitiesDisplayed(void) { return showEntities; };
  static void DisplayEntities(bool show) { showEntities = show; };

  //===== Entities Wireframe Approximation =====
  static void ApproxItems(bool approximate);

  //===== Color Settings =====
  static int GetColor(int colorIndex);
  static int SetColor(int colorIndex, int color);

  //===== Object Rendering =====
  void RenderWireFrame(bool renderWorld = true, bool renderSelected = true,
                       bool renderCrossHair = true) const;
  void RenderSolid(bool tmap) const;

 private:

  //===== Object Selection Funcs =====
  Line &SetRay(float scrnX, float scrnY, Line &ray, bool relView = true) const;
  Object *SelectObject(float scrnX, float scrnY, int funcIndex) const;

  //===== Object Rendering =====
  static bool RenderObjWireFrame(Object &objNode);
  static bool RenderObjSolid(Object &objNode);

  void RenderSlctFaceOutline(Object &objNode) const;
  void DisplayManipPts(void) const;

  QDraw *qdraw;
  Object *viewObj;
  bool perspective;
  Clip clipView;
  float winWidth;
  const Selector *selectorPtr;

  int displayPts;
  bool dispFaceSlct;
  static bool showEntities;
  static Object *pCrossHair, *pCrossHairNorth;

  // static Object *pLeakGeomObj;
  static Object *pLeakObj;

  static bool aprxWFRender;

  static int manipVertex;
  static const Edge3d *manipEdge;
  static const GPolygon *manipFace;

  static int renderColors[VRC_NUMRENDERCOLORS];

  static const View *renderView;
  static QDraw *renderQDraw;
  static bool skipSlctObjs;
  static bool skipRender;
  static Points3d vBuffer;  // Used for wireframe.
  static Point3d ptsBuf1[48], ptsBuf2[48];
};


#endif // _VIEW_H_
