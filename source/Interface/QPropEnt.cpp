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

// qpropent.cpp : implementation file
//

#include "stdafx.h"
#include "Qoole.h"
#include "QMainFrm.h"
#include "QPropEnt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//======================================================================
// QPropPageEntity property page
//======================================================================

IMPLEMENT_DYNCREATE(QPropPageEntity, QPropPage)

QPropPageEntity::QPropPageEntity() : QPropPage(QPropPageEntity::IDD) {
	//{{AFX_DATA_INIT(QPropPageEntity)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	pEntInfo = NULL;
}

QPropPageEntity::~QPropPageEntity() {
}

void QPropPageEntity::DoDataExchange(CDataExchange* pDX) {
	QPropPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(QPropPageEntity)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(QPropPageEntity, QPropPage)
	//{{AFX_MSG_MAP(QPropPageEntity)
	ON_BN_CLICKED(IDC_PPE_LIGHTBUTTON, OnLightButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

////////////////////////////////////////
// QPropPageEntity message handlers
BOOL QPropPageEntity::OnInitDialog() {
	QPropPage::OnInitDialog();
	
	CWnd *pWnd = GetDlgItem(IDC_PPE_ENTLIST);

	propList.SubclassWindow(pWnd->m_hWnd);
	propList.RegisterProcs(OnClickCallBack, DlgProcCallBack);

	// Create the 2 columns.
	propList.InsertColumn(0, "Property");
	propList.InsertColumn(1, "Value");

	CRect rect;
	// Force a resize message to center the columns.
	propList.GetWindowRect(&rect);
	propList.SendMessage(WM_SIZE, SIZE_RESTORED,
					 rect.Height() << 16 | rect.Width());

	// Create the header control.
	DWORD style = WS_CHILD | WS_VISIBLE |
				/*	WS_EX_WINDOWEDGE | WS_EX_STATICEDGE | */
				HDS_BUTTONS | HDS_HORZ;

	CDC *pDC = GetDC();
	CSize size = pDC->GetTextExtent("A", 1);
	ReleaseDC(pDC);

	// rect.left -= 1;
	rect.right -= 1;
	rect.bottom = rect.top + 1;
	rect.top = rect.bottom - (size.cy / 5  + size.cy);
	ScreenToClient(&rect);
	headerCtrl.Create(style, rect, this, 1);
	headerCtrl.SetFont(propList.GetFont());

	// Setup the 2 column headers.
	HD_ITEM hdi;
	hdi.mask = HDI_TEXT | HDI_WIDTH | HDI_FORMAT;
	hdi.fmt = HDF_CENTER | HDF_STRING;
	hdi.cxy = rect.Width() * 3 / 10 + 1;
	hdi.pszText = "Property";
	hdi.cchTextMax = 8;
	headerCtrl.InsertItem(0, &hdi);
	propList.SetColumnWidth(0, hdi.cxy - 2);

	hdi.pszText = "Value";
	hdi.cchTextMax = 5;
	hdi.cxy = rect.Width() - hdi.cxy;
	headerCtrl.InsertItem(1, &hdi);
	propList.SetColumnWidth(1, hdi.cxy - 2);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void QPropPageEntity::OnUpdate(LPARAM lHint, Object *pScope) {
	if (lHint & DUAV_NOQPROPPAGES || !active)
		return;

	Selector *pSlctr = &(pQMainFrame->GetSelector());
	/*
	Object *pEditScope = pSlctr->GetScopePtr();
	if (pScope != NULL && pEditScope != pScope &&
		!pScope->IsMyAncestor(*pEditScope)) {
		return;
	}
	*/

	if (pScope != NULL && !(lHint & (DUAV_OBJSSEL | DUAV_OBJENTMODATTRIB)))
		return;

	CWnd *editCName = GetDlgItem(IDC_PPE_CLASSNAME);
	CWnd *lightButton = GetDlgItem(IDC_PPE_LIGHTBUTTON);

	lightButton->EnableWindow(false);

	Entity *pEntity = NULL;

	if (pSlctr->GetNumMSelectedObjects() == 1) {
		ObjectPtr *pObjPtr = &((pSlctr->GetMSelectedObjects())[0]);
		pEntity = pObjPtr->GetPtr()->GetEntityPtr();
	}
	else { // Display the worldspawn entity.
		Object *pRootObj = pQMainFrame->GetDeskTopDocument()->GetRootObjectPtr();
		if (pRootObj->HasEntity())
			pEntity = pRootObj->GetEntityPtr();
	}

	if (pEntity == NULL) {
		// Disable the child controls.
		headerCtrl.EnableWindow(FALSE);
		propList.EnableWindow(FALSE);
		propList.DeleteAllItems();
		editCName->SetWindowText("");
		return;
	}

	headerCtrl.EnableWindow(TRUE);
	propList.EnableWindow(TRUE);

	CStringList propNames, vals;

	pEntInfo = pEntity->GetEntInfo();
	ASSERT(pEntInfo != NULL);

	QEntEntry *pEntry;
	for (int i = 0; i < pEntInfo->GetNumEntries(); i++) {
		pEntry = pEntInfo->GetEntryNum(i);
		if(pEntry->vartype == VAR_NOTUSER)
			continue;

		if(!strcmpi(pEntry->name, "_color"))
			lightButton->EnableWindow(true);

		propNames.AddTail(pEntry->name);
		const char *arg = pEntity->GetKey(pEntry->name);
		
		if (arg == NULL && pEntry->vartype == VAR_FLAGDEF)
			arg = "0";
		else if (arg == NULL)
			arg = "";

		if(pEntry->vartype == VAR_TYPEDEF) {
			int val = atoi(arg);
			if(val >= 0 && val < pEntry->entDef->GetNumEntries())
				arg = pEntry->entDef->GetEntryNum(val)->name;
		}

		if(pEntry->vartype == VAR_STRDEF) {
			for(int j = 0; j < pEntry->entDef->GetNumEntries(); j++) {
				if(!strcmp(arg, pEntry->entDef->GetEntryNum(j)->str)) {
					arg = pEntry->entDef->GetEntryNum(j)->name;
					break;
				}
			}
		}

		vals.AddTail(arg);
	}

	//
	propNames.AddTail("<new field>");
	vals.AddTail("");
	//

	propList.FillGrids(propNames, vals);

	editCName->SetWindowText(pEntInfo->GetClassname());
}

BOOL QPropPageEntity::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) {
	NMHDR *pNMHDR = (NMHDR *) lParam;

	if (pNMHDR->idFrom == 1) {
		HD_NOTIFY *pHDN = (HD_NOTIFY *) lParam;

		if (pNMHDR->code == HDN_BEGINTRACKW || pNMHDR->code == HDN_BEGINTRACKA) {
			if (GetFocus() != &propList)
				propList.SetFocus();

			if (pHDN->iItem == 1) {
				*pResult = TRUE;
				return TRUE;
			}
		}

		if (pNMHDR->code == HDN_ENDTRACKW || pNMHDR->code == HDN_ENDTRACKA) {
			CRect rect;
			propList.GetWindowRect(&rect);

			int width = Max(pHDN->pitem->cxy, 100);
			width = Min(width, rect.Width() - 100);

			headerCtrl.SetRedraw(FALSE);

			HD_ITEM hd_item;
			hd_item.mask = HDI_WIDTH;
			hd_item.cxy = width;
			headerCtrl.SetItem(0, &hd_item);

			width = rect.Width() - width - 1;
			hd_item.cxy = width;
			headerCtrl.SetItem(1, &hd_item);

			headerCtrl.SetRedraw(TRUE);

			*pResult = TRUE;
			return TRUE;
		}

		if ((pNMHDR->code == HDN_ITEMCHANGEDW || pNMHDR->code == HDN_ITEMCHANGEDA)
			&& (pHDN->pitem->mask & HDI_WIDTH)) {
			propList.SetColumnWidth(pHDN->iItem, pHDN->pitem->cxy - 2);
			propList.Invalidate();
		}

	}

	if(pNMHDR->code == LVN_ENDLABELEDIT) {
		int i, j;
		LV_DISPINFO *pdi = (LV_DISPINFO FAR *) lParam;
		char *arg = pdi->item.pszText;
		char buf[16];

		if(pdi->item.iSubItem == 0) {
			char *name = pdi->item.pszText;
			if(name[0] != '\0' && name[0] != '<') {
				pEntInfo->AddUserEntry(name);
				OnUpdate(0, NULL);
			}
		}

		else if(arg) {
			QEntEntry *pEntry;
			for(i = 0, j = 0; i < pEntInfo->GetNumEntries(); i++) {
				pEntry = pEntInfo->GetEntryNum(i);
				if(pEntry->vartype == VAR_NOTUSER)
					continue;
				if(j++ == pdi->item.iItem)
					break;
			}

			// ghey
			if(pEntry->vartype == VAR_TYPEDEF) {
				for(i = 0; i < pEntry->entDef->GetNumEntries(); i++) {
					if(!strcmp(arg, pEntry->entDef->GetEntryNum(i)->name)) {
						sprintf(buf, "%d", pEntry->entDef->GetEntryNum(i)->val);
						arg = buf;
						break;
					}
				}
			}

			if(pEntry->vartype == VAR_STRDEF) {
				for(i = 0; i < pEntry->entDef->GetNumEntries(); i++) {
					if(!strcmp(arg, pEntry->entDef->GetEntryNum(i)->name))
						break;
				}
				if(i < pEntry->entDef->GetNumEntries())
					arg = pEntry->entDef->GetEntryNum(i)->str;
			}

			Selector *pSlctr = &(pQMainFrame->GetSelector());

			// TODO: handle multi-selection
			// only deal with single selection for now
			Object *pEntObj;
			if (pSlctr->GetNumMSelectedObjects() == 0) {
				pEntObj = pQMainFrame->GetDeskTopDocument()->GetRootObjectPtr();
				ASSERT(pEntObj->HasEntity());
			}
			else {
				pEntObj = (pSlctr->GetMSelectedObjects())[0].GetPtr();
			}

			Entity *pEntity = pEntObj->GetEntityPtr();
			
			const char *oldArg = pEntity->GetKey(pEntry->name);
			if(!oldArg || strcmp(arg, oldArg)) {
				OpEntitySetKey *op = new OpEntitySetKey(pEntObj, pEntry->name, arg);
				pQMainFrame->CommitOperation(*op);
			}
		}

		// Testing....
		if (arg) {
			TRACE1("Field changed: %s\n", arg);
		}
	}

	return QPropPage::OnNotify(wParam, lParam, pResult);
}

EntInfo *QPropPageEntity::pEntInfo = NULL;

void QPropPageEntity::OnClickCallBack(int row, QPropLstInfoStruct *info) {
	ASSERT(pEntInfo != NULL);
	ASSERT(row < pEntInfo->GetNumEntries());

	QEntEntry *pEntry;
	int i, j;

	for (i = 0, j = 0; i < pEntInfo->GetNumEntries(); i++) {
		pEntry = pEntInfo->GetEntryNum(i);
		if(pEntry->vartype == VAR_NOTUSER)
			continue;
		if(j++ == row)
			break;
	}

	if (pEntry->vartype == VAR_TYPEDEF || pEntry->vartype == VAR_STRDEF) {
		info->type = 1;
		for(i = 0; i < pEntry->entDef->GetNumEntries(); i++)
			info->lstItems.AddTail(pEntry->entDef->GetEntryNum(i)->name);
	}
	else if (pEntry->vartype == VAR_FLAGDEF) {
		info->type = 2;
		char flagBuf[16];
		for(i = 0; i < pEntry->entDef->GetNumEntries(); i++) {
			info->lstItems.AddTail(pEntry->entDef->GetEntryNum(i)->name);
			sprintf(flagBuf, "%d", pEntry->entDef->GetEntryNum(i)->val);
			info->lstData.AddTail(flagBuf);
		}
	}
	else {
		info->type = 0;
	}
}

CString *QPropPageEntity::DlgProcCallBack(CString &sInitText) {
	CColorDialog clrDlg;

	unsigned char r = 0, g = 0, b = 0;
	sscanf(sInitText, "(%d, %d, %d)", &r, &g, &g);
	COLORREF clr = (b << 16) | (g < 8) | r;

	clrDlg.m_cc.Flags |= CC_RGBINIT;
	clrDlg.m_cc.rgbResult  = clr;

	int rtnVal = clrDlg.DoModal();
	if (rtnVal == IDOK) {
		sInitText.Format("(%d %d %d)",
						 (clrDlg.GetColor() & 0x000000FF),
						 (clrDlg.GetColor() & 0x0000FF00) >> 8,
						 (clrDlg.GetColor() & 0x00FF0000) >> 16);
		return &sInitText;
	}

	return NULL;
}

void QPropPageEntity::OnLightButton() {
	pQMainFrame->OnEditColorSel();
}
