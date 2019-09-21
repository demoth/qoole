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
 * clip.h
 */

#ifndef _CLIP_H_
#define _CLIP_H_

#include "objects.h"

//==================== Clip ====================
// Used for clipping by the View Class.
// There is no use of this class from outside of view.cpp.

class Clip {
 public:
  Clip(void);
  Clip(const Clip &src) { *this = src; };
  // ~Clip(void);
  // Clip &operator=(const Clip &src);

  const Vector3d &GetMinClipEdges(void) const;
  const Vector3d &GetMaxClipEdges(void) const;

  void SetClipVolume(const Vector3d &minEdges,
                     const Vector3d &maxEdges, bool perspective);
  bool ClipPoint(const Point3d &pt1) const;
  bool ClipLine(Point3d &pt1, Point3d &pt2) const;
  bool ClipRay(const Line &ray, float &rangeBegin, float &rangeEnd) const;
  int  ClipPolygon(int &numPts, Point3d *buf1,
                   Point3d *buf2, Point3d **fPtr) const;
  int  BoundObject(const Object &obj) const;
  bool IsObjectInVolume(Object &obj) const;

 private:
  unsigned char RegionCheck(const Point3d &pt) const;
  Point3d &FindXIntersect(const Point3d &pt1,
                          const Point3d &pt2, float xVal) const;
  Point3d &FindYIntersect(const Point3d &pt1,
                          const Point3d &pt2, float yVal) const;
  Point3d &FindZIntersect(const Point3d &pt1,
                          const Point3d &pt2, float zVal) const;
  Point3d &FindXYIntersect(Point3d &pt1, Point3d &pt2, int side) const;
  Point3d &FindZYIntersect(Point3d &pt1, Point3d &pt2, int side) const;

  Vector3d clipMin, clipMax;
  bool prspctv;
};

#endif // _CLIP_H_