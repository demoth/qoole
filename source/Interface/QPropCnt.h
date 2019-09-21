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

#if !defined(AFX_QPROPCNT_H__FD3ECFAB_62C3_11D2_BA41_004005310168__INCLUDED_)
#define AFX_QPROPCNT_H__FD3ECFAB_62C3_11D2_BA41_004005310168__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// QPropCnt.h : header file
//

#include "QPropPg.h"

//======================================================================
// QPropPageContent dialog
//======================================================================

class QPropPageContent : public QPropPage {
	DECLARE_DYNCREATE(QPropPageContent)

// Construction
public:
	QPropPageContent();
	~QPropPageContent();

	virtual void OnUpdate(LPARAM lHint, Object *pScope);
	BOOL UpdateData(BOOL bSaveAndValidate, UINT &content);

// Dialog Data
	//{{AFX_DATA(QPropPageContent)
	enum { IDD = IDD_PROPPAGE_CONTENT };
	BOOL	m_bAux;
	BOOL	m_bCorpse;
	BOOL	m_bCurrent0;
	BOOL	m_bCurrent180;
	BOOL	m_bCurrent270;
	BOOL	m_bCurrent90;
	BOOL	m_bCurrentDown;
	BOOL	m_bCurrentUp;
	BOOL	m_bDetail;
	BOOL	m_bLadder;
	BOOL	m_bLava;
	BOOL	m_bMist;
	BOOL	m_bOrigin;
	BOOL	m_bPlayerClip;
	BOOL	m_bSlime;
	BOOL	m_bSolid;
	BOOL	m_bTranslucent;
	BOOL	m_bWater;
	BOOL	m_bWindow;
	BOOL	m_bMonsterClip;
	BOOL	m_bMonster;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(QPropPageContent)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CButton &ctrlSolid()       { return *((CButton *) GetDlgItem(IDC_PPC_SOLID)); }
	CButton &ctrlWindow()      { return *((CButton *) GetDlgItem(IDC_PPC_WINDOW)); }
	CButton &ctrlAux()         { return *((CButton *) GetDlgItem(IDC_PPC_AUX)); }
	CButton &ctrlLava()        { return *((CButton *) GetDlgItem(IDC_PPC_LAVA)); }
	CButton &ctrlSlime()       { return *((CButton *) GetDlgItem(IDC_PPC_SLIME)); }
	CButton &ctrlWater()       { return *((CButton *) GetDlgItem(IDC_PPC_WATER)); }
	CButton &ctrlMist()        { return *((CButton *) GetDlgItem(IDC_PPC_MIST)); }
	CButton &ctrlCurrent0()    { return *((CButton *) GetDlgItem(IDC_PPC_CURRENT0)); }
	CButton &ctrlCurrent90()   { return *((CButton *) GetDlgItem(IDC_PPC_CURRENT90)); }
	CButton &ctrlCurrent180()  { return *((CButton *) GetDlgItem(IDC_PPC_CURRENT180)); }
	CButton &ctrlCurrent270()  { return *((CButton *) GetDlgItem(IDC_PPC_CURRENT270)); }
	CButton &ctrlCurrentUp()   { return *((CButton *) GetDlgItem(IDC_PPC_CURRENTUP)); }
	CButton &ctrlCurrentDown() { return *((CButton *) GetDlgItem(IDC_PPC_CURRENTDOWN)); }
	CButton &ctrlPlayerClip()  { return *((CButton *) GetDlgItem(IDC_PPC_PLAYERCLIP)); }
	CButton &ctrlMonsterClip() { return *((CButton *) GetDlgItem(IDC_PPC_MONSTERCLIP)); }
	CButton &ctrlOrigin()      { return *((CButton *) GetDlgItem(IDC_PPC_ORIGIN)); }
	CButton &ctrlMonster()     { return *((CButton *) GetDlgItem(IDC_PPC_MONSTER)); }
	CButton &ctrlCorpse()      { return *((CButton *) GetDlgItem(IDC_PPC_CORPSE)); }
	CButton &ctrlDetail()      { return *((CButton *) GetDlgItem(IDC_PPC_DETAIL)); }
	CButton &ctrlTranslucent() { return *((CButton *) GetDlgItem(IDC_PPC_TRANSLUCENT)); }
	CButton &ctrlLadder()      { return *((CButton *) GetDlgItem(IDC_PPC_LADDER)); }

	void EnableCtrls(bool bEnable);
	void OnSlctNewBrush();
	void RegAttribChanged();

	bool ctrlsEnabled;

	// Generated message map functions
	//{{AFX_MSG(QPropPageContent)
	afx_msg void OnPpcAux();
	afx_msg void OnPpcCorpse();
	afx_msg void OnPpcCurrent0();
	afx_msg void OnPpcCurrent180();
	afx_msg void OnPpcCurrent270();
	afx_msg void OnPpcCurrent90();
	afx_msg void OnPpcCurrentdown();
	afx_msg void OnPpcCurrentup();
	afx_msg void OnPpcDetail();
	afx_msg void OnPpcLadder();
	afx_msg void OnPpcLava();
	afx_msg void OnPpcMist();
	afx_msg void OnPpcMonster();
	afx_msg void OnPpcMonsterclip();
	afx_msg void OnPpcOrigin();
	afx_msg void OnPpcPlayerclip();
	afx_msg void OnPpcSlime();
	afx_msg void OnPpcSolid();
	afx_msg void OnPpcTranslucent();
	afx_msg void OnPpcWater();
	afx_msg void OnPpcWindow();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QPROPCNT_H__FD3ECFAB_62C3_11D2_BA41_004005310168__INCLUDED_)
