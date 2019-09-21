// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__8B1BCCF4_FCC8_11D1_8E6B_004005310168__INCLUDED_)
#define AFX_STDAFX_H__8B1BCCF4_FCC8_11D1_8E6B_004005310168__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef NDEBUG
#include <crtdbg.h>
#endif

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afx.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC OLE automation classes
#include <afxmt.h>			// MFC sync support.
#ifndef _AFX_NO_AFXCMN_SUPPORT
#endif // _AFX_NO_AFXCMN_SUPPORT

#ifndef IS_QDRAWDLL

#include <afxcmn.h>			// MFC support for Windows Common Controls
#include <AFXCVIEW.H>		// CTreeView.

#include <mrcstafx.h>
// #include "mrcext.h"			// Docking control bar.
#include "ToolBarEx.h"		// Flat toolbars.

#include "ProgressBar.h"
#include "ProgressWnd.h"
#include "DirDialog.h"

#include <mmsystem.h>

#endif // IS_QDRAWDLL

// MegaGraphics Library
//#include "mgl.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__8B1BCCF4_FCC8_11D1_8E6B_004005310168__INCLUDED_)
