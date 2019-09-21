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
 * mapio.h
 */

#ifndef _MAPIO_H_
#define _MAPIO_H_

#include "objects.h"

//==================== MapIO ====================

class MapIO {
 public:
  static Object *ReadMap(const char *fName,
	  bool (*ProgressFunc)(int percent) = NULL);
  static bool WriteMap(const char *fName, Object &rootObj, bool (*ProgressFunc)(int percent));

 private:
  static bool ReadEntities(LFile *inFile, LinkList<Object> &objList);
  static int  ReadBrush(LFile *inFile);

  static bool TransObj2Abs(Object &objNode);
  static void WriteEntities2Map(FILE *outFile, Object &objNode);
  static void WriteBrushes2Map(FILE *outFile, Object &objNode,
                               const Entity *ePtr);
  static void CnvrtEnt2Obj(const Entity &ent, Object &objNode);
  static void CnvrtObj2Ent(const Object &objNode, Entity &ent);

  static LinkList<Object> brushes;
  static bool ignore;
  static int  numBrushError, numEntError;

  static bool (*ProgressFunc)(int percent);
  static int pTotal, pDone;
};

#endif // _MAPIO_H_
