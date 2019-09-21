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
#include "LCommon.h"


// ========== Common ==========

void
LError(const char *fmt, ...) {
	va_list arglist;
	char buf[256] = "";
	
	if(fmt) {
		va_start(arglist, fmt);
		vsprintf(buf, fmt, arglist);
		va_end(arglist);
	}

	MessageBox(NULL, buf, "Qoole 99 Error", MB_OK | MB_TOPMOST);
}

void
LFatal(const char *fmt, ...) {
	va_list arglist;
	char buf[256] = "";
	
	if(fmt) {
		va_start(arglist, fmt);
		vsprintf(buf, fmt, arglist);
		va_end(arglist);
	}

	MessageBox(NULL, buf, "Qoole 99 Fatal Error", MB_OK | MB_TOPMOST);

	AfxGetMainWnd()->PostMessage(WM_QUIT);
}


// ========== LFile ==========
#include "direct.h"

char *LFile::dirs[32];
int LFile::numdirs = 0;
LPak *LFile::paks[32];
int LFile::numpaks = 0;

LFile::LFile(void) {
	file = NULL;
	pak = NULL;
	lineNum = 0;
}

LFile::~LFile(void) {
	Close();
}

bool
LFile::Exist(const char *filename) {
	bool ret;
	LFile file;
	ret = file.Open(filename);
	file.Close();
	return ret;
}

bool
LFile::ExistDir(const char *dirname) {
	LFindDirs finddirs(dirname);
	if(finddirs.Next())
		return true;
	else
		return false;
}

bool
LFile::Open(const char *filename, int mode) {
	int i, j;
	char buf[256];

	file = NULL;

	if(!filename || !strlen(filename))
		return false;

	if(mode & LFILE_READ && !(mode & LFILE_FROMPAK))
		file = fopen(filename, "rb");
	if(mode & LFILE_WRITE)
		file = fopen(filename, "wb");
	if(mode & LFILE_APPEND)
		file = fopen(filename, "ab");

	if(!file && !(mode & LFILE_FROMPAK)) {
		for(i = 0; i < numdirs; i++) {
			sprintf(buf, "%s\\%s", dirs[i], filename);
			file = fopen(buf, "rb");
			if(file)
				break;
		}
	}

	if(file) {
		fseek(file, 0, SEEK_END);
		fileSize = ftell(file);
		fseek(file, 0, SEEK_SET);
		return true;
	}

	char searchName[256];
	char *c;
	strcpy(searchName, filename);
	while(c = strchr(searchName, '\\'))
		*c = '/';

	for(i = 0; i < numpaks; i++) {
		pak = paks[i];
		for(j = 0; j < pak->entries; j++) {
			if(!strcmpi(filename, pak->entry[j].filename)) {
				pakOffset = pak->entry[j].offset;
				fileSize = pak->entry[j].size;
				fileOffset = 0;
				pak->Seek(pakOffset);
				return true;
			}
		}
	}

	return false;
}

void
LFile::Close(void) {
	if(file) {
		fclose(file);
		file = NULL;
	}
}

int
LFile::Size(void) {
	return fileSize;
}

int
LFile::Tell(void) {
	return ftell(file);
}

int
LFile::Seek(int offset) {
	if(file)
		return fseek(file, offset, SEEK_SET);

	if(pak) {
		fileOffset = offset;
		return pak->Seek(pakOffset + fileOffset);
	}

	return -1;
}

int
LFile::SeekCur(int offset) {
	if(file)
		return fseek(file, offset, SEEK_CUR);

	if(pak) {
		fileOffset += offset;
		return pak->Seek(pakOffset + fileOffset);
	}

	return -1;
}

int
LFile::Read(void *buf, int size, int count) {
	if(file)
		return fread(buf, size, count, file);

	if(pak) {
		Seek(fileOffset);
		fileOffset += size * count;
		if(fileOffset > fileSize)
			size -= fileOffset - fileSize;
		return pak->Read(buf, size, count);
	}

	return 0;
}

int
LFile::Write(void *buf, int size, int count) {
	if(file)
		return fwrite(buf, size, count, file);

	return 0;
}

bool
LFile::EndOfFile(void) {
	if(file)
		return feof(file) != 0;

	if(pak)
		return fileOffset > fileSize;

	return true;
}

char *
LFile::GetLine(void) {
	if(file) {
		char *c;
		for(c = data; isspace(*c); c++);
		return c;
	}

	if(pak)
		return pak->GetLine();

	return NULL;
}

char *
LFile::GetNextLine(void) {
	if(file) {
		if(fgets(data, 1024, file)) {
			lineNum++;
			return GetLine();
		}
		else
			return NULL;
	}

	if(pak) {
		lineNum++;
		pak->Seek(pakOffset + fileOffset);
		char *line = pak->GetNextLine();
		fileOffset = pak->Tell() - pakOffset;
		if(fileOffset > fileSize)
			return NULL;
		return line;
	}

	return NULL;
}

int
LFile::GetLineNumber(void) {
	return lineNum;
}

char *
LFile::Search(const char *pattern) {
	while(GetNextLine())
		if(!strncmp(GetLine(), pattern, strlen(pattern)))
			return GetLine();

 	return NULL;
}

bool
LFile::Extract(char *filename, char *dst) {
	LFile rFile, wFile;

	if(!rFile.Open(filename, LFILE_FROMPAK))
		return false;

	if(!wFile.Open(dst, LFILE_WRITE))
		return false;

	int size = rFile.Size();

	char *buf = new char[size];
	rFile.Read(buf, size);
	int rtn = wFile.Write(buf, size);
	delete buf;

	return (rtn != 0);
}

void
LFile::MakeDir(char *dirname) {
	char dir[MAX_PATH];
	char test[MAX_PATH];
	char *c = dir;

	strcpy(dir, dirname);
	while(c) {
		c = strchr(dir, '/');
		if(c)
			*c = '\\';
	}

	c = dir;

	if(ExistDir(dir))
		return;

	if(strstr(dir, ":"))
		c = strstr(c, "\\");
	if(!c)
		return;
	c++;
	if(!*c)
		return;
	while(true) {
		c = strstr(c, "\\");
		if(!c)
			break;
		c++;
		if(!*c)
			break;
 		strncpy(test, dir, c - dir - 1);
		test[c - dir - 1] = '\0';
		mkdir(test);
	}
	mkdir(dir);
}

void
LFile::UseDir(const char *dirname) {
	char cwd[128] = "";
	char newDir[256];
	if(!strchr(dirname, ':')) {
		_getcwd(cwd, 127);
		strcat(cwd, "\\");
	}
	sprintf(newDir, "%s%s", cwd, dirname);

	for(int i = 0; i < numdirs; i++)
		if(!strcmpi(dirs[i], newDir))
			return;

	dirs[numdirs] = new char[strlen(newDir) + 1];
	strcpy(dirs[numdirs++], newDir);
}

LPak *
LFile::UsePak(const char *filename) {
	if(!Exist(filename))
		return NULL;

	for(int i = 0; i < numpaks; i++)
		if(!strcmpi(paks[i]->filename, filename))
			return paks[i];

	paks[numpaks++] = new LPak(filename);

	return paks[numpaks - 1];
}

char LFile::initDir[256] = "";

void
LFile::Init(void) {
#ifdef _DEBUG
	_getcwd(initDir, 255);
#else
	GetModuleFileName(AfxGetInstanceHandle(), initDir, 255);
	char *c = strrchr(initDir, '\\');
	if(c)
		*c = 0;
#endif
}

void
LFile::Exit(void) {
	int i;
	
	for(i = 0; i < numdirs; i++)
		delete dirs[i];

	for(i = 0; i < numpaks; i++)
		delete paks[i];

	numdirs = 0;
	numpaks = 0;
}

const char *LFile::GetModifyTime(const char *fileName) {
	if (!Exist(fileName))
		return "";

    struct _finddata_t c_file;
	_findfirst(fileName, &c_file);

	return ctime(&(c_file.time_write));
}

// ========== LPak ==========

LPak::LPak(const char *_filename) {
	strcpy(filename, _filename);
	Open(filename);

	Read(&header, sizeof(header), 1);
	Seek(header.offset);

	if(!strncmp(header.magic, "SPAK", 4)) {
		int tmpEntries = header.size / sizeof(pak_entry_t);
		pak_entry_t *tmpEntry = new pak_entry_t[tmpEntries];

		Read(tmpEntry, sizeof(pak_entry_t), tmpEntries);

		entries = tmpEntries / 2;
		entry = new pak_entry_t[entries];

		for(int i = 0; i < entries; i++) {
			entry[i] = tmpEntry[i * 2];
			entry[i].offset = tmpEntry[i * 2 + 1].offset;
			entry[i].size = tmpEntry[i * 2 + 1].size;
		}

		delete [] tmpEntry;

		return;
	}

	entries = header.size / sizeof(pak_entry_t);
	entry = new pak_entry_t[entries];

	Read(entry, sizeof(pak_entry_t), entries);
}

LPak::~LPak(void) {
	Close();
	delete entry;
}


// ========== LFindFiles ==========

LFindFiles::LFindFiles(const char *rootdir, const char *filemask) {
	if(strlen(rootdir))
		sprintf(name, "%s\\%s", rootdir, filemask);
	else
		strcpy(name, filemask);
	rc = 0;
}

char *
LFindFiles::Next(void) {
	if(!rc) {
		if((rc = _findfirst(name, &fileinfo)) == -1L)
			return NULL;
	}
	else {
		if(_findnext(rc, &fileinfo) != 0L)
			return NULL;
	}
	if(!(fileinfo.attrib & _A_SUBDIR) && strcmp(fileinfo.name, ".") && strcmp(fileinfo.name, ".."))
		return fileinfo.name;
	else
		return Next();
}


// ========== LFindDirs ==========

LFindDirs::LFindDirs(const char *rootdir) {
	if(strlen(rootdir))
		sprintf(name, "%s\\*.*", rootdir);
	else
		strcpy(name, "*.*");
	rc = 0;
}

char *
LFindDirs::Next(void) {
	if(!rc) {
		if((rc = _findfirst(name, &fileinfo)) == -1L)
			return NULL;
	}
	else {
		if(_findnext(rc, &fileinfo) != 0L)
			return NULL;
	}
	if(fileinfo.attrib & _A_SUBDIR && strcmp(fileinfo.name, ".."))
		return fileinfo.name;
	else
		return Next();
}


// ========== LConfig ==========
int LConfig::currentVer = 0;
int LConfig::requiredVer = 0;
int LConfig::registryVer = 0;

LConfig::LConfig(char *name) {
	ASSERT(strlen(name) < 32);
	strcpy(sectionName, name);
	numLVars = 0;
}

LConfig::~LConfig(void) {
	for(int i = 0; i < numLVars; i++)
		delete lvars[i];
}

void
LConfig::RegisterVar(char *name, void *ptr, int type) {
	ASSERT(numLVars < 256);

	LVar *lvar = new LVar;
	lvars[numLVars++] = lvar;

	strcpy(lvar->name, name);
	lvar->ptr = ptr;
	lvar->type = type;

	if (registryVer < requiredVer)
		return;

	CWinApp *pApp = AfxGetApp();
	CString str = pApp->GetProfileString(sectionName, lvar->name);

	if(!strlen(str))
		return;

	char *buf = str.GetBuffer(255);

	if(lvar->type == LVAR_STR)
		strcpy((char *)lvar->ptr, buf);
	else if(lvar->type == LVAR_INT)
		sscanf(buf, "%d", lvar->ptr);
	else if(lvar->type == LVAR_FLOAT)
		sscanf(buf, "%f", lvar->ptr);
}

void
LConfig::SaveVars(void) {
	CWinApp *pApp = AfxGetApp();
	char buf[256] = "";

	for(int i = 0; i < numLVars; i++) {
		LVar *lvar = lvars[i];

		if(lvar->type == LVAR_STR)
			strcpy(buf, (char *)lvar->ptr);
		else if(lvar->type == LVAR_INT)
			sprintf(buf, "%d", *(int *)lvar->ptr);
		else if(lvar->type == LVAR_FLOAT)
			sprintf(buf, "%f", *(float *)lvar->ptr);

		pApp->WriteProfileString(sectionName, lvar->name, buf);
	}
}

void LConfig::Init(int currentVersion, int requiredVersion) {
	currentVer = currentVersion;
	requiredVer = requiredVersion;

	CWinApp *pApp = AfxGetApp();
	ASSERT(pApp != NULL);

	registryVer = pApp->GetProfileInt("_LConfig", "Version", 0);
}

void LConfig::Exit(void) {
	CWinApp *pApp = AfxGetApp();
	ASSERT(pApp != NULL);

	pApp->WriteProfileInt("_LConfig", "Version", currentVer);
}

// ========== LPalette ==========

LPalette::LPalette() {
}

LPalette::~LPalette() {
}

void LPalette::Load(char *filename, float gamma) {
	LFile file;
	if(!file.Open(filename))
		return;

	unsigned char buf[768];

	int i, c = 0;
	file.Read(buf, 768);
	file.Close();
	for(i = 0; i < 256; i++) {
		int r, g, b;
		r = buf[c++];
		g = buf[c++];
		b = buf[c++];
		GammaCorrect(r, g, b, gamma);
		pal[i].red = r;
		pal[i].green = g;
		pal[i].blue = b;
	}
}

void LPalette::GammaCorrect(int &r, int &g, int &b, float gamma) {
	if(r == 0 && g == 0 && b == 0) return;
	r = (int)((float)r - gamma * 100.0f + 100.0f);
	g = (int)((float)g - gamma * 100.0f + 100.0f);
	b = (int)((float)b - gamma * 100.0f + 100.0f);
	if(r < 0) r = 0;
	if(g < 0) g = 0;
	if(b < 0) b = 0;
	if(r > 255) r = 255;
	if(g > 255) g = 255;
	if(b > 255) b = 255;
}


// ========== LLibrary ==========

LinkList<LLibrary> LLibrary::libs;

LLibrary::LLibrary(char *libName) {
	hInst = LoadLibrary(libName);
}

LLibrary::~LLibrary() {
	FreeLibrary(hInst);
}

HINSTANCE LLibrary::FindLibrary(char *libName) {

	IterLinkList<LLibrary> iter(libs);
	iter.Reset();
	while(!iter.IsDone()) {
		LLibrary *lib = iter.GetNext();
		if(!strcmp(lib->name, libName))
			return lib->hInst;
	}

	return (new LLibrary(libName))->hInst;
}


// ========== Print Formatting ==========

#include "string.h"
const char *
FormatFloat(float number) {
  static char rtnVal[32];
  int i, len;

  // Strip the trailing 0's
  sprintf(rtnVal, "%f", number);
  len = strlen(rtnVal);
  for(i = len - 1;
      i > 0 && rtnVal[i] == '0' && rtnVal[i - 1] != '.';
      i--) {
    rtnVal[i] = '\0';
  }

  return rtnVal;
}


//========== Conversion Stuff ==========
bool Str2Int(const char *str, UINT &val)  {
	if (str == NULL || str[0] == '\0')
		return false;

	for(const char *c = str; *c != '\0'; c++) {
		if (!isdigit(*c))
			return false;
	}

	val = (unsigned int) atoi(str);
	return true;
}

// ========== Time Code ==========

#include <sys/timeb.h>

int start_sec;
int start_msec;

void ReadTime(int *sec, int *msec) {
    struct _timeb t;
    _ftime(&t);
	*sec = (int)t.time;
	*msec = (int)t.millitm;
}

int GetTime(void) {
	int sec, msec;

	static bool init = false;
	if(!init) {
		ReadTime(&start_sec, &start_msec);
		init = true;
	}

	ReadTime(&sec, &msec);
	return (sec - start_sec) * 1000 + (msec - start_msec);
}


// ========== Old VOS Stuff ==========
// TODO: fix/replace this stuff up

int String_Split(substr_t substr[], char *org_string, int max) {
	char *ch, last_ch;
	char start_ch;
	char string[512];
	int num, pos;
	int quoting = false;

	strcpy(string, org_string);
	
	for(num = 0; num < max; num++)
		memset(substr[num], 0, sizeof(substr_t));

	// turn tabs to spaces
	ch = string;
	while(*ch != '\0') {
		if(*ch == '\t')
			*ch = ' ';
		ch++;
	}

	String_Crop(string);

	last_ch = '\0';
	ch = string;
	num = 0;
	pos = 0;
	
	while(true) {
		if(*ch == '\"' || (*ch == '\'' && !quoting) || (*ch == '\'' && *ch == start_ch)) {
			quoting = abs(quoting - 1);
			if(quoting)
				start_ch = *ch;
			if(num < max - 1) {
				last_ch = *ch++;
				continue;
			}
		}
		
		if(*ch == '\0')
			return num + 1;
		
		if(*ch != ' ') {
			strncat(substr[num], ch, 1);
			pos++;

			last_ch = *ch++;
			continue;
		}

		if(*ch == ' ' && last_ch != ' ') {
			if(quoting) {
				 strncat(substr[num], ch, 1);
				 pos++;
			}
			else {
				if(num < max - 1) {
					num++;
				}
				else {
					if(*(ch + 1) != ' ') {
						strncat(substr[num], ch, 1);
						pos++;
					}
				}
			}
		}

		if(*ch == ' ' && last_ch == ' ' && quoting) {
			strncat(substr[num], ch, 1);
			pos++;
		}

		if(pos > sizeof(substr_t)) {
			pos = 0;
			num++;
			if(num == max)
				return max;
		}

		last_ch = *ch++;
	}

}

// removing beginning and trailing whitespaces
void String_Crop(char *str) {
	char *c = str;
	while(*c != '\0' && (*c == ' ' || *c == '\t'))
		c++;
	strcpy(str, c);

	c = str + strlen(str) - 1;
	while(c > str && (*c == ' ' || *c == '\t'))
		c--;

	c++;
	if(*c)
		*c = '\0';

	if(str[strlen(str) - 1] == '\n')
		String_Chop(str);
	if(str[strlen(str) - 1] == '\r')
		String_Chop(str);
}

void String_Chop(char *str) {
	int i = strlen(str);
	if(i)
		str[i - 1] = '\0';
}

void String_CutAt(char *str, char *at) {
	char *c = strstr(str, at);
	if(c)
		*c = '\0';
}

int String_Count(char *str, char ch) {
	int i = 0;
	char *c = str;
	while(*c)
		if(*c++ == ch)
			i++;
	return i;
}

int strstri(char *str1, char *str2) {
	int result;
	char *buf1 = (char *)malloc(strlen(str1) + 1);
	char *buf2 = (char *)malloc(strlen(str2) + 1);
	strcpy(buf1, str1);
	strcpy(buf2, str2);
	strupr(buf1);
	strupr(buf2);
	result = (int)strstr(buf1, buf2);
	free(buf1);
	free(buf2);
	return result;
}


sort_t *lmerge(sort_t *p, sort_t *q) {
	sort_t *r, head;

	for(r = &head; p && q; ) {
		if (p->key < q->key) {
			r = r->next = p;
			p = p->next;
		}
		else {
			r = r->next = q;
			q = q->next;
		}
	}
	r->next = (p ? p : q);
	return head.next;
}

sort_t *lsort(sort_t *p) {
	sort_t *q, *r;

	if(p) {
		q = p;
		for(r = q->next; r && (r = r->next) != NULL; r = r->next) { q = q->next; }
		r = q->next;
		q->next = NULL;
		if(r) { p = lmerge(lsort(r), lsort(p)); }
	}
	return p;
}


struct pcx_header_t {
	char manufacturer;
	char version;
	char encoding;
	char bits_per_pixel;
	short xmin, ymin;
	short xmax, ymax;
	short hres, vres;
	char palette[48];
	char reserved;
	char color_planes;
	short btyes_per_line;
	short palette_type;
	char filler[58];
};

void SavePCX(char *name, unsigned char *surface, int width, int height, lpal_t *pal) {

	char filename[256];
	sprintf(filename, "%s.pcx", name);
	char *c = strchr(filename, '//');
	if(c)
		*c = '-';

	LFile file;
	file.Open(filename, LFILE_WRITE);

	pcx_header_t pcx_header;

	pcx_header.manufacturer = 10;
	pcx_header.version = 5;
	pcx_header.encoding = 1;
	pcx_header.bits_per_pixel = 8;
	pcx_header.xmin = 0;
	pcx_header.ymin = 0;
	pcx_header.xmax = (short)width - 1;
	pcx_header.ymax = (short)height - 1;
	pcx_header.hres = 100;
	pcx_header.vres = 100;
	pcx_header.color_planes = 1;
	pcx_header.btyes_per_line = (short)width;
	pcx_header.palette_type = 1;

	file.Write(&pcx_header, sizeof(pcx_header_t));
	
	unsigned char *rle = (unsigned char *)malloc(width * height * 2 + 1);
	unsigned char *r = rle;
	unsigned char *s = surface;

	for(int i = 0; i < width * height; i++) {
		*r++ = 193;
		*r++ = *s++;
	}

	file.Write(rle, width * height * 2);
	file.Write(pal, 768);

	free(rle);
}

int NearestGreaterPower2(int x) {
	int i = 0;
	while(1) {
		if(x <= 1 << i)
			return 1 << i;
		i++;
	}
	return 1;
}


