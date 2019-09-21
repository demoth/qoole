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

// QVFrame.cpp : implementation file
//

#include "stdafx.h"
#include "Qoole.h"
#include "QMainFrm.h"
#include "QVFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//======================================================================
// QVFrame
//======================================================================

IMPLEMENT_DYNCREATE(QVFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(QVFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(QVFrame)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_WM_DESTROY ()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// QVFrame construction/destruction

QVFrame::QVFrame() {
	// TODO: add member initialization code here
}

QVFrame::~QVFrame() {
}

BOOL QVFrame::PreCreateWindow(CREATESTRUCT& cs) {
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.style ^= (FWS_ADDTOTITLE);
	return CMDIChildWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// QVFrame message handlers

void QVFrame::OnClose() {
	// Do a count on the # of siblings left.
	int count = 1;
	CWnd *pCWnd = GetWindow(GW_HWNDFIRST);
	while ((pCWnd = pCWnd->GetWindow(GW_HWNDNEXT)) != NULL)
		count++;
	
	if (count == 1)  // No more views left.  Close the document.
		pQMainFrame->PostMessage(WM_COMMAND, ID_FILE_CLOSE);
	else
		CMDIChildWnd::OnClose();
}

void QVFrame::OnDestroy() {
	// Register the MDI child windows' destruction with the desktop.
	pQMainFrame->RegisterDestroyQVFrame(this);

	CMDIChildWnd::OnDestroy();
}

