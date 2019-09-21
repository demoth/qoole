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
#include "Heretic2.h"
#include "Entity.h"
#include "Texture.h"
#include "LCommon.h"
#include "QDraw.h"
#include <direct.h>

Heretic2::Heretic2(void) : Game(GetNumUtils()) {
	strcpy(gameName, "Heretic 2");
	strcpy(palDef, "");
	strcpy(defTexture, "Andoria/blslate");
	strcpy(texExt, ".m8");

	cfg = new LConfig(gameName);
	QDraw::OutputText("Loading %s settings... ", gameName);
	strcpy(gameDir, "");
	cfg->RegisterVar("GameDir", gameDir, LVAR_STR);

	setPIndex = 0;
	cfg->RegisterVar("SetParamIndex", &setPIndex, LVAR_INT);

	runUtilsFlag = 0x0001;
	cfg->RegisterVar("RunUtils", &runUtilsFlag, LVAR_INT);

	// QBSP3.
	sprintf(utilsPath[0], "%s\\bin\\QBSP3.EXE", LFile::GetInitDir());
	cfg->RegisterVar("UtilPath1", utilsPath[0], LVAR_STR);

	strcpy(utilsParams[0], "%file%");
	cfg->RegisterVar("UtilParam1", utilsParams[0], LVAR_STR);

	// QVIS3
	sprintf(utilsPath[1], "%s\\bin\\QVIS3.EXE", LFile::GetInitDir());
	cfg->RegisterVar("UtilPath2", utilsPath[1], LVAR_STR);

	strcpy(utilsParams[1], "%file%");
	cfg->RegisterVar("UtilParam2", utilsParams[1], LVAR_STR);

	// QRAD3
	sprintf(utilsPath[2], "%s\\bin\\QRAD3.EXE", LFile::GetInitDir());
	cfg->RegisterVar("UtilPath3", utilsPath[2], LVAR_STR);

	strcpy(utilsParams[2], "%file%");
	cfg->RegisterVar("UtilParam3", utilsParams[2], LVAR_STR);

	// Heretic 2
	sprintf(utilsPath[3], "Heretic2.EXE");
	cfg->RegisterVar("GamePath", utilsPath[3], LVAR_STR);
	
	strcpy(utilsParams[3], "+map %file%");
	cfg->RegisterVar("GameParam", utilsParams[3], LVAR_STR);
	QDraw::OutputText("OK.\n");
}

Heretic2::~Heretic2(void) {
	cfg->SaveVars();
	delete cfg;
}

bool
Heretic2::Init(void) {
	if (initialized)
		return true;

	if (!strlen(gameDir) || !LFile::ExistDir(gameDir)) {
		CDirDialog dirDialog;
		dirDialog.m_strTitle = "Locate Heretic 2 Folder";

		if (!dirDialog.DoBrowse())
			return false;

		strcpy(gameDir, dirDialog.m_strPath);
	}

	sprintf(baseDir, "%s\\base", gameDir);
	sprintf(mapDir, "%s\\maps", baseDir);

	texDB = new TexDB(this);
	sprintf(texDir, "%s\\textures", baseDir);
	texDB->AddTexDir(texDir);

	initialized = true;

	char pak0File[256];
	char pak1File[256];

	sprintf(pak0File, "%s\\Htic2-0.pak", baseDir);
	sprintf(pak1File, "%s\\Htic2-1.pak", baseDir);

	LFile::UseDir(baseDir);
	pak0 = LFile::UsePak(pak0File);
	pak1 = LFile::UsePak(pak1File);

/*
	for(int i = 0; i < pak0->entries; i++)
		if(strstr(pak0->entry[i].filename, "textures/"))
			pak0->Extract(pak0->entry[i].filename, pak0->entry[i].filename);
*/

	// Verify game exe.
	if (!LFile::Exist(utilsPath[GetNumUtils() - 1]))
		sprintf(utilsPath[GetNumUtils() - 1], "%s\\Heretic2.EXE", gameDir);

	if(!LFile::ExistDir(texDir))
		ExtractTextures();

	return true;
}

WireFrameGeom *
Heretic2::LoadModel(char *filename) {
	int verts, edges;
	vec3_t *vert;
	edgenum_t *edgenum;
	int i, j, v;
	int v1, v2;
	edge_t *edge;
	char ident[8];
	md2header_t md2header;
	frameinfo_t *fi;
	int *glcmds, *cmd;
	int index;
	/*
	float xMin = 1000.0f, yMin = 1000.0f, zMin = 1000.0f;
	float xMax = 0.0f, yMax = 0.0f, zMax = 0.0f;
	*/
	
	verts = 0;

	LFile file;
	QDraw::OutputText("Loading model: %s... ", filename);
	if(!file.Open(filename))
	{
		QDraw::OutputText("Error. Coudln't open file.\n");
		return NULL;
	}

	file.Read(&ident, 4, 1);
	ident[4] = 0;
	file.Seek(0);

	file.Read(&md2header, sizeof(md2header), 1);

	fi = (frameinfo_t *)malloc(md2header.framesize);
	glcmds = new int[md2header.num_glcmds];

	file.Seek(md2header.ofs_glcmds);
	file.Read(glcmds, md2header.num_glcmds, sizeof(int));

	file.Seek(md2header.ofs_frames);
	file.Read(fi, md2header.framesize, 1);

	file.Close();

	edge = new edge_t[md2header.num_tris * 3];
	vert = new vec3_t[md2header.num_tris * 3 /* + 8 */];

	edges = 0;

	cmd = glcmds;
	while(*cmd) {
		int type;

		if(*cmd > 0) {
			verts = *cmd++;
			type = 0;
		}
		else {
			verts = -(*cmd++);
			type = 1;
		}

		for(i = 0; i < verts; i++) {
			cmd++;	// s
			cmd++;	// t

			index = *cmd++;

			vert[i].x = fi->verts[index].x * fi->scale.x + fi->origin.x;
			vert[i].y = fi->verts[index].y * fi->scale.y + fi->origin.y;
			vert[i].z = fi->verts[index].z * fi->scale.z + fi->origin.z;

			/*
			if(vert[i].x < xMin) xMin = vert[i].x;
			if(vert[i].y < yMin) yMin = vert[i].y;
			if(vert[i].z < zMin) zMin = vert[i].z;
			if(vert[i].x > xMax) xMax = vert[i].x;
			if(vert[i].y > yMax) yMax = vert[i].y;
			if(vert[i].z > zMax) zMax = vert[i].z;
			*/
		}

		for(i = 0; i < verts - 2; i++) {
			if(type == 0) {
				edge[edges].vert[0] = vert[i];
				edge[edges].vert[1] = vert[i + 1];
				edges++;
				edge[edges].vert[0] = vert[i + 2];
				edge[edges].vert[1] = vert[i];
				edges++;
			}
			else {
				edge[edges].vert[0] = vert[0];
				edge[edges].vert[1] = vert[i + 1];
				edges++;
				edge[edges].vert[0] = vert[0];
				edge[edges].vert[1] = vert[i + 2];
				edges++;
			}
			edge[edges].vert[0] = vert[i + 1];
			edge[edges].vert[1] = vert[i + 2];
			edges++;
		}
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

	free(fi);
	delete glcmds;
	delete [] edge;
	delete [] edgenum;


	/*
	vert[verts].x = xMin;
	vert[verts].y = yMin;
	vert[verts].z = zMin;
	int minNum = verts++;

	vert[verts].x = xMin + 10;
	vert[verts].y = yMin;
	vert[verts].z = zMin;
	newEdge = new Edge3d(minNum, verts++);
	edgeList.AppendNode(*newEdge);

	vert[verts].x = xMin;
	vert[verts].y = yMin + 10;
	vert[verts].z = zMin;
	newEdge = new Edge3d(minNum, verts++);
	edgeList.AppendNode(*newEdge);
	
	vert[verts].x = xMin;
	vert[verts].y = yMin;
	vert[verts].z = zMin + 10;
	newEdge = new Edge3d(minNum, verts++);
	edgeList.AppendNode(*newEdge);
	
	vert[verts].x = xMax;
	vert[verts].y = yMax;
	vert[verts].z = zMax;
	int maxNum = verts++;

	vert[verts].x = xMax - 10;
	vert[verts].y = yMax;
	vert[verts].z = zMax;
	newEdge = new Edge3d(maxNum, verts++);
	edgeList.AppendNode(*newEdge);

	vert[verts].x = xMax;
	vert[verts].y = yMax - 10;
	vert[verts].z = zMax;
	newEdge = new Edge3d(maxNum, verts++);
	edgeList.AppendNode(*newEdge);

	vert[verts].x = xMax;
	vert[verts].y = yMax;
	vert[verts].z = zMax - 10;
	newEdge = new Edge3d(maxNum, verts++);
	edgeList.AppendNode(*newEdge);
	*/
	
	QDraw::OutputText("OK.\n");
	// it is WireFrameGeom's job to delete [] vert
	return new WireFrameGeom(verts, (GeomWFVertex *)vert, edgeList);
}

void
Heretic2::ExtractTextures(void) {
	char texdir[MAX_PATH];
	sprintf(texdir, "%s\\textures", baseDir);

	static bool beenHere = false;
	if(beenHere)
		return;
	beenHere = true;

	char text[256];
	sprintf(text, "Some Heretic 2 textures not found.  Qoole 99 can extract them\nto '%s' for you.  Do that now?", texdir);
	if(MessageBox(AfxGetMainWnd()->m_hWnd, text, AfxGetAppName(), MB_YESNO) != IDYES)
		return;

	char outname[MAX_PATH];
	char dirname[MAX_PATH];
	char *name, *c;
	int i, count = 0, total = 0;

	for(i = 0; i < pak0->entries; i++)
		if(strstr(pak0->entry[i].filename, "textures/"))
			if(strstr(pak0->entry[i].filename, ".m8"))
				total++;

	CProgressWnd progressWnd(AfxGetMainWnd(), "Progress");
	progressWnd.SetText("Extracting Textures");
	progressWnd.NoCancelButton();

	SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_WAIT)));

	for(i = 0; i < pak0->entries; i++) {
		name = pak0->entry[i].filename;
		if(!strstr(name, "textures/"))
			continue;
		if(strstr(name, ".m8")) {
			sprintf(outname, "%s\\%s", baseDir, name);
			strcpy(dirname, outname);
			c = strrchr(dirname, '/');
			if(!c)
				c = strrchr(dirname, '\\');
			if(c)
				*c = NULL;
			LFile::MakeDir(dirname);
			if(!LFile::Extract(name, outname)) {
				LError("Error during extraction (probably insufficient disk space).");
				break;
			}

			progressWnd.SetPos(count++ * 100 / total);
			progressWnd.PeekAndPump();
		}
	}

	progressWnd.Clear();
	SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
}

typedef struct h2miptex_s
{
	int			identifier;						// 4 bytes: 02 00 00 00		4		0
	char		name[32];						//							32		4
	unsigned	width[16];						// 2 8's maybe?				64		36
	unsigned	height[16];						// 2 8's maybe?				64		100
	unsigned	offsets[16];					//							64		164
	char		animname[32];					//							32		228
	unsigned char palette[768];					//							768		260
	int			flags;							//							4
	int			contents;						//							4
	int			value;							//							4
} h2miptex_t;

bool
Heretic2::LoadTexture(Texture *texture, char *filename, int offset) {
	h2miptex_t header;
	int size, size2;
	int i;

	LFile file;
	QDraw::OutputText("Loading texture: %s... ", texture->GetName());

	if(!file.Open(filename))
	{
		QDraw::OutputText("Error. Couldn't open file.\n");
		return false;
	}

	file.Seek(offset);
	file.Read(&header, sizeof(header), 1);

	if(header.width[0] <= 0 || header.width[0] > 1024)
	{
		QDraw::OutputText("Error. The texture width is erroneous.\n", filename);
		return false;
	}
	if(header.height[0] <= 0 || header.height[0] > 1024)
	{
		QDraw::OutputText("Error. The texture height is erroneous.\n", filename);
		return false;
	}
			
	size = header.width[0] * header.height[0];
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

		r = header.palette[i * 3 + 0];
		g = header.palette[i * 3 + 1];
		b = header.palette[i * 3 + 2];

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

	texture->realWidth = header.width[0];
	texture->realHeight = header.height[0];
	texture->bits = 16;

	QDraw::OutputText("OK.\n");
	return true;
}

//===== Additional properties pages. =====
#include "resource.h"
#include "QPropSht.h"

DWORD Heretic2::GetExtraPropPages(void) const {
	return (PROPPAGE_ADDSURFACE | PROPPAGE_ADDCONTENT);
}

//===== Game Map Compile Stuff =====
const char *Heretic2::GetUtilName(int index) const {
	static const char *utilNames[] = {"QBSP3", "QVis3", "QRad3", "Heretic 2"};
	ASSERT(index >= 0 && index < 4);
	return utilNames[index];
}

const char *Heretic2::GetLeakFile(const char *docName) const {
	static char leakPath[256];

	ASSERT(docName != NULL);

	if (!LFile::ExistDir(mapDir))
		return NULL;

	sprintf(leakPath, "%s\\%s.lin", mapDir, docName);

	return leakPath;
}

Object *Heretic2::LoadLeakFile(const char *docName) const {
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