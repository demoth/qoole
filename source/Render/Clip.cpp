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
 * clip.cpp
 */

#include "stdafx.h"

#include "math.h"
#include "clip.h"

//==================== Clip ====================

Clip::Clip(void) {
  Vector3d minVec(-1.0f, -1.0f, -1.0f);
  Vector3d maxVec(1.0f, 1.0f, 1.0f);

  SetClipVolume(minVec, maxVec, false);
}

const Vector3d &
Clip::GetMinClipEdges(void) const {
  return clipMin;
}

const Vector3d &
Clip::GetMaxClipEdges(void) const {
  return clipMax;
}

void
Clip::SetClipVolume(const Vector3d &minEdges, const Vector3d &maxEdges,
                    bool perspective) {
  // PreCondition.
  ASSERT(minEdges.GetX() <= maxEdges.GetX());
  ASSERT(minEdges.GetY() <= maxEdges.GetY());
  ASSERT(minEdges.GetZ() <= maxEdges.GetZ());

  clipMin = minEdges;
  clipMax = maxEdges;
  prspctv = perspective;
}

bool
Clip::ClipPoint(const Point3d &pt1) const {
  unsigned char rc = RegionCheck(pt1);
  return (rc == 0x00 ? false : true);
}

bool
Clip::ClipLine(Point3d &pt1, Point3d &pt2) const {
  unsigned char rc1, rc2;

  rc1 = RegionCheck(pt1);
  rc2 = RegionCheck(pt2);

  if (rc1 == 0x00 && rc2 == 0x00)  // both points are inside clip volume.
    return true;
  if ((rc1 & rc2) != 0x00)         // both points are outside clip volume.
    return false;

  // Check Y min plane.
  if ((rc1 ^ rc2) & 0x04) {
    if (rc1 & 0x04)
      rc1 = RegionCheck(pt1 = FindYIntersect(pt1, pt2, clipMin.GetY()));
    else
      rc2 = RegionCheck(pt2 = FindYIntersect(pt1, pt2, clipMin.GetY()));
  }

  // Check Y max plane.
  if ((rc1 ^ rc2) & 0x08) {
    if (rc1 & 0x08)
      rc1 = RegionCheck(pt1 = FindYIntersect(pt1, pt2, clipMax.GetY()));
    else
      rc2 = RegionCheck(pt2 = FindYIntersect(pt1, pt2, clipMax.GetY()));
  }

  // Check X min plane.
  if ((rc1 ^ rc2) & 0x01) {
    if (rc1 & 0x01)
      rc1 = RegionCheck(pt1 = (prspctv ?
                               FindXYIntersect(pt1, pt2, -1) :
                               FindXIntersect(pt1, pt2, clipMin.GetX())));
    else
      rc2 = RegionCheck(pt2 = (prspctv ? 
                               FindXYIntersect(pt1, pt2, -1) :
                               FindXIntersect(pt1, pt2, clipMin.GetX())));
  }

  // Check X max plane.
  if ((rc1 ^ rc2) & 0x02) {
    if (rc1 & 0x02)
      rc1 = RegionCheck(pt1 = (prspctv ?
                               FindXYIntersect(pt1, pt2, 1) :
                               FindXIntersect(pt1, pt2, clipMax.GetX())));
    else
      rc2 = RegionCheck(pt2 = (prspctv ?
                               FindXYIntersect(pt1, pt2, 1) :
                               FindXIntersect(pt1, pt2, clipMax.GetX())));
  }

  // Check Z min plane.
  if ((rc1 ^ rc2) & 0x10) {
    if (rc1 & 0x10)
      rc1 = RegionCheck(pt1 = (prspctv ?
                               FindZYIntersect(pt1, pt2, -1) :
                               FindZIntersect(pt1, pt2, clipMin.GetZ())));
    else
      rc2 = RegionCheck(pt2 = (prspctv ?
                               FindZYIntersect(pt1, pt2, -1) :
                               FindZIntersect(pt1, pt2, clipMin.GetZ())));
  }

  // Check Z max Plane.
  if ((rc1 ^ rc2) & 0x20) {
    if (rc1 & 0x20)
      rc1 = RegionCheck(pt1 = (prspctv ?
                               FindZYIntersect(pt1, pt2, 1) :
                               FindZIntersect(pt1, pt2, clipMax.GetZ())));
    else
      rc2 = RegionCheck(pt2 = (prspctv ?
                               FindZYIntersect(pt1, pt2, 1) :
                               FindZIntersect(pt1, pt2, clipMax.GetZ())));
  }

  // Check if both points are outside clip volume.
  if ((rc1 & rc2) != 0x00)
    return false;

  // Both points are inside clip volume.
  // ASSERT(rc1 == 0x00 && rc2 == 0x00);
  return true;
}

bool
Clip::ClipRay(const Line &ray, float &rangeBegin, float &rangeEnd) const {
  // Precondition.
  ASSERT((ray.GetSlope()).GetMagSqr() > 0.0f);

  bool rtnVal;
  float range;
  Point3d p1, p2;
  Vector3d maxBound, pos;

  range = ((maxBound.SubVector(clipMax, clipMin)).GetMag() /
           (ray.GetSlope()).GetMag()) + 1.0f;
  (pos.MultVector(ray.GetSlope(), range)).AddVector(ray.GetPoint());
  p2.NewPos(pos.GetX(), pos.GetY(), pos.GetZ());
  (pos.MultVector(ray.GetSlope(), -range)).AddVector(ray.GetPoint());
  p1.NewPos(pos.GetX(), pos.GetY(), pos.GetZ());

  rtnVal = ClipLine(p1, p2);
  rangeBegin = ray.Parametrize(p1);
  rangeEnd = ray.Parametrize(p2);

  return rtnVal;
}

int
Clip::ClipPolygon(int &numPts, Point3d *buf1,
                  Point3d *buf2, Point3d **fPtr) const {
  // PreCondition.
  ASSERT(numPts <= 32);
  ASSERT(*fPtr == buf1);
  // Sanity.
  ASSERT(prspctv);

  unsigned char rcBuf1[48], rcBuf2[48];
  unsigned char rcbo, rcba, *frmRc, *toRc, *rcBgn;
  Point3d *frmPtr, *toPtr, *bgn;
  bool bufSwitch = false;
  int i;

  rcba = ~(rcbo = 0x00);
  frmPtr = buf1;
  frmRc = rcBuf1;
  for(i = 0; i < numPts; i++) {
    rcbo |= (*frmRc = RegionCheck(*frmPtr++));
    rcba &= *frmRc++;
  }

  if (rcbo == 0x00)
    return numPts;
  // else if (rcba != 0x00)
  //   return 0;

  *frmPtr = *buf1;
  *frmRc = *rcBuf1;

  // 1. Clip Y Min.
  if ((rcbo ^ rcba) & 0x04) {  // Edges cross clip plane.
    float clipMinY = clipMin.GetY();
    rcba = ~(rcbo = 0x00);
    if (bufSwitch) {
      frmPtr = buf2;  toPtr = bgn = buf1;
      frmRc = rcBuf2;  toRc = rcBgn = rcBuf1;
    }
    else {
      frmPtr = buf1;  toPtr = bgn = buf2;
      frmRc = rcBuf1;  toRc = rcBgn = rcBuf2;
    }
    for(i = 0; i < numPts; i++) {
      if (!(*frmRc & 0x04)) {  // In.
        *toPtr++ = *frmPtr++;
        rcbo |= (*toRc = *frmRc++);
        rcba &= *toRc++;
        if (*frmRc & 0x04) { // Cross out.
          *toPtr = FindYIntersect(*(frmPtr - 1), *frmPtr, clipMinY);
          rcbo |= (*toRc = RegionCheck(*toPtr++));
          rcba &= *toRc++;
        }
      }
      else { // Out.
        ++frmPtr;
        ++frmRc;
        if (!(*frmRc & 0x04)) { // Cross in.
          *toPtr = FindYIntersect(*(frmPtr - 1), *frmPtr, clipMinY);
          rcbo |= (*toRc = RegionCheck(*toPtr++));
          rcba &= *toRc++;
        }
      }
    }
    *toPtr = *bgn;
    *toRc = *rcBgn;
    numPts = toPtr - bgn;
    bufSwitch = !bufSwitch;
  }
  else if (rcbo & 0x04) {  // all points outside.
    ASSERT((rcba & 0x04) != 0);  // Sanityl
    return 0;
  }

  // 2. Clip Y Max.
  if ((rcbo ^ rcba) & 0x08) {  // Edges cross clip plane.
    float clipMaxY = clipMax.GetY();
    rcba = ~(rcbo = 0x00);
    if (bufSwitch) {
      frmPtr = buf2;  toPtr = bgn = buf1;
      frmRc = rcBuf2;  toRc = rcBgn = rcBuf1;
    }
    else {
      frmPtr = buf1;  toPtr = bgn = buf2;
      frmRc = rcBuf1;  toRc = rcBgn = rcBuf2;
    }
    for(i = 0; i < numPts; i++) {
      if (!(*frmRc & 0x08)) {  // In.
        *toPtr++ = *frmPtr++;
        rcbo |= (*toRc = *frmRc++);
        rcba &= *toRc++;
        if (*frmRc & 0x08) { // Cross out.
          *toPtr = FindYIntersect(*(frmPtr - 1), *frmPtr, clipMaxY);
          rcbo |= (*toRc = RegionCheck(*toPtr++));
          rcba &= *toRc++;
        }
      }
      else { // Out.
        ++frmPtr;
        ++frmRc;
        if (!(*frmRc & 0x08)) { // Cross in.
          *toPtr = FindYIntersect(*(frmPtr - 1), *frmPtr, clipMaxY);
          rcbo |= (*toRc = RegionCheck(*toPtr++));
          rcba &= *toRc++;
        }
      }
    }
    *toPtr = *bgn;
    *toRc = *rcBgn;
    numPts = toPtr - bgn;
    bufSwitch = !bufSwitch;
  }
  else if (rcbo & 0x08) {  // all points outside.
    ASSERT((rcba & 0x08) != 0);  // Sanityl
    return 0;
  }

  // 3. Clip X Min.
  if ((rcbo ^ rcba) & 0x01) {  // Edges cross clip plane.
    rcba = ~(rcbo = 0x00);
    if (bufSwitch) {
      frmPtr = buf2;  toPtr = bgn = buf1;
      frmRc = rcBuf2;  toRc = rcBgn = rcBuf1;
    }
    else {
      frmPtr = buf1;  toPtr = bgn = buf2;
      frmRc = rcBuf1;  toRc = rcBgn = rcBuf2;
    }
    for(i = 0; i < numPts; i++) {
      if (!(*frmRc & 0x01)) {  // In.
        *toPtr++ = *frmPtr++;
        rcbo |= (*toRc = *frmRc++);
        rcba &= *toRc++;
        if (*frmRc & 0x01) { // Cross out.
          *toPtr = FindXYIntersect(*(frmPtr - 1), *frmPtr, -1);
          rcbo |= (*toRc = RegionCheck(*toPtr++));
          rcba &= *toRc++;
        }
      }
      else { // Out.
        ++frmPtr;
        ++frmRc;
        if (!(*frmRc & 0x01)) { // Cross in.
          *toPtr = FindXYIntersect(*(frmPtr - 1), *frmPtr, -1);
          rcbo |= (*toRc = RegionCheck(*toPtr++));
          rcba &= *toRc++;
        }
      }
    }
    *toPtr = *bgn;
    *toRc = *rcBgn;
    numPts = toPtr - bgn;
    bufSwitch = !bufSwitch;
  }
  else if (rcbo & 0x01) {  // all points outside.
    ASSERT((rcba & 0x01) != 0);  // Sanityl
    return 0;
  }

  // 4. Clip X Max.
  if ((rcbo ^ rcba) & 0x02) {  // Edges cross clip plane.
    rcba = ~(rcbo = 0x00);
    if (bufSwitch) {
      frmPtr = buf2;  toPtr = bgn = buf1;
      frmRc = rcBuf2;  toRc = rcBgn = rcBuf1;
    }
    else {
      frmPtr = buf1;  toPtr = bgn = buf2;
      frmRc = rcBuf1;  toRc = rcBgn = rcBuf2;
    }
    for(i = 0; i < numPts; i++) {
      if (!(*frmRc & 0x02)) {  // In.
        *toPtr++ = *frmPtr++;
        rcbo |= (*toRc = *frmRc++);
        rcba &= *toRc++;
        if (*frmRc & 0x02) { // Cross out.
          *toPtr = FindXYIntersect(*(frmPtr - 1), *frmPtr, 1);
          rcbo |= (*toRc = RegionCheck(*toPtr++));
          rcba &= *toRc++;
        }
      }
      else { // Out.
        ++frmPtr;
        ++frmRc;
        if (!(*frmRc & 0x02)) { // Cross in.
          *toPtr = FindXYIntersect(*(frmPtr - 1), *frmPtr, 1);
          rcbo |= (*toRc = RegionCheck(*toPtr++));
          rcba &= *toRc++;
        }
      }
    }
    *toPtr = *bgn;
    *toRc = *rcBgn;
    numPts = toPtr - bgn;
    bufSwitch = !bufSwitch;
  }
  else if (rcbo & 0x02) {  // all points outside.
    ASSERT((rcba & 0x02) != 0);  // Sanityl
    return 0;
  }

  // 5. Clip Z Min.
  if ((rcbo ^ rcba) & 0x10) {  // Edges cross clip plane.
    rcba = ~(rcbo = 0x00);
    if (bufSwitch) {
      frmPtr = buf2;  toPtr = bgn = buf1;
      frmRc = rcBuf2;  toRc = rcBgn = rcBuf1;
    }
    else {
      frmPtr = buf1;  toPtr = bgn = buf2;
      frmRc = rcBuf1;  toRc = rcBgn = rcBuf2;
    }
    for(i = 0; i < numPts; i++) {
      if (!(*frmRc & 0x10)) {  // In.
        *toPtr++ = *frmPtr++;
        rcbo |= (*toRc = *frmRc++);
        rcba &= *toRc++;
        if (*frmRc & 0x10) { // Cross out.
          *toPtr = FindZYIntersect(*(frmPtr - 1), *frmPtr, -1);
          rcbo |= (*toRc = RegionCheck(*toPtr++));
          rcba &= *toRc++;
        }
      }
      else { // Out.
        ++frmPtr;
        ++frmRc;
        if (!(*frmRc & 0x10)) { // Cross in.
          *toPtr = FindZYIntersect(*(frmPtr - 1), *frmPtr, -1);
          rcbo |= (*toRc = RegionCheck(*toPtr++));
          rcba &= *toRc++;
        }
      }
    }
    *toPtr = *bgn;
    *toRc = *rcBgn;
    numPts = toPtr - bgn;
    bufSwitch = !bufSwitch;
  }
  else if (rcbo & 0x10) {  // all points outside.
    ASSERT((rcba & 0x10) != 0);  // Sanity
    return 0;
  }

  // 6. Clip Z Max.
  if ((rcbo ^ rcba) & 0x20) {  // Edges cross clip plane.
    rcba = ~(rcbo = 0x00);
    if (bufSwitch) {
      frmPtr = buf2;  toPtr = bgn = buf1;
      frmRc = rcBuf2;  toRc = rcBgn = rcBuf1;
    }
    else {
      frmPtr = buf1;  toPtr = bgn = buf2;
      frmRc = rcBuf1;  toRc = rcBgn = rcBuf2;
    }
    for(i = 0; i < numPts; i++) {
      if (!(*frmRc & 0x20)) {  // In.
        *toPtr++ = *frmPtr++;
        rcbo |= (*toRc = *frmRc++);
        rcba &= *toRc++;
        if (*frmRc & 0x20) { // Cross out.
          *toPtr = FindZYIntersect(*(frmPtr - 1), *frmPtr, 1);
          rcbo |= (*toRc = RegionCheck(*toPtr++));
          rcba &= *toRc++;
        }
      }
      else { // Out.
        ++frmPtr;
        ++frmRc;
        if (!(*frmRc & 0x20)) { // Cross in.
          *toPtr = FindZYIntersect(*(frmPtr - 1), *frmPtr, 1);
          rcbo |= (*toRc = RegionCheck(*toPtr++));
          rcba &= *toRc++;
        }
      }
    }
    *toPtr = *bgn;
    *toRc = *rcBgn;
    numPts = toPtr - bgn;
    bufSwitch = !bufSwitch;
  }
  else if (rcbo & 0x20) {  // all points outside.
    ASSERT((rcba & 0x20) != 0);  // Sanityl
    return 0;
  }

  // Post Condition.
  // ASSERT(rcbo == 0x00);

  *fPtr = (bufSwitch ? buf2 : buf1);

  return numPts;
}

int
Clip::BoundObject(const Object &obj) const {
  static float root2 = (float) sqrt(2.0);
  const Matrix44 *trans = &(obj.GetTransformation());
  float maxRad = obj.GetBoundRadius();
  Vector3d pos(0.0f, 0.0f, 0.0f), xv(maxRad, 0.0f, 0.0f),
           yv(0.0f, maxRad, 0.0f), zv(0.0f, 0.0f, maxRad);
  float rx, ry, rz;
  int rcx, rcy, rcz;

  trans->Transform(pos);
  trans->Transform(xv);
  trans->Transform(yv);
  trans->Transform(zv);

  rx = (xv.SubVector(pos)).GetMagSqr();
  ry = (yv.SubVector(pos)).GetMagSqr();
  rz = (zv.SubVector(pos)).GetMagSqr();

  maxRad = (float) sqrt(Max(Max(rx, ry), rz));

  // Check Y.
  if ((pos.GetY() + maxRad) < clipMin.GetY() ||
      (pos.GetY() - maxRad) > clipMax.GetY())
    rcy = -1;
  else if ((pos.GetY() - maxRad) > clipMin.GetY() &&
           (pos.GetY() + maxRad) < clipMax.GetY())
    rcy = 1;
  else
    rcy = 0;

  if (prspctv) {
    // Sanity Check.
    ASSERT(clipMin.GetY() > 0.0f);

    float bound1 = pos.GetY() + maxRad * root2;
    float bound2 = pos.GetY() - maxRad * root2;

    // Check X.
    if (pos.GetX() < -bound1 || pos.GetX() > bound1)
      rcx = -1;
    else if (pos.GetX() > -bound2 && pos.GetX() < bound2)
      rcx = 1;
    else
      rcx = 0;

    // Check Z.
    if (pos.GetZ() < -bound1 || pos.GetZ() > bound1)
      rcz = -1;
    else if (pos.GetZ() > -bound2 && pos.GetZ() < bound2)
      rcz = 1;
    else
      rcz = 0;
  }
  else {
    // Check X.
    if ((pos.GetX() + maxRad) < clipMin.GetX() ||
        (pos.GetX() - maxRad) > clipMax.GetX())
      rcx = -1;
    else if ((pos.GetX() - maxRad) > clipMin.GetX() &&
             (pos.GetX() + maxRad) < clipMax.GetX())
      rcx = 1;
    else
      rcx = 0;

    // Check Z.
    if ((pos.GetZ() + maxRad) < clipMin.GetZ() ||
        (pos.GetZ() - maxRad) > clipMax.GetZ())
      rcz = -1;
    else if ((pos.GetZ() - maxRad) > clipMin.GetZ() &&
             (pos.GetZ() + maxRad) < clipMax.GetZ())
      rcz = 1;
    else
      rcz = 0;
  }

  if (rcx == 1 && rcy == 1 && rcz == 1)
    return 1;
  if (rcx == -1 && rcy == -1 && rcz == -1)
    return -1;
  return 0;
}

// Hack....
// The class Clip was original written and optimized for
//  clipping used in the View class.  In particular,
//  in perspective views, viewing angles are assumed
//  to be 45 degrees in each direction (+x -x +z and -z).
// This function is used in drag box selection, where
//  the clip volume is most likely different from the
//  view volume.  In the case of perspective view,
//  always manually test each of vertices.

bool
Clip::IsObjectInVolume(Object &obj) const {
  // Sanity.
  ASSERT(!obj.IsRoot());

  // Set the the obj.
  Matrix44 trans;
  trans = (obj.GetParent()).GetTransformation();
  obj.CalTransSpaceMatrix(trans);
  obj.SetTransformation(trans);

  // Bound test.
  int clipVal = (prspctv ? 0 : BoundObject(obj));

  if (clipVal == 1)
    return true;
  if (clipVal == -1)
	return false;

  // clipVal == 0;
  if (obj.HasBrush()) {
    // Test each of the vertices.
    Point3d *pPt;
    Points3d vBuf;
	(obj.GetBrush()).Transform(trans, vBuf);
	for(int i = 0; i < obj.GetBrush().GetNumVertices(); i++) {
      if (!prspctv) {
        if (ClipPoint(vBuf[i]))
          return false;
      }
	  else {
        pPt = &(vBuf[i]);
        if (pPt->GetY() < clipMin.GetY() ||
            pPt->GetY() > clipMax.GetY() ||
            pPt->GetX() < pPt->GetY() * clipMin.GetX() ||
            pPt->GetX() > pPt->GetY() * clipMax.GetX() ||
            pPt->GetZ() < pPt->GetY() * clipMin.GetZ() ||
            pPt->GetZ() > pPt->GetY() * clipMax.GetZ())
          return false;
      }
    }
    return true;
  }
  else if (obj.IsItemNode()) {
    return false;
  }
  else {
    ASSERT(obj.GetNumChildren() > 0);
    Object *pChild;
    IterLinkList<Object> *pIter = &(obj.GetIterChildren());
	pIter->Reset();
	while (!pIter->IsDone()) {
      pChild = pIter->GetNext();
      if (!IsObjectInVolume(*pChild))
        return false;
    }
    return true;
  }
}

unsigned char
Clip::RegionCheck(const Point3d &pt) const {
  unsigned char rtnVal = 0x00;

  if (prspctv) {
    if (-pt.GetX() > pt.GetY())
      rtnVal |= 0x01;
    else if (pt.GetX() > pt.GetY())
      rtnVal |= 0x02;
    if (-pt.GetZ() > pt.GetY())
      rtnVal |= 0x10;
    else if (pt.GetZ() > pt.GetY())
      rtnVal |= 0x20;
  }
  else {
    if (pt.GetX() < clipMin.GetX())
      rtnVal |= 0x01;
    else if (pt.GetX() > clipMax.GetX())
      rtnVal |= 0x02;
    if (pt.GetZ() < clipMin.GetZ())
      rtnVal |= 0x10;
    else if (pt.GetZ() > clipMax.GetZ())
      rtnVal |= 0x20;
  }

  if (pt.GetY() < clipMin.GetY())
    rtnVal |= 0x04;
  else if (pt.GetY() > clipMax.GetY())
    rtnVal |= 0x08;

  return rtnVal;
}

Point3d &
Clip::FindXIntersect(const Point3d &pt1,
                     const Point3d &pt2, float xVal) const {
  static Point3d rtnVal;
  float t = 0.0f;

  if (!EQUAL4(pt2.GetX(), pt1.GetX()))
    t = (xVal - pt1.GetX()) / (pt2.GetX() - pt1.GetX());
  rtnVal.NewPos(xVal,
                pt1.GetY() + t * (pt2.GetY() - pt1.GetY()),
                pt1.GetZ() + t * (pt2.GetZ() - pt1.GetZ()));
  return rtnVal;
}

Point3d &
Clip::FindYIntersect(const Point3d &pt1,
                     const Point3d &pt2, float yVal) const {
  static Point3d rtnVal;
  float t = 0.0f;

  if (!EQUAL4(pt2.GetY(), pt1.GetY()))
    t = (yVal - pt1.GetY()) / (pt2.GetY() - pt1.GetY());
  rtnVal.NewPos(pt1.GetX() + t * (pt2.GetX() - pt1.GetX()),
                yVal,
                pt1.GetZ() + t * (pt2.GetZ() - pt1.GetZ()));
  return rtnVal;
}

Point3d &
Clip::FindZIntersect(const Point3d &pt1,
                     const Point3d &pt2, float zVal) const {
  static Point3d rtnVal;
  float t = 0.0f;

  if (!EQUAL4(pt2.GetZ(), pt1.GetZ()))
    t = (zVal - pt1.GetZ()) / (pt2.GetZ() - pt1.GetZ());
  rtnVal.NewPos(pt1.GetX() + t * (pt2.GetX() - pt1.GetX()),
                pt1.GetY() + t * (pt2.GetY() - pt1.GetY()),
                zVal);
  return rtnVal;
}

Point3d &
Clip::FindXYIntersect(Point3d &pt1, Point3d &pt2, int side) const {
  // PreCondition and Sanity check.
  ASSERT(side == 1 || side == -1);

  static Point3d rtnVal;
  float a = (pt2.GetX() - pt1.GetX());
  float b = (pt2.GetY() - pt1.GetY());
  float demn = (b - a * side);

  if (EQUAL4(demn, 0.0f)) {
    return (pt1.GetX() * side > pt2.GetX() * side ? pt1 : pt2);
  }

  float c = (pt2.GetZ() - pt1.GetZ());
  float t, intz;

  t = (b * pt1.GetX() - a * pt1.GetY()) / demn;
  if (!EQUAL4(a, 0.0f))
    intz = (c / a) * (t - pt1.GetX()) + pt1.GetZ();
  else
    intz = (c / b) * (t * side - pt1.GetY()) + pt1.GetZ();
  return rtnVal.NewPos(t, t * side, intz);
}

Point3d &
Clip::FindZYIntersect(Point3d &pt1, Point3d &pt2, int side) const {
  // PreCondition.
  ASSERT(side == 1 || side == -1);

  static Point3d rtnVal;
  float b = (pt2.GetY() - pt1.GetY());
  float c = (pt2.GetZ() - pt1.GetZ());
  float demn = (b - c * side);

  if (EQUAL4(demn, 0.0f)) {
    return (pt1.GetZ() * side > pt2.GetZ() * side ? pt1 : pt2);
  }

  float a = (pt2.GetX() - pt1.GetX());
  float t, intx;

  t = (b * pt1.GetZ() - c * pt1.GetY()) / demn;
  if (!EQUAL4(c, 0.0f))
    intx = (a / c) * (t - pt1.GetZ()) + pt1.GetX();
  else
    intx = (a / b) * (t * side - pt1.GetY()) + pt1.GetX();
  return rtnVal.NewPos(intx, t * side, t);
}

