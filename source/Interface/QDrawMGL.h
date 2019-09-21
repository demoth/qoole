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

#ifndef __QDRAWMGL_H
#define __QDRAWMGL_H

#ifndef USE_QDRAWDLL

#include "mgl.h"

// ========== QDrawMGL ==========

extern pixel_format_t pixelFormat565;

#define ZBUF unsigned int

class QDrawMGL : public QDraw {
public:
	QDrawMGL(CWnd *wnd);
	~QDrawMGL();

	static void Init(void);
	static void Exit(void);

	void Size(int cx, int cy);
	void Clear(void);
	void Color(int color);
	void Begin(void);
	void Line(int x1, int y1, int x2, int y2);
	void Paint(CPaintDC *pPaint);
	void CopyBuf(int bufNum);
	void RealizePal(void);

	void ZBufInit(void);
	void ZBufClear(void);
	void ZBufTriangle(ZPoint zpoint[3]);
	void ZBufRender(void);
	void ZBufScanTri(IZPoint zpoint[3]);
	void ZBufScanTriTex(IZPoint zpoint[3]);
	void ZBufScanLine(int y, ZBUF *zb, char *r, int x1, int x2,
					  int z1, int z2);
	void ZBufScanLineTex(int y, ZBUF *zb, char *r, int w, int h, int x1, int x2,
						 int z1, int z2, int s1, int s2, int t1, int t2);

private:
	MGLDC *winDC;
	MGLDC *drawDC[2];

	ZBUF *zbuf;

	ZBUF zAdd, zMax;

	BITMAPINFO *bmi;
	
	static int count;
};

#endif

#endif
