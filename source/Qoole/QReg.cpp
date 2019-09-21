// QReg.cpp : implementation file
//

#include "stdafx.h"
#include "qoole.h"
#include "LCommon.h"
#include "QReg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// QReg dialog


QReg::QReg(CWnd* pParent /*=NULL*/)
	: CDialog(QReg::IDD, pParent)
{
	//{{AFX_DATA_INIT(QReg)
	//}}AFX_DATA_INIT
}


void QReg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(QReg)
	DDX_Control(pDX, IDC_REG_NAME, m_regName);
	DDX_Control(pDX, IDC_REG_KEY, m_regKey);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(QReg, CDialog)
	//{{AFX_MSG_MAP(QReg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// QReg message handlers

void QReg::OnOK() {
	LConfig cfg("QReg");
	cfg.RegisterVar("RegName", &reg_name, LVAR_STR);
	cfg.RegisterVar("RegCode", &reg_code, LVAR_STR);

	if(!Verify()) {
		MessageBox("Invalid Name/Key", NULL, MB_ICONERROR);
		return;
	}

	cfg.SaveVars();
	MessageBox("Qoole 99 registered.  Thanks!", NULL, MB_OK);

	CDialog::OnOK();
}

bool QReg::Verify(void) {
	char rcode1[10], rcode2[10], rcode3[10], rcode4[10];
	char code3[8] = "", code1[8] = "", code4[8] = "", code2[8] = "";

	char reg_key[64];
	m_regName.GetWindowText(reg_name, 64);
	m_regKey.GetWindowText(reg_key, 64);

	String_Crop(reg_name);
	String_Crop(reg_key);
	strupr(reg_key);

	int i = -1, j = 0;
	char *c, buf[8], code[8];
	c = reg_key;
	while(c) {
		i++;
		if(i == 0 || i == 1) {
			buf[0] = c[0];
			buf[1] = c[1];
			buf[2] = 0;
			c += 2;
			code[i] = (char)atoi(buf);
		}
		if(i == 2 || i == 3) {
			code[i] = c[0];
			c++;
		}
		if(i == 4) {
			c++;
			i = -1;
			code[4] = 0;
			if(j == 0) strcpy(rcode1, code);
			if(j == 1) strcpy(rcode2, code);
			if(j == 2) strcpy(rcode3, code);
			if(j == 3) strcpy(rcode4, code);
			j++;
			if(j == 4) break;
		}
	}

	code1[3] = rcode1[0];
	code2[2] = rcode1[1];
	code3[1] = rcode1[2];
	code4[0] = rcode1[3];
	code2[1] = rcode2[0];
	code1[2] = rcode2[1];
	code4[3] = rcode2[2];
	code3[0] = rcode2[3];
	code4[2] = rcode3[0];
	code3[3] = rcode3[1];
	code2[0] = rcode3[2];
	code1[1] = rcode3[3];
	code3[2] = rcode4[0];
	code4[1] = rcode4[1];
	code1[0] = rcode4[2];
	code2[3] = rcode4[3];

	reg_keynum = code4[1] * 91 + code4[2];
	QReg_CodeFromName(rcode1, reg_name);
	QReg_CodeFromName(rcode2, reg_name + 1);
	QReg_CodeFromNum(rcode3, reg_keynum);
//	QReg_CodeFromNum(rcode3, qreg_keys[reg_keynum]);

	if(code4[0] != 'B' + reg_keynum % 25 ||
	   code4[3] != 'A' + reg_keynum % 26 ||
//	   strncmp(code3, rcode3, 4) ||
	   strncmp(code1, rcode1, 4) ||
	   strncmp(code2, rcode2, 4)) {
		strcpy(reg_name, "Unregistered");
		strcpy(reg_code, "");
		return false;
	}

	QReg_CodeFromName2(rcode1, code1);
	QReg_CodeFromName2(rcode2, code2);
	QReg_CodeFromName2(rcode3, code3);
	sprintf(reg_code, "%s%02d%s%02d%s", rcode1, code4[2], rcode2, code4[1], rcode3);

	return true;
}

bool QReg::Check(void) {
	char reg_name[64] = "";
	char reg_code[64] = "";
	int reg_keynum;

	LConfig cfg("QReg");
	cfg.RegisterVar("RegName", &reg_name, LVAR_STR);
	cfg.RegisterVar("RegCode", &reg_code, LVAR_STR);

	if(!strlen(reg_name) || !strlen(reg_code))
		return false;

	char rcode1[10] = "", rcode2[10] = "", rcode3[10] = "", rcode4[10] = "";
	char code1[10] = "", code2[10] = "", code3[10] = "", code4[10] = "";

	int n1, n2;
	char buf[8];
	strncpy(buf, reg_code + 8, 2);
	buf[2] = 0;
	n1 = atoi(buf);

	strncpy(buf, reg_code + 18, 2);
	buf[2] = 0;
	n2 = atoi(buf);

	reg_keynum = n1 + n2 * 91;

	if(reg_keynum < 1 || reg_keynum >= MAX_KEYS)
		return false;

	if(!QReg_CodeFromName(rcode1, reg_name))
		return false;
	if(!QReg_CodeFromName(rcode2, reg_name + 1))
		return false;
	QReg_CodeFromNum(rcode3, reg_keynum);
//	QReg_CodeFromNum(rcode3, qreg_keys[reg_keynum]);
	QReg_CodeFromName2(code1, rcode1);
	QReg_CodeFromName2(code2, rcode2);
	QReg_CodeFromName2(code3, rcode3);

	strncpy(rcode1, reg_code, 8);
	strncpy(rcode2, reg_code + 10, 8);
	strncpy(rcode3, reg_code + 20, 8);

	if(strncmp(code1, rcode1, 8)) return false;
	if(strncmp(code2, rcode2, 8)) return false;
//	if(strncmp(code3, rcode3, 8)) return false;

//	unreg = false;
//	registered = true;
//	qreg_checked = true;

	if(!strlen(reg_code)) {
		code1[8] = code2[8] = code3[8] = 0;
		sprintf(reg_code, "%s%02d%s%02d%s", code1, n1, code2, n2, code3);
	}

	return true;
}

void QReg::GetRegName(char *regName) {
	if(!Check()) {
		strcpy(regName, "[Unregistered]");
		return;
	}

	LConfig cfg("QReg");
	cfg.RegisterVar("RegName", regName, LVAR_STR);
}

bool QReg_CodeFromName(char *code, char *name) {
	int i, l, v;
	float s;

	l = strlen(name);
	if(l < 4) return false;

	for(i = 0; i < l; i++)
		if((name[i] < 'a' || name[i] > 'z') && (name[i] < 'A' || name[i] > 'Z') && (name[i] < '0' || name[i] > '9')
		   && name[i] != '\'' && name[i] != ' ' && name[i] != '.' && name[i] != ',')
			return false;

	if(String_Count(name, ' ') > 4) return false;
	if(String_Count(name, '\'') > 2) return false;
	if(String_Count(name, '.') > 2) return false;
	if(String_Count(name, ',') > 2) return false;
	
	v = 1;
	s = (float)l / 4;
	for(i = 0; i < 4; i++)
		v *= name[(int)((float)i * s)];

	for(i = 0; i < (int)strlen(name); i++)
		v += i * name[i];
	
	QReg_CodeFromNum(code, v);
	
	return true;
}

void QReg_CodeFromName2(char *code, char *name) {
	int v;
	char c2[8];

	if(name[3])
		v = name[0] * name[1] * name[2] / name[3];
	else
		v = name[0] * name[1] * name[2] / 10;
	QReg_CodeFromNum(code, v);

	v = name[0] * name[1] * name[2] * name[3];
	QReg_CodeFromNum(c2, v);

	strcat(code, c2);

	if(name[0] % 3) code[0] -= 'A' - 'a';
	if(name[1] % 2) code[1] -= 'A' - 'a';
	if(name[2] % 3) code[2] -= 'A' - 'a';
	if(name[3] % 2) code[3] -= 'A' - 'a';
	if(name[0] % 2) code[4] -= 'A' - 'a';
	if(name[1] % 3) code[5] -= 'A' - 'a';
	if(name[2] % 2) code[6] -= 'A' - 'a';
	if(name[3] % 3) code[7] -= 'A' - 'a';
}

void QReg_CodeFromNum(char *code, int num) {
	int v1, v2, v3, v4;

	v1 = num % 26;
	if(!v1) v1 = (num / 523) % 26;
	v2 = (num + v1 * 18) % 25;
	if(!v2) v2 = (num / 816) % 25;
	v3 = (num - v2 * 31) % 26;
	if(!v3) v3 = (num / 334) % 26;
	v4 = (num + v3 * 19) % 25;
	if(!v4) v4 = (num / 449) % 25;
	
	code[0] = 'A' + v1;
	code[1] = 'B' + v2;
	code[2] = 'A' + v3;
	code[3] = 'B' + v4;
	code[4] = '\0';
}
