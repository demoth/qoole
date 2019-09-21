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

// QConfGms.cpp : implementation file
//

#include "stdafx.h"
#include "Qoole.h"
#include "Game.h"
#include "QConfGms.h"
#include "QMainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//======================================================================
// QConfigGames property page
//======================================================================

IMPLEMENT_DYNCREATE(QConfigGames, CPropertyPage)

QConfigGames::QConfigGames() 
		   : CPropertyPage(QConfigGames::IDD) {
	//{{AFX_DATA_INIT(QConfigGames)
	m_sPathUtil = _T("");
	m_sPathGame = _T("");
	//}}AFX_DATA_INIT

	for(int i = 0; i < MAXUTILS; i++) {
		m_sPaths[i] = _T("");
	}
}

QConfigGames::~QConfigGames() {
}

void QConfigGames::DoDataExchange(CDataExchange* pDX) {
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(QConfigGames)
	DDX_Text(pDX, IDC_CONFGAME_PATHUTIL, m_sPathUtil);
	DDV_MaxChars(pDX, m_sPathUtil, 256);
	DDX_Text(pDX, IDC_CONFGAME_PATHGAME, m_sPathGame);
	DDV_MaxChars(pDX, m_sPathGame, 256);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(QConfigGames, CPropertyPage)
	//{{AFX_MSG_MAP(QConfigGames)
	ON_BN_CLICKED(IDC_CONFGAME_BROWSEUTIL, OnBrowseUtil)
	ON_BN_CLICKED(IDC_CONFGAME_BROWSEGAME, OnBrowseGame)
	ON_CBN_SELCHANGE(IDC_CONFGAME_UTILCOMBO, OnUtilSelChanged)
	ON_CBN_SELCHANGE(IDC_CONFGAME_GAMESEL, OnGameSelChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// QConfigGames message handlers

BOOL QConfigGames::OnInitDialog() {
	CPropertyPage::OnInitDialog();

	// TODO: Add extra initialization here
	CComboBox *pCombo;
	pCombo = (CComboBox *) GetDlgItem(IDC_CONFGAME_GAMESEL);

	IterLinkList<Game> iter(Game::GetGames());
	Game *pGame;
	int i, gIndex = 0;
	QooleDoc *pDoc = pQMainFrame->GetDeskTopDocument();

	iter.Reset();
	i = 0;
	while(!iter.IsDone()) {
		pGame = iter.GetNext();
		pCombo->AddString(pGame->GetName());
		if (pDoc != NULL && pDoc->GetGame() == pGame)
			gIndex = i;
		i++;
	}

	// Set to current game.
	pCombo->SetCurSel(gIndex);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL QConfigGames::OnSetActive() {
	if (!CPropertyPage::OnSetActive())
		return FALSE;

	gameIndex = -1;
	utilIndex = -1;
    OnGameSelChanged();

	return TRUE;
}

BOOL QConfigGames::OnKillActive() {
	if (!CPropertyPage::OnKillActive())
		return FALSE;

	OnGameSelChanged();

	return TRUE;
}

void QConfigGames::OnOK() {
	// TODO: Add your specialized code here and/or call the base class
	
	CPropertyPage::OnOK();
}

void QConfigGames::OnCancel() {
	// TODO: Add your specialized code here and/or call the base class
	
	CPropertyPage::OnCancel();
}

void QConfigGames::OnGameSelChanged() {
	// TODO: Add your control notification handler code here
	CComboBox *pCombo, *pUtilCombo;
	pCombo = (CComboBox *) GetDlgItem(IDC_CONFGAME_GAMESEL);
	pUtilCombo = (CComboBox *) GetDlgItem(IDC_CONFGAME_UTILCOMBO);
	Game *pGame;
	int i;

	if (gameIndex >= 0 && gameIndex < (Game::GetGames()).NumOfElm()) {
		pGame = &(Game::GetGames())[gameIndex];

		// Save previously changed data.
		ASSERT(utilIndex >= 0 && utilIndex < pGame->GetNumUtils() - 1);
		UpdateData(TRUE);
		m_sPaths[utilIndex] = m_sPathUtil;
		m_sPaths[pGame->GetNumUtils() -1] = m_sPathGame;

		// Copy over to game object.
		for(i = 0; i < pGame->GetNumUtils(); i++) {
			strcpy(pGame->utilsPath[i], (LPCTSTR) m_sPaths[i]);
		}
	}

	// Load the new changed data.
	if (gameIndex == pCombo->GetCurSel())
		return;

	gameIndex = pCombo->GetCurSel();
	ASSERT(gameIndex >= 0 && gameIndex < (Game::GetGames()).NumOfElm());
	pGame = &(Game::GetGames())[gameIndex];

	pUtilCombo->ResetContent();

	for(i = 0; i < pGame->GetNumUtils() - 1; i++) {
		// Set the util names.
		pUtilCombo->AddString(pGame->GetUtilName(i));
		// Copy paths from game object
		m_sPaths[i] = pGame->utilsPath[i];
	}

	// Game name & path.
	m_sPaths[i] = pGame->utilsPath[i];
	GetDlgItem(IDC_CONFGAME_GAMENAME)->SetWindowText(pGame->GetUtilName(i));

	// Update display
	utilIndex = 0;
	pUtilCombo->SetCurSel(utilIndex);
	m_sPathUtil = m_sPaths[0];
	m_sPathGame = m_sPaths[pGame->GetNumUtils() - 1];
	UpdateData(FALSE);
}

void QConfigGames::OnUtilSelChanged() {
	CComboBox *pUtilCombo = (CComboBox *) GetDlgItem(IDC_CONFGAME_UTILCOMBO);

	if (utilIndex >= 0) {
		UpdateData(TRUE);
		m_sPaths[utilIndex] = m_sPathUtil;
	}

	if (utilIndex == pUtilCombo->GetCurSel())
		return;

	utilIndex = pUtilCombo->GetCurSel();
	m_sPathUtil = m_sPaths[utilIndex];
	UpdateData(FALSE);
}

BOOL QConfigGames::OnBrowsePath(CString &path) {
	CString szFilters = "Executable Files (*.exe; *.bat)|*.exe; *.bat||";
	CString defFileName = path;

	CFileDialog dlg(TRUE, NULL, defFileName,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		szFilters, this);

	if (dlg.DoModal() != IDOK)
		return FALSE;

	path = dlg.GetPathName();
	return TRUE;
}

void QConfigGames::OnBrowseUtil() {
	UpdateData(TRUE);
	if (OnBrowsePath(m_sPathUtil))
		UpdateData(FALSE);
}

void QConfigGames::OnBrowseGame() {
	UpdateData(TRUE);
	if (OnBrowsePath(m_sPathGame))
		UpdateData(FALSE);
}
