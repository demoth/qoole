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

// QColorWnd.cpp : implementation file
//

#include "stdafx.h"
#include "Qoole.h"
#include "QMainFrm.h"
#include "QColrWnd.h"
#include "Selector.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// QColorWnd dialog


QColorWnd::QColorWnd(CWnd* pParent /*=NULL*/)
	: CDialog(QColorWnd::IDD, pParent)
{
	//{{AFX_DATA_INIT(QColorWnd)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	inUse = false;

	red = 0;
	grn = 0;
	blu = 0;
	changed = false;

	pScope = NULL;
	pObject = NULL;
}


void QColorWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(QColorWnd)
	DDX_Control(pDX, IDC_SETCOLOR_COLOR, m_Color);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(QColorWnd, CDialog)
	//{{AFX_MSG_MAP(QColorWnd)
	ON_WM_HSCROLL()
	ON_WM_PAINT()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// QColorWnd message handlers

void QColorWnd::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) {
	if(!inUse) {
		CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
		return;
	}

	int lastPos = pScrollBar->GetScrollPos();

	switch(nSBCode) {
	case SB_LEFT:
		pScrollBar->SetScrollPos(0);
		break;
	case SB_LINELEFT:
		pScrollBar->SetScrollPos(lastPos - 1);
		break;
	case SB_LINERIGHT:
		pScrollBar->SetScrollPos(lastPos + 1);
		break;
	case SB_PAGELEFT:
		pScrollBar->SetScrollPos(lastPos - 16);
		break;
	case SB_PAGERIGHT:
		pScrollBar->SetScrollPos(lastPos + 16);
		break;
	case SB_RIGHT:
		pScrollBar->SetScrollPos(255);
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		pScrollBar->SetScrollPos(nPos);
		break;
	default:
		break;
	}

	CScrollBar *pRedBar = (CScrollBar *)GetDlgItem(IDC_COLORWND_RED);
	CScrollBar *pGrnBar = (CScrollBar *)GetDlgItem(IDC_COLORWND_GRN);
	CScrollBar *pBluBar = (CScrollBar *)GetDlgItem(IDC_COLORWND_BLU);

	red = pRedBar->GetScrollPos();
	grn = pGrnBar->GetScrollPos();
	blu = pBluBar->GetScrollPos();

	changed = true;

	OnRedraw();

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void QColorWnd::OnUpdate(LPARAM lHint, Object *_pScope) {
	if(!(lHint & (DUAV_OBJSSEL | DUAV_OBJENTMODATTRIB)))
		return;

	SetColors();

	inUse = false;
	const char *color = "0.0 0.0 0.0";

	Selector *pSlctr = &pQMainFrame->GetSelector();

	if(pSlctr && pSlctr->GetNumMSelectedObjects() == 1) {
		pObject = (&((pSlctr->GetMSelectedObjects())[0]))->GetPtr();
		Entity *pEntity = pObject->GetEntityPtr();

		if(pEntity) {
			color = pEntity->GetKey("_color");

			if(!color) {
				EntInfo *pEntInfo = pEntity->GetEntInfo();
				for(int i = 0; pEntInfo->GetNumEntries(); i++) {
					if(!strcmpi(pEntInfo->GetEntryNum(i)->name, "_color")) {
						color = "1.0 1.0 1.0";
						break;
					}
				}
			}

			if(color) {
				pScope = _pScope;
				inUse = true;
			}
		}
	}

	float r, g, b;
	sscanf(color, "%f %f %f", &r, &g, &b);

	red = (int)(r * 255);
	grn = (int)(g * 255);
	blu = (int)(b * 255);

	CScrollBar *pRedBar = (CScrollBar *)GetDlgItem(IDC_COLORWND_RED);
	CScrollBar *pGrnBar = (CScrollBar *)GetDlgItem(IDC_COLORWND_GRN);
	CScrollBar *pBluBar = (CScrollBar *)GetDlgItem(IDC_COLORWND_BLU);

	pRedBar->SetScrollPos(red);
	pGrnBar->SetScrollPos(grn);
	pBluBar->SetScrollPos(blu);

	OnRedraw();
}

void QColorWnd::OnRedraw(void) {
	CStatic *pRedVal = (CStatic *)GetDlgItem(IDC_COLORWND_REDVAL);
	CStatic *pGrnVal = (CStatic *)GetDlgItem(IDC_COLORWND_GRNVAL);
	CStatic *pBluVal = (CStatic *)GetDlgItem(IDC_COLORWND_BLUVAL);

	char buf[8];
	sprintf(buf, "%d", red);
	pRedVal->SetWindowText(buf);
	sprintf(buf, "%d", grn);
	pGrnVal->SetWindowText(buf);
	sprintf(buf, "%d", blu);
	pBluVal->SetWindowText(buf);

	RECT rect;
	m_Color.GetWindowRect(&rect);
	ScreenToClient(&rect);
	InvalidateRect(&rect, false);
	UpdateWindow();
}

BOOL QColorWnd::OnInitDialog() {
	CDialog::OnInitDialog();
	
	CScrollBar *pRedBar = (CScrollBar *)GetDlgItem(IDC_COLORWND_RED);
	CScrollBar *pGrnBar = (CScrollBar *)GetDlgItem(IDC_COLORWND_GRN);
	CScrollBar *pBluBar = (CScrollBar *)GetDlgItem(IDC_COLORWND_BLU);

	pRedBar->SetScrollRange(0, 255);
	pGrnBar->SetScrollRange(0, 255);
	pBluBar->SetScrollRange(0, 255);

	OnUpdate(DUAV_OBJSSEL, &pQMainFrame->GetScope());

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void QColorWnd::OnPaint() {
	CPaintDC dc(this); // device context for painting

	CRect rect;
	m_Color.GetWindowRect(&rect);
	ScreenToClient(&rect);
	rect.DeflateRect(1, 1);

	dc.FillSolidRect(&rect, blu << 16 | grn << 8 | red);
}

void QColorWnd::SetColors(void) {
	if(!pScope || !pObject || !changed)
		return;

	if(!pScope->IsMyChild(*pObject)) {
		pScope = NULL;
		changed = false;
		return;
	}

	char arg[32];
	sprintf(arg, "%1.2f %1.2f %1.2f",
		(float)red / 255.0f,
		(float)grn / 255.0f,
		(float)blu / 255.0f);

	OpEntitySetKey *op = new OpEntitySetKey(pObject, "_color", arg);
	pQMainFrame->CommitOperation(*op);

	changed = false;
	pScope = NULL;
	pObject = NULL;

	QooleDoc *pDoc = pQMainFrame->GetDeskTopDocument();
	pDoc->UpdateAllViews(NULL, DUAV_OBJSMODATTRIB, NULL);
}

void QColorWnd::OnClose() {
	SetColors();
	CRect rect;
	GetWindowRect(&rect);
	pQMainFrame->OnDestroyColorWnd(rect);
}

void QColorWnd::OnOK() {
	SetColors();
	CRect rect;
	GetWindowRect(&rect);
	pQMainFrame->OnDestroyColorWnd(rect);
}
