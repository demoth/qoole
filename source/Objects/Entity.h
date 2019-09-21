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
 * entity.h
 */

#ifndef _ENTITY_H_
#define _ENTITY_H_

#include "LCommon.h"
#include "list.h"
#include "math3d.h"
#include "geometry.h"
#include "EntList.h"

#define EA_ORIGIN       0
#define EA_ANGLE        1
#define EA_MANGLE       2
#define ENTITY_FLAGS	3

#define MAX_ENTITY_KEYS		30


//==================== Entity ====================

class Entity : public ElmNode {
public:
	Entity(const char *className);
	Entity(const Entity &src);
	virtual ~Entity(void);

	Entity &operator=(const Entity &src);

	//===== Read / Write Entities. =====
	static Entity *ReadEntity(LFile *inFile, int entNum = -1,
                            int (*BrushCallBackFunc)(LFile *inFile) = NULL,
							int *pDone = NULL, int *pTotal = NULL,
							bool (*ProgressFunc)(int percent) = NULL);
	static Vector3d GetVecPos(const Entity &entity);
	static void WriteEntity(FILE *outFile,
                          const Entity &entity, int entNum = -1);

	//===== Entity Info =====
	bool IsWorldSpawn(void) const;
	bool IsItemEntity(void) const;
	bool IsModelEntity(void) const;

	const char *GetClassName(void) const;

	const Vector3d &GetOrigin(void) const;
	const int GetAngle(void) const;
	const Vector3d &GetMAngle(void) const;

	const char *GetKey(const char *skey) const;

	int GetNumKeys(void);
	const char *GetKeyNum(int num) const;
	void DelKey(const char *skey);	
  
	void SetOrigin(const Vector3d &pos);
	void SetAngle(int ang);
	void SetMAngle(const Vector3d &pos);

	void SetKey(const char *skey, const char *sarg);
	void SetKeyNotUser(const char *skey, bool set = true);

	//===== WireFrameGeom for rendering. =====
	WireFrameGeom &GetItemWireFrame(void) const { return *wfGeomPtr; };

	//===== fake support for mapio.cpp =====/
	bool IsAttribSet(int attribID) const;
	bool IsNotUserAttrib(int attribID) const;
	void UnSetAttrib(int attribID);

	//===== entity position correction.
	bool Need2AdjPos(void) const;

	EntInfo *GetEntInfo(void) const { return entInfo; }

private:
	static bool ParseKeys(const char *line, char **key1, char **key2);
	static Entity *NewEntity(const char *keys[], int numKeys);

	void Check(void);

	char *className;
	WireFrameGeom *wfGeomPtr;

	Vector3d origin, mangle;
	int angle;

	int keys;
	char *key[MAX_ENTITY_KEYS];
	char *arg[MAX_ENTITY_KEYS];

	bool ea_flag[ENTITY_FLAGS];
	bool notuser[ENTITY_FLAGS];

	bool isModel;
	bool isAdjust;

	EntInfo *entInfo;
	// bool delWFGeom;
};


//==================== EntityPtr ====================

class EntityPtr : public ElmNode {
public:
	EntityPtr(Entity *entPtr, int data) { ptr = entPtr;  rec = data; };
	Entity *GetPtr(void) const { return ptr; };
	int GetRec(void) const { return rec; };
private:
	Entity *ptr;
	int rec;
};


#endif // _ENTITY_H
