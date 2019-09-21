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
 * math3d.cpp
 */

#include "stdafx.h"

#include <math.h>
#include "LCommon.h"
#include "math3d.h"

//==================== Functions ====================

float
ASin(float opp, float hyp) {
  return (float) (hyp == 0.0f ? 0.0f : asin(opp / hyp));
}

float
ATan(float opp, float adj) {
  if (adj == 0.0f)
    return (opp >= 0.0f ? (DEG2RAD(90.0f)) : (DEG2RAD(-90.0f)));
  else if (adj > 0.0f)
    return (float) atan(opp / adj);
  else if (opp > 0.0f)  // (adj < 0.0)
    return (float) (atan(opp / adj) + (DEG2RAD(180.0f)));
  else // (adj < 0.0f && opp < 0.0f)
    return (float) (atan(opp / adj) - (DEG2RAD(180.0f)));
}

//==================== Vector3d ====================

const Vector3d Vector3d::origVec(0.0f, 0.0f, 0.0f);
const Vector3d Vector3d::xAxisVec(1.0f, 0.0f, 0.0f);
const Vector3d Vector3d::yAxisVec(0.0f, 1.0f, 0.0f);
const Vector3d Vector3d::zAxisVec(0.0f, 0.0f, 1.0f);

float
Vector3d::GetMag(void) const {
  return (float) sqrt(GetMagSqr());
}

float
Vector3d::GetMagSqr(void) const {
  return (GetX() * GetX() + GetY() * GetY() + GetZ() * GetZ());
}

Vector3d &
Vector3d::Normalize(void) {
  float mag = GetMag();

  x /= mag;
  y /= mag;
  z /= mag;
  return *this;
}

Vector3d &
Vector3d::AddVector(const Vector3d &vec) {
  x += vec.GetX();
  y += vec.GetY();
  z += vec.GetZ();
  return *this;
}

Vector3d &
Vector3d::AddVector(const Vector3d &v1, const Vector3d &v2) {
  *this = v1;
  return AddVector(v2);
}

Vector3d &
Vector3d::SubVector(const Vector3d &vec) {
  x -= vec.GetX();
  y -= vec.GetY();
  z -= vec.GetZ();
  return *this;
}

Vector3d &
Vector3d::SubVector(const Vector3d &v1, const Vector3d &v2) {
  *this = v1;
  return SubVector(v2);
}

Vector3d &
Vector3d::MultVector(float c) {
  x *= c;
  y *= c;
  z *= c;
  return *this;
}

Vector3d &
Vector3d::MultVector(const Vector3d &vec, float c) {
  SetX(vec.GetX() * c);
  SetY(vec.GetY() * c);
  SetZ(vec.GetZ() * c);
  return *this;
}

float
Vector3d::DotVector(const Vector3d &vec) const {
  return (vec.GetX() * GetX() + vec.GetY() * GetY() + vec.GetZ() * GetZ());
}

float
Vector3d::CosTheta(const Vector3d &vec) const {
  // PreConditions.
  ASSERT(GetMagSqr() > 0.0f);
  ASSERT(vec.GetMagSqr() > 0.0f);

  float d = DotVector(vec) / (vec.GetMag() * GetMag());

  if (d > 1.0f && EQUAL6(d, 1.0f))
    d = 1.0f;
  else if (d < -1.0f && EQUAL6(d, -1.0f))
    d = -1.0f;

  // Post Condition.
  ASSERT(d >= -1.0f && d <= 1.0f);

  return d;
}

Vector3d &
Vector3d::CrossVector(const Vector3d &vec) {
  NewVector( (GetY() * vec.GetZ() - GetZ() * vec.GetY()),
            -(GetX() * vec.GetZ() - GetZ() * vec.GetX()),
             (GetX() * vec.GetY() - GetY() * vec.GetX()));
  return *this;
}

Vector3d &
Vector3d::CrossVector(const Vector3d &v1, const Vector3d &v2) {
  NewVector( (v1.GetY() * v2.GetZ() - v1.GetZ() * v2.GetY()),
            -(v1.GetX() * v2.GetZ() - v1.GetZ() * v2.GetX()),
             (v1.GetX() * v2.GetY() - v1.GetY() * v2.GetX()));
  return *this;
}

bool
Vector3d::IsParallel(const Vector3d &vec) const {
  float d = CosTheta(vec);
  d = ABS(d);
  return EQUAL6(d, 1.0f);
}

bool
Vector3d::operator==(const Vector3d &vec) const {
  return (EQUAL4(GetX(), vec.GetX()) &&
          EQUAL4(GetY(), vec.GetY()) &&
          EQUAL4(GetZ(), vec.GetZ()));
}

//==================== SphrVector ====================

SphrVector::SphrVector(float yaw, float pitch, float roll) {
  NewVector(yaw, pitch, roll);
}

SphrVector &
SphrVector::NewVector(float yaw, float pitch, float roll) {
  yawAng = yaw;  pitchAng = pitch;  rollAng = roll;
  return *this;
}

SphrVector &
SphrVector::NewVector(const Vector3d &frontVec, const Vector3d &topVec) {
  Rect2Sphr(frontVec, topVec, *this);
  return *this;
}

SphrVector &
SphrVector::Normalize(void) {
  Vector3d front(0.0f, 1.0f, 0.0f);
  Vector3d top(0.0f, 0.0f, 1.0f);
  Matrix44 trans;

  trans.SetRotate(*this);
  trans.Transform(front);
  trans.Transform(top);
  Rect2Sphr(front, top, *this);

  return *this;
}

Vector3d &
SphrVector::GetRectVector(Vector3d &frontVec, Vector3d &topVec) const {
  return Sphr2Rect(*this, frontVec, topVec);
}

bool
SphrVector::operator==(const SphrVector &oriVec) const {
  return (EQUAL4(oriVec.yawAng, yawAng) &&
          EQUAL4(oriVec.pitchAng, pitchAng) &&
          EQUAL4(oriVec.rollAng, rollAng));
}

Vector3d &
SphrVector::Sphr2Rect(const SphrVector &sphrVec,
                      Vector3d &frontVec, Vector3d &topVec) {
  Vector3d f(0.0f, 1.0f, 0.0f);
  Vector3d t(0.0f, 0.0f, 1.0f);
  Matrix44 trans;

  trans.SetRotate(sphrVec);
  trans.Transform(frontVec, f);
  trans.Transform(topVec, t);

  return frontVec;
}

SphrVector &
SphrVector::Rect2Sphr(const Vector3d &frontVec,
                      const Vector3d &topVec, SphrVector &sphrVec) {
  Vector3d t = topVec;
  Matrix44 trans;
  float yaw, pitch, roll;

  yaw = ATan(frontVec.GetX(), frontVec.GetY());
  pitch = ASin(frontVec.GetZ(), frontVec.GetMag());
  roll = 0.0f;

  sphrVec.NewVector(yaw, pitch, roll);
  trans.SetInvRotate(sphrVec);
  trans.Transform(t);
  roll = ATan(t.GetX(), t.GetZ());
  
  return sphrVec.NewVector(yaw, pitch, roll);
}

//==================== Line ====================

Line::Line(void) {
  Vector3d origin(0.0f, 0.0f, 0.0f);
  Vector3d yAxis(0.0f, 1.0f, 0.0f);
  NewLine(origin, yAxis);
}

Line &
Line::NewLine(const Vector3d &pt, const Vector3d &slope) {
  // PreCondition.
  ASSERT(slope.GetMag() > 0.0f);

  pt0 = pt;
  v = slope;
  v.Normalize();
  return *this;
}

bool
Line::IsOnLine(const Vector3d &pt, float epsilon) const {
  Vector3d p1(pt);

  p1.SubVector(pt0);
  if (p1.GetMagSqr() < 0.01f)
    return true;

  Vector3d p2(v);
  ASSERT(EQUAL4(p2.GetMagSqr(), 1.0f));  // Sanity.
  p2.MultVector(p1.DotVector(p2));
  p1.SubVector(p2);

  return (p1.GetMagSqr() < epsilon);
}

float
Line::Parametrize(const Vector3d &pt) const {
  // PreCondition.
  ASSERT(IsOnLine(pt));
  // Sanity Check.
  ASSERT(EQUAL4(GetSlope().GetMagSqr(), 1.0f));

  float rtnVal, cosAng;
  Vector3d diff;

  diff.SubVector(pt, pt0);
  rtnVal = diff.GetMag();
  if (!EQUAL2(rtnVal, 0.0f)) {
    cosAng = (GetSlope()).CosTheta(diff);
    // Sanity Check.
    // ASSERT(EQUAL3(cosAng, 1.0f) || EQUAL3(cosAng, -1.0f));
    if (cosAng < 0.0f)
      rtnVal *= -1.0f;
  }

  return rtnVal;
}

bool
Line::IsParallel(const Line &line2) const {
  return (line2.GetSlope()).IsParallel(GetSlope());
}

bool
Line::Intersect(const Line &line2, Vector3d &intrsctPt) const {
  float m1, m2, intx, inty, intz;

  if (line2.v.IsParallel(v)) {
    if (line2.IsOnLine(pt0)) {
      intrsctPt = pt0;
      return true;
    }
    else
      return false;
  }

  // find x.
  if (!EQUAL4(line2.v.GetX(), 0.0f) && !EQUAL4(v.GetX(), 0.0f)) {
    m1 = v.GetZ() / v.GetX();
    m2 = line2.v.GetZ() / line2.v.GetX();
    if (!EQUAL4(m1, m2)) {
      intx = (line2.pt0.GetZ() - pt0.GetZ() + (m1 * pt0.GetX()) -
              (m2 * line2.pt0.GetX())) / (m1 - m2);
    }
    else {
      m1 = v.GetY() / v.GetX();
      m2 = line2.v.GetY() / line2.v.GetX();
      ASSERT(m1 != m2);  // Sanity check.
      intx = (line2.pt0.GetY() - pt0.GetY() + (m1 * pt0.GetX()) -
              (m2 * line2.pt0.GetX())) / (m1 - m2);
    }
    intrsctPt.SetX(intx);
  }
  else if (EQUAL4(line2.v.GetX(), 0.0f)) {
    intrsctPt.SetX(line2.pt0.GetX());
  }
  else {  // v.GetX() == 0.0f
    intrsctPt.SetX(pt0.GetX());
  }

  // find y.
  if (!EQUAL4(line2.v.GetY(), 0.0f) && !EQUAL4(v.GetY(), 0.0f)) {
    m1 = v.GetZ() / v.GetY();
    m2 = line2.v.GetZ() / line2.v.GetY();
    if (!EQUAL4(m1, m2)) {
      inty = (line2.pt0.GetZ() - pt0.GetZ() + (m1 * pt0.GetY()) -
              (m2 * line2.pt0.GetY())) / (m1 - m2);
    }
    else {
      m1 = v.GetX() / v.GetY();
      m2 = line2.v.GetX() / line2.v.GetY();
      ASSERT(m1 != m2);  // Sanity Check.
      inty = (line2.pt0.GetX() - pt0.GetX() + (m1 * pt0.GetY()) -
              (m2 * line2.pt0.GetY())) / (m1 - m2);
    }
    intrsctPt.SetY(inty);
  }
  else if (EQUAL4(line2.v.GetY(), 0.0f)) {
    intrsctPt.SetY(line2.pt0.GetY());
  }
  else {  // v.GetY() == 0.0f
    intrsctPt.SetY(pt0.GetY());
  }

  // find z.
  if (!EQUAL4(line2.v.GetX(), 0.0f) && !EQUAL4(line2.v.GetY(), 0.0f)) {
    intz = (intrsctPt.GetX() - line2.pt0.GetX()) * 
           (line2.v.GetZ() / line2.v.GetX()) + line2.pt0.GetZ();
    intrsctPt.SetZ(intz);
    intz = (intrsctPt.GetY() - line2.pt0.GetY()) * 
           (line2.v.GetZ() / line2.v.GetY()) + line2.pt0.GetZ();

    if (!EQUAL2(intz, intrsctPt.GetZ()))
      return false;
    // Sanity Check.
    ASSERT(IsOnLine(intrsctPt));
  }
  else if (!EQUAL4(line2.v.GetX(), 0.0f)) {
    intz = (intrsctPt.GetX() - line2.pt0.GetX()) * 
           (line2.v.GetZ() / line2.v.GetX()) + line2.pt0.GetZ();
    intrsctPt.SetZ(intz);
    if (!IsOnLine(intrsctPt))
      return false;
  }
  else if (!EQUAL4(line2.v.GetY(), 0.0f)) {
    intz = (intrsctPt.GetY() - line2.pt0.GetY()) * 
           (line2.v.GetZ() / line2.v.GetY()) + line2.pt0.GetZ();
    intrsctPt.SetZ(intz);
    if (!IsOnLine(intrsctPt))
      return false;
  }
  else { // line2.v.GetX() == 0.0f && line2.v.GetY() == 0.0f
    if (!EQUAL4(v.GetX(), 0.0f)) {
      intz = (line2.pt0.GetX() - pt0.GetX()) *
               (v.GetZ() / v.GetX()) + pt0.GetZ();
    }
    else {
      ASSERT(v.GetY() != 0.0f); // Sanity check.
      intz = (line2.pt0.GetY() - pt0.GetY()) *
               (v.GetZ() / v.GetY()) + pt0.GetZ();
    }
    intrsctPt.SetZ(intz);
    if (!IsOnLine(intrsctPt))
      return false;
  }

  return true;
}

float
Line::IntersectAngle(const Line &line2) const {
  float d = v.CosTheta(line2.v);
  // Sanity Check.
  ASSERT(d >= -1.0f && d <= 1.0f);
  return (float) acos(d);
}

//==================== Plane ====================

const Plane Plane::xyPlane(Vector3d::zAxisVec, Vector3d::origVec);
const Plane Plane::yzPlane(Vector3d::xAxisVec, Vector3d::origVec);
const Plane Plane::zxPlane(Vector3d::yAxisVec, Vector3d::origVec);

Plane &
Plane::NewPlane(const Vector3d norm, const Vector3d &pt) {
  // PreCondition.
  ASSERT(norm.GetMag() > 0.0f);

  pt0 = pt;  
  n = norm;
  n.Normalize();
  d = n.GetX() * pt0.GetX() + n.GetY() * pt0.GetY() + n.GetZ() * pt0.GetZ();
  return *this;
}

Plane &
Plane::NewPlane(const Vector3d &pt1,
                const Vector3d &pt2, const Vector3d &pt3) {
  Vector3d v1(pt2);
  Vector3d v2(pt3);
  v1.SubVector(pt1).Normalize();
  v2.SubVector(pt1).Normalize();

  // Sanity Check.
  ASSERT(!v2.IsParallel(v1));
  
  pt0 = pt1;
  (n.CrossVector(v2, v1)).Normalize();
  d = n.GetX() * pt0.GetX() + n.GetY() * pt0.GetY() + n.GetZ() * pt0.GetZ();
  return *this;  
}

int
Plane::PointInPlane(const Vector3d &pt, float epsilon) const {
  float dist = (pt.GetX() * n.GetX() +
                pt.GetY() * n.GetY() +
                pt.GetZ() * n.GetZ()) - d;

  if (ABS(dist) < ABS(epsilon))
    return 0;
  return (dist > 0.0f ? 1 : -1);
}

float
Plane::DistOffPlane(const Vector3d &pt) const {
  return ((pt.GetX() * n.GetX() +
           pt.GetY() * n.GetY() +
           pt.GetZ() * n.GetZ()) - d);
}

bool
Plane::Intersect(const Line &l, Vector3d &intrsctPt) const {
  float t;

  if (n.DotVector(l.GetSlope()) == 0.0f)
    return false;
  t = (d - (n.GetX() * (l.GetPoint().GetX()) +
            n.GetY() * (l.GetPoint().GetY()) +
            n.GetZ() * (l.GetPoint().GetZ()))) /
      (n.GetX() * l.GetSlope().GetX() +
       n.GetY() * l.GetSlope().GetY() +
       n.GetZ() * l.GetSlope().GetZ());
  intrsctPt.SetX(l.GetSlope().GetX() * t + l.GetPoint().GetX());
  intrsctPt.SetY(l.GetSlope().GetY() * t + l.GetPoint().GetY());
  intrsctPt.SetZ(l.GetSlope().GetZ() * t + l.GetPoint().GetZ());
  return true;
}

bool
Plane::Intersect(const Plane &plane2, Line &l) const {
  Vector3d m, pt0;
  float demn;

  if ((plane2.GetNorm()).IsParallel(GetNorm()))
    return false;

  m.CrossVector(GetNorm(), plane2.GetNorm());
  // Sanity Check.
  ASSERT(m.GetMag() > 0.0f);

  float dx = ABS(m.GetX());
  float dy = ABS(m.GetY());
  float dz = ABS(m.GetZ());

  if (dx >= dy && dx >= dz) {
    ASSERT(!EQUAL4(m.GetX(), 0.0f));
    demn = n.GetY() * plane2.n.GetZ() - n.GetZ() * plane2.n.GetY();
    ASSERT(demn != 0.0f);  // Sanity Check.

    pt0.SetX(0.0f);
    pt0.SetY((d * plane2.n.GetZ() - n.GetZ() * plane2.d) / demn);
    pt0.SetZ((n.GetY() * plane2.d - plane2.n.GetY() * d) / demn);
  }
  else if (dy >= dx && dy >= dz) {
    ASSERT(!EQUAL4(m.GetY(), 0.0f));
    demn = n.GetZ() * plane2.n.GetX() - n.GetX() * plane2.n.GetZ();
    ASSERT(demn != 0.0f);

    pt0.SetX((n.GetZ() * plane2.d - plane2.n.GetZ() * d) / demn);
    pt0.SetY(0.0f);
    pt0.SetZ((plane2.n.GetX() * d - n.GetX() * plane2.d) / demn);
  }
  else { // m.GetZ() != 0.0f
    // Sanity.
    ASSERT(dz >= dx && dz >= dy);
    ASSERT(!EQUAL4(m.GetZ(), 0.0f));

    demn = n.GetY() * plane2.n.GetX() - n.GetX() * plane2.n.GetY();
    ASSERT(demn != 0.0f);  // Sanity Check.

    pt0.SetX((n.GetY() * plane2.d - plane2.n.GetY() * d) / demn);
    pt0.SetY((plane2.n.GetX() * d - n.GetX() * plane2.d) / demn);
    pt0.SetZ(0.0f);
  }

  l.NewLine(pt0, m);
  return true;
}

bool
Plane::Intersect(const Plane &plane2, const Plane &plane3,
                 Vector3d &intrsctPt) const {
  float denom;
  float x, y, z;

  denom = n.GetX() * plane2.n.GetY() * plane3.n.GetZ() +
          n.GetY() * plane2.n.GetZ() * plane3.n.GetX() +
          n.GetZ() * plane2.n.GetX() * plane3.n.GetY() -
          plane3.n.GetX() * plane2.n.GetY() * n.GetZ() -
          plane3.n.GetY() * plane2.n.GetZ() * n.GetX() -
          plane3.n.GetZ() * plane2.n.GetX() * n.GetY();

  if (EQUAL6(denom, 0.0f))
    return false;

  x = d * plane2.n.GetY() * plane3.n.GetZ() +
      n.GetY() * plane2.n.GetZ() * plane3.d +
      n.GetZ() * plane2.d * plane3.n.GetY() -
      plane3.d * plane2.n.GetY() * n.GetZ() -
      plane3.n.GetY() * plane2.n.GetZ() * d -
      plane3.n.GetZ() * plane2.d * n.GetY();

  y = n.GetX() * plane2.d * plane3.n.GetZ() +
      d * plane2.n.GetZ() * plane3.n.GetX() +
      n.GetZ() * plane2.n.GetX() * plane3.d -
      plane3.n.GetX() * plane2.d * n.GetZ() -
      plane3.d * plane2.n.GetZ() * n.GetX() -
      plane3.n.GetZ() * plane2.n.GetX() * d;

  z = n.GetX() * plane2.n.GetY() * plane3.d +
      n.GetY() * plane2.d * plane3.n.GetX() +
      d * plane2.n.GetX() * plane3.n.GetY() -
      plane3.n.GetX() * plane2.n.GetY() * d -
      plane3.n.GetY() * plane2.d * n.GetX() -
      plane3.d * plane2.n.GetX() * n.GetY();

  intrsctPt.NewVector(x / denom, y / denom, z / denom);
  return true;
}

float
Plane::IntersectAngle(const Plane &plane2) const {
  float d = plane2.GetNorm().CosTheta(GetNorm());
  // Sanity Check.
  ASSERT(d >= -1.0f && d <= 1.0f);
  return (float) acos(d);
}

bool
Plane::IsSame(const Plane &plane2) const {
  float d = (plane2.GetNorm()).CosTheta(GetNorm()); 

  if (EQUAL6(d, 1.0f) && plane2.PointInPlane(GetPoint(), 0.01f) == 0)
    return true;
  else
    return false;
}

// #define RPRECISE 100000.0f
// #define RPRECISE 50000.0f
#define RPRECISE 10000.0f
// #define RPRECISE 8192.0f
// #define RPRECISE 4092.0f

void
Plane::Normalize(Vector3d &p1, Vector3d &p2, Vector3d &p3) const {
  float xAng = n.CosTheta(Vector3d::xAxisVec);
  float yAng = n.CosTheta(Vector3d::yAxisVec);
  float zAng = n.CosTheta(Vector3d::zAxisVec);
  float absX = ABS(xAng);
  float absY = ABS(yAng);
  float absZ = ABS(zAng);
  Vector3d swap;
  Line l;
  bool ok;

  if (absX >= absY && absX >= absZ) {
    ok = Intersect(l.NewLine(Vector3d::origVec, Vector3d::xAxisVec), p1);
    ASSERT(ok);
    ok = Intersect(zxPlane, l);
    ASSERT(ok);
    if ((p2 = l.GetSlope()).GetZ() < 0)
      p2.MultVector(-1.0f);
    ok = Intersect(xyPlane, l);
    ASSERT(ok);
    if ((p3 = l.GetSlope()).GetY() < 0)
      p3.MultVector(-1.0f);
    ok = (xAng >= 0.0f ? true : false);
  }
  else if (absY >= absX && absY >= absZ) {
    ok = Intersect(l.NewLine(Vector3d::origVec, Vector3d::yAxisVec), p1);
    ASSERT(ok);
    ok = Intersect(xyPlane, l);
    ASSERT(ok);
    if ((p2 = l.GetSlope()).GetX() < 0)
      p2.MultVector(-1.0f);
    ok = Intersect(yzPlane, l);
    ASSERT(ok);
    if ((p3 = l.GetSlope()).GetZ() < 0)
      p3.MultVector(-1.0f);
    ok = (yAng >= 0.0f ? true : false);
  }
  else { // (absZ >= absX && absZ >= absY)
    ok = Intersect(l.NewLine(Vector3d::origVec, Vector3d::zAxisVec), p1);
    ASSERT(ok);
    ok = Intersect(yzPlane, l);
    ASSERT(ok);
    if ((p2 = l.GetSlope()).GetY() < 0)
      p2.MultVector(-1.0f);
    ok = Intersect(zxPlane, l);
    ASSERT(ok);
    if ((p3 = l.GetSlope()).GetX() < 0)
      p3.MultVector(-1.0f);
    ok = (zAng >= 0.0f ? true : false);
  }

  if (!ok) {
    swap = p2;
    p2 = p3;
    p3 = swap;
  }

  (p2.MultVector(RPRECISE)).AddVector(p1);
  (p3.MultVector(RPRECISE)).AddVector(p1);
}

void
Plane::Round45(void) {
  n.Normalize();

  // Round and align the plane's normal to 45 degree lines.
  float rx = ROUND3(n.GetX());
  rx = ABS(rx);

  float ry = ROUND3(n.GetY());
  ry = ABS(ry);

  float rz = ROUND3(n.GetZ());
  rz = ABS(rz);

  float sx = (n.GetX() > 0.0f ? 1.0f : -1.0f);
  float sy = (n.GetY() > 0.0f ? 1.0f : -1.0f);
  float sz = (n.GetZ() > 0.0f ? 1.0f : -1.0f);
  float ave;

  if (rx == ry && rx == rz) {
    n.NewVector(sx, sy, sz);
  }
  else if (rx == ry && rx > rz) {
    ave = (rx + ry) / 2;
    n.NewVector(ave * sx, ave * sy, n.GetZ());
  }
  else if (rx == rz && rx > ry) {
    ave = (rx + rz) / 2;
    n.NewVector(ave * sx, n.GetY(), ave * sz);
  }
  else if (ry == rz && ry > rx) {
    ave = (ry + rz) / 2;
    n.NewVector(n.GetX(), ave * sy, ave * sz);
  }

  n.Normalize();
}

//==================== Matrix44 ====================

#if 0  // Commented out for optimazation.  No init performed.
Matrix44::Matrix44(void) {
  matrix[0] = matrix[1] = matrix[2] = matrix[3] =
  matrix[4] = matrix[5] = matrix[6] = matrix[7] =
  matrix[8] = matrix[9] = matrix[10] = matrix[11] =
  matrix[12] = matrix[13] = matrix[14] = matrix[15] = 0.0f;
}
#endif

Matrix44 &
Matrix44::SetTranslation(const Vector3d &transVec) {
  SetIdentity();
  matrix[3] = transVec.GetX();
  matrix[7] = transVec.GetY();
  matrix[11] = transVec.GetZ();
  return *this;
}

Matrix44 &
Matrix44::SetInvTranslation(const Vector3d &transVec) {
  SetIdentity();
  matrix[3] = -transVec.GetX();
  matrix[7] = -transVec.GetY();
  matrix[11] = -transVec.GetZ();
  return *this;
}

Matrix44 &
Matrix44::SetScale(const Vector3d &scaleVec) {
  SetIdentity();
  matrix[0] = scaleVec.GetX();
  matrix[5] = scaleVec.GetY();
  matrix[10] = scaleVec.GetZ();
  return *this;
}

Matrix44 &
Matrix44::SetRotateX(float theta) {
  float cosTheta = (float) cos(theta);
  float sinTheta = (float) sin(theta);

  SetIdentity();
  matrix[5] = cosTheta;
  matrix[6] = -sinTheta;
  matrix[9] = sinTheta;
  matrix[10] = cosTheta;
  return *this;
}

Matrix44 &
Matrix44::SetRotateY(float theta) {
  float cosTheta = (float) cos(theta);
  float sinTheta = (float) sin(theta);

  SetIdentity();
  matrix[0] = cosTheta;
  matrix[2] = sinTheta;
  matrix[8] = -sinTheta;
  matrix[10] = cosTheta;
  return *this;
}

Matrix44 &
Matrix44::SetRotateZ(float theta) {
  float cosTheta = (float) cos(theta);
  float sinTheta = (float) sin(theta);

  SetIdentity();
  matrix[0] = cosTheta;
  matrix[4] = sinTheta;
  matrix[1] = -sinTheta;
  matrix[5] = cosTheta;
  return *this;
}

Matrix44 &
Matrix44::SetRotate(const SphrVector &rotateVec) {
  Matrix44 trans;

  SetRotateZ(-rotateVec.GetYaw());
  Multiply(trans.SetRotateX(rotateVec.GetPitch()));
  Multiply(trans.SetRotateY(rotateVec.GetRoll()));

  return *this;
}

Matrix44 &
Matrix44::SetInvRotate(const SphrVector &rotateVec) {
  Matrix44 trans;

  SetRotateY(-rotateVec.GetRoll());
  Multiply(trans.SetRotateX(-rotateVec.GetPitch()));
  Multiply(trans.SetRotateZ(rotateVec.GetYaw()));

  return *this;  
}

Matrix44 &
Matrix44::AddMatrix(const Matrix44 &m) {
  int i;

  for(i = 0; i < 16; i++)
    matrix[i] += m.matrix[i];
  return *this;
}

Matrix44 &
Matrix44::AddMatrix(const Matrix44 &m1, const Matrix44 &m2) {
  int i;

  for(i = 0; i < 16; i++)
    matrix[i] = m1.matrix[i] + m2.matrix[i];
  return *this;
}

Matrix44 &
Matrix44::SubMatrix(const Matrix44 &m) {
  int i;

  for(i = 0; i < 16; i++)
    matrix[i] -= m.matrix[i];
  return *this;
}

Matrix44 &
Matrix44::SubMatrix(const Matrix44 &m1, const Matrix44 &m2) {
  int i;

  for(i = 0; i < 16; i++)
    matrix[i] = m1.matrix[i] - m2.matrix[i];
  return *this;
}

Matrix44 &
Matrix44::Multiply(const Matrix44 &m) {
  Matrix44 m1(*this);
  return Multiply(m1, m);
}

Matrix44 &
Matrix44::Multiply(const Matrix44 &m1, const Matrix44 &m2) {

  matrix[0] = m1.matrix[0] * m2.matrix[0] +
                m1.matrix[1] * m2.matrix[4] +
                m1.matrix[2] * m2.matrix[8] +
                m1.matrix[3] * m2.matrix[12];
  matrix[1] = m1.matrix[0] * m2.matrix[1] +
                m1.matrix[1] * m2.matrix[5] +
                m1.matrix[2] * m2.matrix[9] +
                m1.matrix[3] * m2.matrix[13];
  matrix[2] = m1.matrix[0] * m2.matrix[2] +
                m1.matrix[1] * m2.matrix[6] +
                m1.matrix[2] * m2.matrix[10] +
                m1.matrix[3] * m2.matrix[14];
  matrix[3] = m1.matrix[0] * m2.matrix[3] +
                m1.matrix[1] * m2.matrix[7] +
                m1.matrix[2] * m2.matrix[11] +
                m1.matrix[3] * m2.matrix[15];

  matrix[4] = m1.matrix[4] * m2.matrix[0] +
                m1.matrix[5] * m2.matrix[4] +
                m1.matrix[6] * m2.matrix[8] +
                m1.matrix[7] * m2.matrix[12];
  matrix[5] = m1.matrix[4] * m2.matrix[1] +
                m1.matrix[5] * m2.matrix[5] +
                m1.matrix[6] * m2.matrix[9] +
                m1.matrix[7] * m2.matrix[13];
  matrix[6] = m1.matrix[4] * m2.matrix[2] +
                m1.matrix[5] * m2.matrix[6] +
                m1.matrix[6] * m2.matrix[10] +
                m1.matrix[7] * m2.matrix[14];
  matrix[7] = m1.matrix[4] * m2.matrix[3] +
                m1.matrix[5] * m2.matrix[7] +
                m1.matrix[6] * m2.matrix[11] +
                m1.matrix[7] * m2.matrix[15];

  matrix[8] = m1.matrix[8] * m2.matrix[0] +
                m1.matrix[9] * m2.matrix[4] +
                m1.matrix[10] * m2.matrix[8] +
                m1.matrix[11] * m2.matrix[12];
  matrix[9] = m1.matrix[8] * m2.matrix[1] +
                m1.matrix[9] * m2.matrix[5] +
                m1.matrix[10] * m2.matrix[9] +
                m1.matrix[11] * m2.matrix[13];
  matrix[10] = m1.matrix[8] * m2.matrix[2] +
                m1.matrix[9] * m2.matrix[6] +
                m1.matrix[10] * m2.matrix[10] +
                m1.matrix[11] * m2.matrix[14];
  matrix[11] = m1.matrix[8] * m2.matrix[3] +
                m1.matrix[9] * m2.matrix[7] +
                m1.matrix[10] * m2.matrix[11] +
                m1.matrix[11] * m2.matrix[15];

  matrix[12] = m1.matrix[12] * m2.matrix[0] +
                m1.matrix[13] * m2.matrix[4] +
                m1.matrix[14] * m2.matrix[8] +
                m1.matrix[15] * m2.matrix[12];
  matrix[13] = m1.matrix[12] * m2.matrix[1] +
                m1.matrix[13] * m2.matrix[5] +
                m1.matrix[14] * m2.matrix[9] +
                m1.matrix[15] * m2.matrix[13];
  matrix[14] = m1.matrix[12] * m2.matrix[2] +
                m1.matrix[13] * m2.matrix[6] +
                m1.matrix[14] * m2.matrix[10] +
                m1.matrix[15] * m2.matrix[14];
  matrix[15] = m1.matrix[12] * m2.matrix[3] +
                m1.matrix[13] * m2.matrix[7] +
                m1.matrix[14] * m2.matrix[11] +
                m1.matrix[15] * m2.matrix[15];

  return *this;
}

Plane &
Matrix44::Transform(Plane &plane) const {
  Vector3d v0(0.0f, 0.0f, 0.0f);
  Vector3d v1(1000.0f, 0.0f, 0.0f);
  Vector3d v2(0.0f, 0.0f, 1000.0f);
  SphrVector rVec;
  Matrix44 t1, t2;
  const Vector3d *n = &(plane.GetNorm());

  rVec.NewVector(*n, (n->DotVector(Vector3d::xAxisVec) <
                      n->DotVector(Vector3d::yAxisVec) ?
                      Vector3d::xAxisVec : Vector3d::yAxisVec));
  (t2.SetTranslation(plane.GetPoint())).Multiply(t1.SetRotate(rVec));
  t1.Multiply(*this, t2);
  t1.Transform(v0);
  t1.Transform(v1);
  t1.Transform(v2);
  return plane.NewPlane(v0, v1, v2);
}

//==================== TransSpace ====================

TransSpace::TransSpace(void) {
  position.NewVector(0.0f, 0.0f, 0.0f);
  orientFront.NewVector(0.0f, 1.0f, 0.0f);
  orientTop.NewVector(0.0f, 0.0f, 1.0f);
  scale.NewVector(1.0f, 1.0f, 1.0f);
}

TransSpace &
TransSpace::operator=(const TransSpace &src) {

  if (&src == this)
    return *this;

  position = src.position;
  orientFront = src.orientFront;
  orientTop = src.orientTop;
  orientation = src.orientation;
  scale = src.scale;

  return *this;
}

Vector3d &
TransSpace::GetPosition(Vector3d &newPos) const {
  return (newPos = position);
}

const Vector3d &
TransSpace::GetPosition(void) const {
  return position;
}

const Vector3d &
TransSpace::SetPosition(const Vector3d &newPos) {
  return (position = newPos);
}

const Vector3d &
TransSpace::MoveRelPosition(const Vector3d &posDelta) {
  Matrix44 m;
  Vector3d pos;

  m.SetRotate(orientation);
  m.Transform(pos, posDelta);

  return (position.AddVector(pos));
}

Vector3d &
TransSpace::GetOrientation(Vector3d &frontVec, Vector3d &topVec) const {
  frontVec = orientFront;
  topVec = orientTop;
  return frontVec;
}

SphrVector &
TransSpace::GetOrientation(SphrVector &orientVec) const {
  return (orientVec = orientation);
}

const SphrVector &
TransSpace::GetOrientation(void) const {
  return orientation;
}

const Vector3d &
TransSpace::SetOrientation(const Vector3d &frontVec, const Vector3d &topVec) {
  orientFront = frontVec;
  orientFront.Normalize();
  orientTop = topVec;
  orientTop.Normalize();
  orientation.NewVector(orientFront, orientTop);

  return orientFront;
}

const SphrVector &
TransSpace::SetOrientation(const SphrVector &orientVec) {
  orientation = orientVec;
  SphrVector::Sphr2Rect(orientation, orientFront, orientTop);

  return orientation;
}

const SphrVector &
TransSpace::ChangeRelOrientation(const SphrVector &deltaVec) {
  Vector3d ff(0.0f, 1.0f, 0.0f), ft(0.0f, 0.0f, 1.0f);
  Matrix44 m1, m2;

  m2.SetRotate(deltaVec);
  m1.SetRotate(orientation);
  m1.Multiply(m2);

  m1.Transform(ff);
  m1.Transform(ft);

  SetOrientation(ff, ft);
  return orientation;
}

Vector3d &
TransSpace::GetScale(Vector3d &scaleVec) const {
  return (scaleVec = scale);
}

const Vector3d &
TransSpace::GetScale(void) const {
  return scale;
}

const Vector3d &
TransSpace::SetScale(const Vector3d &newScale) {
  return (scale = newScale);
}

const Vector3d &
TransSpace::ChangeScale(const Vector3d &scaleDelta) {
  scale.AddVector(scaleDelta);
  scale.NewVector(Max(scale.GetX(), 0.01f),
                  Max(scale.GetY(), 0.01f),
                  Max(scale.GetZ(), 0.01f));
  return scale;
}

Matrix44 &
TransSpace::CalTransSpaceMatrix(Matrix44 &transMatrix) const {
  Matrix44 trans;

  transMatrix.Multiply(trans.SetTranslation(position));
  transMatrix.Multiply(trans.SetRotate(orientation));
  transMatrix.Multiply(trans.SetScale(scale));

  return transMatrix;
}

Matrix44 &
TransSpace::CalInvTransSpaceMatrix(Matrix44 &transMatrix) const {
  Matrix44 trans;
  Vector3d inverse;

  // Sanity Check.
  ASSERT(!EQUAL4(scale.GetX(), 0.0) &&
         !EQUAL4(scale.GetY(), 0.0) &&
         !EQUAL4(scale.GetZ(), 0.0));

  inverse.NewVector(1.0f / scale.GetX(),
                    1.0f / scale.GetY(),
                    1.0f / scale.GetZ());
  transMatrix.Multiply(trans.SetScale(inverse));
  transMatrix.Multiply(trans.SetInvRotate(orientation));
  inverse.NewVector(position.GetX(), position.GetY(), position.GetZ());
  transMatrix.Multiply(trans.SetInvTranslation(inverse));

  return transMatrix;
}
