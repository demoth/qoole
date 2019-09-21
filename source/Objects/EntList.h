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

#ifndef __ENTLIST_H
#define __ENTLIST_H

#include "list.h"
#include "geometry.h"

//==================== EntList ====================

#define LINE_MAX	160

#define VAR_UNDEF	0
#define VAR_VEC		1
#define VAR_INT		2
#define VAR_STR		3
#define VAR_NOTUSER	4
#define	VAR_FLOAT	5
#define VAR_TYPEDEF	6
#define VAR_FLAGDEF	7
#define VAR_USER	8
#define VAR_COLOR	9
#define VAR_STRDEF	10

class EntList;

class EntClass : public ElmNode {
public:
	EntClass(char *name);
	~EntClass(void);

	int count;
	char name[50];
	int nonmodels;
	int models;
};

struct QDefEntry {
	int val;
	char *str;
	char *name;
	char *desc;
};

class EntDef : public ElmNode {
public:
	EntDef(char hbuf[LINE_MAX], char lbuf[][LINE_MAX], int lnum, EntList *entList);
	~EntDef(void);

	char *GetName(void) { return name; }
	int GetDefType(void) { return deftype; }
	int GetNumEntries(void) { return entries; }
	QDefEntry *GetEntryNum(int i) { ASSERT(i >= 0 && i < entries); return &qdefentry[i]; }

private:
	char name[80];
	int deftype;
	int entries;
	QDefEntry *qdefentry;
};

struct QEntEntry {
	char *name;
	int vartype;
	char *desc;
	char *def;
	EntDef *entDef;
};

#define ENTFILE_GEM 1
#define ENTFILE_MDL 2
#define ENTFILE_MD2 3

class EntInfo : public ElmNode {
public:
	EntInfo(char hbuf[LINE_MAX], char lbuf[][LINE_MAX], int lnum, EntList *entList);
	~EntInfo(void);

	char *GetClassname(void) { return classname; }
	char *GetClassdesc(void) { return classdesc; }
	char *GetFilename(void) { return filename; }
	bool IsModel(void) { return isModel; }
	EntClass *GetEntClass(void) { return entClass; }
	WireFrameGeom *Use(bool &isModel, bool &isAdjust);
	void DoneUse(void);

	int GetNumEntries(void) { return entries; }
	QEntEntry *GetEntryNum(int i) { ASSERT(i >= 0 && i < entries); return &qentry[i]; }

	bool AddUserEntry(char *name);

private:
	char classname[80];
	char classdesc[50];
	EntClass *entClass;
	char desc[120];

	int filetype;
	char filename[80];

	int entries;
	QEntEntry *qentry;

	bool isModel;
	bool isAdjust;
	WireFrameGeom *wfGeomPtr;
};

class EntList : public ElmNode {
public:
	EntList(char *filename);
	~EntList(void);

	LinkList<EntInfo> &GetInfoList(void) { return infoList; }
	LinkList<EntDef> &GetDefList(void) { return defList; }
	LinkList<EntClass> &GetClassList(void) { return classList; }

	EntClass *FindClass(char *name);
	EntDef *FindDef(const char *name);
	EntInfo *FindEnt(const char *name);

	char *GetName(void) { return name; }
//	char *GetGameName(void) { return gameName; }

	static void Set(EntList *_entList) { entList = _entList; }
	static EntList *Get(void) { return entList; }
	static EntList *Find(char *name);
	static void Exit(void);

	static LinkList<EntList> &GetEntLists(void) { return entLists; }

	static WireFrameGeom *cross1Ptr;
	static WireFrameGeom *cross2Ptr;
	static WireFrameGeom *undefPtr;

private:
	void ReadEntFile(char *filename);

	char name[64];
	char gameName[32];

	LinkList<EntClass> classList;
	LinkList<EntDef> defList;
	LinkList<EntInfo> infoList;

	static EntList *entList;
	static LinkList<EntList> entLists;
};


#endif
