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

// QTexView.cpp : implementation file
//

#include "stdafx.h"
#include "qoole.h"
#include "QMainFrm.h"
#include "QTexView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TABLE_X	4
#define TABLE_Y	4

/////////////////////////////////////////////////////////////////////////////
// QTexView

QTexView::QTexView() {
	buildLater = true;
}

QTexView::~QTexView() {
}

IMPLEMENT_DYNCREATE(QTexView, CWnd)

BEGIN_MESSAGE_MAP(QTexView, CWnd)
	//{{AFX_MSG_MAP(QTexView)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(2, OnSelChange)
	ON_CBN_CLOSEUP(2, OnCloseUp)
	ON_CBN_SELCHANGE(3, OnSelChange2)
	ON_CBN_CLOSEUP(3, OnCloseUp2)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// QTexView message handlers

int QTexView::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	if(CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	texCur.Create(NULL, NULL, WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, 1);

	texListBox.Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL |
		CBS_DROPDOWNLIST | CBS_SORT, CRect(0, 0, 0, 0), this, 2);

	texSizeBox.Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL |
		CBS_DROPDOWNLIST, CRect(0, 0, 0, 0), this, 3);

	texWnd.Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL,
		CRect(0, 0, 0, 0), this, 4);

	return 0;
}

void QTexView::OnSize(UINT nType, int cx, int cy) {
	CWnd::OnSize(nType, cx, cy);

	if(!cx || !cy)
		return;

	CDC *pDC = GetDC();
	int comboHeight = pDC->GetTextExtent("x").cy + 8;

	int curHeight = 73;
	int sizeWidth = 56;

	texCur.MoveWindow(0, 0, cx, curHeight);
	texListBox.MoveWindow(0, curHeight, cx + 2 - sizeWidth, comboHeight * 5);
	texSizeBox.MoveWindow(cx + 2 - sizeWidth, curHeight, sizeWidth, comboHeight * 5);
	texWnd.MoveWindow(0, curHeight + comboHeight, cx, cy - curHeight - comboHeight);

	ReleaseDC(pDC);
}

void QTexView::OnSelChange(void) {
	int num = texListBox.GetCurSel();
	if(num != -1)
		texWnd.SetTexList(new TexList(texFiles[num],
			pQMainFrame->GetDeskTopDocument()->GetGame()));
}

void QTexView::OnCloseUp(void) {
	// Set the focus away from the combo box.
	texWnd.SetFocus();
}

void QTexView::OnSelChange2(void) {
	int num = texSizeBox.GetCurSel();
	if(num != -1)
		texWnd.SetCellSize(1 << (num + 4));
}

void QTexView::OnCloseUp2(void) {
	// Set the focus away from the combo box.
	texWnd.SetFocus();
}

void QTexView::OnShowWindow(BOOL bShow, UINT nStatus) {
	CWnd::OnShowWindow(bShow, nStatus);

	if(buildLater)
		BuildComboBox();

	buildLater = false;
}

void QTexView::OnNewContents(void) {
	if(IsWindowVisible())
		BuildComboBox();
	else
		buildLater = true;

	texCur.RegisterDocument(pQMainFrame->GetDeskTopDocument());
	texCur.SetTexture(NULL);
}

void QTexView::BuildComboBox(void) {
	texListBox.ResetContent();

	if(!pQMainFrame)
		return;
	QooleDoc *pDoc = pQMainFrame->GetDeskTopDocument();
	if(!pDoc)
		return;
	Game *game = pDoc->GetGame();
	if(!game)
		return;

	char searchDir[MAX_PATH];
	sprintf(searchDir, "%s\\texlists\\%s",
		LFile::GetInitDir(), game->GetName());

	int i = 0;
	char *texFile;
	LFindFiles findTexFiles(searchDir, "*.tex");
	while(texFile = findTexFiles.Next()) {
		char *c = strrchr(texFile, '.');
		*c = '\0';
		texListBox.AddString(texFile);

		if(i++ == 64) {
			LError("Limit of 64 .tex files reached, skipping extras.");
			break;
		}
	}

	for(int j = 0; j < i; j++) {
		char texFile[64];
		texListBox.GetLBText(j, texFile);
		sprintf(texFiles[j], "%s\\%s.tex", searchDir, texFile);
	}

	texSizeBox.ResetContent();
	texSizeBox.AddString("16");
	texSizeBox.AddString("32");
	texSizeBox.AddString("64");
	texSizeBox.AddString("128");
	texSizeBox.AddString("1:1");

	if(texWnd.cellTexSize == 16)
		texSizeBox.SetCurSel(0);
	else if(texWnd.cellTexSize == 32)
		texSizeBox.SetCurSel(1);
	else if(texWnd.cellTexSize == 64)
		texSizeBox.SetCurSel(2);
	else if(texWnd.cellTexSize == 128)
		texSizeBox.SetCurSel(3);
	else if(texWnd.cellTexSize == 256)
		texSizeBox.SetCurSel(4);

	texListBox.SetCurSel(0);
	OnSelChange();
}

/////////////////////////////////////////////////////////////////////////////
// QTexWnd

QTexWnd::QTexWnd() {
	numCells = 0;
	texCells = NULL;
	selCell = NULL;
	isFocused = true;
	cellTexSize = 64;
	oldCellTexSize = 0;
	texList = NULL;

	font.CreatePointFont(72, "verdana", NULL);

	for(int i = 0; i < MAX_DC; i++) {
		dc[i] = NULL;
		bmp[i] = NULL;
	}
}

QTexWnd::~QTexWnd() {
	int i;
	for(i = 0; i < numCells; i++)
		delete texCells[i];
	if(texCells)
		free(texCells);
	numCells = 0;

	if(texList)
		delete texList;

	for(i = 0; i < MAX_DC; i++) {
		delete dc[i];
		delete bmp[i];
		dc[i] = NULL;
		bmp[i] = NULL;
	}
}


BEGIN_MESSAGE_MAP(QTexWnd, CWnd)
	//{{AFX_MSG_MAP(QTexWnd)
	ON_WM_VSCROLL()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEWHEEL()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_RBUTTONUP()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_TEXWND_APPLY, OnTextureApply)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// QTexWnd message handlers

#define PADX	6
#define PADY	16

int fontHeight = 0;

void QTexWnd::OnSize(UINT nType, int cx, int cy) {
	CWnd::OnSize(nType, cx, cy);
	
	winWidth = cx;
	winHeight = cy;

	CDC *pDC = GetDC();
	pDC->SelectObject(font);
	fontHeight = pDC->GetTextExtent("x").cy;
	ReleaseDC(pDC);

	CalcTexCells(false);

	InvalidateRect(NULL, false);
	UpdateWindow();
}

void QTexWnd::OnPaint() {
	CPaintDC dc(this);

	CBrush brush;
	brush.CreateStockObject(BLACK_BRUSH);

	if(!numCells) {
		dc.FillSolidRect(CRect(0, 0, winWidth, winHeight), RGB(0, 0, 0));
		return;
	}

	dc.SelectObject(font);
	dc.SetBkColor(RGB(0, 0, 0));
	dc.SetTextColor(RGB(255, 255, 255));

	CRgn diffRgn, drawRgn;
	drawRgn.CreateRectRgn(0, 0, winWidth, winHeight);

	for(int i = 0; i < numCells; i++) {
		QTexCell *texCell = texCells[i];
		if(!texCell->show)
			continue;

		texCell->Draw(&dc, selCell == texCells[i], isFocused);

		diffRgn.DeleteObject();
		diffRgn.CreateRectRgn(texCell->rect.left, texCell->rect.top,
			texCell->rect.right, texCell->rect.bottom);
		drawRgn.CombineRgn(&drawRgn, &diffRgn, RGN_DIFF);
	}

	dc.FillRgn(&drawRgn, &brush);
}

void QTexWnd::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) {
	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);

	int lastPos = GetScrollPos(SB_VERT);

	switch(nSBCode) {
	case SB_BOTTOM:
		SetScrollPos(SB_VERT, 0);
		break;
	case SB_ENDSCROLL:
		break;
	case SB_LINEDOWN:
		SetScrollPos(SB_VERT, lastPos + 1);
		break;
	case SB_LINEUP:
		SetScrollPos(SB_VERT, lastPos - 1);
		break;
	case SB_PAGEDOWN:
		SetScrollPos(SB_VERT, lastPos + winRows);
		break;
	case SB_PAGEUP:
		SetScrollPos(SB_VERT, lastPos - winRows);
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		SetScrollPos(SB_VERT, nPos);
		break;
	case SB_TOP:
		break;
	}

	UpdateTexCells();

	InvalidateRect(NULL, false);
	UpdateWindow();
}

// NT 4.0 processing.
BOOL QTexWnd::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) {
	int n = ABS(zDelta / 120) * 3;
	for(; n > 0; n--) {
		SendMessage(WM_VSCROLL,
					(WPARAM) (zDelta > 0 ? SB_LINEUP : SB_LINEDOWN));
	}
	return TRUE;
}

void QTexWnd::OnSetFocus(CWnd* pOldWnd) {
	CWnd::OnSetFocus(pOldWnd);
	isFocused = true;
	InvalidateRect(NULL, false);
	UpdateWindow();
}

void QTexWnd::OnKillFocus(CWnd* pNewWnd) {
	CWnd::OnKillFocus(pNewWnd);
	isFocused = false;
	InvalidateRect(NULL, false);
	UpdateWindow();
}

void QTexWnd::SetTexList(TexList *newList) {
	if(newList) {
		if(texList)
			delete texList;
		texList = newList;
		if(!texList)
			return;
	}

	int i;
	int cells = texList->GetNumTexs();
	int saveCells = numCells;

	numCells = 0;

	SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_WAIT)));

	CProgressWnd progressWnd(AfxGetMainWnd());
	progressWnd.SetText("Loading Textures");

	for(i = 0; i < cells; i++) {
		texList->GetTexNum(i)->Cache();

		progressWnd.SetPos(i * 100 / cells);
		progressWnd.PeekAndPump();

		if(progressWnd.Cancelled()) {
			SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
			delete texList;
			texList = NULL;
			return;
		}
	}

	SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));

	SetScrollPos(SB_VERT, 0);

	selCell = NULL;

	numCells = saveCells;
	CalcTexCells(true);

	InvalidateRect(NULL, false);
	UpdateWindow();
}

void QTexWnd::SetCellSize(int size) {
	cellTexSize = size;
	OnSize(0, winWidth, winHeight);
	SetTexList(NULL);
}


#define TVOX	4
#define TVOY	6

void QTexWnd::CalcTexCells(bool redraw) {
	QTexCell *texCell;
	int i, x, y;

	if(!texList)
		return;

	if(redraw) {
		for(i = 0; i < numCells; i++)
			delete texCells[i];
		if(texCells)
			free(texCells);

		numCells = texList->GetNumTexs();
		texCells = (QTexCell **)malloc(sizeof(QTexCell) * numCells);

		for(i = 0; i < numCells; i++) {
			texCell = new QTexCell(texList->GetTexNum(i));
			texCells[i] = texCell;
		}

		for(i = 0; i < MAX_DC; i++) {
			if(dc[i]) {
				delete dc[i];
				delete bmp[i];
				dc[i] = NULL;
				bmp[i] = NULL;
			}
		}

		CDC *winDC = GetDC();

		if(cellTexSize < 256) {
			cellWidth = cellTexSize + PADX;
			cellHeight = cellTexSize + PADY + fontHeight;

			for(i = 0; i < numCells / (TABLE_X * TABLE_Y) + 1; i++) {
				dc[i] = new CDC;
				bmp[i] = new CBitmap;

				dc[i]->CreateCompatibleDC(winDC);
				bmp[i]->CreateCompatibleBitmap(winDC, TABLE_X * cellWidth, TABLE_Y * cellHeight);
				dc[i]->SelectObject(bmp[i]);
				dc[i]->BitBlt(0, 0, TABLE_X * cellWidth, TABLE_Y * cellHeight, NULL, 0, 0, BLACKNESS);
			}

		}
		else {
			for(i = 0; i < numCells / (TABLE_X * TABLE_Y) + 1; i++) {
				dcCellX[i] = 0;
				dcCellY[i] = 0;

				for(int j = 0; j < TABLE_X * TABLE_Y; j++) {
					int num = i * TABLE_X * TABLE_Y + j;
					if(num >= numCells)
						break;
					Texture *texture = texList->GetTexNum(num);
					dcCellX[i] = Max(dcCellX[i], texture->realWidth);
					dcCellY[i] = Max(dcCellY[i], texture->realHeight);
				}

				dc[i] = new CDC;
				bmp[i] = new CBitmap;

				dc[i]->CreateCompatibleDC(winDC);
				bmp[i]->CreateCompatibleBitmap(winDC, TABLE_X * dcCellX[i], TABLE_Y * dcCellY[i]);
				dc[i]->SelectObject(bmp[i]);
				dc[i]->BitBlt(0, 0, TABLE_X * dcCellX[i], TABLE_Y * dcCellY[i], NULL, 0, 0, BLACKNESS);
			}
		}

		ReleaseDC(winDC);
	}

	oldCellTexSize = cellTexSize;

	if(cellTexSize < 256) {
		winColumns = Max(1, (winWidth - 4) / cellWidth);
		totalRows = numCells / winColumns - (numCells % winColumns ? 0 : 1);
		winRows = (winHeight - 4) / cellHeight;

		for(x = 0; x < winColumns; x++) {
			for(y = 0; y < numCells / winColumns + 1; y++) {
				int cell = y * winColumns + x;
				if(cell >= numCells)
					break;
				texCell = texCells[cell];

				texCell->width = cellWidth;
				texCell->height = cellHeight;
				texCell->scale = cellTexSize;
				texCell->fixed = false;

				texCell->posX = x * cellWidth + TVOX;
				texCell->posY = y * cellHeight + TVOY;
				
				texCell->num = cell % (TABLE_X * TABLE_Y);

				texCell->tableX = (texCell->num % TABLE_X) * cellWidth;
				texCell->tableY = (texCell->num / TABLE_X) * cellHeight;

				posTable[y] = y * cellHeight;

				if(redraw)
					texCell->PreDraw(dc[cell / (TABLE_X * TABLE_Y)]);
			}
		}
	}
	else {	// 1:1
		int posX = TVOX, posY = TVOY, y = 0;
		int maxY = 0;
		for(i = 0; i < numCells; i++) {
			texCell = texCells[i];

			texCell->width = texCell->texture->realWidth + PADX;
			texCell->height = texCell->texture->realHeight + PADY + fontHeight;
			texCell->scale = Max(texCell->texture->realWidth,
				texCell->texture->realHeight);
			texCell->fixed = true;

			if(posX + texCell->width > winWidth && posX > 0) {
				posTable[y++] = posY - TVOY;
				posX = TVOX;
				posY += maxY;
				maxY = 0;
			}

			maxY = Max(texCell->height, maxY);

			texCell->posX = posX;
			texCell->posY = posY;

			texCell->num = i % (TABLE_X * TABLE_Y);

			texCell->tableX = (texCell->num % TABLE_X) * dcCellX[i / (TABLE_X * TABLE_Y)];
			texCell->tableY = (texCell->num / TABLE_X) * dcCellY[i / (TABLE_X * TABLE_Y)];

			posX += texCell->width;

			if(redraw)
				texCell->PreDraw(dc[i / (TABLE_X * TABLE_Y)]);
		}

		winRows = winHeight / 256;
		totalRows = y - winRows - 1;
	}

	UpdateTexCells();
}

void QTexWnd::UpdateTexCells(void) {
	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask = SIF_PAGE;
	si.nPage = winRows;

	SetScrollInfo(SB_VERT, &si);
	SetScrollRange(SB_VERT, 0, totalRows);

	QTexCell *texCell;
	int ystart = posTable[GetScrollPos(SB_VERT)];
	int ypos;

	for(int i = 0; i < numCells; i++) {
		texCell = texCells[i];
		ypos = texCell->posY - ystart;

		if(ypos >= 0 && ypos + texCell->height < winHeight) {
			texCell->rect.left = texCell->posX;
			texCell->rect.top = ypos;
			texCell->rect.right = texCell->posX + texCell->width;
			texCell->rect.bottom = ypos + texCell->height;
			texCell->show = true;
		}
		else
			texCell->show = false;
	}
}

void QTexWnd::SelectTexture(CPoint point) {
	for(int i = 0; i < numCells; i++) {
		if(texCells[i]->show && texCells[i]->rect.PtInRect(point)) {
			selCell = texCells[i];
			InvalidateRect(NULL, false);
			UpdateWindow();
			return;
		}
	}
	selCell = NULL;
}

void QTexWnd::OnLButtonDown(UINT nFlags, CPoint point) {
	CWnd::OnLButtonDown(nFlags, point);

	SelectTexture(point);

	// QView from MDIMainFrame gains focus by default.
	// Need to set focus here.
	SetFocus();
}

void QTexWnd::OnLButtonDblClk(UINT nFlags, CPoint point) {
	CWnd::OnLButtonDblClk(nFlags, point);

	SelectTexture(point);
	OnTextureApply();
}

void QTexWnd::OnRButtonUp(UINT nFlags, CPoint point) {
	SelectTexture(point);

	if(!selCell)
		return;

	CRect rect;
	GetClientRect(&rect);
	ClientToScreen(&rect);

	char size[64];
	sprintf(size, "(%dx%d)", selCell->texture->width, selCell->texture->height);

	CMenu menu;
	menu.CreatePopupMenu();
	menu.AppendMenu(MF_STRING, ID_TEXWND_APPLY, "Apply");
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING, 0, selCell->texture->GetShortName());
	menu.AppendMenu(MF_STRING, 0, size);
	menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON,
		rect.left + point.x, rect.top + point.y, this);
}

Texture *QTexWnd::GetSelTexture(void) {
	if(selCell)
		return selCell->texture;
	else
		return NULL;
}

void QTexWnd::OnTextureApply(void) {
	if(!selCell)
		return;

	Selector *pSlctr = &(pQMainFrame->GetSelector());

	char *name = selCell->texture->GetName();
	int faceIndex = pSlctr->GetSelectFaceIndex();

	if(faceIndex == -1) {
		OpTextureApply *op = new OpTextureApply(name);
		pQMainFrame->CommitOperation(*op);
	}
	else {
		OpTextureApplyFace *op = new OpTextureApplyFace(faceIndex, name);
		pQMainFrame->CommitOperation(*op);
	}
}


/////////////////////////////////////////////////////////////////////////////
// QTexCell

int texCells = 0;

QTexCell::QTexCell(Texture *_texture) {
	texture = _texture;
	show = false;
	width = 0;
	height = 0;
	scale = 0;
	num = 0;

	fontHeight = 12;

	dc = NULL;
}

QTexCell::~QTexCell(void) {
}

void QTexCell::PreDraw(CDC *pDC) {
	if(!texture->Cache())
		return;

	BITMAPINFO *bmi = (BITMAPINFO *)malloc(
		sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256);

	bmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi->bmiHeader.biWidth = texture->width;
	bmi->bmiHeader.biHeight = -texture->height;
	bmi->bmiHeader.biPlanes = 1;
	bmi->bmiHeader.biBitCount = texture->bits;
	bmi->bmiHeader.biSizeImage = 0;
	bmi->bmiHeader.biXPelsPerMeter = 0;
	bmi->bmiHeader.biYPelsPerMeter = 0;

	if(texture->bits == 8) {
		bmi->bmiHeader.biCompression = BI_RGB;
		bmi->bmiHeader.biClrUsed = 256;
		bmi->bmiHeader.biClrImportant = 256;
		LPalette pal = texture->game->GetPal();
		for(int i = 0; i < 256; i++) {
			bmi->bmiColors[i].rgbRed = pal.pal[i].red;
			bmi->bmiColors[i].rgbGreen = pal.pal[i].green;
			bmi->bmiColors[i].rgbBlue = pal.pal[i].blue;
		}
	}
	else if(texture->bits == 16) {
		bmi->bmiHeader.biCompression = BI_BITFIELDS;
		bmi->bmiHeader.biClrUsed = 0;
		bmi->bmiHeader.biClrImportant = 0;
		DWORD *a = (DWORD *)bmi->bmiColors;
		*a++ = 0xF800;
		*a++ = 0x07E0;
		*a++ = 0x001F;
	}

	dc = pDC;

	CDC tmpDC;
	CBitmap tmpBMP;

	tmpDC.CreateCompatibleDC(pDC);

	tmpBMP.CreateCompatibleBitmap(pDC, texture->width, texture->height);
	tmpDC.SelectObject(tmpBMP);

	SetDIBitsToDevice(tmpDC.m_hDC, 0, 0, texture->width, texture->height,
		0, 0, 0, texture->height, texture->surface, bmi, DIB_RGB_COLORS);

	if(texture->width <= scale && texture->height <= scale
		&& texture->divWidth == 1.0f && texture->divHeight == 1.0f) {
		int down = fixed ? 0 : scale - texture->height;
		dc->BitBlt(tableX, tableY + down,
			texture->realWidth, texture->realHeight, &tmpDC, 0, 0, SRCCOPY);
	}
	else {
		int sWidth, sHeight;
		int fScale = scale;

		if(fScale > texture->realWidth || fScale > texture->realHeight)
			fScale = Max(texture->realWidth, texture->realHeight);

		if(fScale > scale)
			fScale = scale;

		if(texture->realWidth == texture->realHeight) {
			sWidth = fScale;
			sHeight = fScale;
		}
		else if(texture->realWidth > texture->realHeight) {
			sWidth = fScale;
			sHeight = texture->realHeight * fScale / texture->realWidth;
		}
		else {
			sWidth = texture->realWidth * fScale / texture->realHeight;
			sHeight = fScale;
		}

		dc->SetStretchBltMode(COLORONCOLOR);
		int down = fixed ? 0 : scale - sHeight;
		dc->StretchBlt(tableX, tableY + down, sWidth, sHeight,
			&tmpDC, 0, 0, texture->width, texture->height, SRCCOPY);
	}

	tmpDC.DeleteDC();

	free(bmi);
}
	
void QTexCell::Draw(CDC *pDC) {
	/*
	if(fixed)
		pDC->BitBlt(rect.left, rect.top, scale, scale, dc,
			(num % TABLE_X) * width, (num / TABLE_X) * height, dc ? SRCCOPY : BLACKNESS);
	else
		pDC->BitBlt(rect.left, rect.top, width, height, dc,
			(num % TABLE_X) * width, (num / TABLE_X) * height, dc ? SRCCOPY : BLACKNESS);
	*/
	if(fixed)
		pDC->BitBlt(rect.left, rect.top, width, height, dc,
			tableX, tableY, dc ? SRCCOPY : BLACKNESS);
	else
		pDC->BitBlt(rect.left, rect.top, scale, scale, dc,
			tableX, tableY, dc ? SRCCOPY : BLACKNESS);
}

void QTexCell::Draw(CDC *pDC, bool isSelected, bool isFocused) {
	if(!show)
		return;

	Draw(pDC);

	COLORREF back;
	if(isSelected) {
		if(isFocused)
			back = RGB(64, 64, 192);
		else
			back = RGB(128, 128, 128);
	}
	else
		back = RGB(0, 0, 0);

	int scalex = fixed ? texture->realWidth : scale;
	int scaley = fixed ? texture->realHeight : scale;

	// clear bottom area
	pDC->FillSolidRect(
		CRect(rect.left, rect.top + scaley, rect.right, rect.bottom), RGB(0, 0, 0));

	// clear right area
	pDC->FillSolidRect(
		CRect(rect.left + scalex, rect.top, rect.right, rect.bottom), RGB(0, 0, 0));

	// highlight box
	if(isSelected)
		pDC->FillSolidRect(
			CRect(rect.left, rect.top + scaley + 3, rect.left + scalex,
				rect.top + scaley + 5 + fontHeight), back);

	// draw text
	pDC->SetBkColor(back);
	pDC->ExtTextOut(rect.left + 1, rect.top + scaley + 4,
		ETO_CLIPPED | ETO_OPAQUE,
		CRect(rect.left + 1, rect.top + scaley + 4, rect.left + scalex - 2,
			rect.top + scaley + 4 + fontHeight), 
		texture->GetShortName(), NULL);
}


/////////////////////////////////////////////////////////////////////////////
// QTexCur

QTexCur::QTexCur() {
	texCell = NULL;
	texture = NULL;
	dc = NULL;
	bmp = NULL;

	font.CreatePointFont(72, "verdana", NULL);
}

QTexCur::~QTexCur() {
	delete texCell;
	delete dc;
	delete bmp;
}


BEGIN_MESSAGE_MAP(QTexCur, CWnd)
	//{{AFX_MSG_MAP(QTexCur)
	ON_WM_SIZE()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// QTexCur message handlers

void QTexCur::OnUpdate(LPARAM lHint, Object *pScope) {
	if(!(lHint & (DUAV_OBJSSEL | DUAV_OBJTEXMODATTRIB)))
		return;

	if(!pQMainFrame)
		return;

	Selector *pSlctr = &(pQMainFrame->GetSelector());
	if(!pSlctr) {
		SetTexture(NULL);
		return;
	}

	FaceTex *pFaceTex = pSlctr->GetSelectedFaceTex();
	if(!pFaceTex) {
		SetTexture(NULL);
		return;
	}

	if(pFaceTex->GetTexture() != texture)
		SetTexture(pFaceTex->GetTexture());
}

void QTexCur::SetTexture(Texture *_texture) {
	texture = _texture;

	delete texCell;

	dc->BitBlt(0, 0, width, height, NULL, 0, 0, BLACKNESS);

	if(!texture) {
		texCell = NULL;
		InvalidateRect(NULL, false);
		UpdateWindow();
		return;
	}

	texCell = new QTexCell(texture);

	texCell->rect.left = 4;
	texCell->rect.top = 4;
	texCell->rect.right = 4 + width;
	texCell->rect.bottom = 4 + height;
	texCell->width = width;
	texCell->height = height;
	texCell->scale = 64;
	texCell->show = true;
	texCell->num = 0;
	texCell->tableX = 0;
	texCell->tableY = 0;
	texCell->PreDraw(dc);

	InvalidateRect(NULL, false);
	UpdateWindow();
}

void QTexCur::OnPaint() {
	CPaintDC dc(this);

	dc.BitBlt(0, 0, width, height, NULL, 0, 0, BLACKNESS);

	if(texCell) {
		texCell->Draw(&dc);

		dc.SelectObject(font);
		dc.SetBkColor(RGB(0, 0, 0));
		dc.SetTextColor(RGB(255, 255, 255));

		char size[64];
		sprintf(size, "(%dx%d)", texCell->texture->realWidth,
			texCell->texture->realHeight);

		dc.ExtTextOut(72, 8, ETO_CLIPPED | ETO_OPAQUE,
			CRect(72, 8, 200, 24), texCell->texture->GetShortName(), NULL);
		dc.ExtTextOut(72, 24, ETO_CLIPPED | ETO_OPAQUE,
			CRect(72, 24, 200, 40), size, NULL);
	}
}

void QTexCur::OnSize(UINT nType, int cx, int cy) {
	CWnd::OnSize(nType, cx, cy);

	width = cx;
	height = cy;

	delete dc;
	delete bmp;

	CDC *pDC = GetDC();

	dc = new CDC;
	bmp = new CBitmap;
	dc->CreateCompatibleDC(pDC);
	bmp->CreateCompatibleBitmap(pDC, cx, cy);
	dc->SelectObject(bmp);

	ReleaseDC(pDC);

	SetTexture(texture);
}

