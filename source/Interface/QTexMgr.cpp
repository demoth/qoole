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

// QTexMgr.cpp : implementation file
//

#include "stdafx.h"
#include "Qoole.h"
#include "Game.h"
#include "QTexMgr.h"
#include "QMainFrm.h"
#include "QDraw.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// QTexMgr dialog


QTexMgr::QTexMgr(CWnd* pParent /*=NULL*/)
	: CDialog(QTexMgr::IDD, pParent)
{
	//{{AFX_DATA_INIT(QTexMgr)
	//}}AFX_DATA_INIT

	texList = NULL;
}

QTexMgr::~QTexMgr() {
	if(texList)
		delete texList;
}

void QTexMgr::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(QTexMgr)
	DDX_Control(pDX, IDC_TEXMGR_REMOVEWAD, m_removeWad);
	DDX_Control(pDX, IDC_TEXMGR_REMOVELIST, m_removeList);
	DDX_Control(pDX, IDC_TEXMGR_REMOVE, m_remove);
	DDX_Control(pDX, IDC_TEXMGR_NEWLIST, m_newList);
	DDX_Control(pDX, IDC_TEXMGR_ADD, m_add);
	DDX_Control(pDX, IDC_TEXMGR_TEXINFO, m_texInfo);
	DDX_Control(pDX, IDC_TEXMGR_ADDDIR, m_addDir);
	DDX_Control(pDX, IDC_TEXMGR_TEXPICK, m_pickList);
	DDX_Control(pDX, IDC_TEXMGR_TEXLIST, m_texList);
	DDX_Control(pDX, IDC_TEXMGR_LISTSEL, m_listList);
	DDX_Control(pDX, IDC_TEXMGR_GAMESEL, m_gameCombo);
	DDX_Control(pDX, IDC_TEXMGR_DIRSEL, m_dirList);
	DDX_Control(pDX, IDC_TEXMGR_TEXTURE, m_texWnd);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(QTexMgr, CDialog)
	//{{AFX_MSG_MAP(QTexMgr)
	ON_CBN_SELCHANGE(IDC_TEXMGR_GAMESEL, OnGameSel)
	ON_LBN_SELCHANGE(IDC_TEXMGR_DIRSEL, OnDirSel)
	ON_LBN_SELCHANGE(IDC_TEXMGR_LISTSEL, OnListSel)
	ON_LBN_SELCHANGE(IDC_TEXMGR_TEXPICK, OnPickSel)
	ON_BN_CLICKED(IDC_TEXMGR_REMOVE, OnTextureRemove)
	ON_BN_CLICKED(IDC_TEXMGR_NEWLIST, OnNewList)
	ON_BN_CLICKED(IDC_TEXMGR_ADD, OnTextureAdd)
	ON_BN_CLICKED(IDC_TEXMGR_ADDDIR, OnAddDir)
	ON_BN_CLICKED(IDC_TEXMGR_REMOVELIST, OnRemoveList)
	ON_LBN_SELCHANGE(IDC_TEXMGR_TEXLIST, OnTexSel)
	ON_BN_CLICKED(IDC_TEXMGR_REMOVEWAD, OnRemoveDir)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// QTexMgr message handlers

BOOL QTexMgr::OnInitDialog() {
	QDraw::OutputText("Loading texture manager... ");

	CDialog::OnInitDialog();

	m_texWnd.OnInit();

	IterLinkList<Game> iter(Game::GetGames());
	int i, gIndex = 0;
	QooleDoc *pDoc = pQMainFrame->GetDeskTopDocument();

	if(!pDoc)
		m_gameCombo.AddString("");

	iter.Reset();
	i = 0;
	while(!iter.IsDone()) {
		pGame = iter.GetNext();
		m_gameCombo.AddString(pGame->GetName());
		if (pDoc != NULL && pDoc->GetGame() == pGame)
			gIndex = i + (!pDoc ? 1 : 0);
		i++;
	}

	pGame = NULL;

	// Set to current game.
	m_gameCombo.SetCurSel(gIndex);
	OnGameSel();

	QDraw::OutputText("OK.\n");
	return TRUE;
}

void QTexMgr::OnGameSel() {
	m_addDir.EnableWindow(false);
	m_removeWad.EnableWindow(false);
	m_newList.EnableWindow(false);
	m_removeList.EnableWindow(false);
	m_add.EnableWindow(false);
	m_remove.EnableWindow(false);

	QooleDoc *pDoc = pQMainFrame->GetDeskTopDocument();

	if(!pDoc && m_gameCombo.GetCurSel() == 0)
		return;

	pGame = &(Game::GetGames())[m_gameCombo.GetCurSel() - (!pDoc ? 1 : 0)];
	if(!pGame->Init()) {
		pGame = NULL;
		m_gameCombo.SetCurSel(0);

		m_dirList.ResetContent();
		m_listList.ResetContent();
		m_pickList.ResetContent();
		m_texList.ResetContent();

		SetTexInfo(NULL, 0);

		return;
	}

	bool usesWad = pGame->UsesWadFile();

	m_dirList.EnableWindow(usesWad);
	if(usesWad)
		m_removeWad.EnableWindow(usesWad);

	m_addDir.EnableWindow(true);
	m_newList.EnableWindow(true);

	BuildDirList();

	m_listList.ResetContent();

	QDraw::OutputText("Loading texture lists... ");
	char searchDir[MAX_PATH];
	sprintf(searchDir, "%s\\texlists\\%s",
		LFile::GetInitDir(), pGame->GetName());

	char *texFile;
	LFindFiles findTexFiles(searchDir, "*.tex");
	while(texFile = findTexFiles.Next()) {
		char *c = strrchr(texFile, '.');
		*c = '\0';
		m_listList.AddString(texFile);
	}
	QDraw::OutputText("OK.\n");

	m_pickList.ResetContent();
	m_texList.ResetContent();

	SetTexInfo(NULL, 0);

	m_dirList.SetCurSel(0);
//	OnDirSel();

	m_listList.SetCurSel(0);
	OnListSel();
}

void QTexMgr::OnDirSel() {
	m_pickList.ResetContent();

	TexDir *texDir = pGame->GetTexDB()->GetTexDir(m_dirList.GetCurSel());
	if(!texDir)
		return;
	
	strcpy(texDirName, texDir->dirName);

	SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_WAIT)));

	if(!pGame->UsesWadFile()) {
		char *dirName, *texName;

		LFindDirs finddirs(texDirName);
		while(dirName = finddirs.Next()) {
			char newDir[MAX_PATH];
			sprintf(newDir, "%s\\%s", texDirName, dirName);

			LFindFiles findfiles(newDir);
			while(texName = findfiles.Next()) {
				char filename[MAX_PATH];
				char realname[64];
				char *c;

				sprintf(filename, "%s\\%s", newDir, texName);

				c = strchr(texName, '.');
				if(c)
					*c = 0;

				sprintf(realname, "%s/%s", dirName, texName);
			
				if(!(texName[0] == '+' && texName[1] > '0')) {
					int num = m_pickList.AddString(realname);
					m_pickList.SetItemData(num, 0);
				}
			}
		}
	}
	else {
		WadHeader wadHeader;
		WadEntry *wadEntry;

		LFile file;
		if(!file.Open(texDirName))
			return;

		file.Read(&wadHeader, sizeof(WadHeader));
		if(strncmp(wadHeader.magic, "WAD2", 4) &&
			strncmp(wadHeader.magic, "WAD3", 4))
			return;
		
		wadEntry = new WadEntry[wadHeader.entries];
		file.Seek(wadHeader.offset);
		file.Read(wadEntry, sizeof(WadEntry), wadHeader.entries);

		int rtn = -1;
		for(int i = 0; i < wadHeader.entries; i++) {
			char *texName = wadEntry[i].name;
			if(!(texName[0] == '+' && texName[1] > '0') && strlen(texName)) {
				int num = m_pickList.AddString(texName);
				m_pickList.SetItemData(num, wadEntry[i].offset);
			}
		}

		delete [] wadEntry;
	}

//	SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
//	SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_WAIT)));

	m_texList.ResetContent();
	for(int i = 0; i < texList->GetNumTexs(); i++) {
		Texture *texture = texList->GetTexNum(i);
		char *texName = texture->GetName();

		if(!strlen(texName))
			continue;

		int num = m_texList.AddString(texName);
		if(pGame->UsesWadFile())
			m_texList.SetItemData(num, texture->texEntry->wadEntry.offset);
		else
			m_texList.SetItemData(num, 0);

		int index = m_pickList.FindString(0, texName);
		if(index != LB_ERR)
			m_pickList.DeleteString(index);
	}

	SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));

	m_add.EnableWindow(false);
	m_remove.EnableWindow(false);

	m_removeWad.EnableWindow(true);
}

void QTexMgr::OnListSel() {
	char listName[MAX_PATH];
	m_listList.GetText(m_listList.GetCurSel(), listName);

	char fullName[MAX_PATH];
	sprintf(fullName, "%s\\texlists\\%s\\%s.tex",
		LFile::GetInitDir(), pGame->GetName(), listName);

	if(texList) {
		texList->Save();
		delete texList;
	}
	texList = new TexList(fullName, pGame);

	OnDirSel();

	m_removeList.EnableWindow(true);
}

void QTexMgr::OnPickSel() {
	char texName[64];
	m_pickList.GetText(m_pickList.GetCurSel(), texName);
	SetTexInfo(texName, m_pickList.GetItemData(m_pickList.GetCurSel()));
	m_texList.SetSel(-1, false);

	m_add.EnableWindow(true);
	m_remove.EnableWindow(false);
}

void QTexMgr::OnTexSel() {
	char texName[64];
	m_texList.GetText(m_texList.GetCurSel(), texName);
	SetTexInfo(texName, m_texList.GetItemData(m_texList.GetCurSel()));
	m_pickList.SetSel(-1, false);

	m_add.EnableWindow(false);
	m_remove.EnableWindow(true);
}

void QTexMgr::SetTexInfo(char *texName, int offset) {

	if(!texName) {
		m_texWnd.SetTexture(NULL);
		m_texInfo.SetWindowText("");
		return;
	}

	char filename[MAX_PATH];
	sprintf(filename, "%s\\%s%s", texDirName, texName, pGame->GetTexExt());

	TexEntry texEntry;
	strcpy(texEntry.name, texName);
	texEntry.texDir = m_dirList.GetCurSel();
	texEntry.wadEntry.offset = offset;
	strcpy(texEntry.wadEntry.name, texName);

	Texture texture(&texEntry, pGame);
	m_texWnd.SetTexture(&texture);

	char texInfo[64];
	sprintf(texInfo, "%s (%dx%d)", 
		texture.GetName(), texture.width, texture.height);
	m_texInfo.SetWindowText(texInfo);
}

void QTexMgr::OnTextureAdd() {
	int index;
	char texName[64];
	char addName[256];

	if(!texList)
		return;
 
	while(m_pickList.GetSelItems(1, &index) == 1) {
		m_pickList.GetText(index, texName);
		m_pickList.SetSel(index, FALSE);

		if(m_texList.FindString(0, texName) == LB_ERR) {
			strcpy(addName, texName);

			if(pGame->UsesWadFile()) {
				TexDir *texDir = pGame->GetTexDB()->GetTexDir(m_dirList.GetCurSel());
				if(texDir)
					sprintf(addName, "%s\\%s", texDir->dirName, texName);
			}
			
			Texture *texture = texList->Add(addName);

			int num = m_texList.AddString(texName);
			if(pGame->UsesWadFile())
				m_texList.SetItemData(num, texture->texEntry->wadEntry.offset);
			else
				m_texList.SetItemData(num, 0);

			int index = m_pickList.FindString(0, texName);
			if(index != LB_ERR)
				m_pickList.DeleteString(index);
		}
	}
}

void QTexMgr::OnTextureRemove() {
	int index;
	char texName[64];

	if(!texList)
		return;

	while(m_texList.GetSelItems(1, &index) == 1) {
		m_texList.GetText(index, texName);
		m_texList.DeleteString(index);

		m_pickList.AddString(texName);

		texList->Remove(texName);
	}
}

void QTexMgr::OnOK() {
	if(texList)
		texList->Save();
	CDialog::OnOK();
}

void QTexMgr::OnAddDir() {
	CFileDialog fileDialog(true, NULL, NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		pGame->UsesWadFile() ? "*.wad|*.wad||" : "*.pak|*.pak||");

	if(pGame->UsesWadFile()) {
		char *c, initDir[256];

		strcpy(initDir, pGame->GetTexDB()->GetTexDir(0)->dirName);
		c = strrchr(initDir, '\\');
		if(c)
			*c = '\0';

		fileDialog.m_ofn.lpstrInitialDir = initDir;
	}
	else
		fileDialog.m_ofn.lpstrInitialDir = pGame->GetGameDir();

	fileDialog.DoModal();

	if(pGame->UsesWadFile()) {
		pGame->GetTexDB()->AddTexDir(fileDialog.GetPathName().GetBuffer(0));
		BuildDirList();
	}
	else {
		LPak *pak = LFile::UsePak(fileDialog.GetPathName().GetBuffer(0));
		ExtractTexsFromPak(pak, pGame->GetBaseDir());
		OnDirSel();
	}
}

void QTexMgr::OnNewList() {
	if(!pGame)
		return;

	QTexMgrNewList newList;
	if(newList.DoModal() == IDCANCEL)
		return;

	char *listName = newList.m_newList.GetBuffer(0);
	char fullName[MAX_PATH];
	sprintf(fullName, "%s\\texlists\\%s\\%s.tex",
		LFile::GetInitDir(), pGame->GetName(), listName);

	if(LFile::Exist(fullName))
		return;

	m_listList.SetCurSel(m_listList.AddString(listName));
	OnListSel();
}

void QTexMgr::OnRemoveList() {
	if(!pGame)
		return;

	if(MessageBox("Are you sure?", NULL, MB_YESNO) == IDNO)
		return;

	char listName[64];
	m_listList.GetText(m_listList.GetCurSel(), listName);
	char fullName[MAX_PATH];
	sprintf(fullName, "%s\\texlists\\%s\\%s.tex",
		LFile::GetInitDir(), pGame->GetName(), listName);

	m_listList.DeleteString(m_listList.GetCurSel());
	OnListSel();

	CFile::Remove(fullName);

	m_removeList.EnableWindow(false);
}

void QTexMgr::OnRemoveDir() {
	if(!pGame)
		return;

	if(MessageBox("Are you sure?", NULL, MB_YESNO) == IDNO)
		return;

	TexDir *texDir = pGame->GetTexDB()->GetTexDir(m_dirList.GetCurSel());
	pGame->GetTexDB()->DelTexDir(texDir);

	BuildDirList();

	m_removeWad.EnableWindow(false);
}

void QTexMgr::BuildDirList() {
	if(!pGame)
		return;

	m_dirList.ResetContent();
	int dirs = pGame->GetTexDB()->GetNumTexDirs();
	for(int i = 0; i < dirs; i++) {
		TexDir *texDir = pGame->GetTexDB()->GetTexDir(i);
		char *c = strrchr(texDir->dirName, '\\');
		if(c)
			c++;
		else
			c = texDir->dirName;
		m_dirList.AddString(c);
	}
}


/////////////////////////////////////////////////////////////////////////////
// QTexMgrNewList dialog

QTexMgrNewList::QTexMgrNewList(CWnd* pParent /*=NULL*/)
	: CDialog(QTexMgrNewList::IDD, pParent)
{
	//{{AFX_DATA_INIT(QTexMgrNewList)
	m_newList = _T("");
	//}}AFX_DATA_INIT
}


void QTexMgrNewList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(QTexMgrNewList)
	DDX_Text(pDX, IDC_TEXMGR_NEWLIST, m_newList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(QTexMgrNewList, CDialog)
	//{{AFX_MSG_MAP(QTexMgrNewList)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

