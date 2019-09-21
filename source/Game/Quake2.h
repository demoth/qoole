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

#ifndef __QUAKE2_H
#define __QUAKE2_H

#include "game.h"

class Quake2 : public Game {
public:
	Quake2();
	~Quake2();

	bool Init(void);
	WireFrameGeom *LoadModel(char *filename);
	bool LoadTexture(Texture *texture, char *filename, int offset);
	void ExtractTextures(void);

	// Additional properties pages.
	virtual DWORD GetExtraPropPages(void) const;

	// Game Map Compile Stuff.
	int GetNumUtils(void) const { return 4; }
	const char *GetUtilName(int index) const;

	const char *GetLeakFile(const char *docName) const;
	Object *LoadLeakFile(const char *docName) const;

private:
	LPak *pak;
};

#include "Quake.h"

typedef struct                     
{
  int ident;
  int version;
  int skinwidth;
  int skinheight;
  int framesize; // byte size of each frame
  int num_skins;
  int num_xyz;
  int num_st; // greater than num_xyz for seams
  int num_tris;
  int num_glcmds; // dwords in strip/fan command list
  int num_frames;
  int ofs_skins; // each skin is a MAX_SKINNAME string
  int ofs_st; // byte offset from start for stverts
  int ofs_tris; // offset for dtriangles
  int ofs_frames; // offset for first frame
  int ofs_glcmds; 
  int ofs_end; // end of file
} md2header_t;

struct texture_header2_t {
	char name[32];
	int width, height;
	int mip1, mip2, mip3, mip4;
};

#endif
