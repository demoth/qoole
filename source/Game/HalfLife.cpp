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
#include "HalfLife.h"
#include "Entity.h"
#include "Texture.h"
#include "LCommon.h"
#include "QDraw.h"
#include <direct.h>

HalfLife::HalfLife(void) : Game(GetNumUtils()) {
	strcpy(gameName, "Half-Life");
	strcpy(palDef, "");
	strcpy(defTexture, "c1a0_w2");
	strcpy(texExt, "");
	specifyWads = true;

	cfg = new LConfig(gameName);

	QDraw::OutputText("Loading %s settings... ", gameName);

	strcpy(gameDir, "");
	cfg->RegisterVar("GameDir", gameDir, LVAR_STR);

	setPIndex = 0;
	cfg->RegisterVar("SetParamIndex", &setPIndex, LVAR_INT);

	runUtilsFlag = 0x0001;
	cfg->RegisterVar("RunUtils", &runUtilsFlag, LVAR_INT);

	// HL QCSG
	sprintf(utilsPath[0], "%s\\bin\\HLQCSG.EXE", LFile::GetInitDir());
	cfg->RegisterVar("UtilPath1", utilsPath[0], LVAR_STR);

	strcpy(utilsParams[0], "%file%");
	cfg->RegisterVar("UtilParam1", utilsParams[0], LVAR_STR);

	// HL QBSP2
	sprintf(utilsPath[1], "%s\\bin\\HLQBSP2.EXE", LFile::GetInitDir());
	cfg->RegisterVar("UtilPath2", utilsPath[1], LVAR_STR);

	strcpy(utilsParams[1], "%file%");
	cfg->RegisterVar("UtilParam2", utilsParams[1], LVAR_STR);

	// HL VIS
	sprintf(utilsPath[2], "%s\\bin\\HLVIS.EXE", LFile::GetInitDir());
	cfg->RegisterVar("UtilPath3", utilsPath[2], LVAR_STR);

	strcpy(utilsParams[2], "%file%");
	cfg->RegisterVar("UtilParam3", utilsParams[2], LVAR_STR);

	// HL QRAD
	sprintf(utilsPath[3], "%s\\bin\\HLQRAD.EXE", LFile::GetInitDir());
	cfg->RegisterVar("UtilPath4", utilsPath[3], LVAR_STR);

	strcpy(utilsParams[3], "%file%");
	cfg->RegisterVar("UtilParam4", utilsParams[3], LVAR_STR);

	// Half Life
	sprintf(utilsPath[4], "HL.EXE");
	cfg->RegisterVar("GamePath", utilsPath[4], LVAR_STR);
	
	strcpy(utilsParams[4], "-console -dev +map %file%");
	cfg->RegisterVar("GameParam", utilsParams[4], LVAR_STR);

	QDraw::OutputText("OK.\n");
}

HalfLife::~HalfLife(void) {
	cfg->SaveVars();
	delete cfg;
}

bool
HalfLife::Init(void) {
	if (initialized)
		return true;

	if (!strlen(gameDir) || !LFile::ExistDir(gameDir)) {
		CDirDialog dirDialog;
		dirDialog.m_strTitle = "Locate Half-Life Folder";

		if (!dirDialog.DoBrowse())
			return false;

		strcpy(gameDir, dirDialog.m_strPath);
	}

	sprintf(baseDir, "%s\\valve", gameDir);
	sprintf(mapDir, "%s\\maps", baseDir);

	texDB = new TexDB(this);

	strcpy(texDir, baseDir);

	char wadFile[256];
	sprintf(wadFile, "%s\\HalfLife.wad", texDir);
	texDB->AddTexDir(wadFile);
	sprintf(wadFile, "%s\\Xeno.wad", texDir);
	texDB->AddTexDir(wadFile);

	initialized = true;

	char pak0File[256];
	sprintf(pak0File, "%s\\pak0.pak", baseDir);

	LFile::UseDir(baseDir);
	pak0 = LFile::UsePak(pak0File);

	// Verify game exe.
	if (!LFile::Exist(utilsPath[GetNumUtils() - 1]))
		sprintf(utilsPath[GetNumUtils() - 1], "%s\\HL.EXE", gameDir);

	return true;
}

WireFrameGeom *
HalfLife::LoadModel(char *filename) {
	return NULL;
}

bool
HalfLife::LoadTexture(Texture *texture, char *filename, int offset) {
	texture_header_t	header;
	int					size, size2;
	int					i;

	LFile file;
	QDraw::OutputText("Loading texture: %s... ", texture->GetName());
	if(!file.Open(filename))
	{
		QDraw::OutputText("Error. Couldn't open file.\n");
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
	short palCount;
	struct {
		unsigned char r, g, b;
	} pal[256];

	texture->mips = 4;
	
	mip[0] = (unsigned char *)malloc(size);
	mip[1] = (unsigned char *)malloc(size / 4);
	mip[2] = (unsigned char *)malloc(size / 16);
	mip[3] = (unsigned char *)malloc(size / 64);

	file.Seek(offset + header.mip1);
	file.Read(mip[0], size, 1);
	file.Seek(offset + header.mip2);
	file.Read(mip[1], size / 4, 1);
	file.Seek(offset + header.mip3);
	file.Read(mip[2], size / 16, 1);
	file.Seek(offset + header.mip4);
	file.Read(mip[3], size / 64, 1);

	file.Read(&palCount, sizeof(palCount));
	file.Read(pal, 768);

	unsigned short pal565[256];
	for(i = 0; i < 256; i++) {
		int r, g, b;

		r = pal[i].r;
		g = pal[i].g;
		b = pal[i].b;

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

	QDraw::OutputText("OK.\n");
	return true;
}

void HalfLife::ExtractTextures(void) {
	/*
	char wadFile[256];
	char wadDir[256];
	sprintf(wadDir, "%s\\wads", LFile::GetInitDir());
	sprintf(wadFile, "%s\\Quake.wad", wadDir);

	if(LFile::Exist(wadFile))
		return;

	if(MessageBox(NULL, "Extract Quake Textures?", 
		AfxGetAppName(), MB_YESNO) == IDNO)
		return;

	if(!LFile::ExistDir(wadDir))
		_mkdir(wadDir);

	CProgressFunc progFunc("Extracting");
	paks2wad(baseDir, wadFile, CProgressFunc::SetPos);
	*/
}

// Game Map Compile Stuff.
const char *HalfLife::GetUtilName(int index) const {
	static const char *utilNames[] = {
		"QCSG", "QBsp2", "Vis", "QRad", "Half-Life"
	};

	ASSERT(index >= 0 && index < 5);
	return utilNames[index];
}

//===== Leak file =====

const char *HalfLife::GetLeakFile(const char *docName) const {
	static char leakPath[256];

	ASSERT(docName != NULL);

	if (!LFile::ExistDir(mapDir))
		return NULL;

	sprintf(leakPath, "%s\\%s.lin", mapDir, docName);

	return leakPath;
}

Object *HalfLife::LoadLeakFile(const char *docName) const {
	LFile leakFile;
	QDraw::OutputText("Loading leak information: %s... ", GetLeakFile(docName));

	if (!leakFile.Open(GetLeakFile(docName)))
	{
		QDraw::OutputText("Error.\nThere's no leak file.\n");
		return NULL;
	}

	char *inLine;
	float x, y, z;
	Vector3d v1, v2;
	Vector3d posVec;

	// Read the first point.
	while (!leakFile.EndOfFile()) {
		if ((inLine = leakFile.GetNextLine()) != NULL &&
            sscanf(inLine, "%f %f %f", &x, &y, &z) == 3) {
			v1.NewVector(x, y, z);
			break;
		}
	}

	Object *rtnVal = new Object;
	Object *pNewSeg;
	Geometry *pSegBrush;

	// Read the rest.
	while (!leakFile.EndOfFile()) {
		if ((inLine = leakFile.GetNextLine()) != NULL &&
			sscanf(inLine, "%f %f %f", &x, &y, &z) == 3) {
			v2.NewVector(x, y, z);
			pSegBrush = Geometry::MakeSegment(v1, v2);
			pSegBrush->CenterGeometry(posVec);
			pNewSeg = new Object;
			pNewSeg->SetBrush(pSegBrush);
			pNewSeg->SetPosition(posVec);
			rtnVal->AddChild(*pNewSeg, false);
			v1 = v2;
		}
	}

	if (rtnVal->GetNumChildren() == 0) {
		delete rtnVal;
		rtnVal = NULL;
	}
	else {
		rtnVal->SetBoundRadius();
	}

	QDraw::OutputText("OK.\n");
	return rtnVal;
}