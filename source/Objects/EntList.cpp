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
#include "LCommon.h"
#include "EntList.h"

//==================== EntList ====================

EntList *EntList::entList = NULL;
LinkList<EntList> EntList::entLists;

WireFrameGeom *EntList::cross1Ptr = NULL;
WireFrameGeom *EntList::cross2Ptr = NULL;
WireFrameGeom *EntList::undefPtr = NULL;

EntList::EntList(char *filename) {
	if(!EntList::FindClass("Null"))
		classList.AppendNode(*(new EntClass("Null")));

	strcpy(name, filename);
	strcpy(gameName, "");

	ReadEntFile(filename);

	if(!cross1Ptr) {
		cross1Ptr = new WireFrameGeom("gems/cross1.gem");
		cross2Ptr = new WireFrameGeom("gems/cross2.gem");
		undefPtr = new WireFrameGeom("gems/undef.gem");
	}

	entLists.AppendNode(*this);
}

EntList::~EntList(void) {
	while(infoList.NumOfElm() > 0)
		delete &(infoList.RemoveNode(0));
	while(defList.NumOfElm() > 0)
		delete &(defList.RemoveNode(0));
	while(classList.NumOfElm() > 0)
		delete &(classList.RemoveNode(0));
}

void
EntList::Exit(void) {
	while(entLists.NumOfElm() > 0)
		delete &(entLists.RemoveNode(0));

	delete cross1Ptr;
	delete cross2Ptr;
	delete undefPtr;
}

EntList *
EntList::Find(char *name) {
	IterLinkList<EntList> iter(entLists);

	iter.Reset();
	while(!iter.IsDone()) {
		EntList *entList = iter.GetNext();
		if(!stricmp(entList->name, name))
			return entList;
	}

	return new EntList(name);
}

void
EntList::ReadEntFile(char *filename) {
	char hbuf[LINE_MAX];
	char lbuf[1024][LINE_MAX];
	char *buf;
	char *c;
	bool inside = false;
	int l = 0;

	LFile file;
	if(!file.Open(filename)) {
		// try appending "ents/" to beginning
		char entFilename[MAX_PATH];
		sprintf(entFilename, "ents/%s", filename);
		if(!file.Open(entFilename))
			return;
	}

	while(buf = file.GetNextLine()) {
		if(!strlen(buf))
			continue;
		if(buf[0] == '/' && buf[1] == '/')
			continue;
		if(buf[0] == '#')
			continue;

		if(!strnicmp(buf, "include", 7)) {
			char include[LINE_MAX];
			c = strchr(buf, '\"');
			if(c) {
				strcpy(include, c + 1);
				c = strchr(include, '\"');
				if(c)
					*c = 0;
				ReadEntFile(include);
			}
		}
		
		if(!strnicmp(buf, "class", 5)) {
			substr_t sbuf[4];
			String_Split(sbuf, buf, 3);
			classList.AppendNode(*(new EntClass(sbuf[1])));
			continue;
		}

		if(!strnicmp(buf, "game", 4)) {
			substr_t sbuf[4];
			String_Split(sbuf, buf, 3);
			strcpy(gameName, sbuf[1]);
			continue;
		}

		c = strstr(buf, "{");
		if(c) {
			*c = ' ';
			inside = true;
		}

		c = strstr(buf, "}");		
		if(c) {
			if(!strnicmp(hbuf, "entity", 6))
				infoList.AppendNode(*(new EntInfo(hbuf, lbuf, l, this)));
			else if(!strnicmp(hbuf, "typedef", 7) || !strnicmp(hbuf, "flagdef", 7) || !strnicmp(hbuf, "strdef", 6))
				defList.AppendNode(*(new EntDef(hbuf, lbuf, l, this)));

			*c = ' ';
			inside = false;
			l = 0;
		}

		String_Crop(buf);
		
		if(strlen(buf)) {
			if(inside)
				strcpy(lbuf[l++], buf);
			else
				strcpy(hbuf, buf);
		}

	}

	file.Close();
}

EntInfo *
EntList::FindEnt(const char *name) {
	IterLinkList<EntInfo> iter(GetInfoList());
	EntInfo *info;

	iter.Reset();
	while(!iter.IsDone()) {
		info = iter.GetNext();
		if(!strcmpi(name, info->GetClassname()))
			return info;
	}

	return NULL;
}

EntDef *
EntList::FindDef(const char *name) {
	IterLinkList<EntDef> iter(GetDefList());
	EntDef *def;
				
	iter.Reset();
	while(!iter.IsDone()) {
		def = iter.GetNext();
		if(!strcmpi(name, def->GetName()))
			return def;
	}

	return NULL;
}

EntClass *
EntList::FindClass(char *name) {
	IterLinkList<EntClass> iter(GetClassList());
	EntClass *entClass;

	iter.Reset();
	while(!iter.IsDone()) {
		entClass = iter.GetNext();
		if(!strcmpi(name, entClass->name))
			return entClass;
	}

	return NULL;
}


//==================== EntClass ====================

EntClass::EntClass(char *cname) {
	strcpy(name, cname);
	models = 0;
	nonmodels = 0;
}

EntClass::~EntClass(void) {
}


//==================== EntInfo ====================

EntInfo::EntInfo(char hbuf[LINE_MAX], char lbuf[][LINE_MAX], int lnum, EntList *entList) {
	int i;

	substr_t sbuf[5];
	String_Split(sbuf, hbuf, 4);

	// remove previous duplicates
	EntInfo *info = entList->FindEnt(sbuf[1]);
	if(info) {
		entList->GetInfoList().RemoveNode(*info);
		delete info;
	}

	strcpy(classname, sbuf[1]);
	strcpy(desc, sbuf[3]);

	entClass = NULL;
	if(strstr(sbuf[2], "->")) {
		char buf[80];
		strcpy(buf, sbuf[2]);
		String_CutAt(buf, "->");

		entClass = entList->FindClass(buf);

		char *c = strstr(sbuf[2], "->");
		strcpy(classdesc, c + 2);
	}
	
	if(!entClass)
		LFatal("EntInfo: bad entity header (%s)\n", sbuf[1]);

	bool spawnflags = false;
	for(i = 0; i < lnum; i++)
		if(strstri(lbuf[i], "spawnflags"))
			spawnflags = true;

	if(!spawnflags)
		strcpy(lbuf[lnum++], "\"spawnflags\" _spawnflags");

	isModel = false;
	isAdjust = false;
	wfGeomPtr = NULL;

	int real_lnum = lnum;
	int real = 0;

	filetype = ENTFILE_GEM;
	strcpy(filename, classname);
	strcat(filename, ".gem");

	// preprocess
	for(i = 0; i < lnum; i++) {
		String_Crop(lbuf[i]);
		if(lbuf[i][0] == '\"')
			continue;

		if(!strcmpi(lbuf[i], "model"))
			isModel = true;

		if(!strcmpi(lbuf[i], "adjust"))
			isAdjust = true;

		if(strstri(lbuf[i], "gem")) {
			char *c = strchr(lbuf[i], '\"');
			if(c) {
				filetype = ENTFILE_GEM;
				strcpy(filename, c + 1);
				String_Chop(filename);

				if(!strstr(filename, ".gem"))
					strcat(filename, ".gem");
			}
		}

		if(strstri(lbuf[i], "mdl")) {
			char *c = strchr(lbuf[i], '\"');
			if(c) {
				filetype = ENTFILE_MDL;
				strcpy(filename, c + 1);
				String_Chop(filename);
			}
		}

		if(strstri(lbuf[i], "md2")) {
			char *c = strchr(lbuf[i], '\"');
			if(c) {
				filetype = ENTFILE_MD2;
				strcpy(filename, c + 1);
				String_Chop(filename);
			}
		}

		lbuf[i][0] = NULL;
		real_lnum--;
	}

	entries = real_lnum;
	qentry = new QEntEntry[entries];

	for(i = 0; i < lnum; i++) {
		if(!strlen(lbuf[i])) continue;
		QEntEntry *entry = &qentry[real++];
		String_Split(sbuf, lbuf[i], 5);
		entry->name = new char[strlen(sbuf[0]) + 1];
		strcpy(entry->name, sbuf[0]);

		entry->vartype = VAR_UNDEF;
		if(!strcmpi(sbuf[1], "vec")) entry->vartype = VAR_VEC;
		if(!strcmpi(sbuf[1], "int")) entry->vartype = VAR_INT;
		if(!strcmpi(sbuf[1], "str")) entry->vartype = VAR_STR;
		if(!strcmpi(sbuf[1], "notuser")) entry->vartype = VAR_NOTUSER;
		if(!strcmpi(sbuf[1], "float")) entry->vartype = VAR_FLOAT;
		if(!strcmpi(sbuf[1], "color")) entry->vartype = VAR_COLOR;

		entry->entDef = entList->FindDef(sbuf[1]);
		if(entry->entDef)
			entry->vartype = entry->entDef->GetDefType();
			
		char def[40] = "";
		int edesc;
		if(sbuf[2][0] == '(') {
			strcpy(def, sbuf[2] + 1);
			String_Chop(def);
			edesc = 3;
		}
		else {
			edesc = 2;
		}

		entry->def = new char[strlen(def) + 1];
		strcpy(entry->def, def);
		entry->desc = new char[strlen(sbuf[edesc]) + 1];
		strcpy(entry->desc, sbuf[edesc]);
	}

	if(isModel)
		entClass->models++;
	else
		entClass->nonmodels++;
}

EntInfo::~EntInfo(void) {
	ASSERT(qentry != NULL);
	for(int i = 0; i < entries; i++) {
		delete qentry[i].name;
		delete qentry[i].def;
		delete qentry[i].desc;
	}
	delete qentry;
}

#include "Game.h"
#include "QMainFrm.h"

WireFrameGeom *
EntInfo::Use(bool &ret_isModel, bool &ret_isAdjust) {

	char *c, fName[256];
	c = strchr(filename, ':');
	if(c) {
		strcpy(fName, c + 1);

		char dir[256], add[256];
		strncpy(dir, filename, c - filename);
		dir[c - filename] = '\0';

		Game *game = Game::Get();
		ASSERT(game != NULL);

		sprintf(add, "%s\\%s", game->GetBaseDir(), dir);
		LFile::UseDir(add);

		for(int i = 0; i < 10; i++) {
			sprintf(add, "%s\\%s\\pak%d.pak", game->GetGameDir(), dir, i);
			if(LFile::Exist(add))
				LFile::UsePak(add);
		}
	}
	else
		strcpy(fName, filename);

	if(!wfGeomPtr && strlen(fName) && LFile::Exist(fName)) {
		if(strstr(fName, ".gem"))
			wfGeomPtr = new WireFrameGeom(fName);
		else {
			Game *game = Game::Get();
			ASSERT(game != NULL);
			wfGeomPtr = game->LoadModel(fName);
		}
	}

	ret_isModel = isModel;
	ret_isAdjust = isAdjust;

	return wfGeomPtr;
}

void
EntInfo::DoneUse(void) {
	if(wfGeomPtr)
		if(wfGeomPtr->Uses(-1))
			wfGeomPtr = NULL;
}

bool
EntInfo::AddUserEntry(char *name) {

	for(int i = 0; i < entries; i++)
		if(!strcmpi(qentry[i].name, name))
			return false;

	entries++;
	qentry = (QEntEntry *)realloc(qentry, sizeof(QEntEntry) * entries);
	QEntEntry *entry = &qentry[entries - 1];

	entry->name = (char *)malloc(strlen(name) + 1);
	strcpy(entry->name, name);
	entry->vartype = VAR_USER;
	entry->desc = (char *)malloc(2);
	strcpy(entry->desc, "");
	entry->def = (char *)malloc(2);
	strcpy(entry->def, "");
	entry->entDef = NULL;

	return true;
}


//==================== EntDef ====================

EntDef::EntDef(char hbuf[LINE_MAX], char lbuf[][LINE_MAX], int lnum, EntList *entList) {
	int i;

	substr_t sbuf[5];
	String_Split(sbuf, hbuf, 4);

	// remove previous duplicates
	EntDef *def = entList->FindDef(sbuf[1]);
	if(def) {
		entList->GetDefList().RemoveNode(*def);
		delete def;
	}

	strcpy(name, sbuf[1]);

	if(!strcmpi(sbuf[0], "TYPEDEF"))
		deftype = VAR_TYPEDEF;
	else if(!strcmpi(sbuf[0], "FLAGDEF"))
		deftype = VAR_FLAGDEF;
	else
		deftype = VAR_STRDEF;

	if(deftype == VAR_FLAGDEF) {
		strcpy(lbuf[lnum++], "256 \"Don't appear if skill 0 (easy)\"");
		strcpy(lbuf[lnum++], "512 \"Don't appear if skill 1 (medium)\"");
		strcpy(lbuf[lnum++], "1024 \"Don't appear if skill 2 (hard and nightmare)\"");
		strcpy(lbuf[lnum++], "2048 \"Don't appear if deathmatch game\"");
	}
		
	entries = lnum;
	qdefentry = new QDefEntry[lnum];

	for(i = 0; i < lnum; i++) {
		QDefEntry *defentry = &qdefentry[i];
		String_Split(sbuf, lbuf[i], 4);
		defentry->name = new char[strlen(sbuf[1]) + 1];
		strcpy(defentry->name, sbuf[1]);
		defentry->desc = new char[strlen(sbuf[2]) + 1];
		strcpy(defentry->desc, sbuf[2]);
		if(deftype == VAR_STRDEF) {
			defentry->str = new char[strlen(sbuf[0]) + 1];
			strcpy(defentry->str, sbuf[0]);
		}
		else	
			defentry->val = atoi(sbuf[0]);

		if(deftype == VAR_FLAGDEF && !defentry->val)
			LFatal("EntDef: flagdef entry can't be 0 (%s)\n", name);
	}
}

EntDef::~EntDef(void) {
	ASSERT(qdefentry != NULL);
	for(int i = 0; i < entries; i++) {
		delete qdefentry[i].name;
		delete qdefentry[i].desc;
		if(deftype == VAR_STRDEF)
			delete qdefentry[i].str;
	}
	delete qdefentry;
}
