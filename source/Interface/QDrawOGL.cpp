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

#include <GL/gl.h>
#include "glext.h"

#ifdef IS_QDRAWDLL
QDrawOpenGL *
NewQDraw(CWnd *wnd) {
	return new QDrawOpenGL(wnd);
}

void
LFatal(const char *fmt, ...) {
	va_list arglist;
	char buf[256] = "";
	
	if(fmt) {
		va_start(arglist, fmt);
		vsprintf(buf, fmt, arglist);
		va_end(arglist);
	}

	MessageBox(NULL, buf, "Qoole 99 Fatal Error", MB_OK);

	exit(1);
}
#endif

// ========== QDrawOpenGL ==========

QDrawOpenGL::QDrawOpenGL(CWnd *wnd) : QDraw(wnd) {

	numBufs = 1;

	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),  /* size */
		1,                              /* version */
		PFD_SUPPORT_OPENGL |
		PFD_DRAW_TO_WINDOW |
		PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,                  /* color type */
		16,                             /* prefered color depth */
		0, 0, 0, 0, 0, 0,               /* color bits (ignored) */
		0,                              /* no alpha buffer */
		0,                              /* alpha bits (ignored) */
		0,                              /* no accumulation buffer */
		0, 0, 0, 0,                     /* accum bits (ignored) */
		16,                             /* depth buffer */
		0,                              /* no stencil buffer */
		0,                              /* no auxiliary buffers */
		PFD_MAIN_PLANE,                 /* main layer */
		0,                              /* reserved */
		0, 0, 0,                        /* no layer, visible, damage masks */
	};

	hDC = GetDC(wnd->m_hWnd);

	int pixelFormat = ChoosePixelFormat(hDC, &pfd);
	if(!pixelFormat)
		LFatal("ChoosePixelFormat fallo.");

    if(!SetPixelFormat(hDC, pixelFormat, &pfd))
		LFatal("SetPixelFormat fallo.");

	LOGPALETTE *pPal;
	int paletteSize;

	DescribePixelFormat(hDC, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

	if(pfd.dwFlags & PFD_NEED_PALETTE) {
		paletteSize = 1 << pfd.cColorBits;

		pPal = (LOGPALETTE*)
			malloc(sizeof(LOGPALETTE) + paletteSize * sizeof(PALETTEENTRY));
		pPal->palVersion = 0x300;
		pPal->palNumEntries = paletteSize;

		/* build a simple RGB color palette */
		int redMask = (1 << pfd.cRedBits) - 1;
		int greenMask = (1 << pfd.cGreenBits) - 1;
		int blueMask = (1 << pfd.cBlueBits) - 1;
		int i;

		for (i=0; i<paletteSize; ++i) {
			pPal->palPalEntry[i].peRed =
				(((i >> pfd.cRedShift) & redMask) * 255) / redMask;
			pPal->palPalEntry[i].peGreen =
				(((i >> pfd.cGreenShift) & greenMask) * 255) / greenMask;
			pPal->palPalEntry[i].peBlue =
				(((i >> pfd.cBlueShift) & blueMask) * 255) / blueMask;
			pPal->palPalEntry[i].peFlags = 0;
		}

		hPalette = CreatePalette(pPal);
		free(pPal);

		if(hPalette) {
			SelectPalette(hDC, hPalette, FALSE);
			RealizePalette(hDC);
		}
	}
	else
		hPalette = NULL;

	hGLRC = wglCreateContext(hDC);
	if(!hGLRC)
		LFatal("wglCreateContext fallo");
	if(!wglMakeCurrent(hDC, hGLRC))
		LFatal("wglMakeCurrent fallo");

	glLineWidth(1.0f);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

QDrawOpenGL::~QDrawOpenGL() {
	if(hGLRC) {
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(hGLRC);
	}
	if(hPalette)
		DeleteObject(hPalette);
}

void
QDrawOpenGL::Size(int cx, int cy) {
	QDraw::Size(cx, cy);

	wglMakeCurrent(hDC, hGLRC);

	glViewport(0, 0, cx, cy);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
	glOrtho(0.0f, (GLfloat)cx, (GLfloat)cy, 0.0f, -1.0f, 1.0f);

	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	glDrawBuffer(GL_BACK);
}

void
QDrawOpenGL::Clear(void) {
	lines = 0;
	tris = 0;
}

void
QDrawOpenGL::Color(int newcolor) {
	color = newcolor;
}

void
QDrawOpenGL::Begin(void) {
}

void
QDrawOpenGL::Line(int x1, int y1, int x2, int y2) {
	if(lines >= 10000)
		return;

	line[lines].x1 = (float)x1;
	line[lines].y1 = (float)y1;
	line[lines].x2 = (float)x2;
	line[lines].y2 = (float)y2;
	line[lines].r = (float)pal[color].red / 256.0f;
	line[lines].g = (float)pal[color].green / 256.0f;
	line[lines].b = (float)pal[color].blue / 256.0f;
	lines++;
}

void
QDrawOpenGL::Paint(CPaintDC *pPaint) {
	wglMakeCurrent(hDC, hGLRC);

	float v1[3], v2[3];
	float length;
	int shared = 0;

	if(renderMode == ID_VIEW_RENDER_WIREFRAME) {
		glClear(GL_COLOR_BUFFER_BIT);

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_TEXTURE_2D);

		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);

		glBegin(GL_LINES);

		for(int i = 0; i < lines; i++) {
			glColor3f(line[i].r, line[i].g, line[i].b);
			glVertex2f(line[i].x1, line[i].y1);
			glVertex2f(line[i].x2, line[i].y2);
		}

		glEnd();
	}

	else if(renderMode == ID_VIEW_RENDER_SOLID)
	{

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_DEPTH_TEST);
		glDisable(GL_TEXTURE_2D);

		glBegin(GL_TRIANGLES);

		for(int i = 0; i < tris; i++)
		{
			glColor3f(tri[i].r, tri[i].g, tri[i].b);
			for(int j = 0; j < 3; j++)
			{
				glVertex3f(tri[i].x[j], tri[i].y[j], tri[i].z[j]);
			}
		}

		glEnd();
	}

	else if(renderMode == ID_VIEW_RENDER_TEXTURE) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		int i, j;

		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_TEXTURE_2D);

		glCullFace(GL_FRONT);
		glEnable(GL_CULL_FACE);

		for(i = 0; i < tris; i++) {
			Texture *texture = tri[i].texture;
			if(!texture) {
				glDisable(GL_TEXTURE_2D);
				glBegin(GL_TRIANGLES);
				glColor3f(tri[i].r, tri[i].g, tri[i].b);
				for(int j = 0; j < 3; j++)
					glVertex3f(tri[i].x[j], tri[i].y[j], tri[i].z[j]);
				glEnd();
				glEnable(GL_TEXTURE_2D);
				continue;
			}

			if(!texture->tInfo || !glIsTexture(texture->tInfo)) {
				glGenTextures(1, &texture->tInfo);
				glBindTexture(GL_TEXTURE_2D, texture->tInfo);

				if(texture->bits == 8)
					glTexImage2D(GL_TEXTURE_2D, 0, 3, texture->width, texture->height,
						0, GL_COLOR_INDEX, GL_UNSIGNED_BYTE, texture->mip[0]);
				else {
					struct RGB {
						unsigned char r, g, b;
					};

					RGB *tex = new RGB[texture->width * texture->height];

					unsigned short *src = (unsigned short *)texture->mip[0];
					unsigned char *dst = (unsigned char *)tex;
					
					for(int j = 0; j < texture->width * texture->height; j++) {
						*dst++ = (((*src >> 11) & 31) << 3);
						*dst++ = (((*src >> 5) & 63) << 2);
						*dst++ = (((*src >> 0) & 31) << 3);
						src++;
					}

					glTexImage2D(GL_TEXTURE_2D, 0, 3, texture->width, texture->height,
						0, GL_RGB, GL_UNSIGNED_BYTE, tex);

					delete tex;

				}

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			}

			glBindTexture(GL_TEXTURE_2D, texture->tInfo);

			glBegin(GL_TRIANGLES);

			v1[0] = tri[i].x[0] - tri[i].y[0];
			v1[1] = tri[i].x[1] - tri[i].y[1];
			v1[2] = tri[i].x[2] - tri[i].y[2];
			
			v2[0] = tri[i].y[0] - tri[i].z[0];
			v2[1] = tri[i].y[1] - tri[i].z[1];
			v2[2] = tri[i].y[2] - tri[i].z[2];
			
			normals[i].nf[0] = v1[1]*v2[2] - v1[2]*v2[1];
			normals[i].nf[1] = v1[2]*v2[0] - v1[0]*v2[2];
			normals[i].nf[2] = v1[0]*v2[1] - v1[1]*v2[0];

			length= (float)sqrt((normals[i].nf[0] * normals[i].nf[0]) +
								(normals[i].nf[1] * normals[i].nf[1]) +
								(normals[i].nf[2] * normals[i].nf[2]));
			
			if(length == 0.0f)
				length = 1.0f;

			normals[i].nf[0] /= length;
			normals[i].nf[1] /= length;
			normals[i].nf[2] /= length;

			glNormal3fv(normals[i].nf);
			for(j = 0; j < 3; j++) {
				glTexCoord4f(tri[i].s[j], tri[i].t[j], 0.0f, tri[i].z[j]);
				glVertex3f(tri[i].x[j], tri[i].y[j], tri[i].z[j]);
			}

			glEnd();
		}
	}

	if(lines) {
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_TEXTURE_2D);

		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);

		glBegin(GL_LINES);

		for(int i = 0; i < lines; i++) {
			glColor3f(line[i].r, line[i].g, line[i].b);
			glVertex2f(line[i].x1, line[i].y1);
			glVertex2f(line[i].x2, line[i].y2);
		}

		glEnd();
	}

	SwapBuffers(hDC);
}

void
QDrawOpenGL::CopyBuf(int bufNum) {
}

void
QDrawOpenGL::RealizePal(void) {
	float values[256];
	int i;

	wglMakeCurrent(hDC, hGLRC);

	for(i = 0; i < 256; i++)
		values[i] = (float)pal[i].red / 256.0f;
	glPixelMapfv(GL_PIXEL_MAP_I_TO_R, 256, values);

	for(i = 0; i < 256; i++)
		values[i] = (float)pal[i].green / 256.0f;
	glPixelMapfv(GL_PIXEL_MAP_I_TO_G, 256, values);
	
	for(i = 0; i < 256; i++)
		values[i] = (float)pal[i].blue / 256.0f;
	glPixelMapfv(GL_PIXEL_MAP_I_TO_B, 256, values);
}

void
QDrawOpenGL::ZBufInit(void) {
}

void
QDrawOpenGL::ZBufClear(void) {
}

void
QDrawOpenGL::ZBufTriangle(ZPoint zpoint[3]) {
	if(tris >= 3000)
		return;

	for(int i = 0; i < 3; i++) {
		tri[tris].x[i] = zpoint[i].x + orgX;
		tri[tris].y[i] = orgY - zpoint[i].y;
		tri[tris].z[i] = 1.0f / zpoint[i].z;
		if(zbufTex) {
			tri[tris].s[i] = zpoint[i].s / zbufTex->width * tri[tris].z[i];
			tri[tris].t[i] = zpoint[i].t / zbufTex->height * tri[tris].z[i];
		}
	}

	if(bits == 8) {
		tri[tris].r = (float)pal[zbufColor].red / 256.0f;
		tri[tris].g = (float)pal[zbufColor].green / 256.0f;
		tri[tris].b = (float)pal[zbufColor].blue / 256.0f;
	}
	else { // bits == 16
		tri[tris].r = (float)((((zbufColor >> 11) & 31) << 3) / 256.0f);
		tri[tris].g = (float)((((zbufColor >> 5) & 63) << 2) / 256.0f);
		tri[tris].b = (float)((((zbufColor >> 0) & 31) << 3) / 256.0f);
	}

	tri[tris].texture = zbufTex;

	tris++;
}

void
QDrawOpenGL::ZBufRender(void) {
}

#endif
