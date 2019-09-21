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
 * mapio.cpp
 */

#include "stdafx.h"

#include <string.h>
#include "mapio.h"
#include "LCommon.h"
#include "QDraw.h"

//==================== MapIO ====================

LinkList<Object> MapIO::brushes;
bool MapIO::ignore = false;
int MapIO::numBrushError = 0;
int MapIO::numEntError = 0;
int MapIO::pTotal = 0;
int MapIO::pDone = 0;
bool (*MapIO::ProgressFunc)(int percent) = NULL;

//===== read map =====
Object *
MapIO::ReadMap(const char *fName, bool (*_ProgressFunc)(int percent)) {
	// PreCondition.
	ASSERT(fName != NULL);

	LFile *inFile = new LFile;
	Object *hObjPtr, *rtnVal = NULL;
	LinkList<Object> objList;
	IterLinkList<Object> iterObj(objList);
	char tmpError[80];

	ignore = false;
	numBrushError = numEntError = 0;
	tmpError[0] = '\0';

	QDraw::OutputText("Loading %s... ", (char *)fName);
	if (!inFile->Open(fName))
	{
		QDraw::OutputText("Error.\nUnable to load map sucessfully.\n");
		sprintf(tmpError, "Can't open file %60s", fName);
		goto ERROR_READMAP;
	}

	ProgressFunc = _ProgressFunc;
	pTotal = 0;
	while(inFile->GetNextLine())
		if(strchr(inFile->GetLine(), '{'))
			pTotal++;
	inFile->Close();

	inFile->Open(fName);
	inFile->ResetLineNumber();
	pDone = 0;

	if (!ReadEntities(inFile, objList))
	{
		QDraw::OutputText("Error. Unable to load the entities.\n");
		goto ERROR_READMAP;
	}

	iterObj.Reset();
	while (!iterObj.IsDone())
	{
		hObjPtr = iterObj.GetNext();
		if (hObjPtr->HasEntity() &&
			strcmp((hObjPtr->GetEntityPtr())->GetClassName(), "worldspawn") == 0)
		{
			rtnVal = &(objList.RemoveNode(*hObjPtr));
			break;
		}
	}

	if (rtnVal == NULL)
	{
		QDraw::OutputText("Error. Can't find Worldspawn entity.\n");
		sprintf(tmpError, "Can't find Worldspawn entity.");
		goto ERROR_READMAP;
	}

	while (objList.NumOfElm() > 0) {
		rtnVal->AddChild(objList.RemoveNode(0), false);
	}

	// Set bound radius for all nodes.
	rtnVal->PostOrderApply(Object::ResetBoundRadius);

	if (numBrushError > 0 || numEntError > 0)
	{
		QDraw::OutputText("Error. Ignored %d bad brushes and %d bad entities in %s.\n", numBrushError, numEntError, fName);
		sprintf(tmpError, "Ignored %d bad brushes and %d bad entities in %s.", numBrushError, numEntError, fName);
		// QERROR(tmpError);
		// Dialog_NoticeMsg(tmpError);
	}
	
	goto RTN_READMAP;

ERROR_READMAP:
	if (tmpError[0] != '\0')
		LError(tmpError);

	if (rtnVal != NULL)
		delete rtnVal;
	rtnVal = NULL;

	while (objList.NumOfElm() > 0)
		delete &(objList.RemoveNode(0));

RTN_READMAP:
	delete inFile;

	return rtnVal;
}

bool
MapIO::ReadEntities(LFile *inFile, LinkList<Object> &objList) {
	Entity	*newEnt, *oldEnt;
	Object	*newObj;
	char	tmpError[80];
	int		stat;

	while (1)
	{
		// Sanity.
		ASSERT(brushes.NumOfElm() == 0);

		newEnt = Entity::ReadEntity(inFile, -1, ReadBrush, &pDone, &pTotal, ProgressFunc);
		if (newEnt == NULL)
		{
			if (inFile->EndOfFile())  // EOF
				break;
			while (brushes.NumOfElm() > 0)
			{
				numBrushError++;
				delete &(brushes.RemoveNode(0));
			}
			numEntError++;

			//== Ignore the Entity ==
			if (!ignore)
			{
				QDraw::OutputText("Error loading entity before line %d.\n", inFile->GetLineNumber());
				sprintf(tmpError, "Error loading entity before line %d.  Ignore and continue?", inFile->GetLineNumber());
				// stat = LoadErrorPrompt(tmpError);
				
				stat = -1;
				if (stat == -1)
					goto ERROR_READENTITIES;
				else if (stat == 1)
					ignore = true;
			}
			continue;
		}

		if (newEnt->IsItemEntity())
		{
			if (brushes.NumOfElm() > 0)
			{
				//== Ignore the brushes ==
				if (!ignore)
				{
					QDraw::OutputText("Error loading entity before line %d.\n", inFile->GetLineNumber());
					sprintf(tmpError, "Error loading entity before line %d.  Ignore and continue?",	inFile->GetLineNumber());
					// stat = LoadErrorPrompt(tmpError);
					stat = -1;
					if (stat == -1)
						goto ERROR_READENTITIES;
					else if (stat == 1)
						ignore = true;
				}
				
				while (brushes.NumOfElm() > 0)
				{
					numBrushError++;
					delete &(brushes.RemoveNode(0));
				}
			}
			newObj = new Object();
		}
		else if (brushes.NumOfElm() == 0)
		{
			newObj = new Object();
		}
		
		else if (brushes.NumOfElm() == 1)
		{
			newObj = &(brushes.RemoveNode(0));
		}
		else
		{
			newObj = new Object();
			while (brushes.NumOfElm() > 0)
				newObj->AddChild(brushes.RemoveNode(0), false);
		}
		
		oldEnt = newObj->SetEntity(newEnt);
		ASSERT(oldEnt == NULL);  // Sanity.

		// Adjust obj prop.
		CnvrtEnt2Obj(*newEnt, *newObj);

		objList.AppendNode(*newObj);
	}

	goto RTN_READENTITIES;

ERROR_READENTITIES:
	while (brushes.NumOfElm() > 0)
		delete &(brushes.RemoveNode(0));

	return false;
  
RTN_READENTITIES:
	// Sanity.
	ASSERT(brushes.NumOfElm() == 0);

	return true;
}

int
MapIO::ReadBrush(LFile *inFile)
{
	Geometry *holdPtr = Geometry::ReadBoundGeometry(inFile);
	Object *newBrushObj;
	Vector3d pos;
	char tmpError[80];
	int stat;

	if (holdPtr == NULL)
	{
		if (!ignore)
		{
			QDraw::OutputText("Error loading brush before line %d.\n", inFile->GetLineNumber());
			sprintf(tmpError, "Error loading brush before line %d.  Ignore and continue?", inFile->GetLineNumber());
			// stat = LoadErrorPrompt(tmpError);
			stat = 1;
			if (stat == -1)
				return -1;
			else if (stat == 1)
				ignore = true;
		}
		numBrushError++;
		return 0;
	}
	holdPtr->CenterGeometry(pos);
	newBrushObj = new Object();
	newBrushObj->SetBrush(holdPtr);
	newBrushObj->SetPosition(pos);
	brushes.AppendNode(*newBrushObj);
	return true;
}

//===== write map =====
bool
MapIO::WriteMap(const char *fName, Object &rootObj, bool (*_ProgressFunc)(int percent)) {
	// PreConditions.
	ASSERT(rootObj.IsRoot());
	ASSERT(!rootObj.IsLeaf());
	ASSERT(rootObj.HasEntity() && strcmp((rootObj.GetEntityPtr())->GetClassName(), "worldspawn") == 0);

	ProgressFunc = _ProgressFunc;
	pDone = 0;
	pTotal = 0;

	FILE *outFile;  

	if ((outFile = fopen(fName, "wt")) == NULL)
		return false;

	fprintf(outFile, "// %s generated MAP\n", AfxGetAppName());
	fprintf(outFile, "// http://qoole.gamedesign.net\n");
	fprintf(outFile, "\n");
  
	// Transform the object tree to absolute coord first.
	rootObj.PreOrderApply(TransObj2Abs);

	// Traverse the tree and output entities.
	WriteEntities2Map(outFile, rootObj);

	fclose(outFile);
	return true;
}

bool
MapIO::TransObj2Abs(Object &objNode) {
	Matrix44 trans;

	if (objNode.IsRoot())
		trans.SetIdentity();
	else
		trans = (objNode.GetParent()).GetTransformation();

	objNode.CalTransSpaceMatrix(trans);
	objNode.SetTransformation(trans);

	pTotal++;

	return true;
}

void
MapIO::WriteEntities2Map(FILE *outFile, Object &objNode) {
	Entity *wEntPtr;

	if (objNode.HasEntity())
	{
		fprintf(outFile, "{\n");

		wEntPtr = new Entity(*(objNode.GetEntityPtr()));
		CnvrtObj2Ent(objNode, *wEntPtr);  // Adjust the ent prop.
		Entity::WriteEntity(outFile, *wEntPtr);
		delete wEntPtr;

		if (objNode.IsModelNode())
			WriteBrushes2Map(outFile, objNode, objNode.GetEntityPtr());

		fprintf(outFile, "}\n");
	}

	// Traverse children.
	IterLinkList<Object> *iter = &(objNode.GetIterChildren());
	for(iter->Reset(); !iter->IsDone(); ) {
		WriteEntities2Map(outFile, *(iter->GetNext()));
	}

	if(ProgressFunc)
		ProgressFunc(++pDone * 100 / pTotal);
}

void
MapIO::WriteBrushes2Map(FILE *outFile, Object &objNode, const Entity *ePtr) {
	// PreCondition.
	ASSERT(outFile != NULL);

	// Check for the same entity.
	if (objNode.HasEntity() && objNode.GetEntityPtr(true) != ePtr)
		return;

	if (objNode.HasBrush())
	{
		// Sanity Check.
		ASSERT(objNode.IsLeaf());

		Geometry::WriteBoundGeometry(outFile, objNode.GetBrush(), -1, objNode.GetTransformation(), true);
		
		return;
	}

	// Traverse the sub tree.
	IterLinkList<Object> *iter = &(objNode.GetIterChildren());
	for(iter->Reset(); !iter->IsDone(); ) {
		WriteBrushes2Map(outFile, *(iter->GetNext()), ePtr);
	}
}

//===== Convert Entity Prop to Object Prop When Loading Map =====
void
MapIO::CnvrtEnt2Obj(const Entity &ent, Object &objNode) {
	Vector3d posVec, fv, tv(0.0f, 0.0f, 1.0f);
	Matrix44 trans;

	// Set position.
	if (ent.IsAttribSet(EA_ORIGIN))
	{
		posVec.AddVector(objNode.GetPosition(), ent.GetOrigin());
		objNode.SetPosition(posVec);
	}

	// Set orientation.
#if 1
	if (ent.IsNotUserAttrib(EA_ANGLE)) {
#else
	if (ent.IsAttribSet(EA_ANGLE) &&
		(strncmp(ent.GetClassName(), "info_player_", 12) == 0 ||
		strncmp(ent.GetClassName(), "info_teleport", 13) == 0 ||
		strncmp(ent.GetClassName(), "monster_", 8) == 0)) {
#endif
		trans.SetRotateZ(DEG2RAD(ent.GetAngle()));
		trans.Transform(fv.NewVector(1.0f, 0.0f, 0.0f));
		objNode.SetOrientation(fv, tv);
	}


	// Adjust position.
	if (ent.Need2AdjPos())
	{
		Vector3d minBVec, maxBVec;

		(ent.GetItemWireFrame()).FindSelfDimen(minBVec, maxBVec);
		fv.NewVector(maxBVec.GetX(), maxBVec.GetY(), 0.0f);
		trans.SetRotateZ(DEG2RAD(ent.GetAngle()));
		objNode.GetPosition(posVec);
		posVec.AddVector(trans.Transform(fv));
		objNode.SetPosition(posVec);
	}

#if 0
	if (ent.IsAttribSet(EA_MANGLE) &&
#endif
	if (strcmp(ent.GetClassName(), "info_intermission") == 0) {
		SphrVector orientVec;
		Vector3d tVec(ent.GetMAngle());

		tVec.NewVector(90.0f - tVec.GetY(), -tVec.GetX(), tVec.GetZ());
		orientVec.NewVector(DEG2RAD(tVec.GetX()), DEG2RAD(tVec.GetY()), DEG2RAD(tVec.GetZ()));
		objNode.SetOrientation(orientVec);
	}
}

//===== Convert Object Prop to Entity Prop When Saving Map =====
void
MapIO::CnvrtObj2Ent(const Object &objNode, Entity &ent) {
	const Vector3d xAxis(1.0f, 0.0f, 0.0f), yAxis(0.0f, 1.0f, 0.0f);
	const char *cName = (objNode.GetEntityPtr())->GetClassName();
	Vector3d ov(0.0f, 0.0f, 0.0f);
	Vector3d fv;
	int ang;

	// Set origin.
	(objNode.GetTransformation()).Transform(ov);
	if (objNode.IsModelNode()) {
		ent.UnSetAttrib(EA_ORIGIN);
	}
	else { // if (objNode.IsItemNode())
#if 1
		ASSERT(objNode.IsItemNode());  // Sanity.
		
		Vector3d ov, minBVec, maxBVec;

		if ((objNode.GetEntityPtr())->Need2AdjPos())
		{
			objNode.GetItemWireFrame().FindSelfDimen(minBVec, maxBVec);
			(objNode.GetTransformation()).Transform(ov, maxBVec.GetX(), minBVec.GetY(), 0.0f);
		}
		else {
			(objNode.GetTransformation()).Transform(ov, 0.0f, 0.0f, 0.0f);
		}

		ent.SetOrigin(ov);

#else
		ASSERT(objNode.IsItemNode());  // Sanity.

		(objNode.GetTransformation()).Transform(ov);
		ent.SetOrigin(ov);
#endif
	}

#if 0
	// Set angle.
	if ((objNode.IsModelNode() ||
		strncmp(cName, "trap_s", 6) == 0) &&
		(ent.IsAttribSet(EA_ANGLE) &&
		ent.GetAngle() >= 0 && ent.GetAngle() <= 360))
	{
		trans.SetRotateZ(DEG2RAD(ent.GetAngle()));
		trans.Transform(fv.NewVector(1.0f, 0.0f, 0.0f));
		(objNode.GetTransformation()).Transform(fv);
		fv.SubVector(ov);
		ang = (int) ROUND(RAD2DEG(ATan(fv.DotVector(yAxis), fv.DotVector(xAxis))));
		
		if (ang < 0)
			ang += 360;
		ASSERT(ang >= 0 && ang <= 360);	// Sanity
		ent.SetAngle(ang);
	}
	else if (strcmp(cName, "info_intermission") == 0) {
		fv.NewVector(0.0f, 100.0f, 0.0f);
		(objNode.GetTransformation()).Transform(fv);
		fv.SubVector(ov);
		ent.SetMAngle(fv);
	}
	else if(objNode.IsItemNode())
	{
		fv.NewVector(0.0f, 1.0f, 0.0f);
		(objNode.GetTransformation()).Transform(fv);
		fv.SubVector(ov);
		ang = (int) ROUND(RAD2DEG(ATan(fv.DotVector(yAxis), fv.DotVector(xAxis))));
		if (ang < 0)
			ang += 360;
		ASSERT(ang >= 0 && ang <= 360);	// Sanity
		ent.SetAngle(ang);
	}
#endif
	// Set angle.
#if 1
	if(ent.IsNotUserAttrib(EA_ANGLE)) {
#else
	if ((objNode.IsModelNode() ||
		strncmp(cName, "trap_s", 6) == 0) &&
		(ent.IsAttribSet(EA_ANGLE) &&
		ent.GetAngle() >= 0 && ent.GetAngle() <= 360))
	{
		trans.SetRotateZ(DEG2RAD(ent.GetAngle()));
		trans.Transform(fv.NewVector(1.0f, 0.0f, 0.0f));
#endif
#if 1
		fv.NewVector(0.0f, 1.0f, 0.0f);
#endif
		(objNode.GetTransformation()).Transform(fv);
		fv.SubVector(ov);
		ang = (int) ROUND(RAD2DEG(ATan(fv.DotVector(yAxis), fv.DotVector(xAxis))));
		
		if (ang < 0)
			ang += 360;
		ASSERT(ang >= 0 && ang <= 360);	// Sanity
		ent.SetAngle(ang);
	}
#if 0
	else if (objNode.IsItemNode() &&
		(strncmp(cName, "info_player_", 12) == 0 ||
		strncmp(cName, "info_teleport_", 13) == 0 ||
		strncmp(cName, "monster_", 8) == 0))
	{
		fv.NewVector(0.0f, 1.0f, 0.0f);
		(objNode.GetTransformation()).Transform(fv);
		fv.SubVector(ov);
		ang = (int) ROUND(RAD2DEG(ATan(fv.DotVector(yAxis), fv.DotVector(xAxis))));
		
		if (ang < 0)
			ang += 360;
		ASSERT(ang >= 0 && ang <= 360);	// Sanity
		ent.SetAngle(ang);
	}
	else if (strcmp(cName, "info_intermission") == 0) {
#endif
	if (strcmp(cName, "info_intermission") == 0) {
		SphrVector orientVec;

		objNode.GetOrientation(orientVec);
		fv.NewVector(-RAD2DEG(orientVec.GetPitch()), 90.0f - RAD2DEG(orientVec.GetYaw()), RAD2DEG(orientVec.GetRoll()));
		fv.NewVector(ROUND(fv.GetX()), ROUND(fv.GetY()), ROUND(fv.GetZ()));
		ent.SetMAngle(fv);
	}
}