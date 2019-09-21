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
#include "Texture.h"
#include "Game.h"
#include "QDraw.h"

// ===========================================================================
// Texture

Texture::Texture(TexEntry *entry, Game *_game) {
	cached = false;
	pInfo = NULL;
	tInfo = 0;
	texEntry = entry;
	game = _game;
}

Texture::~Texture(void) {
	if(cached)
		for(int i = 0; i < mips; i++)
			free(mip[i]);

	if(pInfo)
		free(pInfo);
}

char *
Texture::GetFilename(void) {
	static char filename[MAX_PATH];

	TexDir *texDir = game->GetTexDB()->GetTexDir(texEntry->texDir);
	strcpy(filename, texDir->dirName);

	if(!texEntry->wadEntry.offset) {
		// we're a .wal dir then, so append name
		strcat(filename, "\\");
		strcat(filename, texEntry->name);
		strcat(filename, texDir->game->GetTexExt());
	}

	return filename;
}

bool
Texture::Cache(void) {
	if(cached)
		return true;

	if(texEntry->wadEntry.offset == -1) {
//		ASSERT(false);
		return false;
	}

	if(!game->LoadTexture(this, GetFilename(), texEntry->wadEntry.offset)) {
//		ASSERT(false);
		return false;
	}

	cached = true;

	// =======================================================
	// force the width and height to be a power of 2
	// (all the QDraw render libraries require this)
	// texture surface is stretched appropiately here
	// divWidth, divHeight keep track of stretch ratio
	// width, height keep track of memory dimensions
	// realWidth, realHeight keep track of real dimensions
	width = NearestGreaterPower2(realWidth);
	height = NearestGreaterPower2(realHeight);

	divWidth = (float)realWidth / (float)width;
	divHeight = (float)realHeight / (float)height;

	if(width != realWidth || height != realHeight) {
		int m, w, h, rw, rh, x, y;
		void *oldMip, *newMip;

		// we have to stretch all mips
		for(m = 0; m < mips; m++) {
			w = width / (1 << m);
			h = height / (1 << m);
			rw = realWidth / (1 << m);
			rh = realHeight / (1 << m);

			oldMip = mip[m];
			newMip = malloc(w * h * bits / 8);

			if(bits == 8) {
				unsigned char *oldPtr;
				unsigned char *newPtr = (unsigned char *)newMip;
				for(y = 0; y < h; y++) {
					oldPtr = (unsigned char *)oldMip + (int)((float)y * divHeight) * rw;
					for(x = 0; x < w; x++)
						*newPtr++ = *(oldPtr + (int)((float)x * divWidth));
				}
			}
			else if(bits == 16) {
				unsigned short *oldPtr;
				unsigned short *newPtr = (unsigned short *)newMip;
				for(y = 0; y < h; y++) {
					oldPtr = (unsigned short *)oldMip + (int)((float)y * divHeight) * rw;
					for(x = 0; x < w; x++)
						*newPtr++ = *(oldPtr + (int)((float)x * divWidth));
				}
			}
			
			free(mip[m]);
			mip[m] = newMip;
		}

		surface = mip[0];
	}
	// =======================================================

	return true;
}

char *
Texture::GetShortName(void) {
	static char shortName[32];

	char *c = strrchr(texEntry->name, '/');
	if(c)
		strcpy(shortName, c + 1);
	else
		strcpy(shortName, texEntry->name);

	c = strrchr(shortName, '.');
	if(c)
		*c = '\0';

	strlwr(shortName);

	return shortName;
}

char *
Texture::GetTexListName(TexDB *texDB) {
	static char name[256];

	strcpy(name, "");

	TexDir *texDir = texDB->GetTexDir(texEntry->texDir);

	if(texDir->game->UsesWadFile()) {
		char *c = strrchr(texDir->dirName, '\\');
		if(c)
			strcpy(name, c + 1);
		else
			strcpy(name, texDir->dirName);
		strcat(name, "\\");
	}

	strcat(name, texEntry->name);

	return name;
}

short *
Texture::GetSolidInfo(void) {
	if(texEntry->solid[7])
		return texEntry->solid;

	if(!Cache()) {
		static short tmp[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
		return tmp;
	}

	// build solid info
	if(bits == 8) {
		int i, j, table[256];
		int best, high;
		unsigned char *surf = (unsigned char *)surface;
	
		for(i = 0; i < 256; i++)
			table[i] = 0;

		for(i = 0; i < width * height; i++)
			table[*surf++]++;

		for(j = 0; j < 7; j++) {
			high = 0;
			for(i = 1; i < 256; i++)
				if(table[i] > high) {
					best = i;
					high = table[i];
				}
				texEntry->solid[j] = best;
				table[best] = 0;
		}
	}
	else {
		int i, j, table[65536];
		int best, high;
		unsigned short *surf = (unsigned short *)surface;
	
		for(i = 0; i < 65536; i++)
			table[i] = 0;

		for(i = 0; i < width * height; i++)
			table[*surf++]++;

		for(j = 0; j < 7; j++) {
			high = 0;
			for(i = 1; i < 65536; i++)
				if(table[i] > high) {
					best = i;
					high = table[i];
				}
				texEntry->solid[j] = best;
				table[best] = 0;
		}
	}

	texEntry->solid[7] = 1;

	return texEntry->solid;
}


// ===========================================================================
// TexList

TexList::TexList(char *_filename, Game *game) {
	strcpy(filename, _filename);
	texDB = game->GetTexDB();

	LFile file;
	if(!file.Open(filename))
		return;

	while(file.GetNextLine())
		Add(file.GetLine());
}

TexList::~TexList() {
}

Texture *TexList::Add(char *texName) {
	Texture *texture = texDB->AddTexture(texName);
	textures.Add(texture);
	return texture;
}

void TexList::Remove(char *texName) {
	for(int i = 0; i < textures.GetSize(); i++) {
		Texture *texture = textures.GetAt(i);
		if(!strcmp(texName, texture->GetName())) {
			textures.RemoveAt(i);
			return;
		}
	}
}

void TexList::Save(void) {
	if(!textures.GetSize())
		return;

	LFile file;
	char buf[256];

	QDraw::OutputText("Loading TexList: %s... ", filename);
	TRY {
		if(LFile::Exist(filename))
		{
			CFileStatus status;
			CFile::GetStatus(filename, status);
			status.m_attribute &= ~0x01;
			CFile::SetStatus(filename, status);
			CFile::Remove(filename);
		}
		else
			QDraw::OutputText("Error. Couldn't open TexList.\n");
	}
	CATCH(CFileException, e)
	{
		char buf[256];
		QDraw::OutputText("Error. Couldn't save TexList.\n");
		sprintf(buf, "Error saving TexList '%s'", filename);
		MessageBox(NULL, buf, NULL, MB_ICONERROR | MB_TOPMOST);
		return;
	}
	END_CATCH

	file.Open(filename, LFILE_WRITE);
	for(int i = 0; i < textures.GetSize(); i++) {
		Texture *texture = textures.GetAt(i);
		sprintf(buf, "%s\r\n", texture->GetTexListName(texDB));
		file.Write(buf, strlen(buf));
	}
	QDraw::OutputText("OK.\n");
}


// ===========================================================================
// TexDB

TexDB::TexDB(Game *_game) {
	game = _game;

	int i;
	int ver;

	sprintf(filename, "%s\\texlists\\%s.db",
		LFile::GetInitDir(), game->GetName());

	texEntries = 0;
	texAlloc = 0;
	texDirs = 0;
	texEntry = NULL;
	texture = NULL;
	tryExtract = true;

	LFile file;
	QDraw::OutputText("Loading textures database: %s ... ", filename);
	if(!file.Open(filename))
	{
		QDraw::OutputText("Error. Couldn't open the database.\n");
		return;
	}

	file.Read(&ver, sizeof(ver));
	if(ver != TEXDB_VER)
	{
		QDraw::OutputText("Error.\nDB version is %s, should be %s.\n", ver, TEXDB_VER);
		return;
	}

	file.Read(&texEntries, sizeof(texEntries));
	texAlloc = texEntries / TEXDB_ALLOC + 1;
	texAlloc *= TEXDB_ALLOC;
	texEntry = (TexEntry *)malloc(sizeof(TexEntry) * texAlloc);
	texture = (Texture **)malloc(sizeof(Texture *) * texAlloc);

	file.Read(texEntry, sizeof(TexEntry), texEntries);
	file.Read(&texDirs, sizeof(texDirs));
	texDirs = Min(texDirs, TEXDIR_MAX);
	file.Read(texDir, sizeof(TexDir), texDirs);

	for(i = 0; i < texDirs; i++)
		texDir[i].game = game;

	for(i = 0; i < texEntries; i++)
		texture[i] = new Texture(&texEntry[i], game);
	QDraw::OutputText("OK.\n");
}

TexDB::~TexDB() {
	LFile file;
	if(!file.Open(filename, LFILE_WRITE))
		return;

	int ver = TEXDB_VER;

	file.Write(&ver, sizeof(ver));
	file.Write(&texEntries, sizeof(texEntries));
	file.Write(texEntry, sizeof(TexEntry), texEntries);

	file.Write(&texDirs, sizeof(texDirs));
	file.Write(texDir, sizeof(TexDir), texDirs);

	for(int i = 0; i < texEntries; i++)
		delete texture[i];

	free(texEntry);
	free(texture);
}

Texture *
TexDB::AddTexture(char *name) {
	char fullName[256];
	char texName[32];
	char dirName[128];
	char *c;

	strcpy(fullName, name);
	c = strchr(fullName, 13);
	if(c)
		*c = '\0';

	strcpy(dirName, fullName);
	c = strrchr(dirName, '\\');
	if(c) {
		*c = '\0';
		strcpy(texName, c + 1);
	}
	else {
		strcpy(texName, fullName);
		strcpy(dirName, "");
	}

	// strip .wal from texName
	if(!game->UsesWadFile()) {
		c = strstr(texName, game->GetTexExt());
		if(c)
			*c = '\0';
	}

	Texture *tex = FindTexture(texName, false);
	if(tex)
		return tex;

	int texDir = FindTexDir(dirName);
	WadEntry wadEntry;

	if(texDir == -1) {
//		if(strlen(dirName) && (LFile::Exist(dirName) || LFile::ExistDir(dirName)))
		if(strlen(dirName) && (strchr(dirName, '\\') || strchr(dirName, '/')))
			texDir = AddTexDir(dirName);
		else {
			char oldName[128];
			strcpy(oldName, dirName);
			strcpy(dirName, game->GetTexDir());
			if(strlen(oldName))
				strcat(dirName, "\\");
			strcat(dirName, oldName);

			if(LFile::Exist(dirName) || LFile::ExistDir(dirName))
				texDir = AddTexDir(dirName);
		}
	}

	int result = -1;
	char walName[32];
	if(texDir != -1) {
		result = GetWadEntry(dirName, texName, &wadEntry);
		if(result == -1) {
			sprintf(walName, "%s%s", texName, game->GetTexExt());
			result = GetWadEntry(dirName, walName, &wadEntry);
		}
	}

	/*
	if(result == -1 && tryExtract) {
		game->ExtractTextures();
		result = GetWadEntry(dirName, texName, &wadEntry);
		if(result == -1)
			result = GetWadEntry(dirName, walName, &wadEntry);
	}
	*/

	if(result == -1) {
		TRACE("TexDB::AddTexture: couldn't find texture '%s'\n", fullName);
		static TexEntry notFoundEntry;
		static Texture notFoundTex(&notFoundEntry, game);
		strcpy(notFoundEntry.wadEntry.name, "NotFound");
		notFoundEntry.wadEntry.offset = -1;
		return &notFoundTex;
	}

	if(texEntries == texAlloc) {
		texAlloc += TEXDB_ALLOC;
		texEntry = (TexEntry *)realloc(texEntry, sizeof(TexEntry) * texAlloc);
		texture = (Texture **)realloc(texture, sizeof(Texture *) * texAlloc);

		for(int i = 0; i < texEntries; i++)
			texture[i]->texEntry = &texEntry[i];
	}

	TexEntry *entry = &texEntry[texEntries];

	if(result > 0)
		strcpy(entry->name, wadEntry.name);
	else {
		if(strlen(texName) < 16)
			strcpy(wadEntry.name, texName);
		else {
			strncpy(wadEntry.name, texName, 15);
			wadEntry.name[15] = '\0';
		}
		strcpy(entry->name, texName);
	}
	entry->solid[7] = 0;
	entry->wadEntry = wadEntry;
	entry->texDir = texDir;

	tex = new Texture(entry, game);
	texture[texEntries] = tex;
	texEntries++;

	return tex;
}

Texture *
TexDB::FindTexture(char *name, bool iterate) {
	static bool adding = false;
	if(adding)
		return NULL;

	if(!name)
		return NULL;

	if(!strlen(name))
		return NULL;

	int i;

	for(i = 0; i < texEntries; i++)
		if(game == GetTexDir(texEntry[i].texDir)->game &&
			!strcmpi(name, texEntry[i].name))
			return texture[i];

	if(!iterate)
		return NULL;

	// iterate through texdirs
	for(i = 0; i < texDirs; i++) {
		char checkName[128];
		sprintf(checkName, "%s\\%s", texDir[i].dirName, name);

		adding = true;
		Texture *texture = AddTexture(checkName);
		adding = false;

		if(texture && texture->texEntry->wadEntry.offset != -1)
			return texture;
	}

	return NULL;
}

int
TexDB::AddTexDir(char *name) {
	int val = FindTexDir(name);
	if(val != -1)
		return val;

	strcpy(texDir[texDirs].dirName, name);
	strcpy(texDir[texDirs].gameName, game->GetName());
	texDir[texDirs].game = game;

	return texDirs++;
}

int
TexDB::FindTexDir(char *name) {
	for(int i = 0; i < texDirs; i++)
		if(!strcmpi(name, texDir[i].dirName))
			return i;

	return -1;
}

void
TexDB::DelTexDir(TexDir *texDir) {
	bool found = false;
	for(int i = 0; i < texDirs; i++) {
		if(texDir == &texDir[i])
			found = true;
		if(found && i + 1 < texDirs)
			texDir[i] = texDir[i + 1];
	}
	texDirs--;
}

int
TexDB::GetWadEntry(char *dirName, char *texName, WadEntry *rtnWadEntry) {
	if(LFile::ExistDir(dirName)) {
		char filename[MAX_PATH];
		sprintf(filename, "%s\\%s", dirName, texName);
		if(LFile::Exist(filename)) {
			rtnWadEntry->offset = 0;
			return 0;
		}
		else
			return -1;
	}
	else {
		WadHeader wadHeader;
		WadEntry *wadEntry;

		LFile file;
		if(!file.Open(dirName))
			return -1;

		file.Read(&wadHeader, sizeof(WadHeader));
		if(strncmp(wadHeader.magic, "WAD2", 4) &&
			strncmp(wadHeader.magic, "WAD3", 4))
			return -1;
		
		wadEntry = new WadEntry[wadHeader.entries];
		file.Seek(wadHeader.offset);
		file.Read(wadEntry, sizeof(WadEntry), wadHeader.entries);

		int rtn = -1;
		for(int i = 0; i < wadHeader.entries; i++)
			if(!strcmpi(texName, wadEntry[i].name)) {
				*rtnWadEntry = wadEntry[i];
				rtn = wadEntry[i].offset;
				break;
			}

		delete [] wadEntry;

		return rtn;
	}
}

void
TexDB::ResetUsed(void) {
	for(int i = 0; i < texEntries; i++)
		texEntry[i].used = false;
}

void
TexDB::BuildWadFromUsed(char *filename) {
	int i, c;

	WadHeader wadHeader;
	wadHeader.entries = 0;

	for(i = 0; i < texEntries; i++)
		if(texEntry[i].used)
			wadHeader.entries++;

	CProgressWnd progressWnd(AfxGetMainWnd(), "Progress");
	progressWnd.NoCancelButton();

	char building[256];
	const char *c1 = strrchr(filename, '\\');
	sprintf(building, "Building %s", (c1 != NULL ? c1 + 1 : filename));
	progressWnd.SetText(building);

	WadEntry *wadEntry = new WadEntry[wadHeader.entries];

	// hack!!
	if(!strcmp(game->GetName(), "Half-Life"))
		strncpy(wadHeader.magic, "WAD3", 4);
	else
		strncpy(wadHeader.magic, "WAD2", 4);

	wadHeader.offset = sizeof(WadHeader);

	LFile outFile;
	outFile.Open(filename, LFILE_WRITE);
	outFile.Seek(wadHeader.offset);

	char qwad[4];
	strncpy(qwad, "QWAD", 4);
	outFile.Write(qwad, 4);
	wadHeader.offset += 4;

	c = 0;
	for(i = 0; i < texEntries; i++) {
		if(!texEntry[i].used)
			continue;

		int size = texEntry[i].wadEntry.dsize;

		LFile inFile;
		inFile.Open(texture[i]->GetFilename());
		inFile.Seek(texEntry[i].wadEntry.offset);

		char *buf = (char *)malloc(size);
		inFile.Read(buf, size);
		outFile.Write(buf, size);
		free(buf);

		wadEntry[c] = texEntry[i].wadEntry;
		wadEntry[c].offset = wadHeader.offset;
		wadHeader.offset += size;

		progressWnd.SetPos(c * 100 / wadHeader.entries);
		c++;
	}

	outFile.Write(wadEntry, sizeof(WadEntry), wadHeader.entries);
	outFile.Seek(0);
	outFile.Write(&wadHeader, sizeof(WadHeader));
}

char *
TexDB::BuildWadListFromUsed(void) {
	static char list[1024];

	bool dirUsed[TEXDIR_MAX];
	int i;

	for(i = 0; i < texDirs; i++)
		dirUsed[i] = false;

	for(i = 0; i < texEntries; i++)
		if(texEntry[i].used)
			dirUsed[texEntry[i].texDir] = true;

	strcpy(list, "");

	for(i = 0; i < texDirs; i++) {
		if(dirUsed[i]) {
			if(strlen(list))
				strcat(list, ";");
			strcat(list, texDir[i].dirName);
		}
	}

	return list;
}	
