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

/*
 * entity.cpp
 */

#include "stdafx.h"

#include "LCommon.h"
#include "entity.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//==================== Entity ====================

#include "LCommon.h"

Entity::Entity(const char *cName) {

	int i;

	className = new char[strlen(cName) + 1];
	strcpy(className, cName);

	// delWFGeom = false;
	wfGeomPtr = NULL;

	EntList *entList = EntList::Get();
	ASSERT(entList != NULL);

	entInfo = entList->FindEnt(className);
	if(entInfo) {
		wfGeomPtr = entInfo->Use(isModel, isAdjust);
	}
	else {
		if(!strcmp(className, "cross1")) {
			wfGeomPtr = entList->cross1Ptr;
			isModel = false;
		}
		else if(!strcmp(className, "cross2")) {
			wfGeomPtr = entList->cross2Ptr;
			isModel = false;
		}
		else {
			isModel = true;
			isAdjust = false;
		}
	}

	if(!isModel && !wfGeomPtr)
		wfGeomPtr = entList->undefPtr;

	if(wfGeomPtr)
		wfGeomPtr->Uses(1);

	keys = 0;

	for(i = 0; i < MAX_ENTITY_KEYS; i++) {
		key[i] = NULL;
		arg[i] = NULL;
	}

	for(i = 0; i < ENTITY_FLAGS; i++) {
		ea_flag[i] = false;
		notuser[i] = false;
	}

	notuser[EA_ORIGIN] = true;

	Check();
}

Entity::Entity(const Entity &src) {
	int i;

	className = NULL;
	wfGeomPtr = NULL;
	
	for(i = 0; i < MAX_ENTITY_KEYS; i++) {
		key[i] = NULL;
		arg[i] = NULL;
	}

	keys = 0;

	*this = src;
}

Entity::~Entity(void) {
	int i;

	if(className)
		delete className;

	if(entInfo)
		entInfo->DoneUse();

	for(i = 0; i < MAX_ENTITY_KEYS; i++) {
		if(key[i])
			delete key[i];
		if(arg[i])
			delete arg[i];
	}
}

Entity &
Entity::operator=(const Entity &src) {
	ASSERT(src.className != NULL);

	int i;

	if (&src == this)
		return *this;

	if(className) delete className;
	className = new char[strlen(src.className) + 1];
	strcpy(className, src.className);

	if (src.wfGeomPtr != NULL)
		src.wfGeomPtr->Uses(1);
	if (wfGeomPtr != NULL)
		wfGeomPtr->Uses(-1);
	wfGeomPtr = src.wfGeomPtr;

	origin = src.origin;
	angle = src.angle;
	entInfo = src.entInfo;

	if ((ea_flag[EA_ORIGIN] = src.ea_flag[EA_ORIGIN]) != 0)
		origin = src.origin;
	if ((ea_flag[EA_ANGLE] = src.ea_flag[EA_ANGLE]) != 0)
		angle = src.angle;
	if ((ea_flag[EA_MANGLE] = src.ea_flag[EA_MANGLE]) != 0)
		mangle = src.mangle;

	for(i = 0; i < ENTITY_FLAGS; i++)
		notuser[i] = src.notuser[i];
  
	isModel = src.isModel;
  
	keys = src.keys;

	for(i = 0; i < src.keys; i++) {
		if(key[i])
			delete key[i];
		if(arg[i])
			delete arg[i];

		key[i] = new char[strlen(src.key[i]) + 1];
		strcpy(key[i], src.key[i]);

		arg[i] = new char[strlen(src.arg[i]) + 1];
		strcpy(arg[i], src.arg[i]);
	}

	return *this;
}

//===== Read / Write Entities. =====

Entity *
Entity::ReadEntity(LFile *inFile, int entNum,
                   int (*BrushCallBackFunc)(LFile *inFile),
				   int *pDone, int *pTotal,
				   bool (*ProgressFunc)(int percent)) {
	// PreCondition
	ASSERT(entNum >= -1);

	Entity *rtnVal = NULL;
	char *argBuf[MAX_ENTITY_KEYS];
	int i, argNum;
	char srch[20];

	for(i = 0; i < MAX_ENTITY_KEYS; i++)
		argBuf[i] = NULL;

	// Search for entity.
	if (entNum == -1)
		sprintf(srch, "{");
	else
		sprintf(srch, "{ // Entity %d", entNum);

	if (inFile->Search(srch) == NULL)
		goto ERROR_READ_ENTITY;

	if(ProgressFunc)
		ProgressFunc(++*pDone * 100 / *pTotal);

	argNum = 0;
	// Parse until end of entity Section.
	while (1) {
		if (inFile->GetNextLine() == NULL)
			goto ERROR_READ_ENTITY;
		else if (strncmp(inFile->GetLine(), "{", 1) == 0) {
			if(ProgressFunc)
				ProgressFunc(++*pDone * 100 / *pTotal);
			if (entNum != -1)
				goto ERROR_READ_ENTITY;
			if (BrushCallBackFunc(inFile) == -1)
				goto ERROR_READ_ENTITY;
		}
		else if (strncmp(inFile->GetLine(), "}", 1) == 0)
			break;
		else if (strncmp(inFile->GetLine(), "//", 2) == 0)
			continue;
		else if (strncmp(inFile->GetLine(), "\"", 1) == 0) {
			if (argNum >= MAX_ENTITY_KEYS)
				goto ERROR_READ_ENTITY;
			
			if(argBuf[argNum])
				delete argBuf[argNum];
			argBuf[argNum] = new char[strlen(inFile->GetLine()) + 1];
			strcpy(argBuf[argNum], inFile->GetLine());
			argNum++;
		}
	}

	if (argNum == 0)
		goto ERROR_READ_ENTITY;

	rtnVal = NewEntity((const char **) argBuf, argNum);
	if (rtnVal == NULL)
		goto ERROR_READ_ENTITY;

	goto RTN_READ_ENTITY;

ERROR_READ_ENTITY:
	if (entNum != -1)
		LError("Error load entity\n");

	if (rtnVal != NULL)
		delete rtnVal;
	rtnVal = NULL;

RTN_READ_ENTITY:
	for(i = 0; i < MAX_ENTITY_KEYS; i++)
		if(argBuf[i])
			delete argBuf[i];

	if(rtnVal)
		rtnVal->Check();

	return rtnVal;
}

Vector3d
Entity::GetVecPos(const Entity &entity) {
#if 1
	return entity.origin;
#else
	Vector3d minVec, maxVec;
	entity.wfGeomPtr->GetMinMax(minVec, maxVec);
	Vector3d vec = entity.origin;
	vec.SetX(vec.GetX() - minVec.GetX());
	vec.SetY(vec.GetY() - -minVec.GetY());
	vec.SetZ(vec.GetZ() - minVec.GetZ());
	return vec;
#endif
}

void
Entity::WriteEntity(FILE *outFile, const Entity &entity, int entNum) {
	// PreCondition.
	ASSERT(entNum >= -1);
	ASSERT(entity.className != NULL);

	int i;
	const Vector3d vec;
	char wkey[80], warg[512];

	if(entNum != -1)
		fprintf(outFile, "{ // Entity %d\n", entNum);

	strcpy(wkey, "\"classname\"");
	sprintf(warg, "\"%s\"", entity.className);
	fprintf(outFile, "  %-16s%s\n", wkey, warg);

	for(i = 0; i < entity.keys; i++) {
		if(!strcmpi(entity.key[i], "origin") && entity.ea_flag[EA_ORIGIN]) {
			Vector3d vec = Entity::GetVecPos(entity);
			sprintf(warg, "\"%d %d %d\"", (int)ROUND(vec.GetX()),
				(int)ROUND(vec.GetY()), (int)ROUND(vec.GetZ()));
		}
		else if(!strcmpi(entity.key[i], "angle") && entity.ea_flag[EA_ANGLE]) {
			sprintf(warg, "\"%d\"", entity.angle);
		}
		else if(!strcmpi(entity.key[i], "mangle") && entity.ea_flag[EA_MANGLE]) {
			sprintf(warg, "\"%d %d %d\"", (int)ROUND(entity.mangle.GetX()),
				(int)ROUND(entity.mangle.GetY()), (int)ROUND(entity.mangle.GetZ()));
		}
		else {
			sprintf(warg, "\"%s\"", entity.arg[i]);
		}

		sprintf(wkey, "\"%s\"", entity.key[i]);

		if(strcmp(warg, "\"\""))
			fprintf(outFile, "  %-16s%s\n", wkey, warg);
	}
  
	if(entNum != -1)
		fprintf(outFile, "} // Entity %d\n", entNum);
}

//===== Key Manip =====

void
Entity::SetKey(const char *skey, const char *sarg) {
	int i;

	if(!strcmpi(skey, "origin"))
		ea_flag[EA_ORIGIN] = true;
	if(!strcmpi(skey, "angle"))
		SetAngle(atoi(sarg));
	if(!strcmpi(skey, "mangle"))
		ea_flag[EA_MANGLE] = true;
      
	for(i = 0; i < keys; i++) {
		if(!strcmpi(skey, key[i])) {
			if(!strcmpi(skey, "classname") || !strcmpi(skey, "origin")) {
				LError("Entity::SetKey: not allowed to set %s that way\n", skey);
				return;
			}

			if(arg[i])
				delete arg[i];
			arg[i] = new char[strlen(sarg) + 1];
			strcpy(arg[i], sarg);
			return;
		}
	}

	if(keys > MAX_ENTITY_KEYS)
		LError("Entity::SetKey: keys > MAX_ENTITY_KEYS");

	if(key[keys])
		delete key[keys];
	key[keys] = new char[strlen(skey) + 1];
	strcpy(key[keys], skey);

	if(arg[keys])
		delete arg[keys];
	arg[keys] = new char[strlen(sarg) + 1];
	strcpy(arg[keys], sarg);

	keys++;
}

void
Entity::SetKeyNotUser(const char *skey, bool set) {
	int i;
	for(i = 0; i < keys; i++) {
		if(!strcmpi(skey, key[i])) {
			if(!strcmpi(skey, "angle")) {
				notuser[EA_ANGLE] = set;
				break;
			}
			if(!strcmpi(skey, "mangle")) {
				notuser[EA_MANGLE] = set;
				break;
			}
		}
	}
}

const char *
Entity::GetKey(const char *skey) const {
	int i;
	for(i = 0; i < keys; i++)
		if(!strcmpi(skey, key[i]) && strlen(arg[i]))
			return arg[i];
	return NULL;
}

int
Entity::GetNumKeys(void) {
	return keys;
}

const char *
Entity::GetKeyNum(int num) const {
	ASSERT(num >= 0 && num < keys);
	return key[num];
}

void
Entity::DelKey(const char *skey) {
	int i;
	for(i = 0; i < keys; i++)
		if(!strcmpi(skey, key[i]))
			strcpy(arg[i], "");

	if(!strcmpi(skey, "origin"))
		ea_flag[EA_ORIGIN] = false;
	if(!strcmpi(skey, "angle"))
		ea_flag[EA_ANGLE] = false;
	if(!strcmpi(skey, "mangle"))
		ea_flag[EA_MANGLE] = false;
}

//===== Entity Info =====

bool
Entity::IsWorldSpawn(void) const {
	return (stricmp(className, "worldspawn") == 0);
}

bool
Entity::IsItemEntity(void) const {
	return !isModel;
}

bool
Entity::IsModelEntity(void) const {
	return isModel;
}

const char *
Entity::GetClassName(void) const {
	// Sanity Check.
	ASSERT(className != NULL);
	return className;
};

const Vector3d &
Entity::GetOrigin(void) const {
	return origin;
}

const int
Entity::GetAngle(void) const {
	return angle;
}

const Vector3d &
Entity::GetMAngle(void) const {
  return mangle;
}

void
Entity::SetOrigin(const Vector3d &pos) {
	ea_flag[EA_ORIGIN] = true;
	origin = pos;
}

void
Entity::SetAngle(int ang) {
	ea_flag[EA_ANGLE] = true;
	angle = ang;
}

void
Entity::SetMAngle(const Vector3d &pos) {
	ea_flag[EA_MANGLE] = true;
	mangle = pos;
}

bool
Entity::ParseKeys(const char *line, char **key, char **key2) {
	static char rtnVal1[80];
	static char rtnVal2[512];

	const char *c, *c1;
	int len;

	*key = *key2 = NULL;

	if ((c = strchr(line, '"')) == NULL)
		return false;
	if ((c1 = strchr(c + 1, '"')) == NULL)
		return false;
	len = Min(c1 - c - 1, 79);
	strncpy(rtnVal1, c + 1, len);
	rtnVal1[len] = '\0';
	*key = rtnVal1;

	if ((c = strchr(c1 + 1, '"')) == NULL)
		return true;
	if ((c1 = strchr(c + 1, '"')) == NULL)
		return true;
	len = Min(c1 - c - 1, 511);
	strncpy(rtnVal2, c + 1, len);
	rtnVal2[len] = '\0';
	*key2 = rtnVal2;

	return true;
}

Entity *
Entity::NewEntity(const char *keys[], int numKeys) {
	// Sanity Check.
	ASSERT(keys != NULL);

	int c, i, val;
	float x, y, z;
	Vector3d tmpVec;
	char *key, *arg;
	Entity *rtnVal = NULL;

	// Find the class name first.
	for(int cIndex = 0; cIndex < numKeys; cIndex++) {
		if (ParseKeys(keys[cIndex], &key, &arg) && strcmp(key, "classname") == 0)
			break;
	}

	if (cIndex == numKeys || arg == NULL)
		goto ERROR_NEWENTITY;

	rtnVal = new Entity(arg);

	c = 0;
	for(i = 0; i < numKeys; i++) {
		if(!rtnVal->ParseKeys(keys[i], &key, &arg))
			goto ERROR_NEWENTITY;

		if(strcmpi(key, "classname") == 0) {
			continue;
		}
		else if(strcmpi(key, "origin") == 0) {
			if(sscanf(arg, "%f %f %f", &x, &y, &z) == 3) {
				tmpVec.NewVector(x, y, z);
				rtnVal->SetOrigin(tmpVec);
			}
		}
		else if(strcmpi(key, "angle") == 0) {
			if(sscanf(arg, "%d", &val) == 1)
				rtnVal->SetAngle(val);
		}

		if(rtnVal->key[c])
			delete rtnVal->key[c];
		if(rtnVal->arg[c])
			delete rtnVal->arg[c];

		rtnVal->key[c] = new char[strlen(key) + 1];
		strcpy(rtnVal->key[c], key);

		rtnVal->arg[c] = new char[strlen(arg) + 1];
		strcpy(rtnVal->arg[c], arg);

		c++;
	}
  
	rtnVal->keys = c;
	rtnVal->Check();

	if(!rtnVal->className)
		goto ERROR_NEWENTITY;

	goto RTN_NEWENTITY;

ERROR_NEWENTITY:
	delete rtnVal;
	rtnVal = NULL;

RTN_NEWENTITY:
	return rtnVal;
}

void
Entity::Check(void) {
	int i;
	const char *key;
	QEntEntry *entry;

	if(!entInfo)
		return;

	for(i = 0; i < entInfo->GetNumEntries(); i++) {
		entry = entInfo->GetEntryNum(i);
		key = GetKey(entry->name);
		if(!key && (strlen(entry->def) || entry->vartype == VAR_NOTUSER))
			SetKey(entry->name, entry->def);
		if(entry->vartype == VAR_NOTUSER)
			SetKeyNotUser(entry->name);
	}

	int j;
	for(i = 0; i < GetNumKeys(); i++) {
		bool found = false;
		key = GetKeyNum(i);

		for(j = 0; j < entInfo->GetNumEntries(); j++) {
			if(!strcmpi(key, entInfo->GetEntryNum(j)->name)) {
				found = true;
				break;
			}
		}

		if(!found)
			entInfo->AddUserEntry((char *)key);
	}
}

bool
Entity::IsAttribSet(int attribID) const {
	return ea_flag[attribID];
}

void
Entity::UnSetAttrib(int attribID) {
	ea_flag[attribID] = false;
}

bool
Entity::IsNotUserAttrib(int attribID) const {
	return notuser[attribID];
}

bool
Entity::Need2AdjPos(void) const {
	return isAdjust;
}


