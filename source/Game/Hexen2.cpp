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
#include "Hexen2.h"
#include "Entity.h"
#include "Texture.h"
#include "LCommon.h"
#include "QDraw.h"
#include <direct.h>

Hexen2::Hexen2(void) : Game(GetNumUtils()) {
	strcpy(gameName, "Hexen 2");
	strcpy(palDef, "Hexen2.pal");
	strcpy(defTexture, "rtex003");
	strcpy(texExt, "");
	exportWad = true;

	strcpy(palName, palDef);

	cfg = new LConfig(gameName);
	QDraw::OutputText("Loading %s settings... ", gameName);

	strcpy(gameDir, "");
	cfg->RegisterVar("GameDir", gameDir, LVAR_STR);

	setPIndex = 0;
	cfg->RegisterVar("SetParamIndex", &setPIndex, LVAR_INT);

	runUtilsFlag = 0x0001;
	cfg->RegisterVar("RunUtils", &runUtilsFlag, LVAR_INT);

	// H2QBSP
	sprintf(utilsPath[0], "%s\\bin\\H2QBSP.EXE", LFile::GetInitDir());
	cfg->RegisterVar("UtilPath1", utilsPath[0], LVAR_STR);

	strcpy(utilsParams[0], "%file%");
	cfg->RegisterVar("UtilParam1", utilsParams[0], LVAR_STR);

	// H2LIGHT
	sprintf(utilsPath[1], "%s\\bin\\H2LIGHT.EXE", LFile::GetInitDir());
	cfg->RegisterVar("UtilPath2", utilsPath[1], LVAR_STR);

	strcpy(utilsParams[1], "%file%");
	cfg->RegisterVar("UtilParam2", utilsParams[1], LVAR_STR);

	// VIS
	sprintf(utilsPath[2], "%s\\bin\\H2VIS.EXE", LFile::GetInitDir());
	cfg->RegisterVar("UtilPath3", utilsPath[2], LVAR_STR);

	strcpy(utilsParams[2], "%file%");
	cfg->RegisterVar("UtilParam3", utilsParams[2], LVAR_STR);

	// Hexen2
	sprintf(utilsPath[3], "H2.EXE");
	cfg->RegisterVar("GamePath", utilsPath[3], LVAR_STR);
	
	strcpy(utilsParams[3], "+map %file%");
	cfg->RegisterVar("GameParam", utilsParams[3], LVAR_STR);

	QDraw::OutputText("OK.\n");
}

Hexen2::~Hexen2(void) {
	cfg->SaveVars();
	delete cfg;
}

bool
Hexen2::Init(void) {
	if (initialized)
		return true;

	if (!strlen(gameDir) || !LFile::ExistDir(gameDir)) {
		CDirDialog dirDialog;
		dirDialog.m_strTitle = "Locate Hexen 2 Folder";

		if (!dirDialog.DoBrowse())
			return false;

		strcpy(gameDir, dirDialog.m_strPath);
	}

	sprintf(baseDir, "%s\\data1", gameDir);
	sprintf(mapDir, "%s\\maps", baseDir);

	texDB = new TexDB(this);

	sprintf(texDir, "%s\\wads", LFile::GetInitDir());

	char wadFile[256];
	sprintf(wadFile, "%s\\Hexen2.wad", texDir);
	texDB->AddTexDir(wadFile);

	initialized = true;

	char pak0File[256];
	char pak1File[256];

	sprintf(pak0File, "%s\\pak0.pak", baseDir);
	sprintf(pak1File, "%s\\pak1.pak", baseDir);

	LFile::UseDir(baseDir);
	pak0 = LFile::UsePak(pak0File);
	pak1 = LFile::UsePak(pak1File);

	// Verify game exe.
	if (!LFile::Exist(utilsPath[GetNumUtils() - 1]))
		sprintf(utilsPath[GetNumUtils() - 1], "%s\\H2.EXE", gameDir);

	ExtractTextures();

	return true;
}

WireFrameGeom *
Hexen2::LoadModel(char *filename) {
	int verts, edges;
	vec3_t *vert;
	edgenum_t *edgenum;
	int i, j, v;
	int v1, v2;
	edge_t *edge;
	char ident[8];
	mdlheader_t mdlheader;
	int group;
	int type;
	itriangle_t *tri;
	trivertx_t *trivert;

	verts = 0;

	LFile file;
	QDraw::OutputText("Loading model: %s... ", filename);
	if(!file.Open(filename))
	{
		QDraw::OutputText("Error. Couldn't open the file.\n");
		return NULL;
	}

	file.Read(&ident, 4, 1);
	ident[4] = 0;
	file.Seek(0);

	file.Read(&mdlheader, sizeof(mdlheader_t), 1);

	for(i = 0; i < mdlheader.numskins; i++) {
		file.Read(&group, sizeof(group), 1);
		if(group) {
			file.Read(&group, sizeof(group), 1);
			file.SeekCur(group * (4 + mdlheader.skinwidth * mdlheader.skinheight));
		}
		else
			file.SeekCur(mdlheader.skinwidth * mdlheader.skinheight);
	}

	file.SeekCur(mdlheader.numverts * 12);

	tri = new itriangle_t[mdlheader.numtris];
	trivert = new trivertx_t[mdlheader.numverts];
	edge = new edge_t[mdlheader.numtris * 3];
	vert = new vec3_t[mdlheader.numtris * 3];

	file.Read(tri, sizeof(itriangle_t), mdlheader.numtris);

	file.Read(&type, sizeof(type), 1);
	if(type)
		file.SeekCur(8 + 4 * mdlheader.numframes);

	file.SeekCur(24);
	file.Read(trivert, sizeof(trivertx_t), mdlheader.numverts);
	for(i = 0; i < mdlheader.numverts; i++) {
		vert[i].x = trivert[i].x * mdlheader.scale.x + mdlheader.origin.x;
		vert[i].y = trivert[i].y * mdlheader.scale.y + mdlheader.origin.y;
		vert[i].z = trivert[i].z * mdlheader.scale.z + mdlheader.origin.z;
	}

	edges = 0;

	for(i = 0; i < mdlheader.numtris; i++) {
		edge[edges].vert[0] = vert[tri[i].vertices[0]];
		edge[edges].vert[1] = vert[tri[i].vertices[1]];
		edges++;
		edge[edges].vert[0] = vert[tri[i].vertices[1]];
		edge[edges].vert[1] = vert[tri[i].vertices[2]];
		edges++;
		edge[edges].vert[0] = vert[tri[i].vertices[2]];
		edge[edges].vert[1] = vert[tri[i].vertices[0]];
		edges++;
	}

	edgenum = new edgenum_t[edges];

	v = 0;
	for(i = 0; i < edges; i++) {
		v1 = -1;
		v2 = -1;
		for(j = 0; j < v; j++) {
			if(vert[j].x == edge[i].vert[0].x &&
				vert[j].y == edge[i].vert[0].y && 
				vert[j].z == edge[i].vert[0].z)
					v1 = j;
			if(vert[j].x == edge[i].vert[1].x &&
				vert[j].y == edge[i].vert[1].y && 
				vert[j].z == edge[i].vert[1].z)
					v2 = j;
		}

		if(v1 == -1) 
			v1 = v++;
		if(v2 == -1)
			v2 = v++;

		vert[v1] = edge[i].vert[0];
		vert[v2] = edge[i].vert[1];

		edgenum[i].v1 = v1;
		edgenum[i].v2 = v2;
	}

	verts = v;
	ASSERT(verts > 0);

	LinkList<Edge3d> edgeList;
	Edge3d *newEdge;

	for(i = 0; i < edges; i++) {
		for(j = 0; j < i; j++) {
			if(edgenum[i].v1 == edgenum[j].v1 && edgenum[i].v2 == edgenum[j].v2)
				continue;
			if(edgenum[i].v2 == edgenum[j].v1 && edgenum[i].v1 == edgenum[j].v2)
				continue;
		}

		newEdge = new Edge3d(edgenum[i].v1, edgenum[i].v2);
	    edgeList.AppendNode(*newEdge);
	}

	delete [] tri;
	delete [] trivert;
	delete [] edge;
	delete [] edgenum;

	QDraw::OutputText("OK.\n");
	// it is WireFrameGeom's job to delete [] vert
	return new WireFrameGeom(verts, (GeomWFVertex *)vert, edgeList);
}

bool
Hexen2::LoadTexture(Texture *texture, char *filename, int offset) {
	texture_header_t header;
	int size;

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

	texture->mips = 4;
	texture->mip[0] = malloc(size);
	texture->mip[1] = malloc(size / 4);
	texture->mip[2] = malloc(size / 16);
	texture->mip[3] = malloc(size / 64);

	file.Seek(offset + header.mip1);
	file.Read(texture->mip[0], size, 1);
	file.Seek(offset + header.mip2);
	file.Read(texture->mip[1], size / 4, 1);
	file.Seek(offset + header.mip3);
	file.Read(texture->mip[2], size / 16, 1);
	file.Seek(offset + header.mip4);
	file.Read(texture->mip[3], size / 64, 1);

	texture->surface = texture->mip[0];

	texture->realWidth = header.width;
	texture->realHeight = header.height;
	texture->bits = 8;

	QDraw::OutputText("OK.\n");
	return true;
}

bool paks2wad(char *quake_root, char *wad_output, bool (*ProgFunc)(int p));

void
Hexen2::ExtractTextures(void) {
	char wadDir[256];
	char wadFile[256];
	sprintf(wadDir, "%s\\wads", LFile::GetInitDir());
	sprintf(wadFile, "%s\\Hexen2.wad", wadDir);

	if(LFile::Exist(wadFile))
		return;

	if(MessageBox(NULL, "Extract Hexen 2 Textures?", 
		AfxGetAppName(), MB_YESNO) == IDNO)
		return;
	
	if(!LFile::ExistDir(wadDir))
		_mkdir(wadDir);

	sprintf(baseDir, "%s\\data1", gameDir);

	CProgressFunc progFunc("Extracting");
	paks2wad(baseDir, wadFile, CProgressFunc::SetPos);
}

//===== Map Compile Stuff =====
const char *Hexen2::GetUtilName(int index) const {
	static const char *utilNames[] = {"H2QBSP", "H2Light", "H2Vis", "Hexen 2"};
	ASSERT(index >= 0 && index < 4);
	return utilNames[index];
}

const char *Hexen2::GetLeakFile(const char *docName) const {
	static char leakPath[256];

	ASSERT(docName != NULL);

	if (!LFile::ExistDir(mapDir))
		return NULL;

	sprintf(leakPath, "%s\\%s.pts", mapDir, docName);

	return leakPath;
}

Object *Hexen2::LoadLeakFile(const char *docName) const {
	LFile leakFile;
	QDraw::OutputText("Loading leak information: %s... ", GetLeakFile(docName));
	
	if (!leakFile.Open(GetLeakFile(docName)))
	{
		QDraw::OutputText("Error. There's no leak file.\n");
		return NULL;
	}

	char *inLine;
	float x, y, z;
	Vector3d v0, v1, v2;
	Vector3d posVec;
	Line currLine;
	bool firstSeg = true;

	// Read the first point.
	while (!leakFile.EndOfFile()) {
		if ((inLine = leakFile.GetNextLine()) != NULL &&
            sscanf(inLine, "%f %f %f", &x, &y, &z) == 3) {
			v0.NewVector(x, y, z);
			v1 = v0;
			break;
		}
	}

	Object *rtnVal = new Object;
	Object *pNewSeg;
	Geometry *pSegBrush;

	// Read the rest.
	int count = 0;
	while (!leakFile.EndOfFile()) {
		if ((inLine = leakFile.GetNextLine()) != NULL &&
			sscanf(inLine, "%f %f %f", &x, &y, &z) == 3) {
			v2.NewVector(x, y, z);
			if (firstSeg) {
				Vector3d norm;
				norm.SubVector(v2, v1);
				currLine.NewLine(v1, norm);
				firstSeg = false;
			}
			if (!currLine.IsOnLine(v2, 1.0f)) {
				pSegBrush = Geometry::MakeSegment(v0, v1);
				pSegBrush->CenterGeometry(posVec);
				pNewSeg = new Object;
				pNewSeg->SetBrush(pSegBrush);
				pNewSeg->SetPosition(posVec);
				rtnVal->AddChild(*pNewSeg, false);
				v0 = v1;
				firstSeg = true;
				if (++count > 300)
					break;
			}
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
