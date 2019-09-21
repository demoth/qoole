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
#include "Sin.h"
#include "Entity.h"
#include "Texture.h"
#include "LCommon.h"
#include "QDraw.h"
#include <direct.h>

Sin::Sin(void) : Game(GetNumUtils()) {
	strcpy(gameName, "Sin");
	strcpy(palDef, "");
	strcpy(defTexture, "aqueduct/wl_aqd_58");
	strcpy(texExt, ".swl");

	cfg = new LConfig(gameName);
	QDraw::OutputText("Loading %s settings... ", gameName);

	strcpy(gameDir, "");
	cfg->RegisterVar("GameDir", gameDir, LVAR_STR);

	setPIndex = 0;
	cfg->RegisterVar("SetParamIndex", &setPIndex, LVAR_INT);

	runUtilsFlag = 0x0001;
	cfg->RegisterVar("RunUtils", &runUtilsFlag, LVAR_INT);

	// QBSP3.
	sprintf(utilsPath[0], "%s\\bin\\", LFile::GetInitDir());
	cfg->RegisterVar("UtilPath1", utilsPath[0], LVAR_STR);

	strcpy(utilsParams[0], "%file%");
	cfg->RegisterVar("UtilParam1", utilsParams[0], LVAR_STR);

	// QVIS3
	sprintf(utilsPath[1], "%s\\bin\\", LFile::GetInitDir());
	cfg->RegisterVar("UtilPath2", utilsPath[1], LVAR_STR);

	strcpy(utilsParams[1], "%file%");
	cfg->RegisterVar("UtilParam2", utilsParams[1], LVAR_STR);

	// QRAD3
	sprintf(utilsPath[2], "%s\\bin\\", LFile::GetInitDir());
	cfg->RegisterVar("UtilPath3", utilsPath[2], LVAR_STR);

	strcpy(utilsParams[2], "%file%");
	cfg->RegisterVar("UtilParam3", utilsParams[2], LVAR_STR);

	// Sin.
	sprintf(utilsPath[3], "Sin.EXE");
	cfg->RegisterVar("GamePath", utilsPath[3], LVAR_STR);
	
	strcpy(utilsParams[3], "+map %file%");
	cfg->RegisterVar("GameParam", utilsParams[3], LVAR_STR);

	QDraw::OutputText("OK.\n");
}

Sin::~Sin(void) {
	cfg->SaveVars();
	delete cfg;
}

bool
Sin::Init(void) {
	if (initialized)
		return true;

	if (!strlen(gameDir) || !LFile::ExistDir(gameDir)) {
		CDirDialog dirDialog;
		dirDialog.m_strTitle = "Locate Sin Folder";

		if (!dirDialog.DoBrowse())
			return false;

		strcpy(gameDir, dirDialog.m_strPath);
	}

	sprintf(baseDir, "%s\\base", gameDir);
	sprintf(mapDir, "%s\\maps", baseDir);

	texDB = new TexDB(this);

	sprintf(texDir, "%s\\textures", baseDir);
	texDB->AddTexDir(texDir);
	/*
	texDB->AddTexDir(pak0File);
	texDB->AddTexDir(pak1File);
	*/

	initialized = true;

	char pak0File[256];
	char pak1File[256];
	sprintf(pak0File, "%s\\pak0.sin", baseDir);
	sprintf(pak1File, "%s\\pak2.sin", baseDir);

	LFile::UseDir(baseDir);
	pak0 = LFile::UsePak(pak0File);
	pak1 = LFile::UsePak(pak1File);

	/*
	// dump .texs
for(int j = 0; j < 2; j++) {
	LPak *pak;
	char pakName[256];
	if(j == 0) {
		pak = pak0;
		strcpy(pakName, "pak0.sin");
	}
	else {
		pak = pak1;
		strcpy(pakName, "pak2.sin");
	}
	for(int i = 0; i < pak->entries; i++) {
		char swl[256];
		strcpy(swl, pak->entry[i].filename);
		strcat(swl, "\r\n");
		if(!strstr(swl, ".swl"))
			continue;

		char dir[256];
		char *c = strchr(swl, '/');
		if(!c)
			continue;
		strcpy(dir, c + 1);
		c = strchr(dir, '/');
		if(!c)
			continue;
		*c = '\0';

		char tex[256];
		sprintf(tex, "TexLists\\Sin\\%s.tex", dir);
		LFile sin;
		sin.Open(tex, LFILE_APPEND);

		char out[256];
//		sprintf(out, "%s\\%s", pakName, swl);

		c = strchr(swl, '/');
		strcpy(out, c + 1);

		sin.Write(out, strlen(out));
	}
}
*/

	if(!LFile::ExistDir(texDir))
		ExtractTextures();

	// Verify game exe.
	if (!LFile::Exist(utilsPath[GetNumUtils() - 1]))
		sprintf(utilsPath[GetNumUtils() - 1], "%s\\Sin.EXE", gameDir);

	return true;
}

WireFrameGeom *
Sin::LoadModel(char *filename) {
	return NULL;
}

#define SIN_PALETTE_SIZE 256*4
#define MIPLEVELS 4

struct sinmiptex_t
{
	char  name[64];
	int   width, height;
	unsigned char palette[SIN_PALETTE_SIZE];
	short   palcrc;
	int   offsets[MIPLEVELS];		// four mip maps stored
	char  animname[64];			// next frame in animation chain
	int   flags;
	int   contents;
	short   value;
	short   direct;
	float animtime;
	float nonlit;      
	short   directangle;
	short   trans_angle;
	float directstyle;
	float translucence; 
	float friction;   
	float restitution;   
	float trans_mag;     
	float color[3];      
};

bool
Sin::LoadTexture(Texture *texture, char *filename, int offset) {
	sinmiptex_t header;
	int size, size2;
	int i;

	LFile file;
	QDraw::OutputText("Loading texture: %s... ", texture->GetName());

	if(!file.Open(filename))
	{
		QDraw::OutputText("Error. Couldn't open the file.\n");
		return false;
	}

	file.Seek(offset);
	file.Read(&header, sizeof(header), 1);

	if(header.width <= 0 || header.width > 1024)
	{
		QDraw::OutputText("Error. The texture width is erroneous.\n", filename);
		return false;
	}
	if(header.height <= 0 || header.height > 1024)
	{
		QDraw::OutputText("Error. The texture height is erroneous.\n", filename);
		return false;
	}
			
	size = header.width * header.height;
	size2 = size * 2;
	
	unsigned char *mip[4];

	texture->mips = 4;
	
	mip[0] = (unsigned char *)malloc(size);
	mip[1] = (unsigned char *)malloc(size / 4);
	mip[2] = (unsigned char *)malloc(size / 16);
	mip[3] = (unsigned char *)malloc(size / 64);

	file.Seek(offset + header.offsets[0]);
	file.Read(mip[0], size, 1);
	file.Seek(offset + header.offsets[1]);
	file.Read(mip[1], size / 4, 1);
	file.Seek(offset + header.offsets[2]);
	file.Read(mip[2], size / 16, 1);
	file.Seek(offset + header.offsets[3]);
	file.Read(mip[3], size / 64, 1);

	unsigned short pal565[256];
	for(i = 0; i < 256; i++) {
		int r, g, b;

		r = header.palette[i * 4 + 0];
		g = header.palette[i * 4 + 1];
		b = header.palette[i * 4 + 2];

		LPalette::GammaCorrect(r, g, b, QDraw::textureGamma);

		pal565[i] = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
	}

	texture->mip[0] = malloc(size2);
	texture->mip[1] = malloc(size2 / 4);
	texture->mip[2] = malloc(size2 / 16);
	texture->mip[3] = malloc(size2 / 64);

	unsigned char *src; 
	unsigned short *dst;

	for(int m = 0; m < 4; m++) {
		src = mip[m];
		dst = (unsigned short *)texture->mip[m];
		for(i = 0; i < size / (1 << (m * 2)); i++)
			*dst++ = pal565[*src++];
	}

	free(mip[0]);
	free(mip[1]);
	free(mip[2]);
	free(mip[3]);

	texture->surface = texture->mip[0];

	texture->realWidth = header.width;
	texture->realHeight = header.height;
	texture->bits = 16;

	return true;
}

void
Sin::ExtractTextures(void) {
	char texdir[MAX_PATH];
	sprintf(texdir, "%s\\textures", baseDir);

	static bool beenHere = false;
	if(beenHere)
		return;
	beenHere = true;

	char text[256];
	sprintf(text, "Some Sin textures not found.  Qoole 99 can extract them\nto '%s' for you.  Do that now?", texdir);
	if(MessageBox(AfxGetMainWnd()->m_hWnd, text, AfxGetAppName(), MB_YESNO) != IDYES)
		return;

	char outname[MAX_PATH];
	char dirname[MAX_PATH];
	char *name, *c;
	int i, count = 0, total = 0;

	for(i = 0; i < pak0->entries; i++)
		if(strstr(pak0->entry[i].filename, ".swl"))
			total++;
	for(i = 0; i < pak1->entries; i++)
		if(strstr(pak1->entry[i].filename, ".swl"))
			total++;

	CProgressWnd progressWnd(AfxGetMainWnd(), "Progress");
	progressWnd.SetText("Extracting Textures");
	progressWnd.NoCancelButton();

	SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_WAIT)));

	for(int j = 0; j < 2; j++) {
	LPak *pak = j ? pak1 : pak0;
	for(i = 0; i < pak->entries; i++) {
		name = pak->entry[i].filename;
		if(strstr(name, ".swl")) {
			sprintf(outname, "%s\\%s", baseDir, name);
			strcpy(dirname, outname);
			c = strrchr(dirname, '/');
			if(!c)
				c = strrchr(dirname, '\\');
			if(c)
				*c = NULL;
			LFile::MakeDir(dirname);
			LFile::Extract(name, outname);

			progressWnd.SetPos(count++ * 100 / total);
			progressWnd.PeekAndPump();
		}
	}
	}

	progressWnd.Clear();
	SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
}

// Game Map Compile Stuff.
const char *Sin::GetUtilName(int index) const {
	static const char *utilNames[] = {"SinQBSP3", "SinQVis3", "SinQRad3", "Sin"};
	ASSERT(index >= 0 && index < 4);
	return utilNames[index];
}
