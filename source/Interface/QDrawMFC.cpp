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

#include "stdafx.h"
#include "QDraw.h"

#ifndef USE_QDRAWDLL

#ifdef IS_QDRAWDLL
QDrawMFC *
NewQDraw(CWnd *wnd) {
	return new QDrawMFC(wnd);
}
#endif

// ========== QDrawMFC ==========

QDrawMFC::QDrawMFC(CWnd *wnd) : QDraw(wnd) {
	CDC *pDC = wnd->GetDC();
	for(int i = 0; i < numBufs; i++)
		dc[i].CreateCompatibleDC(pDC);
	wnd->ReleaseDC(pDC);
}

QDrawMFC::~QDrawMFC() {
}

void
QDrawMFC::Size(int cx, int cy) {
	QDraw::Size(cx, cy);

	CDC *pDC = pWnd->GetDC();

	for(int i = 0; i < numBufs; i++) {
		bmp[i].DeleteObject();
		bmp[i].CreateCompatibleBitmap(pDC, cx, cy);
		dc[i].SelectObject(bmp[i]);
	}

	pWnd->ReleaseDC(pDC);
}

void
QDrawMFC::Clear(void) {
	dc[useBuf].BitBlt(0, 0, width, height, NULL, 0, 0, BLACKNESS);
}

void
QDrawMFC::Color(int color) {
	dc[useBuf].SelectObject(pen[color]);
}

void
QDrawMFC::Begin(void) {
}

void
QDrawMFC::Line(int x1, int y1, int x2, int y2) {
	dc[useBuf].MoveTo(x1, y1);
	dc[useBuf].LineTo(x2, y2);
}

void 
QDrawMFC::Paint(CPaintDC *pPaint) {
	pPaint->BitBlt(0, 0, width, height, &dc[useBuf], 0, 0, SRCCOPY);
}

void
QDrawMFC::CopyBuf(int bufNum) {
	dc[useBuf].BitBlt(0, 0, width, height, &dc[bufNum], 0, 0, SRCCOPY);
}

void
QDrawMFC::RealizePal(void) {
	for(int i = 0; i < 256; i++)
		pen[i].CreatePen(PS_SOLID, 1, RGB(pal[i].red, pal[i].green, pal[i].blue));
}

void QDrawMFC::ZBufInit(void) { }
void QDrawMFC::ZBufClear(void) { }
void QDrawMFC::ZBufTriangle(ZPoint zpoint[3]) { }
void QDrawMFC::ZBufRender(void) { }

#endif
