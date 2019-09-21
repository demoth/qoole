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

// Qoole.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"

#include "Qoole.h"
#include "QooleDoc.h"

#include "QMainFrm.h"
#include "QVFrame.h"
#include "QView.h"

#include "QVersion.h"

#include "LCommon.h"

#include "Quake.h"
#include "Quake2.h"
#include "Hexen2.h"
#include "HalfLife.h"
#include "Sin.h"
#include "Heretic2.h"

#include "CreditStatic.h"
#define  DISPLAY_TIMER_ID		151

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

char *pArrCredit = { "|Qoole 99 v0.99\t||"
		"Programming\r|Matt 'WhiteFang' Ayres|"
		"Paul 'Hsup' Hsu|"
		"Pablo '^Fishman' Zurita||"
		"Beta testing\r|Tim 'Ishkish' Barry, Jr.|"
		"Aaron 'TheRelentless' Bellante.|"
		"Barnaby 'VolVE' Claydon.|"
		"Ken 'keville' Neville.|"
		"Thomas 'skeeter' Tripp.||"
		"Ent Sets\r|Tim 'Ishkish' Barry, Jr.|"
		"Aaron 'TheRelentless' Bellante.|"
		"Barnaby 'VolVE' Claydon.|"
		"Matt 'Grendel' Lee.|"
		"Ken 'keville' Neville.|"
		"Thomas 'skeeter' Tripp.||"
		"GFX\r|Paul 'Hsup' Hsu.|"
		"Ken 'keville' Neville.||"
		"Documentation\r|Thomas 'skeeter' Tripp.||"
		"Additional Credits\r|"
		"Geoffrey 'G_DeWan' DeWan.|"
		"Paul 'Cyber-Freak' Lawrence.|"
		"Jesse 'raYGunn' McCree.|"
		"Jaimi McEntire.||||||||||||||||||||||||"
};

/////////////////////////////////////////////////////////////////////////////
// Crash Interception

#include "signal.h"
#include "process.h"

void OnSignal(int sigNum) {
	static bool beenhere = false;
	if(!beenhere && pQMainFrame->GetDeskTopDocument()) {
		char buf[128] = "Got signal ";

		if(sigNum == SIGABRT) strcat(buf, "SIGABRT");
		if(sigNum == SIGFPE) strcat(buf, "SIGFPE");
		if(sigNum == SIGILL) strcat(buf, "SIGILL");
		if(sigNum == SIGINT) strcat(buf, "SIGINT");
		if(sigNum == SIGSEGV) strcat(buf, "SIGSEGV");
		if(sigNum == SIGTERM) strcat(buf, "SIGTERM");
		if(sigNum == SIGBREAK) strcat(buf, "SIGBREAK");

		strcat(buf, ".  Qoole 99 is about to crash due to an internal\n"
			"application error.  Click OK to have your work saved to #crash files.\n");

		MessageBox(NULL, buf, "Qoole 99 Crash Interception", MB_OK);

		QooleDoc *pDoc;

		while(true) {
			pDoc = pQMainFrame->GetDeskTopDocument();
			if(!pDoc)
				break;

			char buf[16] = "#crash.qle";
			for(int i = 0; i < 100; i++) {
				sprintf(buf, "#crash%02d.qle", i);
				if(!LFile::Exist(buf))
					break;
			}

			pDoc->OnSaveDocument(buf);
			pDoc->OnCloseDocument();
		}
	}
	beenhere = true;

	MessageBox(NULL,
		"Click OK to continue with the crash.  Please report this to fish@gamedesign.net.\n"
		"Include the information found in the Output bar, a description of what mode you\n"
		"were in what operation you were trying to perform, and any other relevant\n"
		"information. There is no need to include the debug information that you get after\n"
		"this screen, it is essentially useless. Thanks.\n",
		"Qoole 99 Crash Interception", MB_OK);
}

/////////////////////////////////////////////////////////////////////////////
// QooleApp

BEGIN_MESSAGE_MAP(QooleApp, CWinApp)
	//{{AFX_MSG_MAP(QooleApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// QooleApp construction

QooleApp::QooleApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only QooleApp object

QooleApp theApp;

/////////////////////////////////////////////////////////////////////////////
// QooleApp initialization

BOOL QooleApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Lithium Software"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Qoole specific registry stuff.
	LConfig::Init(Q_CURRENT_VERSION, Q_REQUIRED_VERSION);

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.
	QDocTemplate* pDocTemplate;
	pDocTemplate = new QDocTemplate(
		IDR_QOOLETYPE, // | */ IDR_MAINFRAME_MENU,// | IDR_MAINFRAME_ACCEL,
		RUNTIME_CLASS(QooleDoc),
		RUNTIME_CLASS(QVFrame), // custom MDI child frame
		RUNTIME_CLASS(QView));
	AddDocTemplate(pDocTemplate);

	// Initialize drawing subsystem
	QDraw::Init();
	QView::Init();

	// create main MDI Frame window
	QMainFrame* pMainFrame = new QMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	if (!pMainFrame->LoadAccelTable(MAKEINTRESOURCE(IDR_MAINFRAME_ACCEL)))
		return FALSE;
	m_pMainWnd = pMainFrame;

	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();

	// Enable DDE Execute open
	// EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Turns off the display of a new MDI child window on startup.
	cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The main window has been initialized, so show and update it.
	if (pMainFrame->wndMaximized)
		m_nCmdShow = SW_SHOWMAXIMIZED;

	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	// Initialize data file
	LFile::Init();

	char q99pak[256];
	sprintf(q99pak, "%s\\%s", LFile::GetInitDir(), "Qoole99.pak");

	if (!LFile::Exist(q99pak)) {
		// Can't find the q99 pak file in the current working dir.
		// Try the same dir where the app is running from.
		char appPath[256];
		GetModuleFileName(AfxGetInstanceHandle(), appPath, 256);
		char *c = strrchr(appPath, '\\');
		if (c != NULL)
			*c = '\0';
		sprintf(q99pak, "%s\\%s", appPath, "Qoole99.pak");

		if (!LFile::Exist(q99pak)) // 2nd try.
			LFatal("Required file not found: %s", q99pak);
	}

	LFile::UsePak(q99pak);

	// Register games
	new Quake;
	new Quake2;
	new Hexen2;
	new HalfLife;
	new Sin;
	new Heretic2;

	if(LFile::Exist("texlists\\texture.db"))
		CFile::Remove("texlists\\texture.db");

#ifdef NDEBUG
	signal(SIGABRT, OnSignal);
	signal(SIGFPE, OnSignal);
	signal(SIGILL, OnSignal);
	signal(SIGINT, OnSignal);
	signal(SIGSEGV, OnSignal);
	signal(SIGTERM, OnSignal);
	signal(SIGBREAK, OnSignal);
#endif

#ifdef _SHAREWARE_DEMO_
	CTime t = CTime::GetCurrentTime();
	if (!(t.GetYear() == 1999 && t.GetMonth() <= 6)) {
		LFatal("This demo has expired.  Please visit "
		"http://www.qoole.com for the latest version.");
	}
#endif

	return TRUE;
}

int QooleApp::ExitInstance() {
	// Clean up
	EntList::Exit();
	Game::Exit();
	QDraw::Exit();
	LFile::Exit();
	QView::Exit();

	LConfig::Exit();
	
	return CWinApp::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CButton m_qpop;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CCreditStatic m_static;
	UINT TimerOn;
	UINT bitmapID;
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	virtual void OnQpopURL();
	virtual void OnQpopEmail();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_BN_CLICKED(IDC_QPOP, OnQpopURL)
	ON_BN_CLICKED(IDC_EMAIL, OnQpopEmail)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog() 
{
	bitmapID = IDB_QCREDITS;
	CDialog::OnInitDialog();

	m_static.SubclassDlgItem(IDC_DISPLAY_STATIC,this);
	m_static.SetCredits(pArrCredit,'|');
	m_static.SetSpeed(DISPLAY_FAST);
	m_static.SetColor(BACKGROUND_COLOR, RGB(0,0,0));
	m_static.SetTransparent();
	m_static.SetBkImage(bitmapID);
	m_static.StartScrolling();
	TimerOn = SetTimer(DISPLAY_TIMER_ID,5000,NULL);
    ASSERT(TimerOn != 0);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAboutDlg::OnQpopURL()
{
	ShellExecute (0, "open", "http://qoole.gamedesign.net", 0, 0, SW_SHOW);
}

void CAboutDlg::OnQpopEmail()
{
	ShellExecute (0, "open", "mailto:fish@gamedesign.net", 0, 0, SW_SHOW);
}

void CAboutDlg::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent != DISPLAY_TIMER_ID)
	{
		CDialog::OnTimer(nIDEvent);
		return;
	}
	
	CDialog::OnTimer(nIDEvent);
}

void CAboutDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	if(TimerOn)
		ASSERT(KillTimer(DISPLAY_TIMER_ID));
}

/////////////////////////////////////////////////////////////////////////////
// QooleApp commands


// App command to run the dialog
void QooleApp::OnAppAbout() {
	/*
	char wav[256];
	GetTempPath(128, wav);
	strcat(wav, "cool.wav");
	LFile::Extract("cool.wav", wav);

	PlaySound(wav, NULL, SND_FILENAME | SND_ASYNC);
	*/

	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}