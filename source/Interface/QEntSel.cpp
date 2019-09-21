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

// QEntSel.cpp : implementation file
//

#include "stdafx.h"
#include "qoole.h"
#include "QEntSel.h"
#include "EntList.h"
#include "Game.h"
#include "list.h"
#include "QDraw.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

struct EntSet {
	char caption[64];
	char entName[32];
	char gameName[32];
	char palName[32];
} EntSets[64];
int numEntSets = 0;

/////////////////////////////////////////////////////////////////////////////
// QEntSel dialog


QEntSel::QEntSel(CWnd* pParent /*=NULL*/)
	: CDialog(QEntSel::IDD, pParent) {

	m_gameName = NULL;

	//{{AFX_DATA_INIT(QEntSel)
	//}}AFX_DATA_INIT

	gameNum = 0;
	cfg = new LConfig("Settings");
	cfg->RegisterVar("LastGameNum", &gameNum, LVAR_INT);
}

QEntSel::~QEntSel() {
	cfg->SaveVars();
	delete cfg;
}

void QEntSel::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(QEntSel)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(QEntSel, CDialog)
	//{{AFX_MSG_MAP(QEntSel)
	ON_CBN_SELCHANGE(IDC_ENTSET_GAME, OnSelchangeCombo1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// QEntSel message handlers

BOOL QEntSel::OnInitDialog() {
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CComboBox *combo;
	combo = (CComboBox *)GetDlgItem(IDC_ENTSET_GAME);

	IterLinkList<Game> iter(Game::GetGames());
	Game *game;

	iter.Reset();
	while(!iter.IsDone()) {
		game = iter.GetNext();
		combo->AddString(game->GetName());
	}

	combo->SetCurSel(gameNum);

	if(!numEntSets)
		LoadEntSets();

	OnSelchangeCombo1();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void QEntSel::OnSelchangeCombo1() {
	CComboBox *combo;
	char gameName[32];
	int i;

	combo = (CComboBox *)GetDlgItem(IDC_ENTSET_GAME);
	combo->GetLBText(combo->GetCurSel(), gameName);

	combo = (CComboBox *)GetDlgItem(IDC_ENTSET_ENTITY);
	combo->ResetContent();

	for(i = 0; i < numEntSets; i++)
		if(!strcmp(gameName, EntSets[i].gameName))
			combo->AddString(EntSets[i].caption);

	combo->SetCurSel(0);
}

void QEntSel::OnOK() {
	CDialog::OnOK();

	CComboBox *combo;
	char gameName[32];
	int i, j = 0, sel;

	combo = (CComboBox *)GetDlgItem(IDC_ENTSET_GAME);
	combo->GetLBText(combo->GetCurSel(), gameName);

	gameNum = combo->GetCurSel();

	combo = (CComboBox *)GetDlgItem(IDC_ENTSET_ENTITY);
	sel = combo->GetCurSel();

	for(i = 0; i < numEntSets; i++)
		if(!strcmp(gameName, EntSets[i].gameName))
			if(sel == j++)
				break;

	m_gameName = EntSets[i].gameName;
	m_entName = EntSets[i].entName;
	m_palName = EntSets[i].palName;
}

void QEntSel::LoadEntSets(void) {
	LFile file;
	QDraw::OutputText("Loading entity sets...");
	if(!file.Open("GameSets.txt"))
	{
		QDraw::OutputText("Error.\nGameSets.txt not found.\n");
		LFatal("GameSets.txt not found.");
	}

	numEntSets = 0;

	char *buf, *c, tmp[256];
	while(buf = file.GetNextLine()) {
		c = strchr(buf, 13);
		if(c)
			*c = '\0';

		if(strlen(buf) < 4)
			continue;

		if(buf[0] == ';' || buf[0] == '#' || (buf[0] == '/' && buf[1] == '/'))
			continue;

		strcpy(tmp, buf);
		c = strchr(tmp, ',');
		if(!c)
			continue;
		*c = '\0';
		strcpy(EntSets[numEntSets].caption, tmp);

		c++;
		while(*c == ' ' || *c == 9) c++;
		strcpy(tmp, c);
		c = strchr(tmp, ',');
		if(!c)
			continue;
		*c = '\0';
		strcpy(EntSets[numEntSets].gameName, tmp);

		c++;
		while(*c == ' ' || *c == 9) c++;
		strcpy(tmp, c);
		c = strchr(tmp, ',');
		if(!c) {
			strcpy(EntSets[numEntSets].entName, tmp);
			strcpy(EntSets[numEntSets].palName, "");
		}
		else {
			*c = '\0';
			strcpy(EntSets[numEntSets].entName, tmp);
			c++;
			while(*c == ' ' || *c == 9) c++;
			strcpy(EntSets[numEntSets].palName, c);
		}

		numEntSets++;
	}

	QDraw::OutputText(" OK.\n");
}
