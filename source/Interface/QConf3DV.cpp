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

// QConf3DV.cpp : implementation file
//

#include "stdafx.h"
#include "Qoole.h"
#include "QView.h"
#include "QConf3DV.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//======================================================================
// QConfig3DView property page
//======================================================================

IMPLEMENT_DYNCREATE(QConfig3DView, CPropertyPage)

QConfig3DView::QConfig3DView()
			  :CPropertyPage(QConfig3DView::IDD) {
	//{{AFX_DATA_INIT(QConfig3DView)
	m_bInvLMouse = FALSE;
	m_iWalkSpeed = 0;
	m_iSensitivity = 0;
	//}}AFX_DATA_INIT
}

QConfig3DView::~QConfig3DView() {
}

void QConfig3DView::DoDataExchange(CDataExchange* pDX) {
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(QConfig3DView)
	DDX_Control(pDX, IDC_CONF3D_FLYDOWN, m_kFlyDown);
	DDX_Control(pDX, IDC_CONF3D_FLYUP, m_kFlyUp);
	DDX_Control(pDX, IDC_CONF3D_RIGHT, m_kStepRight);
	DDX_Control(pDX, IDC_CONF3D_LEFT, m_kStepLeft);
	DDX_Control(pDX, IDC_CONF3D_BACKWARD, m_kBackward);
	DDX_Control(pDX, IDC_CONF3D_FORWARD, m_kForward);
	DDX_Check(pDX, IDC_CONF3D_INVMOUSE, m_bInvLMouse);
	DDX_Text(pDX, IDC_CONF3D_WALKSPEED, m_iWalkSpeed);
	DDV_MinMaxUInt(pDX, m_iWalkSpeed, 0, 32);
	DDX_Text(pDX, IDC_CONF3D_SENSITIVITY, m_iSensitivity);
	DDV_MinMaxUInt(pDX, m_iSensitivity, 1, 50);
	//}}AFX_DATA_MAP
	BOOL spin = !m_bInvRMouse;
	DDX_Check(pDX, IDC_CONF3D_RMSPINBALL, spin);
	DDX_Check(pDX, IDC_CONF3D_RMORBIT, m_bInvRMouse);
}


BEGIN_MESSAGE_MAP(QConfig3DView, CPropertyPage)
	//{{AFX_MSG_MAP(QConfig3DView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// QConfig3DView message handlers

BOOL QConfig3DView::OnInitDialog() {
	CPropertyPage::OnInitDialog();

	((CHotKeyCtrl *) GetDlgItem(IDC_CONF3D_FORWARD))->SetRules(HKCOMB_NONE, 0);
	((CHotKeyCtrl *) GetDlgItem(IDC_CONF3D_BACKWARD))->SetRules(HKCOMB_NONE, 0);
	((CHotKeyCtrl *) GetDlgItem(IDC_CONF3D_LEFT))->SetRules(HKCOMB_NONE, 0);
	((CHotKeyCtrl *) GetDlgItem(IDC_CONF3D_RIGHT))->SetRules(HKCOMB_NONE, 0);
	((CHotKeyCtrl *) GetDlgItem(IDC_CONF3D_FLYUP))->SetRules(HKCOMB_NONE, 0);
	((CHotKeyCtrl *) GetDlgItem(IDC_CONF3D_FLYDOWN))->SetRules(HKCOMB_NONE, 0);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL QConfig3DView::OnSetActive() {
	m_bInvLMouse = QView::invLMouse;
	m_bInvRMouse = QView::invRMouse;
	m_iWalkSpeed = QView::walkSpeed;
	m_iSensitivity = QView::sensitivity;

	UpdateData(FALSE);

	((CHotKeyCtrl *) GetDlgItem(IDC_CONF3D_FORWARD))->SetHotKey(QView::keyForward, 0);
	((CHotKeyCtrl *) GetDlgItem(IDC_CONF3D_BACKWARD))->SetHotKey(QView::keyBackward, 0);
	((CHotKeyCtrl *) GetDlgItem(IDC_CONF3D_LEFT))->SetHotKey(QView::keyLeft, 0);
	((CHotKeyCtrl *) GetDlgItem(IDC_CONF3D_RIGHT))->SetHotKey(QView::keyRight, 0);
	((CHotKeyCtrl *) GetDlgItem(IDC_CONF3D_FLYUP))->SetHotKey(QView::keyUp, 0);
	((CHotKeyCtrl *) GetDlgItem(IDC_CONF3D_FLYDOWN))->SetHotKey(QView::keyDown, 0);

	return CPropertyPage::OnSetActive();
}

BOOL QConfig3DView::OnKillActive() {
	if (!CPropertyPage::OnKillActive())
		return FALSE;

	QView::invLMouse = (m_bInvLMouse ? true : false);
	QView::invRMouse = (m_bInvRMouse ? true : false);
	QView::walkSpeed = m_iWalkSpeed;
	QView::sensitivity = m_iSensitivity;

	WORD modifier;

	((CHotKeyCtrl *) GetDlgItem(IDC_CONF3D_FORWARD))->GetHotKey(QView::keyForward, modifier);
	((CHotKeyCtrl *) GetDlgItem(IDC_CONF3D_BACKWARD))->GetHotKey(QView::keyBackward, modifier);
	((CHotKeyCtrl *) GetDlgItem(IDC_CONF3D_LEFT))->GetHotKey(QView::keyLeft, modifier);
	((CHotKeyCtrl *) GetDlgItem(IDC_CONF3D_RIGHT))->GetHotKey(QView::keyRight, modifier);
	((CHotKeyCtrl *) GetDlgItem(IDC_CONF3D_FLYUP))->GetHotKey(QView::keyUp, modifier);
	((CHotKeyCtrl *) GetDlgItem(IDC_CONF3D_FLYDOWN))->GetHotKey(QView::keyDown, modifier);

	return TRUE;
}

void QConfig3DView::OnOK() {
	// TODO: Add your specialized code here and/or call the base class
	
	CPropertyPage::OnOK();
}

BOOL QConfig3DView::OnApply() {
	// TODO: Add your specialized code here and/or call the base class
	
	return CPropertyPage::OnApply();
}
