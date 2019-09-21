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

#ifndef __TEXTURE_H
#define __TEXTURE_H

#include <afxtempl.h>
#include "list.h"
#include "LCommon.h"

#define TEXDB_VER 27
#define TEXDB_ALLOC 500
#define TEXDIR_MAX 128

class Game;
class TexDB;

struct WadHeader {
	char magic[4];
	long entries;
	long offset;
};

struct WadEntry {
	long offset;
	long dsize;
	long size;
	char type;
	char cmprs;
	short dummy;
	char name[16];
};

struct TexDir {
	char dirName[128];
	char gameName[16];
	Game *game;
};

struct TexEntry {
	WadEntry wadEntry;
	char name[24];
	short solid[8];
	short texDir;
	short used;
};

class Texture {
public:
	Texture(TexEntry *texEntry, Game *game);
	~Texture();

	char *GetFilename(void);

	bool Cache(void);
	char *GetName(void) { return texEntry->name; }
	char *GetShortName(void);
	char *GetTexListName(TexDB *texDB);
	short *GetSolidInfo(void);

	bool cached;

	int width, height, bits;
	int realWidth, realHeight;
	float divWidth, divHeight;
	void *surface;

	int mips;
	void *mip[4];

	void *pInfo;
	unsigned int tInfo;

	Game *game;
	TexEntry *texEntry;
};

class TexDB {
public:
	TexDB(Game *Game);
	~TexDB();

	Texture *AddTexture(char *fullName);
	Texture *FindTexture(char *name, bool iterate = true);

	int AddTexDir(char *name);
	void DelTexDir(TexDir *texDir);
	TexDir *GetTexDir(int dirNum) { return &texDir[dirNum]; }
	int GetNumTexDirs(void) { return texDirs; }
	int FindTexDir(char *name);

	void ResetUsed(void);
	void BuildWadFromUsed(char *filename);
	char *BuildWadListFromUsed(void);

	void SetTryExtract(bool set) { tryExtract = set; }

private:
	Game *game;

	int GetWadEntry(char *dirName, char *texName, WadEntry *wadEntry);

	char filename[MAX_PATH];
	int texEntries;
	int texAlloc;
	TexEntry *texEntry;
	Texture **texture;
	int texDirs;
	TexDir texDir[TEXDIR_MAX];

	bool tryExtract;
};

class TexList {
public:
	TexList(char *filename, Game *game);
	~TexList();

	Texture *Add(char *texName);
	void Remove(char *texName);
	void Save(void);

	int GetNumTexs(void) { return textures.GetSize(); }
	Texture *GetTexNum(int texNum) { return textures.GetAt(texNum); }

	char filename[256];
	TexDB *texDB;
	CArray <Texture *, Texture *> textures;
};

#endif
