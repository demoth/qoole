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

// QConf2DV.cpp : implementation file
//

#include "stdafx.h"
#include "Qoole.h"
#include "QDraw.h"
#include "QConf2DV.h"
#include "QMainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//======================================================================
// QConfig2DViews property page

IMPLEMENT_DYNCREATE(QConfig2DViews, CPropertyPage)

QConfig2DViews::QConfig2DViews()
			   :CPropertyPage(QConfig2DViews::IDD) {
	//{{AFX_DATA_INIT(QConfig2DViews)
	m_bGrid = FALSE;
	m_sGridSize = _T("");
	m_bGridThick = FALSE;
	m_iGridThickSize = 0;
	m_bRotSnap = FALSE;
	m_sRotSnapSize = _T("");
	m_bSnap = FALSE;
	m_sSnapSize = _T("");
	//}}AFX_DATA_INIT
}

QConfig2DViews::~QConfig2DViews() {
}

void QConfig2DViews::DoDataExchange(CDataExchange* pDX) {
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(QConfig2DViews)
	DDX_Check(pDX, IDC_CONF2D_GRID, m_bGrid);
	DDX_CBString(pDX, IDC_CONF2D_GRIDSIZE, m_sGridSize);
	DDV_MaxChars(pDX, m_sGridSize, 5);
	DDX_Check(pDX, IDC_CONF2D_GRIDTHICK, m_bGridThick);
	DDX_Text(pDX, IDC_CONF2D_GRIDTHICKSIZE, m_iGridThickSize);
	DDX_Check(pDX, IDC_CONF2D_ROTSNAP, m_bRotSnap);
	DDX_CBString(pDX, IDC_CONF2D_ROTSNAPSIZE, m_sRotSnapSize);
	DDV_MaxChars(pDX, m_sRotSnapSize, 5);
	DDX_Check(pDX, IDC_CONF2D_SNAP, m_bSnap);
	DDX_CBString(pDX, IDC_CONF2D_SNAPSIZE, m_sSnapSize);
	DDV_MaxChars(pDX, m_sSnapSize, 5);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(QConfig2DViews, CPropertyPage)
	//{{AFX_MSG_MAP(QConfig2DViews)
	ON_BN_CLICKED(IDC_CONF2D_GRID, OnConf2DGrid)
	ON_BN_CLICKED(IDC_CONF2D_GRIDTHICK, OnConf2DGridThick)
	ON_BN_CLICKED(IDC_CONF2D_ROTSNAP, OnConf2DRotSnap)
	ON_BN_CLICKED(IDC_CONF2D_SNAP, OnConf2DSnap)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// QConfig2DViews message handlers

BOOL QConfig2DViews::OnInitDialog() {
	CPropertyPage::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL QConfig2DViews::OnSetActive() {
	char textBuf[16];

	m_bGrid = QDraw::drawGridStep1;

	sprintf(textBuf, "%d", QDraw::gridStep1);
	m_sGridSize = textBuf;
	GetDlgItem(IDC_CONF2D_GRIDSIZE)->EnableWindow(m_bGrid);

	m_bGridThick = QDraw::drawGridStep2;
	GetDlgItem(IDC_CONF2D_GRIDTHICK)->EnableWindow(m_bGrid);

	m_iGridThickSize = (unsigned int) QDraw::gridStep2;
	GetDlgItem(IDC_CONF2D_GRIDTHICKSIZE)->EnableWindow(m_bGrid && m_bGridThick);

	m_bSnap = QView::gridSnap;

	sprintf(textBuf, "%d", QView::gridSnapVal);
	m_sSnapSize = textBuf;
	GetDlgItem(IDC_CONF2D_SNAPSIZE)->EnableWindow(m_bSnap);

	m_bRotSnap = QView::rotSnap;

	sprintf(textBuf, "%d", QView::rotSnapVal);
	m_sRotSnapSize = textBuf;
	GetDlgItem(IDC_CONF2D_ROTSNAPSIZE)->EnableWindow(m_bRotSnap);

	UpdateData(FALSE);

	return CPropertyPage::OnSetActive();
}

BOOL QConfig2DViews::OnKillActive() {
	if (!CPropertyPage::OnKillActive())
		return FALSE;

	UINT value;

	QDraw::drawGridStep1 = (m_bGrid ? TRUE : FALSE);
	if (Str2Int((LPCTSTR) m_sGridSize, value))
		QDraw::gridStep1 = value;

	QDraw::drawGridStep2 = (m_bGridThick ? TRUE : FALSE);
	QDraw::gridStep2 = m_iGridThickSize;

	// Update QViews.
	if (pQMainFrame->GetDeskTopDocument() != NULL)
		QView::UpdateGridsDisplay();

	QView::gridSnap = (m_bSnap ? TRUE : FALSE);
	if (Str2Int((LPCTSTR) m_sSnapSize, value))
		QView::gridSnapVal = value;

	QView::rotSnap = (m_bRotSnap ? TRUE : FALSE);
	if (Str2Int((LPCTSTR) m_sRotSnapSize, value)) 
		QView::rotSnapVal = value;

	return TRUE;
}

BOOL QConfig2DViews::OnApply() {
	// TODO: Add your specialized code here and/or call the base class
	
	return CPropertyPage::OnApply();
}

void QConfig2DViews::OnOK() {
	// TODO: Add your specialized code here and/or call the base class
	
	CPropertyPage::OnOK();
}

void QConfig2DViews::OnConf2DGrid() {
	UpdateData(TRUE);

	m_bGridThick = m_bGrid;

	GetDlgItem(IDC_CONF2D_GRIDTHICK)->EnableWindow(m_bGrid);
	GetDlgItem(IDC_CONF2D_GRIDSIZE)->EnableWindow(m_bGrid);
	GetDlgItem(IDC_CONF2D_GRIDTHICKSIZE)->EnableWindow(m_bGrid);

	UpdateData(FALSE);
}

void QConfig2DViews::OnConf2DGridThick() {
	UpdateData(TRUE);

	GetDlgItem(IDC_CONF2D_GRIDTHICKSIZE)->EnableWindow(m_bGridThick);
}

void QConfig2DViews::OnConf2DSnap() {
	UpdateData(TRUE);

	GetDlgItem(IDC_CONF2D_SNAPSIZE)->EnableWindow(m_bSnap);
}

void QConfig2DViews::OnConf2DRotSnap() {
	UpdateData(TRUE);

	GetDlgItem(IDC_CONF2D_ROTSNAPSIZE)->EnableWindow(m_bRotSnap);
}
