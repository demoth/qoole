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
sort_t *lmerge(sort_t *p, sort_t *q) {
	sort_t *r, head;

	for(r = &head; p && q; ) {
		if (p->key < q->key) {
			r = r->next = p;
			p = p->next;
		}
		else {
			r = r->next = q;
			q = q->next;
		}
	}
	r->next = (p ? p : q);
	return head.next;
}

sort_t *lsort(sort_t *p) {
	sort_t *q, *r;

	if(p) {
		q = p;
		for(r = q->next; r && (r = r->next) != NULL; r = r->next) { q = q->next; }
		r = q->next;
		q->next = NULL;
		if(r) { p = lmerge(lsort(r), lsort(p)); }
	}
	return p;
}

QDrawMGL *
NewQDraw(CWnd *wnd) {
	return new QDrawMGL(wnd);
}
#endif


// ========== QDrawMGL ==========

int QDrawMGL::count = 0;

QDrawMGL::QDrawMGL(CWnd *wnd) : QDraw(wnd) {
	if(!count) {
		MGL_registerDriver(MGL_PACKED8NAME, PACKED8_driver);
		MGL_registerDriver(MGL_PACKED16NAME, PACKED16_driver);
		MGL_initWindowed("");
	}
	count++;

	winDC = NULL;
	for(int i = 0; i < 2; i++)
		drawDC[i] = NULL;
	zbuf = NULL;
	texTri = NULL;

	bmi = (BITMAPINFO *)malloc(
		sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256);
}

QDrawMGL::~QDrawMGL() {
//	if(winDC)
//		MGL_destroyDC(winDC);
	for(int i = 0; i < 2; i++)
		if(drawDC[i])
			MGL_destroyDC(drawDC[i]);

	if(zbuf)
		free(zbuf);
	if(texTri)
		delete [] texTri;

	count--;
	if(!count) {
		MGL_exit();
	}

	free(bmi);
}

void
QDrawMGL::Size(int cx, int cy) {
	QDraw::Size(cx, cy);

	int i;

	for(i = 0; i < numBufs; i++)
		if(drawDC[i])
			MGL_destroyDC(drawDC[i]);

	cx /= 2;
	cx *= 2;
	cx += 1;
	width = cx;

	/*
	if(!winDC)
		winDC = MGL_createWindowedDC(pWnd->m_hWnd);
	else
		MGL_resizeWinDC(winDC);
	*/

	for(i = 0; i < numBufs; i++) {
		drawDC[i] = MGL_createMemoryDC(cx, cy, 
			bits, bits == 8 ? NULL : &pixelFormat565);
	}

	RealizePal();

	if(useZBuf) {
		if(zbuf)
			free(zbuf);
		zbuf = (ZBUF *)malloc(cx * cy * sizeof(ZBUF));
		zAdd = 0;
		zMax = 0;
	}
}

void
QDrawMGL::RealizePal(void) {
	if(bits == 8) {
		bmi->bmiHeader.biCompression = BI_RGB;
		bmi->bmiHeader.biClrUsed = 256;
		bmi->bmiHeader.biClrImportant = 256;
		for(int i = 0; i < 256; i++) {
			bmi->bmiColors[i].rgbRed = pal[i].red;
			bmi->bmiColors[i].rgbGreen = pal[i].green;
			bmi->bmiColors[i].rgbBlue = pal[i].blue;
		}

		for(i = 0; i < numBufs; i++) {
			MGL_setPalette(drawDC[i], pal, 256, 0);
			MGL_realizePalette(drawDC[i], 256, 0, false);
		}
	}
	else if(bits == 16) {
		bmi->bmiHeader.biCompression = BI_BITFIELDS;
		bmi->bmiHeader.biClrUsed = 0;
		bmi->bmiHeader.biClrImportant = 0;
		DWORD *a = (DWORD *)bmi->bmiColors;
		*a++ = 0xF800;
		*a++ = 0x07E0;
		*a++ = 0x001F;
	}

 	/*
	if(winDC) {
		MGL_setPalette(winDC, pal, 256, 0);
		MGL_realizePalette(winDC, 256, 0, false);
	}
	*/
}

void
QDrawMGL::Clear(void) {
	if(bits == 8)
		MGL_setColorCI(0);
	else
		MGL_setColorRGB(0, 0, 0);
	MGL_fillRectCoord(0, 0, width, height);
}

void
QDrawMGL::Color(int color) {
	if(bits == 8)
		MGL_setColorCI(color);
	else
		MGL_setColorRGB(pal[color].red, pal[color].green, pal[color].blue);
}

void
QDrawMGL::Begin(void) {
	MGL_makeCurrentDC(drawDC[useBuf]);
}

void
QDrawMGL::Line(int x1, int y1, int x2, int y2) {
	MGL_lineCoord(x1, y1, x2, y2);
}

void 
QDrawMGL::Paint(CPaintDC *pPaint) {

	bmi->bmiHeader.biWidth = width;
	bmi->bmiHeader.biHeight = -height;
	bmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi->bmiHeader.biPlanes = 1;
	bmi->bmiHeader.biBitCount = bits;
	bmi->bmiHeader.biSizeImage = 0;
	bmi->bmiHeader.biXPelsPerMeter = 0;
	bmi->bmiHeader.biYPelsPerMeter = 0;

	SetDIBitsToDevice(pPaint->m_ps.hdc, 0, 0, width, height,
		0, 0, 0, height, drawDC[useBuf]->surface, bmi, DIB_RGB_COLORS);

	/*
	rect_t rect = { 0, 0, width, height };
	MGL_setWinDC(winDC, pPaint->m_ps.hdc);
	MGL_bitBlt(winDC, drawDC[useBuf], rect, 0, 0, MGL_REPLACE_MODE);
	*/
}

void
QDrawMGL::CopyBuf(int bufNum) {
	rect_t rect = { 0, 0, width, height };
	MGL_bitBlt(drawDC[useBuf], drawDC[bufNum], rect, 0, 0, MGL_REPLACE_MODE);
}


void
QDrawMGL::ZBufInit(void) {
	useZBuf = true;

	if(zbuf)
		free(zbuf);
	if(texTri)
		delete texTri;

	zbuf = (ZBUF *)malloc(width * height * sizeof(ZBUF));
	texTri = new TexTri[TEXTRI_MAX];

	zAdd = 0;
	zMax = 0;
}

void
QDrawMGL::ZBufClear(void) {
	if(!zAdd)
		memset(zbuf, 0, width * height * sizeof(ZBUF));
	zAdd = zMax;

	zTexes = 0;
}	

int ftoi(float f) {
	if(f > 0)
		return (int)(fabs(f) + 0.5);
	else
		return -(int)(fabs(f) + 0.5);
}

void
QDrawMGL::ZBufTriangle(ZPoint zpoint[3]) {
	/*
	IZPoint zp[3];

	for(int i = 0; i < 3; i++) {
		zp[i].x = ftoi(zpoint[i].x) + orgX;
		zp[i].y = orgY - ftoi(zpoint[i].y);
		zp[i].z = ftoi(zpoint[i].z);
		zp[i].s = ftoi(zpoint[i].s);
		zp[i].t = ftoi(zpoint[i].t);
	}

	if(zbufTex && zbufTextured)
		ZBufScanTriTex(zp);
	else
		ZBufScanTri(zp);
	*/

	int texNum = zTexes;

	if(texNum == TEXTRI_MAX)
		return;
	else
		zTexes++;

	for(int i = 0; i < 3; i++) {
		texTri[texNum].zp[i].x = ftoi(zpoint[i].x) + orgX;
		texTri[texNum].zp[i].y = orgY - ftoi(zpoint[i].y);
		texTri[texNum].zp[i].z = ftoi(zpoint[i].z);
		texTri[texNum].zp[i].s = ftoi(zpoint[i].s);
		texTri[texNum].zp[i].t = ftoi(zpoint[i].t);
	}

	texTri[texNum].zbufTex = zbufTex;
	texTri[texNum].zbufColor = zbufColor;
	texTri[texNum].next = NULL;
	texTri[texNum].zAvg = (int)((zpoint[0].z + zpoint[1].z + zpoint[2].z) / 3.0f);

	if(texNum > 0 && texNum < TEXTRI_MAX)
		texTri[texNum - 1].next = (void *)&texTri[texNum];
}

void
QDrawMGL::ZBufRender(void) {
	if(!zTexes)
		return;

	/*
	for(int i = 0; i < zTexes; i++) {
		zbufTex = texTri[i].zbufTex;
		zbufColor = texTri[i].zbufColor;

		if(zbufTex && zbufTextured)
			ZBufScanTriTex(texTri[i].zp);
		else
			ZBufScanTri(texTri[i].zp);
	}
	*/

	TexTri *tt;
	tt = (TexTri *)lsort((sort_t *)texTri);

	while(tt) {
		zbufTex = tt->zbufTex;
		zbufColor = tt->zbufColor;

		if(zbufTex && zbufTextured)
			ZBufScanTriTex(tt->zp);
		else
			ZBufScanTri(tt->zp);

		tt = (TexTri *)tt->next;
	}
}
	
	
// -------
// z solid

#define ZS_DIV		0xFFFF
#define ZS_SHIFT		16

void
QDrawMGL::ZBufScanTri(IZPoint zpoint[3]) {
	IZPoint *zp1 = &zpoint[0];
	IZPoint *zp2 = &zpoint[1];
	IZPoint *zp3 = &zpoint[2];
	IZPoint *swap;

	int dy1, dy2, dy3, ix1, ix2, ix3, x1, x2, y2, y;
	int z1, z2, iz1, iz2, iz3;

	ZBUF *zptr;
	char *rptr;
	int rwidth = drawDC[useBuf]->mi.bytesPerLine;
	
	if(zp1->y > zp2->y) { swap = zp1; zp1 = zp2; zp2 = swap; }
	if(zp1->y > zp3->y) { swap = zp1; zp1 = zp3; zp3 = swap; }
	if(zp2->y > zp3->y) { swap = zp2; zp2 = zp3; zp3 = swap; }

	if(zp1->z == 0 || zp2->z == 0 || zp3->z == 0)
		return;

	zp1->z = ZS_DIV / zp1->z;
	zp2->z = ZS_DIV / zp2->z;
	zp3->z = ZS_DIV / zp3->z;

	dy1 = zp2->y - zp1->y;
	dy2 = zp3->y - zp2->y;
	dy3 = zp3->y - zp1->y;

	if(dy1) {
		dy1 = ZS_DIV / dy1;
		ix1 = (zp2->x - zp1->x) * dy1;
		iz1 = (zp2->z - zp1->z) * dy1;
	}
	else
		ix1 = iz1 = 0;
	
	if(dy2) {
		dy2 = ZS_DIV / dy2;
		ix2 = (zp3->x - zp2->x) * dy2;
		iz2 = (zp3->z - zp2->z) * dy2;
	}
	else
		ix2 = iz2 = 0;
	
	if(dy3) {
		dy3 = ZS_DIV / dy3;
		ix3 = (zp3->x - zp1->x) * dy3;
		iz3 = (zp3->z - zp1->z) * dy3;
	}
	else
		ix3 = iz3 = 0;
	
	y2 = zp2->y;
	x1 = x2 = zp1->x << ZS_SHIFT;
	z1 = z2 = zp1->z << ZS_SHIFT;
	zptr = zbuf + zp1->y * width;
	rptr = (char *)drawDC[useBuf]->surface + zp1->y * rwidth;
	if(y2 >= height) y2 = height - 1;
	if(x2 + ix3 > x1 + ix1) {
		for(y = zp1->y; y < y2; y++) {
			ZBufScanLine(y, zptr, rptr, x1 >> ZS_SHIFT, x2 >> ZS_SHIFT, z1, z2);
			x1 += ix1; x2 += ix3; z1 += iz1; z2 += iz3;
			zptr += width;
			rptr += rwidth;
		}
	}
	else {
		for(y = zp1->y; y < y2; y++) {
			ZBufScanLine(y, zptr, rptr, x2 >> ZS_SHIFT, x1 >> ZS_SHIFT, z2, z1);
			x1 += ix1; x2 += ix3; z1 += iz1; z2 += iz3;
			zptr += width;
			rptr += rwidth;
		}
	}

	if(y >= height)
		return;
	
	y2 = zp3->y;
	x1 = zp2->x << ZS_SHIFT;
	z1 = zp2->z << ZS_SHIFT;
	if(y2 >= height) y2 = height - 1;
	if(x2 > x1) {
		for(y = zp2->y; y <= y2; y++) {
			ZBufScanLine(y, zptr, rptr, x1 >> ZS_SHIFT, x2 >> ZS_SHIFT, z1, z2);
			x1 += ix2; x2 += ix3; z1 += iz2; z2 += iz3;
			zptr += width;
			rptr += rwidth;
		}
	}
	else {
		for(y = zp2->y; y <= y2; y++) {
			ZBufScanLine(y, zptr, rptr, x2 >> ZS_SHIFT, x1 >> ZS_SHIFT, z2, z1);
			x1 += ix2; x2 += ix3; z1 += iz2; z2 += iz3;
			zptr += width;
			rptr += rwidth;
		}
	}
}

void
QDrawMGL::ZBufScanLine(int y, ZBUF *zb, char *r, int x1, int x2, int z1, int z2) {
	if(y < 0 || x1 == x2 || y >= height || x1 >= width || x2 < 0)
		return;

	//========================
	z1 >>= 8;
	z2 >>= 8;

	z1 += zAdd;
	z2 += zAdd;

	if((ZBUF)z1 > zMax)
		zMax = z1;
	if((ZBUF)z2 > zMax)
		zMax = z2;
	//========================

	ZBUF z = z1, iz = (z2 - z1) / (x2 - x1);

	if(x1 < 0) {
		z += iz * -x1;
		x1 = 0;
	}
	if(x2 >= width)
		x2 = width - 1;
	
	ZBUF *zptr = zb + x1;
	ZBUF *zend = zptr + (x2 - x1);

if(bits == 8) {
	char *rptr = r + x1;
	while(zptr <= zend) {
		if(z > *zptr) {
			*zptr = z;
			*rptr = (char)zbufColor;
		}

		z += iz;
		zptr++;
		rptr++;
	}
}
else {
	short *rptr = (short *)r + x1;
	while(zptr <= zend) {
		if(z > *zptr) {
			*zptr = z;
			*rptr = zbufColor;
		}

		z += iz;
		zptr++;
		rptr++;
	}
}

}

// ----------
// z tmapping

#define XY_DIV		0xFFFF
#define XY_SHIFT	16

#define Z_DIV		0xFFFF
#define Z_SHIFT		16

#define ZL_DIV		0x3FFFFFF
#define ZL_SHIFT	12
#define ST_SHIFT	22

void
QDrawMGL::ZBufScanTriTex(IZPoint zpoint[3]) {
	IZPoint *zp1 = &zpoint[0];
	IZPoint *zp2 = &zpoint[1];
	IZPoint *zp3 = &zpoint[2];
	IZPoint *swap;

	int dy1, dy2, dy3, ix1, ix2, ix3, x1, x2, y1, y2, y;
	int z1, z2, iz1, iz2, iz3;

	int is1, is2, is3, it1, it2, it3;
	int s1, s2, t1, t2;

	ZBUF *zptr;
	char *rptr;
	int rwidth = drawDC[useBuf]->mi.bytesPerLine;
	int zwidth = width;

	int fast_count = 0;

	int twidth = zbufTex->width;
	int theight = zbufTex->height;

	if(zp1->y > zp2->y) { swap = zp1; zp1 = zp2; zp2 = swap; }
	if(zp1->y > zp3->y) { swap = zp1; zp1 = zp3; zp3 = swap; }
	if(zp2->y > zp3->y) { swap = zp2; zp2 = zp3; zp3 = swap; }

	if(zp1->z == 0 || zp2->z == 0 || zp3->z == 0)
		return;

	int zp1z = zp1->z;
	int zp2z = zp2->z;
	int zp3z = zp3->z;
	if(zp1z < 4) zp1z = 4;
	if(zp2z < 4) zp2z = 4;
	if(zp3z < 4) zp3z = 4;

	zp1z = Z_DIV / zp1z;
	zp2z = Z_DIV / zp2z;
	zp3z = Z_DIV / zp3z;

	dy1 = zp2->y - zp1->y;
	dy2 = zp3->y - zp2->y;
	dy3 = zp3->y - zp1->y;

	if(dy1) {
		iz1 = (zp2z - zp1z) * (Z_DIV / dy1);

		dy1 = XY_DIV / dy1;
		ix1 = (zp2->x - zp1->x) * dy1;
		is1 = ((zp2->s * zp2z - zp1->s * zp1z) * dy1) >> XY_SHIFT;
		it1 = ((zp2->t * zp2z - zp1->t * zp1z) * dy1) >> XY_SHIFT;
}
	else {
		ix1 = iz1 = is1 = it1 = 0;
	}
	
	if(dy2) {
		iz2 = (zp3z - zp2z) * (Z_DIV / dy2);

		dy2 = XY_DIV / dy2;
		ix2 = (zp3->x - zp2->x) * dy2;
		is2 = ((zp3->s * zp3z - zp2->s * zp2z) * dy2) >> XY_SHIFT;
		it2 = ((zp3->t * zp3z - zp2->t * zp2z) * dy2) >> XY_SHIFT;
	}
	else {
		ix2 = iz2 = is2 = it2 = 0;
	}
	
	if(dy3) {
		iz3 = (zp3z - zp1z) * (Z_DIV / dy3);

		dy3 = XY_DIV / dy3;
		ix3 = (zp3->x - zp1->x) * dy3;
		is3 = ((zp3->s * zp3z - zp1->s * zp1z) * dy3) >> XY_SHIFT;
		it3 = ((zp3->t * zp3z - zp1->t * zp1z) * dy3) >> XY_SHIFT;
	}
	else {
		ix3 = iz3 = is3 = it3 = 0;
	}

	y1 = zp1->y;
	y2 = zp2->y;
	x1 = x2 = zp1->x << XY_SHIFT;
	z1 = z2 = zp1z << Z_SHIFT;
	s1 = s2 = zp1->s * zp1z;
	t1 = t2 = zp1->t * zp1z;
	zptr = zbuf + zp1->y * width;
	rptr = (char *)drawDC[useBuf]->surface + zp1->y * rwidth;
	if(y2 >= height) y2 = height - 1;
	if(x2 + ix3 > x1 + ix1) {
		for(y = y1; y < y2; y++) {
			ZBufScanLineTex(y, zptr, rptr, twidth, theight, 
				x1 >> XY_SHIFT, x2 >> XY_SHIFT, z1, z2, s1, s2, t1, t2);
				
			x1 += ix1; x2 += ix3; z1 += iz1; z2 += iz3;
			s1 += is1; s2 += is3; t1 += it1; t2 += it3;
			zptr += zwidth;
			rptr += rwidth;
		}
	}
	else {
		for(y = y1; y < y2; y++) {
			ZBufScanLineTex(y, zptr, rptr, twidth, theight,
				x2 >> XY_SHIFT, x1 >> XY_SHIFT, z2, z1, s2, s1, t2, t1);

			x1 += ix1; x2 += ix3; z1 += iz1; z2 += iz3;
			s1 += is1; s2 += is3; t1 += it1; t2 += it3;
			zptr += zwidth;
			rptr += rwidth;
		}
	}

	fast_count = 0;
	
	if(y >= height)
		return;
	
	y2 = zp3->y;
	x1 = zp2->x << XY_SHIFT;
	z1 = zp2z << Z_SHIFT;
	s1 = zp2->s * zp2z;
	t1 = zp2->t * zp2z;
	if(y2 >= height) y2 = height - 1;
	if(x2 > x1) {
		for(y = zp2->y; y <= y2; y++) {
			ZBufScanLineTex(y, zptr, rptr, twidth, theight,
				x1 >> XY_SHIFT, x2 >> XY_SHIFT, z1, z2, s1, s2, t1, t2);

			x1 += ix2; x2 += ix3; z1 += iz2; z2 += iz3;
			s1 += is2; s2 += is3; t1 += it2; t2 += it3;
			zptr += zwidth;
			rptr += rwidth;
		}
	}
	else {
		for(y = zp2->y; y <= y2; y++) {
			ZBufScanLineTex(y, zptr, rptr, twidth, theight,
				x2 >> XY_SHIFT, x1 >> XY_SHIFT, z2, z1, s2, s1, t2, t1);

			x1 += ix2; x2 += ix3; z1 += iz2; z2 += iz3;
			s1 += is2; s2 += is3; t1 += it2; t2 += it3;
			zptr += zwidth;
			rptr += rwidth;
		}
	}
}

void
QDrawMGL::ZBufScanLineTex(int y, ZBUF *zb, char *r, int w, int h, int x1, int x2,
						   int z1, int z2, int s1, int s2, int t1, int t2) {
	if(y < 0 || x1 >= x2 || y >= height || x1 >= width || x2 < 0)
		return;

	//========================
	z1 >>= 8;
	z2 >>= 8;

	z1 += zAdd;
	z2 += zAdd;

	if((ZBUF)z1 > zMax)
		zMax = z1;
	if((ZBUF)z2 > zMax)
		zMax = z2;
	//========================

	ZBUF z = z1, iz = (z2 - z1) / (x2 - x1);
	int s = s1, is = (s2 - s1) / (x2 - x1);
	int t = t1, it = (t2 - t1) / (x2 - x1);

	if(x1 < 0) {
		z += iz * -x1;
		s += is * -x1;
		t += it * -x1;
		x1 = 0;
	}
	if(x2 >= width)
		x2 = width - 1;

if(bits == 8) {
	ZBUF *zptr = zb + x1;
	ZBUF *zend = zptr + (x2 - x1);
	char *rptr = r + x1;
	int zv;
	int hm1, wm1;
	char *tptr;
	int mip, m;

	int zd = MAX(z1, z2) - zAdd;
	if(zd < 5 * 4096)
		mip = 3;
	else if(zd < 10 * 4096)
		mip = 2;
	else if(zd < 20 * 4096)
		mip = 1;
	else
		mip = 0;

	m = 1 << mip;
	w /= m;
	h /= m;
	s /= m;
	t /= m;
	is /= m;
	it /= m;

	tptr = (char *)zbufTex->mip[mip];

	wm1 = w - 1;
	hm1 = h - 1;

	while(zptr <= zend) {
		if(z > *zptr) {
//			zv = z >> ZL_SHIFT;
			zv = (z - zAdd) >> (ZL_SHIFT - 8);
			if(zv) {
				*zptr = z;
				zv = ZL_DIV / zv;
				*rptr = *(tptr +
					(((t * zv) >> ST_SHIFT) & hm1) * w +
					(((s * zv) >> ST_SHIFT) & wm1));
			}
		}
		z += iz;
		s += is;
		t += it;
		zptr++;
		rptr++;
	}
}
else {
	ZBUF *zptr = zb + x1;
	ZBUF *zend = zptr + (x2 - x1);
	short *rptr = (short *)r + x1;
	int zv;
	int hm1, wm1;
	short *tptr;
	int mip, m;

	int zd = MAX(z1, z2) - zAdd;
	if(zd < 5 * 4096)
		mip = 3;
	else if(zd < 10 * 4096)
		mip = 2;
	else if(zd < 20 * 4096)
		mip = 1;
	else
		mip = 0;

	m = 1 << mip;
	w /= m;
	h /= m;
	s /= m;
	t /= m;
	is /= m;
	it /= m;

	tptr = (short *)zbufTex->mip[mip];

	wm1 = w - 1;
	hm1 = h - 1;

	while(zptr <= zend) {
		if(z > *zptr) {
//			zv = z >> ZL_SHIFT;
			zv = (z - zAdd) >> (ZL_SHIFT - 8);
			if(zv) {
				*zptr = z;
				zv = ZL_DIV / zv;
				*rptr = *(tptr +
					(((t * zv) >> ST_SHIFT) & hm1) * w +
					(((s * zv) >> ST_SHIFT) & wm1));
			}
		}
		z += iz;
		s += is;
		t += it;
		zptr++;
		rptr++;
	}
}

}

#endif
