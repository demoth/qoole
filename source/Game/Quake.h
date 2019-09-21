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

#ifndef __QUAKE_H
#define __QUAKE_H

#include "game.h"

class Quake : public Game {
public:
	Quake();
	~Quake();

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

typedef struct
{
  int id;                     // 0x4F504449 = "IDPO" for IDPOLYGON
  int version;                // Version = 6
  vec3_t scale;                // Model scale factors.
  vec3_t origin;               // Model origin.
  float radius;             // Model bounding radius.
  vec3_t offsets;              // Eye position (useless?)
  int numskins ;              // the number of skin textures
  int skinwidth;              // Width of skin texture
                               //           must be multiple of 8
  int skinheight;             // Height of skin texture
                               //           must be multiple of 8
  int numverts;               // Number of vertices
  int numtris;                // Number of triangles surfaces
  int numframes;              // Number of frames
  int synctype;               // 0= synchron, 1= random
  int flags;                  // 0 (see Alias models)
  float size;               // average size of triangles} mdl_t;
} mdlheader_t;

typedef struct{
  int facesfront;             // boolean
  int vertices[3];            // Index of 3 triangle vertices
                               // in range [0,numverts[} itriangle_t;
} itriangle_t;

typedef struct
{ unsigned char x;
  unsigned char y;                    // X,Y,Z coordinate, packed on 0-255 
  unsigned char z;
  unsigned char lightnormalindex;     // index of the vertex normal
} trivertx_t;

typedef struct
{
  vec3_t scale; // multiply byte verts by this
  vec3_t origin; // then add this
  char name[16]; // frame name from grabbing
  trivertx_t verts[1]; // variable sized
} frameinfo_t;

struct texture_header_t {
	char name[16];
	int width, height;
	int mip1, mip2, mip3, mip4;
};

struct wad_header_t {
	char magic[4];
	long entries;
	long offset;
};

struct wad_entry_t {
	long offset;
	long dsize;
	long size;
	char type;
	char cmprs;
	short dummy;
	char name[16];
};

#endif
