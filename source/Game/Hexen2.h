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

#ifndef __HEXEN2_H
#define __HEXEN2_H

#include "game.h"
#include "Quake.h"

class Hexen2 : public Game {
public:
	Hexen2();
	~Hexen2();

	bool Init(void);
	WireFrameGeom *LoadModel(char *filename);
	bool LoadTexture(Texture *texture, char *filename, int offset);
	void ExtractTextures(void);

	// Game Map Compile Stuff.
	int GetNumUtils(void) const { return 4; }
	const char *GetUtilName(int index) const;

	const char *GetLeakFile(const char *docName) const;
	Object *LoadLeakFile(const char *docName) const;

private:
	LPak *pak0, *pak1;
};

#endif
