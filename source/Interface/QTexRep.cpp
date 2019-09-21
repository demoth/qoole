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

// QTexRep.cpp : implementation file
//

#include "stdafx.h"
#include "qoole.h"
#include "QMainFrm.h"
#include "QTexRep.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// QTexRep dialog


QTexRep::QTexRep(CWnd* pParent /*=NULL*/)
	: CDialog(QTexRep::IDD, pParent)
{
	//{{AFX_DATA_INIT(QTexRep)
	//}}AFX_DATA_INIT
}


void QTexRep::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(QTexRep)
	DDX_Control(pDX, IDC_TEXREP_OLD, m_Old);
	DDX_Control(pDX, IDC_TEXREP_NEW, m_New);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(QTexRep, CDialog)
	//{{AFX_MSG_MAP(QTexRep)
	ON_BN_CLICKED(IDC_TEXREP_APPLY, OnApply)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// QTexRep message handlers

char QTexRep::oldTex[64];
char QTexRep::newTex[64];

void QTexRep::OnApply() {
	m_Old.GetWindowText(oldTex, 63);
	m_New.GetWindowText(newTex, 63);

	Object *pObj = &pQMainFrame->GetScope();
	pObj->PreOrderApply(QTexRep::Apply);
}

bool QTexRep::Apply(Object &obj) {
	if(!obj.HasBrush())
		return true;

	Geometry *pBrush = &(obj.GetBrush());
	for(int i = 0; i < pBrush->GetNumFaces(); i++) {
		FaceTex *faceTex = pBrush->GetFaceTexturePtr(i);
		if(!strcmpi(faceTex->GetTName(), oldTex)) {
			faceTex->SetTName(newTex);
		}
	}

	return true;
}
