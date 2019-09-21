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

// QConfigDraw.cpp : implementation file
//

#include "stdafx.h"
#include "Qoole.h"
#include "QView.h"
#include "QConfDrw.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// QConfigDraw property page

IMPLEMENT_DYNCREATE(QConfigDraw, CPropertyPage)

QConfigDraw::QConfigDraw() : CPropertyPage(QConfigDraw::IDD)
{
	//{{AFX_DATA_INIT(QConfigDraw)
	m_draw2dWire = _T("");
	m_draw3dWire = _T("");
	m_draw3dSolid = _T("");
	m_draw3dTex = _T("");
	//}}AFX_DATA_INIT
}

QConfigDraw::~QConfigDraw()
{
}

void QConfigDraw::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(QConfigDraw)
	DDX_CBString(pDX, IDC_CONFDRAW_2DWIRE, m_draw2dWire);
	DDX_CBString(pDX, IDC_CONFDRAW_3DWIRE, m_draw3dWire);
	DDX_CBString(pDX, IDC_CONFDRAW_3DSOLID, m_draw3dSolid);
	DDX_CBString(pDX, IDC_CONFDRAW_3DTEX, m_draw3dTex);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(QConfigDraw, CPropertyPage)
	//{{AFX_MSG_MAP(QConfigDraw)
	ON_CBN_SELCHANGE(IDC_CONFDRAW_2DWIRE, OnSelchangeConfdraw2dwire)
	ON_CBN_SELCHANGE(IDC_CONFDRAW_3DSOLID, OnSelchangeConfdraw3dsolid)
	ON_CBN_SELCHANGE(IDC_CONFDRAW_3DTEX, OnSelchangeConfdraw3dtex)
	ON_CBN_SELCHANGE(IDC_CONFDRAW_3DWIRE, OnSelchangeConfdraw3dwire)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// QConfigDraw message handlers

BOOL QConfigDraw::OnInitDialog() {
	CPropertyPage::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL QConfigDraw::OnSetActive() {

	m_draw2dWire = QView::draw2dWire;
	m_draw3dWire = QView::draw3dWire;
	m_draw3dSolid = QView::draw3dSolid;
	m_draw3dTex = QView::draw3dTex;

	UpdateData(FALSE);

	return CPropertyPage::OnSetActive();
}

BOOL QConfigDraw::OnKillActive() {
	if(!CPropertyPage::OnKillActive())
		return FALSE;

	strcpy(QView::draw2dWire, m_draw2dWire.GetBuffer(8));
	strcpy(QView::draw3dWire, m_draw3dWire.GetBuffer(8));
	strcpy(QView::draw3dSolid, m_draw3dSolid.GetBuffer(8));
	strcpy(QView::draw3dTex, m_draw3dTex.GetBuffer(8));

	QView::RefreshAll();

	return TRUE;
}

void QConfigDraw::OnSelchangeConfdraw2dwire() {
	SetModified();
}

void QConfigDraw::OnSelchangeConfdraw3dsolid() {
	SetModified();
}

void QConfigDraw::OnSelchangeConfdraw3dtex() {
	SetModified();
}

void QConfigDraw::OnSelchangeConfdraw3dwire() {
	SetModified();
}

BOOL QConfigDraw::OnApply() {
	QView::RefreshAll();
	return CPropertyPage::OnApply();
}
