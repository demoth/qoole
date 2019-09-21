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

// virtual QDraw functions that aren't pure

#ifndef USE_QDRAWDLL
pixel_format_t pixelFormat565 = { 31, 63, 31, 0, 11, 3, 5, 2, 0, 3, 0, 0 };
#endif

QDraw::QDraw(CWnd *wnd) {
	pWnd = wnd;
	numBufs = 2;
	useBuf = 0;
	pZoomVal = NULL;
	useZBuf = false;
	renderMode = ID_VIEW_RENDER_WIREFRAME;
	fixedPal = false;
}

QDraw::~QDraw() {
}

void
QDraw::Size(int cx, int cy) {
	if(cx < 1)
		cx = 1;
	if(cy < 1)
		cy = 1;

	width = cx;
	height = cy;
	orgX = cx / 2;
	orgY = cy / 2;
}
