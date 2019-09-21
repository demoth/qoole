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

#include "Glide.h"

#ifdef IS_QDRAWDLL
QDrawGlide *
NewQDraw(CWnd *wnd) {
	return new QDrawGlide(wnd);
}
#endif

// ========== QDrawGlide ==========

bool glideInit = false;

int refCount = 0;

HWND glideHwnd;
FxU32 orgAddress, startAddress, endAddress;

void
GlideInit(void) {
	if(glideInit)
		return;

	glideHwnd = CreateWindow("QGlide", "QGlide",
		WS_OVERLAPPED,
		0, 0, 640, 480,
		NULL, NULL, AfxGetInstanceHandle(), NULL);

	grGlideInit();
	grSstSelect(0);

	grSstWinOpen((FxU32)AfxGetMainWnd()->m_hWnd, GR_RESOLUTION_640x480, 
		GR_REFRESH_60Hz, GR_COLORFORMAT_RGBA, GR_ORIGIN_UPPER_LEFT, 2, 1);

	grDisable(GR_PASSTHRU);

	grCoordinateSpace(GR_WINDOW_COORDS);
	grVertexLayout(GR_PARAM_XY, 0, GR_PARAM_ENABLE);
	grVertexLayout(GR_PARAM_Z, 8, GR_PARAM_ENABLE);
	grVertexLayout(GR_PARAM_Q, 12, GR_PARAM_ENABLE);
	grVertexLayout(GR_PARAM_ST0, 16, GR_PARAM_ENABLE);

	orgAddress = startAddress = grTexMinAddress(GR_TMU0);
	endAddress = grTexMaxAddress(GR_TMU0);
	
	glideInit = true;
}

void
GlideExit(void) {
	if(glideInit)
		grGlideShutdown();
	glideInit = false;
}

QDrawGlide::QDrawGlide(CWnd *wnd) : QDraw(wnd) {
	if(!refCount)
		GlideInit();
	refCount++;

	dc = NULL;
	lastMode = 0;
}

QDrawGlide::~QDrawGlide() {
	/*
	refCount--;
	if(!refCount)
		GlideExit();
	*/
}

void
QDrawGlide::Size(int cx, int cy) {

	if(cx & 3)
		cx += 3;
	cx &= ~3;

	width = cx;
	height = cy;
	orgX = cx / 2;
	orgY = cy / 2;

	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 16;
	bmi.bmiHeader.biCompression = BI_BITFIELDS;
	bmi.bmiHeader.biSizeImage = 0;
	bmi.bmiHeader.biClrUsed = 0;
	bmi.bmiHeader.biClrImportant = 0;
	bmi.bmiHeader.biWidth = width;
	bmi.bmiHeader.biHeight = -height;
	bmi.bmiHeader.biSizeImage = 0;

	unsigned int *a = (unsigned int *)bmi.bmiColors;
	a[0] = 0xF800;
	a[1] = 0x7E0;
	a[2] = 0x1F;

	if(dc)
		delete dc;
	dc = new CDC;
	dc->CreateCompatibleDC(NULL);
	HBITMAP bmp = CreateDIBSection(*dc, &bmi, DIB_RGB_COLORS, &data, NULL, 0);
	SelectObject(*dc, bmp);

	numBufs = 1;

	if(!glideInit)
		return;

	grClipWindow(0, 0, width, height);
	Clear();
}

void
QDrawGlide::Clear(void) {
	if(!glideInit)
		return;

	FxI32 wrange[2];
	grGet(GR_WDEPTH_MIN_MAX, 8, wrange);
	grBufferClear(0, 0, wrange[1]);
}

void
QDrawGlide::Color(int color) {
	if(!glideInit)
		return;

	grConstantColorValue(
		((pal[color].red & 0xFF) << 24) |
		((pal[color].green & 0xFF) << 16) |
		((pal[color].blue & 0xFF) << 8));
}

void
QDrawGlide::Begin(void) {

//	if(renderMode == lastMode)
//		return;

	grDisableAllEffects();

	if(renderMode == ID_VIEW_RENDER_WIREFRAME) {
		grDepthBufferMode(GR_DEPTHBUFFER_DISABLE); 
		grDepthBufferFunction(GR_CMP_NEVER);
		grDepthMask(FXFALSE);
	
		grColorCombine(GR_COMBINE_FUNCTION_LOCAL, GR_COMBINE_FACTOR_NONE,
			GR_COMBINE_LOCAL_CONSTANT, GR_COMBINE_OTHER_CONSTANT, FXFALSE);
	}
	else if(renderMode == ID_VIEW_RENDER_SOLID) {
		grDepthBufferMode(GR_DEPTHBUFFER_ZBUFFER);
		grDepthBufferFunction(GR_CMP_LESS);
		grDepthMask(FXTRUE);
	
		grColorCombine(GR_COMBINE_FUNCTION_LOCAL, GR_COMBINE_FACTOR_NONE,
			GR_COMBINE_LOCAL_CONSTANT, GR_COMBINE_OTHER_CONSTANT, FXFALSE);
	}
	else if(renderMode == ID_VIEW_RENDER_TEXTURE) {
		grDepthBufferMode(GR_DEPTHBUFFER_WBUFFER); 
		grDepthBufferFunction(GR_CMP_LESS);
		grDepthMask(FXTRUE);

		grColorCombine(GR_COMBINE_FUNCTION_SCALE_OTHER, GR_COMBINE_FACTOR_ONE,
			GR_COMBINE_LOCAL_NONE, GR_COMBINE_OTHER_TEXTURE, FXFALSE);

		grTexCombine(GR_TMU0, GR_COMBINE_FUNCTION_LOCAL, GR_COMBINE_FACTOR_NONE,
			GR_COMBINE_FUNCTION_LOCAL, GR_COMBINE_FACTOR_NONE, FXFALSE, FXFALSE);

		grTexMipMapMode(GR_TMU0, GR_MIPMAP_NEAREST, FXFALSE);

		grTexClampMode(GR_TMU0, GR_TEXTURECLAMP_WRAP, GR_TEXTURECLAMP_WRAP);
		grTexFilterMode(GR_TMU0, GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);
		grTexLodBiasValue(GR_TMU0, 0.25f);
	}

	lastMode = renderMode;
}

void
QDrawGlide::Line(int x1, int y1, int x2, int y2) {
	if(!glideInit)
		return;

	GrVertex v1, v2;
	v1.x = (float)x1;
	v1.y = (float)y1;
	v2.x = (float)x2;
	v2.y = (float)y2;

	grDrawLine(&v1, &v2);
}

void
QDrawGlide::Paint(CPaintDC *pPaint) {
	if(!glideInit)
		return;
	
	GdiFlush();
	grLfbReadRegion(GR_BUFFER_BACKBUFFER, 0, 0, width, height, width * 2, data);
	pPaint->BitBlt(0, 0, width, height, dc, 0, 0, SRCCOPY);
}

void
QDrawGlide::CopyBuf(int bufNum) {
}

void
QDrawGlide::RealizePal(void) {
	if(!glideInit)
		return;

	GuTexPalette glidePal;

	float gamma = 1.0f;
	int i, r, g, b;

	for(i = 0; i < 256; i++) {
		r = (FxU32)(pal[i].red - gamma * 100.0f + 100.0f);
		g = (FxU32)(pal[i].green - gamma * 100.0f + 100.0f);
		b = (FxU32)(pal[i].blue - gamma * 100.0f + 100.0f);
		if(r < 0) r = 0; if(r > 255) r = 255;
		if(g < 0) g = 0; if(g > 255) g = 255;
		if(b < 0) b = 0; if(b > 255) b = 255;
		glidePal.data[i] = (r << 16) | (g << 8) | b;
	}

	grTexDownloadTable(GR_TEXTABLE_PALETTE, &glidePal);
}

void
QDrawGlide::ZBufInit(void) {
}

void
QDrawGlide::ZBufClear(void) {
}

struct GlideTex {
	FxU32 start;
	GrTexInfo info;
	float wmul, hmul;
};

void
QDrawGlide::ZBufTriangle(ZPoint zpoint[3]) {
	ZPoint *zp1 = zpoint;
	ZPoint *zp2 = zpoint + 1;
	ZPoint *zp3 = zpoint + 2;

	GrVertex v[3];

	if(zp1->y == 0.0f || zp2->y == 0.0f || zp3->y == 0.0f)
		return;

	v[0].x = zp1->x + orgX;
	v[0].y = orgY - zp1->y;
	v[1].x = zp2->x + orgX;
	v[1].y = orgY - zp2->y;
	v[2].x = zp3->x + orgX;
	v[2].y = orgY - zp3->y;

	/*
	if(v[0].x < 0 || v[0].x >= width) return;
	if(v[0].y < 0 || v[0].y >= height) return;
	if(v[1].x < 0 || v[1].x >= width) return;
	if(v[1].y < 0 || v[1].y >= height) return;
	if(v[2].x < 0 || v[2].x >= width) return;
	if(v[2].y < 0 || v[2].y >= height) return;
	*/

	if(renderMode == ID_VIEW_RENDER_SOLID) {
		v[0].ooz = -65536.0f / zp1->z;
		v[1].ooz = -65536.0f / zp2->z;
		v[2].ooz = -65536.0f / zp3->z;

		if(bits == 8)
			Color(zbufColor);
		else // bits == 16
			grConstantColorValue(
				((((zbufColor >> 11) & 31) << 3) << 24) |
				((((zbufColor >> 5) & 63) << 2) << 16) |
				((((zbufColor >> 0) & 31) << 3) << 8)
				);
	}

	else if(renderMode == ID_VIEW_RENDER_TEXTURE) {
		if(!zbufTex)
			return;

		if(!zbufTex->width || !zbufTex->height)
			return;

		GlideTex *glideTex = (GlideTex *)zbufTex->pInfo;

		if(!glideTex) {
			LoadTexture(zbufTex);
			glideTex = (GlideTex *)zbufTex->pInfo;
		}

		if(!glideTex)
			return;

		v[0].oow = 1.0f / zp1->z;
		v[0].s = zp1->s * v[0].oow * glideTex->wmul;
		v[0].t = zp1->t * v[0].oow * glideTex->hmul;
	
		v[1].oow = 1.0f / zp2->z;
		v[1].s = zp2->s * v[1].oow * glideTex->wmul;
		v[1].t = zp2->t * v[1].oow * glideTex->hmul;
	
		v[2].oow = 1.0f / zp3->z;
		v[2].s = zp3->s * v[2].oow * glideTex->wmul;
		v[2].t = zp3->t * v[2].oow * glideTex->hmul;

		grTexSource(GR_TMU0, glideTex->start, GR_MIPMAPLEVELMASK_BOTH, &glideTex->info); 
	}

	grDrawTriangle(&v[0], &v[1], &v[2]);
}

void
QDrawGlide::ZBufRender(void) {
	grDepthBufferMode(GR_DEPTHBUFFER_DISABLE); 
	grDepthBufferFunction(GR_CMP_NEVER);
	grDepthMask(FXFALSE);
	grColorCombine(GR_COMBINE_FUNCTION_LOCAL, GR_COMBINE_FACTOR_NONE,
		GR_COMBINE_LOCAL_CONSTANT, GR_COMBINE_OTHER_CONSTANT, FXFALSE);
}

int
QDrawGlide::LodSize(int size) {
	if(size == 4) return GR_LOD_LOG2_4;
	else if(size == 8) return GR_LOD_LOG2_8;
	else if(size == 16) return GR_LOD_LOG2_16;
	else if(size == 32) return GR_LOD_LOG2_32;
	else if(size == 64) return GR_LOD_LOG2_64;
	else if(size == 128) return GR_LOD_LOG2_128;
	else return GR_LOD_LOG2_256;
}

void
QDrawGlide::LoadTexture(Texture *texture) {

	ASSERT(texture != NULL);

	GlideTex *glideTex = (GlideTex *)malloc(sizeof(GlideTex));
	texture->pInfo = (void *)glideTex;

	FxU32 mipSize;

	GrLOD_t lod;
	GrAspectRatio_t aspectRatio;

	int width = texture->width;
	int height = texture->height;
	int size = width * height;

	if(width > height) {
		lod = LodSize(width);
		glideTex->wmul = 256.0f;
		if(width == height * 2) { aspectRatio = GR_ASPECT_LOG2_2x1; glideTex->hmul = 128.0f; }
		else if(width == height * 4) { aspectRatio = GR_ASPECT_LOG2_4x1; glideTex->hmul = 64.0f; }
		else if(width == height * 8) { aspectRatio = GR_ASPECT_LOG2_8x1; glideTex->hmul = 16.0f; }
		else { aspectRatio = GR_ASPECT_LOG2_1x1; glideTex->hmul = 256.0f; }
	}
	else {
		lod = LodSize(height);
		glideTex->hmul = 256.0f;
		if(height == width * 2) { aspectRatio = GR_ASPECT_LOG2_1x2; glideTex->wmul = 128.0f; }
		else if(height == width * 4) { aspectRatio = GR_ASPECT_LOG2_1x4; glideTex->wmul = 64.0f; }
		else if(height == width * 8) { aspectRatio = GR_ASPECT_LOG2_1x8; glideTex->wmul = 16.0f; }
		else { aspectRatio = GR_ASPECT_LOG2_1x1; glideTex->wmul = 256.0f; }
	}

	glideTex->info.smallLodLog2 = lod - (texture->mips - 1);
	glideTex->info.largeLodLog2 = lod;
	glideTex->info.aspectRatioLog2 = aspectRatio;

	mipSize = grTexTextureMemRequired(GR_MIPMAPLEVELMASK_BOTH, &glideTex->info); 

	if(texture->bits == 8)
		glideTex->info.format = GR_TEXFMT_P_8;
	else {
		size *= 2;
		mipSize *= 2;
		glideTex->info.format = GR_TEXFMT_RGB_565;
	}

	glideTex->info.data = (void *)malloc(mipSize);
	memcpy(glideTex->info.data, texture->mip[0], size);
	memcpy((unsigned char *)glideTex->info.data + size, texture->mip[1], size / 4);
	memcpy((unsigned char *)glideTex->info.data + size + size / 4, texture->mip[2], size / 16);
	memcpy((unsigned char *)glideTex->info.data + size + size / 4 + size / 16, texture->mip[3], size / 64);

	glideTex->wmul /= (float)texture->width;
	glideTex->hmul /= (float)texture->height;

	if(startAddress + mipSize > endAddress) {
		// FIXME: this needs to be dealt with properly!

		free(glideTex);
		texture->pInfo = NULL;
		return;
	}

	// for 4mb cards, we can't load mip maps across the 2mb boundary
	if(startAddress < 0x200000 && startAddress + mipSize > 0x200000)
		startAddress = 0x200000;

	glideTex->start = startAddress;
	startAddress += mipSize;

	grTexDownloadMipMap(GR_TMU0, glideTex->start, GR_MIPMAPLEVELMASK_BOTH, &glideTex->info);

	free(glideTex->info.data);
}

void
QDrawGlide::Suspend(void) {
	GlideExit();
}

void
QDrawGlide::Resume(void) {
	GlideInit();
	RealizePal();
}

#endif
