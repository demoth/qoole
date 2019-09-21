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

#include "stdafx.h"
#include <direct.h>
#include "Game.h"
#include "QDraw.h"

Game *Game::game = NULL;
LinkList<Game> Game::games;

Game::Game(int numUtils) {
	games.AppendNode(*this);
	initialized = false;
	exportWad = false;
	specifyWads = false;
	
	cfg = NULL;

	gameName[0] = '\0';
	gameDir[0] = '\0';
	mapDir[0] = '\0';
	texDir[0] = '\0';
	palName[0] = '\0';

	int i;
	for(i = 0; i < numUtils; i++) {
		utilsPath[i] = new char[256];
		utilsParams[i] = new char[256];
		*utilsPath[i] = *utilsParams[i] = '\0';
	}
	for(; i < MAXUTILS; i++) {
		utilsPath[i] = NULL;
		utilsParams[i] = NULL;
	}

	texDB = NULL;
}

Game::~Game(void) {
	for(int i = 0; i < MAXUTILS; i++) {
		delete utilsPath[i];
		delete utilsParams[i];
	}
	if(texDB)
		delete texDB;
}

void
Game::Exit(void) {
	while(games.NumOfElm() > 0)
		delete &(games.RemoveNode(0));
}

bool
Game::Set(Game *_game, char *palName, float gamma) {
	ASSERT(_game != NULL);
	if (!_game->Init())
		return false;

	game = _game;
	
	if(strlen(palName))
		strcpy(game->palName, palName);
	else		
		strcpy(game->palName, game->GetPalDef());

	game->pal.Load(game->palName, gamma);

	return true;
}

Game *
Game::Get(void) {
	return game;
}

Game *
Game::Find(char *name) {
	IterLinkList<Game> iter(games);

	iter.Reset();
	while(!iter.IsDone()) {
		Game *pGame = iter.GetNext();
		if(!stricmp(pGame->gameName, name))
			return pGame;
	}

	return NULL;
}

//===== Additional properties pages. =====
DWORD Game::GetExtraPropPages(void) const {
	// Default to add nothing.
	return 0;
}

//===== Compile map stuff. =====
void Game::InitCompileParams(BOOL runUtils[], CString params[],
							 int &paramIndex) const {
	int i, numUtils = GetNumUtils();

	for(i = 0; i < numUtils; i++) {
		runUtils[i] = ((runUtilsFlag & (0x01 << i)) != 0);
		params[i] = utilsParams[i];
	}

	paramIndex = setPIndex;
}

void Game::SaveCompileParams(BOOL runUtils[], CString params[],
							 int &paramIndex) {
	int i, numUtils = GetNumUtils();
	runUtilsFlag = 0x00;

	for(i = 0; i < numUtils; i++) {
		runUtilsFlag |= (runUtils[i] ? (0x01 << i) : 0);
		strcpy(utilsParams[i], (LPCTSTR) params[i]);
	}

	setPIndex = paramIndex;
}

const char *Game::GetMapExportPath(const char *docName,
								   OutTextProc pTextProc) const {
	static char mapPath[256];
	ASSERT(docName != NULL);

	// Make sure the dir exists.
	if (!LFile::ExistDir(mapDir)) {
		// Need to create the dir.
		if (pTextProc != NULL) {
			char textBuf[256];
			sprintf(textBuf, "Creating directory: %s\n", mapDir);
			(*pTextProc)(textBuf);
		}

		if (_mkdir(mapDir) != 0) {
			if (pTextProc != NULL)
				(*pTextProc)("Failed to create directory.\n");
			return NULL;
		}
	}

	// Clean up the map Dir.
	CleanUpCompile(docName, false, pTextProc);

	// Derive the export map file path.
	sprintf(mapPath, "%s\\%s.map", mapDir, docName);

	return mapPath;
}

bool Game::GetUtilCmdLine(int utilIndex, const char *docName,
						   const char **pCmdLine, const char **pInitDir,
						   OutTextProc pTextProc) const {
	ASSERT(docName != NULL);

	static char cmdLine[256];
	static char initDir[256];

	int numUtils = GetNumUtils();

	if (utilIndex < 0 || utilIndex >= numUtils)
		return false;

	sprintf(cmdLine, "%s %s", utilsPath[utilIndex], utilsParams[utilIndex]);

	// Variable substitution. %FILE%
	char *c;
	if ((c = strstr(cmdLine, "%file%")) != NULL ||
		(c = strstr(cmdLine, "%FILE%")) != NULL) {
		const char *c1;
		if ((c1 = strstr(utilsParams[utilIndex], "%file%")) == NULL)
			c1 = strstr(utilsParams[utilIndex], "%FILE%");
		c1 += 6;
		sprintf(c, "%s%s", docName, c1);
	}

	// InitDir
	if (utilIndex != numUtils - 1)
		strcpy(initDir, mapDir);
	else
		strcpy(initDir, gameDir);

	*pCmdLine = cmdLine;
	*pInitDir = initDir;

	return true;
}

void Game::CleanUpCompile(const char *docName, bool skipBSP, OutTextProc pTextProc) const {
	ASSERT(docName != NULL);

	if (!LFile::ExistDir(mapDir))
		return;

	// Clean up the map dir.
	char textBuf[256];
	char files[64];

	sprintf(files, "%s.*", docName);
	LFindFiles delFiles(mapDir, files);

	bool skipLine = false;
	const char *delFName, *fext;
	while ((delFName = delFiles.Next()) != NULL) {
		for(fext = delFName; *fext && *fext != '.'; fext++);
		if (stricmp(fext, ".bsp") == 0 && skipBSP)
			continue;  // Skip the .bsp file.
		if (stricmp(fext, ".map") == 0 && skipBSP)
			continue;  // Skip the .map file.
		if (stricmp(fext, ".qle") == 0)
			continue;  // Skip the .qle

		if (pTextProc != NULL) {
			sprintf(textBuf, "Deleting file: %s\n", delFName);
			(*pTextProc)(textBuf);
		}
		sprintf(textBuf, "%s\\%s", mapDir, delFName);
		_unlink(textBuf);
		skipLine = true;
	}

	if (skipLine && pTextProc != NULL)
		(*pTextProc)("\n");
}

Texture *Game::GetDefaultTexture(void) {
	return texDB->FindTexture(defTexture);
}

void Game::ExtractTextures(void) {
}


void ExtractTexsFromPak(LPak *pak, char *baseDir) {
	char outname[MAX_PATH];
	char dirname[MAX_PATH];
	char *name, *c;
	int i, count = 0, total = 0;

	for(i = 0; i < pak->entries; i++)
		if(strstr(pak->entry[i].filename, ".wal"))
			total++;

	CProgressWnd progressWnd(AfxGetMainWnd(), "Progress");
	progressWnd.SetText("Extracting Textures");
	QDraw::OutputText("Extracting textures from Pak... ");

	SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_WAIT)));

	for(i = 0; i < pak->entries; i++) {
		name = pak->entry[i].filename;
		if(strstr(name, ".wal")) {
			sprintf(outname, "%s\\%s", baseDir, name);
			strcpy(dirname, outname);
			c = strrchr(dirname, '/');
			if(!c)
				c = strrchr(dirname, '\\');
			if(c)
				*c = NULL;

			LFile::MakeDir(dirname);
			LFile::Extract(name, outname);

			progressWnd.SetPos(count++ * 100 / total);
			progressWnd.PeekAndPump();

			if(progressWnd.Cancelled())
			{
				QDraw::OutputText("Cancelled.\n");
				break;
			}
		}
	}

	QDraw::OutputText("OK.\n");
	progressWnd.Clear();
	SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
}

