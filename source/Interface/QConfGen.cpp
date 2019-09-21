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

// QConfGen.cpp : implementation file
//

#include "stdafx.h"
#include "Qoole.h"
#include "QMainFrm.h"
#include "QConfGen.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// QConfGen property page

IMPLEMENT_DYNCREATE(QConfGen, CPropertyPage)

QConfGen::QConfGen()
		 : CPropertyPage(QConfGen::IDD) {
	//{{AFX_DATA_INIT(QConfGen)
	m_bHollowPrompt = FALSE;
	m_iHollowWidth = 0;
	m_iUndoLevels = 0;
	m_bGroupPrompt = FALSE;
	//}}AFX_DATA_INIT
}

QConfGen::~QConfGen() {
}

void QConfGen::DoDataExchange(CDataExchange* pDX) {
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(QConfGen)
	DDX_Check(pDX, IDC_CONFGEN_HOLLOWPROMPT, m_bHollowPrompt);
	DDX_Text(pDX, IDC_CONFGEN_HOLLOWWIDTH, m_iHollowWidth);
	DDV_MinMaxInt(pDX, m_iHollowWidth, 4, 128);
	DDX_Text(pDX, IDC_CONFGEN_UNDOLEVELS, m_iUndoLevels);
	DDV_MinMaxInt(pDX, m_iUndoLevels, 1, 100);
	DDX_Check(pDX, IDC_CONFGEN_GROUPNAME, m_bGroupPrompt);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(QConfGen, CPropertyPage)
	//{{AFX_MSG_MAP(QConfGen)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//================================================================
// QConfGen message handlers
//================================================================

BOOL QConfGen::OnInitDialog() {
	CPropertyPage::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL QConfGen::OnSetActive() {
	m_iUndoLevels = pQMainFrame->GetUndoLevels();
	m_bHollowPrompt = (QMainFrame::promptHollow != 0);
	m_iHollowWidth = QMainFrame::hollowThickness;
	m_bGroupPrompt = (QMainFrame::promptGroupName != 0);

	UpdateData(FALSE);

	return CPropertyPage::OnSetActive();
}


BOOL QConfGen::OnKillActive() {
	if (!CPropertyPage::OnKillActive())
		return FALSE;
	
	pQMainFrame->SetUndoLevels(m_iUndoLevels);
	QMainFrame::promptHollow = (m_bHollowPrompt ? 1 : 0);
	QMainFrame::hollowThickness = m_iHollowWidth;
	QMainFrame::promptGroupName = (m_bGroupPrompt ? 1 : 0);

	return TRUE;
}
