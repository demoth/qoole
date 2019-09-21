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

// QCompile.cpp : implementation file
//

#include "stdafx.h"
#include "Qoole.h"
#include "Game.h"
#include "QMainFrm.h"
#include "QCompile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//======================================================================
// QCompile dialog
//======================================================================
BOOL	QCompile::m_bUtils[MAXUTILS];
CString	QCompile::m_sParams[MAXUTILS];
int		QCompile::m_iSetParam;
int		QCompile::cmpStage = -1;
bool	QCompile::renderSuspend = false;

QCompile::QCompile(CWnd* pParent /*=NULL*/)
		 : CDialog(QCompile::IDD, pParent) {
	//{{AFX_DATA_INIT(QCompile)
	m_sParam = _T("");
	//}}AFX_DATA_INIT
	for(int i = 0; i < MAXUTILS; i++) {
		m_bUtils[i] = FALSE;
		m_sParams[i] = _T("");
	}
}

void QCompile::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(QCompile)
	DDX_Text(pDX, IDC_CMPL_PARAM, m_sParam);
	DDV_MaxChars(pDX, m_sParam, 128);
	//}}AFX_DATA_MAP
	Game *pGame = pQMainFrame->GetDeskTopDocument()->GetGame();
	int numUtils = pGame->GetNumUtils();
	for(int i = 0; i < numUtils - 1; i++) {
		DDX_Check(pDX, IDC_CMPL_UTIL1 + i, m_bUtils[i]);
	}
	DDX_Check(pDX, IDC_CMPL_GAME, m_bUtils[i]);
}

BEGIN_MESSAGE_MAP(QCompile, CDialog)
	//{{AFX_MSG_MAP(QCompile)
	ON_CBN_SELCHANGE(IDC_CMPL_UTILCOMBO, OnUtilSelChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// QCompile message handlers

BOOL QCompile::OnInitDialog() {
	CDialog::OnInitDialog();

	Game *pGame = pQMainFrame->GetDeskTopDocument()->GetGame();
	int i, numUtils = pGame->GetNumUtils();
	ASSERT(numUtils <= MAXUTILS);

	const char *utilName;
	CComboBox *pCombo = (CComboBox *) GetDlgItem(IDC_CMPL_UTILCOMBO);

	// Change the util names.
	for(i = 0; i < numUtils - 1; i++) {
		utilName = pGame->GetUtilName(i);
		pCombo->AddString(utilName);
		GetDlgItem(IDC_CMPL_UTIL1 + i)->SetWindowText(utilName);
		GetDlgItem(IDC_CMPL_UTIL1 + i)->ShowWindow(SW_SHOW);
	}

	// Change the game name.
	utilName = pGame->GetUtilName(i);
	pCombo->AddString(utilName);
	GetDlgItem(IDC_CMPL_GAME)->SetWindowText(utilName);
	GetDlgItem(IDC_CMPL_GAME)->ShowWindow(SW_SHOW);

	// Load the dialog states.
	pGame->InitCompileParams(m_bUtils, m_sParams, m_iSetParam);

	pCombo->SetCurSel(m_iSetParam);
	m_sParam = m_sParams[m_iSetParam];

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void QCompile::OnOK() {
	CDialog::OnOK();

	Game *pGame = pQMainFrame->GetDeskTopDocument()->GetGame();
	CComboBox *pCombo = (CComboBox *) GetDlgItem(IDC_CMPL_UTILCOMBO);

	m_iSetParam = pCombo->GetCurSel();
	m_sParams[m_iSetParam] = m_sParam;

	// Save the dialog state.
	pGame->SaveCompileParams(m_bUtils, m_sParams, m_iSetParam);

	// Init the compile stage.
	// ASSERT(cmpStage == -1);
	cmpStage = 0;
}

void QCompile::OnUtilSelChanged() {
	UpdateData(TRUE);
	m_sParams[m_iSetParam] = m_sParam;

	CComboBox * pCombo = (CComboBox *) GetDlgItem(IDC_CMPL_UTILCOMBO);
	
	if (m_iSetParam == pCombo->GetCurSel())
		return;

	m_iSetParam = pCombo->GetCurSel();
	m_sParam = m_sParams[m_iSetParam];
	UpdateData(FALSE);
}

// Compile map.
void QCompile::ExecuteCompiles(void) {
	QProcessWnd *pProcWnd = pQMainFrame->GetProcessWnd();
	QooleDoc *pDoc = pQMainFrame->GetDeskTopDocument();
	Game *pGame = pDoc->GetGame();
	const char *docName = pDoc->GetDocName();
	int i, numUtils = pGame->GetNumUtils();

	ASSERT(cmpStage >= 0 && cmpStage <= numUtils);

	// Find the next compile stage.
	for(i = 0; i < numUtils; i++) {
		if (cmpStage == i && !m_bUtils[i])
			cmpStage++;
	}

	// Check if need to resume rendering library from game.
	if (renderSuspend) {
		// PAUL: cmpStage was 0 here once?
		ASSERT(cmpStage == numUtils);
		renderSuspend = false;

		pQMainFrame->SuspendRenderLib(false);
	}

	// Check if compile's finished.
	if (cmpStage == numUtils)
		goto COMPILE_FINISH;
	
	// Check for the 1st compile stage.
	if (cmpStage == 0) {
		// Clear the window. 
		pProcWnd->ClearWindow();

		// Export the map first.
		const char *mapPath = pGame->GetMapExportPath(docName, PrintText);
		if (mapPath == NULL)
			goto COMPILE_FINISH;

		PrintText("Saving file: ");
		PrintText(mapPath);
		PrintText("\n\n");

		if (!pDoc->OnExportMap(mapPath)) {
			goto COMPILE_FINISH;
			PrintText("Failed to save file\n");
		}
	}

	// Get the util command line.
	const char *cmdLine, *initDir;
	if (!pGame->GetUtilCmdLine(cmpStage, docName, &cmdLine, &initDir, PrintText))
		goto COMPILE_FINISH;

	// Run the util.
	ASSERT(!pProcWnd->IsProcessRunning());

	// Check for the run game stage to suspend 3dfx card.
	ASSERT(renderSuspend == false);
	if (cmpStage == numUtils - 1) {
		renderSuspend = true;
		pQMainFrame->SuspendRenderLib(true);
	}

	pProcWnd->ExecProcess((char *) cmdLine, (char *) initDir, ExecuteCompiles);
	cmpStage++;

	return;

COMPILE_FINISH:
	cmpStage = -1;
	return;
}

void QCompile::StopCompiles(void) {
	Game *pGame = pQMainFrame->GetDeskTopDocument()->GetGame();
	cmpStage = pGame->GetNumUtils();

	QProcessWnd *pProcWnd = pQMainFrame->GetProcessWnd();
	pProcWnd->TerminateProcess();

	return;
}

void QCompile::PrintText(LPCTSTR text) {
	ASSERT(text != NULL);

	QProcessWnd *pProcWnd = pQMainFrame->GetProcessWnd();
	ASSERT(pProcWnd != NULL);

	pProcWnd->OutputText((char *) text);
}
