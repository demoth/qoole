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

#if !defined(AFX_QPROCWND_H__7FF513CD_29B7_11D2_9645_004005310168__INCLUDED_)
#define AFX_QPROCWND_H__7FF513CD_29B7_11D2_9645_004005310168__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// QProcWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// QProcessWnd window

#define PIPEBUFSIZE	4096
#define QPROCWND_UPDATETIME			250

typedef void (*PostProc)(void);

class QProcessWnd : public CRichEditCtrl {
// Construction
public:
	QProcessWnd();

// Attributes
public:

// Operations
public:
	void ExecProcess(char *pCmdLine, char *pStartDir = NULL,
					 PostProc pProc = NULL, BOOL bClearWnd = FALSE);
	void TerminateProcess(void);
	bool IsProcessRunning(void) const;

	void OnIdleUpdateContent(void);
	void ClearWindow(void);
	void OutputText(char *pText);
	void GetViewPos(void);
	void SetViewPos(void);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(QProcessWnd)
	public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~QProcessWnd();

protected:
	bool InitChildStdPipes(STARTUPINFO &si);
	void RestoreStdPipes(void);

	bool IsSelEmpty(void) const;

	static UINT ThreadFunc(LPVOID pParam);
	static DWORD CALLBACK GetInputFromBuffer(DWORD dwCookie, LPBYTE bpBuff,
											 LONG cb, LONG FAR *pcb);


/*	void GetViewPos(void);
	void SetViewPos(void);*/

	CCriticalSection lockBuffer;
	char childStdinBuf[PIPEBUFSIZE];
	int bufSize;

	HANDLE hChildStdinRd, hChildStdinWr,
		   hChildStdoutRd, hChildStdoutWr;

	HANDLE hChildProcess;

	bool terminateFlag;
	bool blockPainting;

	bool scrollEnd;
	long viewLinePos, curStart, curEnd;
	int  winRowHeight;

	PostProc pPostExecProc;
	CFont font;

	static QProcessWnd *pDelObj;

	// Generated message map functions
protected:
	//{{AFX_MSG(QProcessWnd)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	afx_msg void OnHandleOutputBarCmds();
	afx_msg void OnExecProcFinished();
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QPROCWND_H__7FF513CD_29B7_11D2_9645_004005310168__INCLUDED_)
