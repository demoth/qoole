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
#include "Quake.h"

struct dentry_t {
	long offset;
	long size;
};

struct dheader_t {
	long version;
	dentry_t entities;
	dentry_t planes;
	dentry_t miptex;
	// ...who cares about the rest
};

struct miptex_t {
	char name[16];
	long width;
	long height;
	long offset1;
	long offset2;
	long offset4;
	long offset8;
};

struct tex_t {
	char name[16];
	FILE *file;
	long offset;
	miptex_t miptex;
};

int texs = 0;
tex_t *tex;

int count = 0;
int meter = 0;

int pTotal;
bool (*ProgressFunc)(int percent);

void readbsp(char *filename, FILE *file, long offset) {

	dheader_t dheader;

	fseek(file, offset, SEEK_SET);
	
	fread(&dheader, sizeof(dheader), 1, file);

	if(dheader.version > 29)
		return;

	long numtex;
	long *texoffset;

	fseek(file, offset + dheader.miptex.offset, SEEK_SET);
	fread(&numtex, sizeof(numtex), 1, file);
	
	texoffset = (long *)malloc(sizeof(long) * numtex);
	fread(texoffset, sizeof(long), numtex, file);

	for(int i = 0; i < numtex; i++) {
		miptex_t miptex;
		fseek(file, offset + dheader.miptex.offset + texoffset[i], SEEK_SET);
		fread(&miptex, sizeof(miptex), 1, file);

		bool dup = false;
		for(int j = 0; j < texs; j++) {
			if(!strcmpi(tex[j].name, miptex.name)) {
				dup = true;
				break;
			}
		}
		if(dup)
			continue;

		strcpy(tex[texs].name, miptex.name);
		tex[texs].file = file;
		tex[texs].offset = offset + dheader.miptex.offset + texoffset[i];
		tex[texs].miptex = miptex;

		texs++;
	}
}

void readpak(FILE *file) {

	pak_header_t pak_header;
	pak_entry_t *pak_entry;

	fread(&pak_header, sizeof(pak_header), 1, file);

	pak_entry = (pak_entry_t *)malloc(pak_header.size);

	fseek(file, pak_header.offset, SEEK_SET);
	fread(pak_entry, pak_header.size, 1, file);

	int t = (int)(pak_header.size / sizeof(pak_entry_t));

	if(count == 0)
//		Dialog_MeterChange(t * 4);
		pTotal = t * 4;
	else
//		Dialog_MeterChange((meter + t) * 2);
		pTotal = (meter + t) * 2;

	count++;

	for(int i = 0; i < t; i++) {
		strlwr(pak_entry[i].filename);
		if(strstr(pak_entry[i].filename, ".bsp") && !strstr(pak_entry[i].filename, "b_")) {
//			Dialog_MeterUpdate(meter + i, "Scanning %s", pak_entry[i].filename);
			ProgressFunc((meter + i) * 100 / pTotal);
			readbsp(pak_entry[i].filename, file, pak_entry[i].offset);
		}
		else
//			Dialog_MeterUpdate(meter + i, NULL);
			ProgressFunc((meter + i) * 100 / pTotal);
	}

	meter += t;
}

void getpalfrompak(char *pak_filename) {
	FILE *file = fopen(pak_filename, "rb");

	pak_header_t pak_header;
	pak_entry_t *pak_entry;

	fread(&pak_header, sizeof(pak_header), 1, file);

	pak_entry = (pak_entry_t *)malloc(pak_header.size);

	fseek(file, pak_header.offset, SEEK_SET);
	fread(pak_entry, pak_header.size, 1, file);

	int t = (int)(pak_header.size / sizeof(pak_entry_t));

	for(int i = 0; i < t; i++) {
		strlwr(pak_entry[i].filename);
		if(strstr(pak_entry[i].filename, "palette.lmp")) {
			char buf[768];
			fseek(file, pak_entry[i].offset, SEEK_SET);
			fread(&buf, 768, 1, file);
			FILE *pal_file = fopen("\\palette.lmp", "wb");
			fwrite(&buf, 768, 1, pal_file);
			fclose(pal_file);
		}
	}

	fclose(file);
}

bool paks2wad(char *quake_root, char *wad_output, bool (*ProgFunc)(int p)) {
	char pak0[256], pak1[256];

	ProgressFunc = ProgFunc;

	tex = (tex_t *)malloc(sizeof(tex_t) * 2500);

	char quake_dir[256];
	strcpy(quake_dir, quake_root);
	if(*(quake_dir + strlen(quake_dir) - 1) != '\\')
		strcat(quake_dir, "\\");

	/*
	if(qoole_game == GAME_QUAKE)
		strcat(quake_dir, "\\id1\\");
	else if(qoole_game == GAME_HEXEN2) {
		strcpy(quake_dir, hexen2_pak);
		char *c = strrchr(quake_dir, '\\');
		if(c)
			*c = 0;
	}
	*/

	sprintf(pak0, "%s\\pak0.pak", quake_dir);
	sprintf(pak1, "%s\\pak1.pak", quake_dir);

	FILE *pak0_file = fopen(pak0, "rb");
	FILE *pak1_file = fopen(pak1, "rb");

	if(!pak0_file && !pak1_file)
		return false;

//	Dialog_Meter(1, "Building %s", wad_output);

	if(pak0_file) readpak(pak0_file);
	if(pak1_file) readpak(pak1_file);

//	Dialog_MeterChange(meter + texs);
	pTotal = meter + texs;

	wad_header_t wad_header;
	strncpy(wad_header.magic, "WAD2", 4);
	wad_header.entries = texs;
	wad_header.offset = sizeof(wad_header);

	wad_entry_t *wad_entry;
	wad_entry = (wad_entry_t *)malloc(sizeof(wad_entry_t) * texs);

	FILE *wad_file = fopen(wad_output, "wb");

	unsigned char *buf;
	int size;
	int i;

	for(i = 0; i < texs; i++) {
		if(!strlen(tex[i].name))
			continue;

//		Dialog_MeterUpdate(meter + i, "Adding %s", tex[i].name);
		ProgressFunc((meter + i) * 100 / pTotal);

		size = sizeof(miptex_t) + tex[i].miptex.width * tex[i].miptex.height * 85 / 64;

		wad_entry[i].offset = wad_header.offset;
		wad_entry[i].dsize = size;
		wad_entry[i].size = 0;
		wad_entry[i].type = 0x44;
		wad_entry[i].cmprs = 0;
		strcpy(wad_entry[i].name, tex[i].miptex.name);

		fseek(tex[i].file, tex[i].offset, SEEK_SET);
		fseek(wad_file, wad_header.offset, SEEK_SET);

		buf = (unsigned char *)malloc(size);

		fread(buf, size, 1, tex[i].file);
		fwrite(buf, size, 1, wad_file);

		free(buf);

		wad_header.offset += size;

		TRACE("%s\n", wad_entry[i].name);
	}

	fseek(wad_file, 0, SEEK_SET);
	fwrite(&wad_header, sizeof(wad_header), 1, wad_file);

	fseek(wad_file, wad_header.offset, SEEK_SET);
	fwrite(wad_entry, sizeof(wad_entry_t), texs, wad_file);

	if(pak0_file) fclose(pak0_file);
	if(pak1_file) fclose(pak1_file);
	fclose(wad_file);

//	Dialog_MeterKill();

	return true;
}
