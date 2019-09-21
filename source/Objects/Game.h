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


#ifndef __GAME_H
#define __GAME_H

#include "Geometry.h"
#include "Objects.h"

class Texture;
class WireFrameGeom;
class TexDB;

typedef void (*OutTextProc)(const char *strText);

// MAXUTILS can't > 32
#define MAXUTILS		5

class Game : public ElmNode {
public:
	Game(int numUtils);
	virtual ~Game(void);

	char *GetName(void) { return gameName; }
	char *GetGameDir(void) { return gameDir; }
	char *GetBaseDir(void) { return baseDir; }
	char *GetTexDir(void) { return texDir; }
	char *GetTexExt(void) { return texExt; }
	TexDB *GetTexDB(void) { ASSERT(texDB != NULL); return texDB; }

	char *GetPalDef(void) { return palDef; }

	bool IsExportWadSet(void) { return exportWad; }
	bool IsSpecifyWadsSet(void) { return specifyWads; }

	bool UsesWadFile(void) { return strlen(texExt) ? false : true; }

	Texture *GetDefaultTexture(void);

	virtual bool Init(void) = 0;
	virtual WireFrameGeom *LoadModel(char *filename) = 0;
	virtual bool LoadTexture(Texture *texture, char *filename, int offset) = 0;

	virtual void ExtractTextures(void);

	static LinkList<Game> &GetGames(void) { return games; }

	static Game *Get(void);
	static bool Set(Game *_game, char *palName, float gamma);
	static Game *Find(char *name);
	static void Exit(void);

	char *GetPalName(void) { return palName; }
	LPalette &GetPal(void) { return pal; }

	// Additional properties pages.
	virtual DWORD GetExtraPropPages(void) const;

	// Game Map Compile Stuff.
	virtual int GetNumUtils(void) const = 0;
	virtual const char *GetUtilName(int index) const = 0;

	virtual void InitCompileParams(BOOL runUtils[], CString params[],
								   int &paramIndex) const;
	virtual void SaveCompileParams(BOOL runUtils[], CString params[],
								   int &paramIndex);

	virtual const char *GetMapExportPath(const char *docName,
										 OutTextProc pTextProc = NULL) const;

	virtual bool GetUtilCmdLine(int utilIndex, const char *docName,
								const char **pCmdLine, const char **pInitDir,
								OutTextProc pTextProc = NULL) const;

	virtual const char *GetLeakFile(const char *docName) const { return NULL; }
	virtual Object *LoadLeakFile(const char *docName) const { return NULL; }

	virtual void CleanUpCompile(const char *docName, bool skipBSP = true,
								OutTextProc pTextProc = NULL) const;

	char *utilsPath[MAXUTILS];

protected:
	LConfig *cfg;
	char gameName[16];
	char palName[32];
	char palDef[32];
	char gameDir[256];
	char mapDir[256];
	char baseDir[256];
	char texDir[256];
	char texExt[8];

	bool initialized;
	bool exportWad;
	bool specifyWads;

	int  setPIndex;
	UINT runUtilsFlag;
	char *utilsParams[MAXUTILS];

	char defTexture[32];

	TexDB *texDB;

	LPalette pal;

private:
	static Game *game;
	static LinkList<Game> games;
};

typedef struct {
	float x, y, z;
} vec3_t;

typedef struct {
	vec3_t vert[2];
} edge_t;

typedef struct {
	int v1, v2;
} edgenum_t;

void ExtractTexsFromPak(LPak *pak, char *baseDir);

#endif
