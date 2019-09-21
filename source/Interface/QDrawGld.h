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

#ifndef __QDRAWGLD_H
#define __QDRAWGLD_H

typedef struct {
  float x, y;
  float ooz;                    /* 65535/Z (used for Z-buffering) */
  float oow;                    /* 1/W (used for W-buffering, texturing) */
  float s, t;
} GrVertex;

// ========== QDrawGlide ==========

class QDrawGlide : public QDraw {
public:
	QDrawGlide(CWnd *wnd);
	~QDrawGlide();

	void Size(int cx, int cy);

	void Suspend(void);
	void Resume(void);

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

	int LodSize(int size);
	void LoadTexture(Texture *texture);

private:
	BITMAPINFO bmi;
	RGBQUAD bmiColors[256];
	LPVOID data;
	CDC *dc;

	int lastMode;
};

#endif
