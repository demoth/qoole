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

// ========== QDrawOpenGL ==========

class QDrawOpenGL : public QDraw {
public:
	QDrawOpenGL(CWnd *wnd);
	~QDrawOpenGL();

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

private:
	HDC hDC;
	HGLRC hGLRC;
	HPALETTE hPalette;

	int color;

	struct {
		float x1, y1, x2, y2;
		float r, g, b;
	} line[10000];
	int lines;

	struct {
		float x[3], y[3], z[3];
		float s[3], t[3];
		float n[3];
		float r, g, b;
		Texture *texture;
	} tri[3000];

	struct {
		float nf[3];
		float nv[3];
	} normals[9000];

	int tris;

	static HDC currentDC;
};
