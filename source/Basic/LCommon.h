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

#ifndef __LCOMMON_H
#define __LCOMMON_H

#ifndef ASSERT
#include <assert.h>
#define ASSERT assert
#endif

#include "list.h"

// ========== LCommon ==========

void LError(const char *fmt, ...);
void LFatal(const char *fmt = 0, ...);


// ========== LFile ==========

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <io.h>

#define LFILE_READ 1
#define LFILE_WRITE 2
#define LFILE_APPEND 4
#define LFILE_FROMPAK 8

class LPak;

class LFile {
public:
	LFile(void);
	~LFile(void);

	static bool Exist(const char *filename);
	static bool ExistDir(const char *dirname);

	bool Open(const char *filename, int mode = LFILE_READ);
	void Close(void);

	int Size(void);
	int Tell(void);
	int Seek(int offset);
	int SeekCur(int offset);
	int Read(void *buf, int size, int count = 1);
	int Write(void *buf, int size, int count = 1);

	bool EndOfFile(void);
	char *GetLine(void);
	char *GetNextLine(void);
	void ResetLineNumber(void) { lineNum = 0; }
	int GetLineNumber(void);
	char *Search(const char *pattern);

	static const char *GetModifyTime(const char *fileName);

	static bool Extract(char *filename, char *dst);
	static void MakeDir(char *dirname);

	static void UseDir(const char *dirname);
	static LPak *UsePak(const char *filename);

	static const char *GetInitDir(void) { return initDir; }

	static void Init(void);
	static void Exit(void);

private:
	FILE *file;
	int fileSize;

	LPak *pak;
	int fileOffset;
	int pakOffset;

	int lineNum;
	char data[1024];

	static char *dirs[32];
	static int numdirs;
	static LPak *paks[32];
	static int numpaks;

	static char initDir[256];
};


// ========== LPak ==========

struct pak_header_t {
	char magic[4];
	int offset;
	int size;
};

struct pak_entry_t {
	char filename[56];
	int offset;
	int size;
};

class LPak : public LFile {
public:
	LPak(const char *filename);
	~LPak(void);

	char filename[256];
	pak_header_t header;
	pak_entry_t *entry;
	int entries;
};


// ========== LFindFiles ==========

class LFindFiles {
public:
	LFindFiles(const char *rootdir, const char *filemask = "*.*");
	char *Next(void);
private:
	char name[256];
	struct _finddata_t fileinfo;
	int rc;
};


// ========== LFindDirs ==========

class LFindDirs {
public:
	LFindDirs(const char *dirname);
	char *Next(void);
private:
	char name[256];
	struct _finddata_t fileinfo;
	int rc;
};


// ========== LConfig ==========

#define LVAR_STR 1
#define LVAR_INT 2
#define LVAR_FLOAT 3

struct LVar {
	char name[32];
	void *ptr;
	int type;
};

class LConfig {
public:
	LConfig(char *name);
	~LConfig(void);

	void RegisterVar(char *name, void *ptr, int type);
	void SaveVars(void);

	static void Init(int curVer, int reqVer);
	static void Exit(void);

private:
	char sectionName[32];
	int numLVars;
	LVar *lvars[256];

	static int currentVer, requiredVer, registryVer;
};


// ========== LPalette ==========

#ifndef PALETTE_T
#define PALETTE_T
typedef struct {
    unsigned char   blue;
    unsigned char   green;
    unsigned char   red;
    unsigned char   alpha;
	} palette_t;
#pragma pack()
#endif

class LPalette {
public:
	LPalette();
	~LPalette();

	void Load(char *filename, float gamma = 1.0f);
	static void GammaCorrect(int &r, int &g, int &b, float gamma);

	palette_t pal[256];
};


// ========== LLibrary ==========

class LLibrary : public ElmNode {
public:
	LLibrary(char *libName);
	~LLibrary();

	static HINSTANCE FindLibrary(char *libName);

private:
	char name[64];
	HINSTANCE hInst;

	static LinkList<LLibrary> libs;
};


// ========== Time Code ==========

void StartTime(void);
int GetTime(void);


// ========== Print Formatting ==========

extern const char *FormatFloat(float number);

// ========== Conversion stuff ==========
extern bool Str2Int(const char *str, UINT &val);


// ========== Old VOS Stuff ==========

typedef char substr_t[160];
int String_Split(substr_t substr[], char *org_string, int max);
void String_Crop(char *str);
void String_Chop(char *str);
void String_CutAt(char *str, char *at);
int String_Count(char *str, char ch);
int strstri(char *str1, char *str2);

typedef struct sort_t {
	sort_t *next;
	int key;
} sort_t;

sort_t *lsort(sort_t *p);


struct lpal_t {
	unsigned char r, g, b;
};

void SavePCX(char *name, unsigned char *surface, int width, int height, lpal_t *pal);

int NearestGreaterPower2(int x);


#endif // __LCOMMON_H

