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


#ifndef __QDRAW_H
#define __QDRAW_H

#include "stdafx.h"
#include "Texture.h"
#include "Objects.h"
#include "Geometry.h"
#include "resource.h"
#include <math.h>

// ============================
// define to use QDrawXXX.dll's
#ifndef IS_QDRAWDLL
#define USE_QDRAWDLL
#endif

#define TEXTRI_MAX 2500

typedef struct {
	float x, y, z;
	float s, t;
} ZPoint;

typedef struct {
	int x, y, z;
	int s, t;
} IZPoint;


struct TexTri {
	void *next;
	int zAvg;
	IZPoint zp[3];
	Texture *zbufTex;
	unsigned short zbufColor;
};


// ========== QDraw ==========

class QDraw {
public:
	QDraw(CWnd *wnd);
	virtual ~QDraw();

	static void Init(void);
	static void Exit(void);

	static QDraw *New(char *driver, int bits, CWnd *pWnd, int numBufs = 1);
	void UseBuf(int bufNum) { ASSERT(bufNum < numBufs); useBuf = bufNum; }
	int GetNumBufs(void) { return numBufs; }
	void SetZoomPtr(float *_pZoomVal) { pZoomVal = _pZoomVal; }

	void RenderLine(float x1, float y1, float x2, float y2);
	void RenderPoint(float x, float y, int size = 5);
	void Grid(float xorg, float yorg);
	void GridLines(RECT rect, float xorg, float yorg, float diff);
	void Point(int x, int y, int size);

	void Box(int x1, int y1, int x2, int y2);
	void StippleBox(int x1, int y1, int x2, int y2, int l1, int l2);
	void Cross(int x1, int y1, int size);
	void SetColor(int color, int r, int g, int b);
	void UseColor(int color);
	void LoadPal(char *name);

	void RenderMode(int mode) { renderMode = mode; } 

	virtual void Size(int cx, int cy);

	virtual void Suspend(void) { };
	virtual void Resume(void) { };

	virtual void Clear(void) = 0;
	virtual void Color(int color) = 0;
	virtual void Begin(void) = 0;
	virtual void Line(int x1, int y1, int x2, int y2) = 0;
	virtual void Paint(CPaintDC *pPaint) = 0;
	virtual void CopyBuf(int bufNum) = 0;
	virtual void RealizePal(void) = 0;

	// Z-Buffering
	virtual void ZBufInit(void) = 0;
	virtual void ZBufClear(void) = 0;
	virtual void ZBufTriangle(ZPoint zpoint[3]) = 0;
	virtual void ZBufRender(void) = 0;

	void ZBufTextured(bool textured) { zbufTextured = textured; }
	bool ZBufSetup(Object *obj, FaceTex *faceTex, Vector3d norm);
	void ZBufCalcST(float x, float y, float z, float &s, float &t);
	static void OutputText(const char *text, ...);

	char *GetDriver(void) { return driver; }

	// Config stuff.
	static LConfig *cfg;
	static bool drawGridStep1, drawGridStep2;
	static int gridStep1, gridStep2;
	static float textureGamma;

protected:

	CWnd *pWnd;
	int numBufs;
	int useBuf;
	float *pZoomVal;
	int width, height;
	int orgX, orgY;

	int bits;
	palette_t pal[256];
	palette_t setpal[256];
	bool fixedPal;

	bool useZBuf;
	bool zbufTextured;
	Texture *zbufTex;
	unsigned short zbufColor;
	int axisAlign;
	float soffs, toffs;
	float rot;
	float sscale, tscale;

	int zTexes;
	TexTri *texTri;

	int renderMode;

	char driver[16];
};

#include "QDrawMFC.h"
#include "QDrawMGL.h"
#include "QDrawGld.h"
#include "QDrawDX.h"
#include "QDrawOGL.h"

#endif
