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
 * Math3d.h
 */

#ifndef _MATH3D_H_
#define _MATH3D_H_

#include "LCommon.h"

//==================== Functions ====================

#ifndef ABS
#define ABS(X) ((X) >= 0 ? (X) : (-(X)))
#endif

#define Max(X, Y) ((X) > (Y) ? (X) : (Y))
#define Min(X, Y) ((X) < (Y) ? (X) : (Y))

#define EQUAL0(X, Y) (ABS((X) - (Y)) < 1.0f)
#define EQUAL1(X, Y) (ABS((X) - (Y)) < 0.1f)
#define EQUAL2(X, Y) (ABS((X) - (Y)) < 0.01f)
#define EQUAL3(X, Y) (ABS((X) - (Y)) < 0.001f)
#define EQUAL4(X, Y) (ABS((X) - (Y)) < 0.0001f)
#define EQUAL5(X, Y) (ABS((X) - (Y)) < 0.00001f)
#define EQUAL6(X, Y) (ABS((X) - (Y)) < 0.000001f)

// float to float
#define ROUND(X) ( (X) > 0.0f ? \
                 ( (float) ( (int) ( (X) + 0.5f) ) ) : \
                 ( (float) ( (int) ( (X) - 0.5f) ) ) )

#define ROUND0(X, I) ( ( (X) * (I) ) > 0.0f ? \
                     ( ( (float) ( (int) ( (X) * (I) + 0.5f) ) ) / (I) ): \
                     ( ( (float) ( (int) ( (X) * (I) - 0.5f) ) ) / (I) ) )
#define ROUND1(X) (ROUND0((X), 10.0f))
#define ROUND2(X) (ROUND0((X), 100.0f))
#define ROUND3(X) (ROUND0((X), 1000.0f))
#define ROUND4(X) (ROUND0((X), 10000.0f))
#define ROUND5(X) (ROUND0((X), 100000.0f))
#define ROUND6(X) (ROUND0((X), 1000000.0f))

// float to int
#define ROUNDI(X) ((X) > 0.0f ? \
                  (((int) ((X) + 0.5f))) : \
                  (((int) ((X) - 0.5f))))

#ifndef PI
#define PI (3.1415926535897932385f)
#endif
#define RADIAN (PI / 180.0f)
#define RAD2DEG(X) ((X) / RADIAN)
#define DEG2RAD(X) ((X) * RADIAN)
extern float ASin(float opp, float hyp);
extern float ATan(float opp, float adj);

//==================== Vector3d ====================

class Vector3d {
 public:
  Vector3d(float xVec = 0.0f, float yVec = 0.0f, float zVec = 0.0f) {
    x = xVec;  y = yVec;  z = zVec;
  };

  // Vector3d(const Vector3d &src) { *this = src; };
  // ~Vector3d(void);
  // Vector3d &operator=(const Vector3d &src);

  Vector3d &NewVector(float xVec, float yVec, float zVec) {
    x = xVec;  y = yVec;  z = zVec;
    return *this;
  };

  inline float GetX(void) const { return x; };
  inline float GetY(void) const { return y; };
  inline float GetZ(void) const { return z; };

  inline void SetX(float val) { x = val; };
  inline void SetY(float val) { y = val; };
  inline void SetZ(float val) { z = val; };

  float GetMag(void) const;
  float GetMagSqr(void) const;

  Vector3d &Normalize(void);

  Vector3d &AddVector(const Vector3d &vec);
  Vector3d &AddVector(const Vector3d &v1, const Vector3d &v2);
  Vector3d &SubVector(const Vector3d &vec);
  Vector3d &SubVector(const Vector3d &v1, const Vector3d &v2);
  Vector3d &MultVector(float c);
  Vector3d &MultVector(const Vector3d &vec, float c);

  float DotVector(const Vector3d &vec) const;
  float CosTheta(const Vector3d &vec) const;
  Vector3d &CrossVector(const Vector3d &vec);
  Vector3d &CrossVector(const Vector3d &v1, const Vector3d &v2);

  bool IsParallel(const Vector3d &vec) const;
  bool operator==(const Vector3d &vec) const;
  bool operator!=(const Vector3d &vec) const { return !(operator==(vec)); }

  static const Vector3d origVec, xAxisVec, yAxisVec, zAxisVec;

 private:
  float x, y, z;
};

//==================== SphrVector ====================

class SphrVector {
 public:
  SphrVector(float yaw = 0.0f, float pitch = 0.0f, float roll = 0.0f);
  SphrVector(const SphrVector &src) { *this = src; };
  // ~SphrVector(void);
  // SphrVector &operator=(const SphrVector &src);

  SphrVector &NewVector(float yaw, float pitch, float roll);
  SphrVector &NewVector(const Vector3d &frontVec, const Vector3d &topVec);

  inline float GetYaw(void) const { return yawAng; };
  inline float GetPitch(void) const { return pitchAng; };
  inline float GetRoll(void) const { return rollAng; };

  SphrVector &Normalize(void);
  Vector3d &GetRectVector(Vector3d &frontVec, Vector3d &topVec) const;

  bool operator==(const SphrVector &oriVec) const;

  static Vector3d &Sphr2Rect(const SphrVector &sphrVec,
                             Vector3d &frontVec, Vector3d &topVec);
  static SphrVector &Rect2Sphr(const Vector3d &frontVec,
                               const Vector3d &topVec,
                               SphrVector &sphrVec);

 private:
  float yawAng, pitchAng, rollAng;
};

//==================== Line ====================

class Line {
 public:
  Line(void);
  Line(const Vector3d &pt, const Vector3d &slope) { NewLine(pt, slope); };
  Line(const Line &src) { *this = src; };
  // ~Line(void);
  // Line &operator=(const Line &src);

  Line &NewLine(const Vector3d &pt, const Vector3d &slope);

  const Vector3d &GetPoint(void) const { return pt0; };
  const Vector3d &GetSlope(void) const { return v; };
  bool IsOnLine(const Vector3d &pt, float epsilon = 0.01f) const;
  float Parametrize(const Vector3d &pt) const;
  bool IsParallel(const Line &line2) const;
  bool Intersect(const Line &line2, Vector3d &intrsctPt) const;
  float IntersectAngle(const Line &line2) const;

 private:
  Vector3d pt0, v;
};

//==================== Plane ====================

class Plane {
 public:
  Plane(const Vector3d &norm, const Vector3d &pt) { NewPlane(norm, pt); };
  Plane(const Vector3d &pt1, const Vector3d &pt2,
        const Vector3d &pt3) { NewPlane(pt1, pt2, pt3); };
  Plane(const Plane &src) { *this = src; };
  // ~Plane();
  // Plane &operator=(const Plane &src);

  Plane &NewPlane(const Vector3d norm, const Vector3d &pt);
  Plane &NewPlane(const Vector3d &pt1, const Vector3d &pt2,
                  const Vector3d &pt3);

  const Vector3d &GetNorm(void) const { return n; };
  const Vector3d &GetPoint(void) const { return pt0; };
  int  PointInPlane(const Vector3d &pt, float epsilon = 0.0001f) const;
  float DistOffPlane(const Vector3d &pt) const;
  bool Intersect(const Line &l, Vector3d &intrsctPt) const;
  bool Intersect(const Plane &plane2, Line &l) const;
  bool Intersect(const Plane &plane2, const Plane &plane3,
                 Vector3d &intrsctPt) const;
  float IntersectAngle(const Plane &plane2) const;
  bool IsSame(const Plane &plane2) const;
  void Normalize(Vector3d &p1, Vector3d &p2, Vector3d &p3) const;
  void Round45(void);
  
  static const Plane xyPlane, yzPlane, zxPlane;

 private:
  Vector3d n, pt0;
  float d;
};

//==================== Matrix44 ====================

class Matrix44 {

 public:
  // Matrix44(void);  // Warning.  UnInit data for optimization.
  // Matrix44(const Matrix44 &src) { *this = src; };
  // ~Matrix44(void);
  // Matrix44 &operator=(const Matrix44 &src);

  inline float GetMatrix(int row, int col) const {
    ASSERT(row >= 0 && row < 4 && col >= 0 && col < 4);
    return matrix[row * 4 + col];
  };

  inline float SetMatrix(int row, int col, float m) {
    ASSERT(row >= 0 && row < 4 && col >= 0 && col < 4);
    return (matrix[row * 4 + col] = m);
  };

  inline Matrix44 &SetIdentity(void) {
    matrix[1] = matrix[2] = matrix[3] = matrix[4] =
    matrix[6] = matrix[7] = matrix[8] = matrix[9] =
    matrix[11] = matrix[12] = matrix[13] = matrix[14] = 0.0f;
    matrix[0] = matrix[5] = matrix[10] = matrix[15] = 1.0f;
    return *this;
  };

  Matrix44 &SetTranslation(const Vector3d &transVec);
  Matrix44 &SetInvTranslation(const Vector3d &transVec);
  Matrix44 &SetScale(const Vector3d &scaleVec);
  Matrix44 &SetRotateX(float theta);
  Matrix44 &SetRotateY(float theta);
  Matrix44 &SetRotateZ(float theta);
  Matrix44 &SetRotate(const SphrVector &rotateVec);
  Matrix44 &SetInvRotate(const SphrVector &rotateVec);

  Matrix44 &AddMatrix(const Matrix44 &m);
  Matrix44 &AddMatrix(const Matrix44 &m1, const Matrix44 &m2);
  Matrix44 &SubMatrix(const Matrix44 &m);
  Matrix44 &SubMatrix(const Matrix44 &m1, const Matrix44 &m2);

  Matrix44 &Multiply(const Matrix44 &m);
  Matrix44 &Multiply(const Matrix44 &m1, const Matrix44 &m2);

  Plane &Transform(Plane &plane) const;

  Vector3d &Transform(Vector3d &vec) const {
    float x = matrix[0] * vec.GetX() + matrix[1] * vec.GetY() +
              matrix[2] * vec.GetZ() + matrix[3];
    float y = matrix[4] * vec.GetX() + matrix[5] * vec.GetY() +
              matrix[6] * vec.GetZ() + matrix[7];
    float z = matrix[8] * vec.GetX() + matrix[9] * vec.GetY() +
              matrix[10] * vec.GetZ() + matrix[11];
    return vec.NewVector(x, y, z);
  }

  Vector3d &Transform(Vector3d &vec, const Vector3d &fVec) const {
    return vec.NewVector(matrix[0] * fVec.GetX() + matrix[1] * fVec.GetY() +
                         matrix[2] * fVec.GetZ() + matrix[3],
                         matrix[4] * fVec.GetX() + matrix[5] * fVec.GetY() +
                         matrix[6] * fVec.GetZ() + matrix[7],
                         matrix[8] * fVec.GetX() + matrix[9] * fVec.GetY() +
                         matrix[10] * fVec.GetZ() + matrix[11]);
  }

  Vector3d &Transform(Vector3d &vec, float x, float y, float z) const {
    return vec.NewVector(matrix[0] * x + matrix[1] * y +
                         matrix[2] * z + matrix[3],
                         matrix[4] * x + matrix[5] * y +
                         matrix[6] * z + matrix[7],
                         matrix[8] * x + matrix[9] * y +
                         matrix[10] * z + matrix[11]);
  }

 private:
  float matrix[16];
};

//==================== TransSpace ====================

class TransSpace {
 public:
  TransSpace(void);
  TransSpace(const TransSpace &src) { *this = src; };
  // ~TransSpace(void);
  TransSpace &operator=(const TransSpace &src);

  //====================
  Vector3d &GetPosition(Vector3d &newPos) const;
  const Vector3d &GetPosition(void) const;
  const Vector3d &SetPosition(const Vector3d &newPos);
  const Vector3d &MoveRelPosition(const Vector3d &posDelta);

  Vector3d &GetOrientation(Vector3d &frontVec, Vector3d &topVec) const;
  SphrVector &GetOrientation(SphrVector &orientVec) const;
  const SphrVector &GetOrientation(void) const;
  const Vector3d &SetOrientation(const Vector3d &frontVec,
                                 const Vector3d &topVec);
  const SphrVector &SetOrientation(const SphrVector &orientVec);
  const SphrVector &ChangeRelOrientation(const SphrVector &deltaVec);

  Vector3d &GetScale(Vector3d &scaleVec) const;
  const Vector3d &GetScale(void) const;
  const Vector3d &SetScale(const Vector3d &newScale);
  const Vector3d &ChangeScale(const Vector3d &scaleDelta);

  //====================
  Matrix44 &CalTransSpaceMatrix(Matrix44 &transMatrix) const;
  Matrix44 &CalInvTransSpaceMatrix(Matrix44 &transMatrix) const;

 private:
  Vector3d position;
  Vector3d orientFront, orientTop;
  SphrVector orientation;
  Vector3d scale;
};

#endif // _MATH3D_H_
