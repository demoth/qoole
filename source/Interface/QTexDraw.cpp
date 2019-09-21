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

// QTexDraw.cpp : implementation file
//

#include "stdafx.h"
#include "qoole.h"
#include "QTexDraw.h"
#include "QMainFrm.h"
#include "QDraw.h"
#include "Game.h"
#include "Selector.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// QTexDraw

QTexDraw::QTexDraw() {
	dc = NULL;
	bmp = NULL;
}

QTexDraw::~QTexDraw() {
	delete dc;
	delete bmp;
}


BEGIN_MESSAGE_MAP(QTexDraw, CWnd)
	//{{AFX_MSG_MAP(QTexDraw)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// QTexDraw message handlers


void QTexDraw::OnInit(void) {

	dc = new CDC;
	bmp = new CBitmap;

	CDC *winDC = GetParent()->GetDC();
	dc->CreateCompatibleDC(winDC);
	bmp->CreateCompatibleBitmap(winDC, 128, 128);
	dc->SelectObject(bmp);
	ReleaseDC(winDC);

	SetTexture(NULL);
}

void QTexDraw::OnPaint() {
	CPaintDC drawDC(this);
	drawDC.BitBlt(0, 0, 128, 128, dc, 0, 0, SRCCOPY);
}

void QTexDraw::SetTexture(Texture *texture) {
	if(!texture) {
		dc->BitBlt(0, 0, 128, 128, NULL, 0, 0, BLACKNESS);
		InvalidateRect(NULL, false);
		UpdateWindow();
		return;
	}

	if(!texture->Cache())
		return;

	char *palName = texture->game->GetPalName();
	if(LFile::Exist(palName)) {
		texBits = 8;
		pal.Load(palName, QDraw::textureGamma);
	}
	else
		texBits = 16;

	BITMAPINFO *bmi = (BITMAPINFO *)malloc(
		sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256);

	bmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi->bmiHeader.biWidth = texture->width;
	bmi->bmiHeader.biHeight = -texture->height;
	bmi->bmiHeader.biPlanes = 1;
	bmi->bmiHeader.biBitCount = texBits;
	bmi->bmiHeader.biSizeImage = 0;
	bmi->bmiHeader.biXPelsPerMeter = 0;
	bmi->bmiHeader.biYPelsPerMeter = 0;

	if(texBits == 8) {
		bmi->bmiHeader.biCompression = BI_RGB;
		bmi->bmiHeader.biClrUsed = 256;
		bmi->bmiHeader.biClrImportant = 256;
		for(int i = 0; i < 256; i++) {
			bmi->bmiColors[i].rgbRed = pal.pal[i].red;
			bmi->bmiColors[i].rgbGreen = pal.pal[i].green;
			bmi->bmiColors[i].rgbBlue = pal.pal[i].blue;
		}
	}
	else if(texBits == 16) {
		bmi->bmiHeader.biCompression = BI_BITFIELDS;
		bmi->bmiHeader.biClrUsed = 0;
		bmi->bmiHeader.biClrImportant = 0;
		DWORD *a = (DWORD *)bmi->bmiColors;
		*a++ = 0xF800;
		*a++ = 0x07E0;
		*a++ = 0x001F;
	}

	CDC tmpDC;
	CBitmap tmpBMP;

	CDC *winDC = GetDC();
	tmpDC.CreateCompatibleDC(winDC);
	tmpBMP.CreateCompatibleBitmap(winDC, texture->width, texture->height);
	tmpDC.SelectObject(tmpBMP);
	ReleaseDC(winDC);

	SetDIBitsToDevice(tmpDC.m_hDC, 0, 0, texture->width, texture->height,
		0, 0, 0, texture->height, texture->surface, bmi, DIB_RGB_COLORS);

	for(int y = 0; y < 128; y += texture->height)
		for(int x = 0; x < 128; x += texture->width)
			dc->BitBlt(x, y, texture->width, texture->height, &tmpDC, 0, 0, SRCCOPY);

	free(bmi);

	InvalidateRect(NULL, false);
	UpdateWindow();
}

void QTexDraw::SetFaceTex(FaceTex *pFaceTex) {
	Selector *pSlctr = &(pQMainFrame->GetSelector());

	// no particular side selected?
	if(!pFaceTex)
		pFaceTex = pSlctr->GetSelectedFaceTex();

	if(pFaceTex)
		SetTexture(pFaceTex->GetTexture());
	else
		SetTexture(NULL);
}
