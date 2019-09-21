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

// QooleDoc.h : interface of the QooleDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_QOOLEDOC_H__8B1BCCFA_FCC8_11D1_8E6B_004005310168__INCLUDED_)
#define AFX_QOOLEDOC_H__8B1BCCFA_FCC8_11D1_8E6B_004005310168__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "objects.h"
#include "game.h"

//======================================================================
// CQDocTemplate
//======================================================================

// Forward declaration
class QooleDoc;

class QDocTemplate : public CMultiDocTemplate {
public:
	QDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass,
		CRuntimeClass* pFrameClass,	CRuntimeClass* pViewClass) :
	CMultiDocTemplate (nIDResource, pDocClass, pFrameClass, pViewClass)	{ };

	QooleDoc *GetObjsDocPtr(Object *pObj);
	CDocument *GetNextDocPtr(CDocument *pDoc);
	virtual CDocument* OpenDocumentFile(LPCTSTR lpszPathName,
		BOOL bMakeVisible = TRUE);
	virtual void SetDefaultTitle(CDocument* pDocument);

};

//======================================================================
// QooleDoc
//======================================================================

// Forward declaration.
class QooleView;

// void UpdateAllViews(CView* pSender, LPARAM lHint = 0L, CObject* pHint = NULL);
// QView::UpdateView(
//  pSend = NULL.
//  pHint = (Object *) object scope ptr.
//  lHint = following flags.

#define DUAV_OBJSSEL			0x0001	// Objects' selection status has changed.
#define DUAV_OBJSLOCK			0x0002	// Objects' locking status has changed.
#define DUAV_OBJSMODSTRUCT		0x0004  // Objects' subcomponents have changed.
#define DUAV_OBJSMODATTRIB		0x0008  // Objects' attribs (ie pos) have changed.
#define DUAV_OBJENTMODATTRIB	0x0010	// Object's Entity's attribs (Ent Properties) have changed.
#define DUAV_OBJTEXMODATTRIB	0x0020  // Object's Textures attribs (Tex Properties) have changed

#define DUAV_SCOPECHANGED		0X0080  // Changes all views' scope on client side.
#define DUAV_SUPDATEALL			0x007F  // Update everything.

#define DUAV_NOQVIEWS			0x0100  // Dont update QViews.
#define DUAV_NOQTREEVIEW		0x0200  // Dont update TreeView.
#define DUAV_NOQPROPPAGES		0x0400	// Dont update the property pages.

#define DUAV_QVIEWS			(DUAV_NOQTREEVIEW | DUAV_NOQPROPPAGES)	// Only update QViews.
#define DUAV_QTREEVIEW		(DUAV_NOQVIEWS | DUAV_NOQPROPPAGES)		// Only update QTreeView.
#define DUAV_QPROPPAGES		(DUAV_NOQVIEWS | DUAV_NOQTREEVIEW)		// Only update property pages.

#define NUM_NONSTD_VIEWS	16

class QooleDoc : public CDocument {
protected: // create from serialization only
	QooleDoc();
	DECLARE_DYNCREATE(QooleDoc)

// Attributes
public:
	virtual void SetModifiedFlag(BOOL bModified = TRUE);
	virtual void SetTitle(LPCTSTR lpszTitle);
	const char *GetDocName(void) const;

// Operations
public:
	BOOL OnExportMap(LPCTSTR lpszPathName);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(QooleDoc)
	public:
	virtual void DeleteContents();
	virtual BOOL OnNewDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void OnCloseDocument();
	//}}AFX_VIRTUAL

	int RegisterNonStdView(QooleView *pView, bool reg);
	void UpdateAllViews(CView* pSender, LPARAM lHint = 0L, CObject* pHint = NULL);

// Implementation
public:
	virtual ~QooleDoc();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	QooleView *pNonStdViews[NUM_NONSTD_VIEWS];

// Generated message map functions
protected:
	//{{AFX_MSG(QooleDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

//========================================
// Qoole specific stuff.
//========================================
// Things to do in document module.
//  1. Store the map data.
//  2. Load and Save map.
//  3. Provide interface layer to modify the map.
//     Must make all modification to the map through
//     these functions.  Do not modify map data
//     indirectly through objects.
//  4. Update relevant views when map is modified.
//  5. Resource (object) locking for network use.
//
//  Don't know what to do with these yet:
//    Texture change.
//    Entity info change.
//    Texture locking implementation.

public:

	Object &GetRootObject(void) { ASSERT(rootObject); return *rootObject; }
	Object *GetRootObjectPtr(void) { return rootObject; }

	void ObjectsAdd(const LinkList<ObjectPtr> &newObjs,	Object &parent,
					bool updateViews = true, bool resetParent = true);
	void ObjectsDelete(const LinkList<ObjectPtr> &delObjs, bool destroyObjs = false,
					   bool updateViews = true, bool resetParent = true);
	void ObjectReplace(Object &replacee, Object &replacer,
					   bool updateViews = true, bool resetParent = true);

	// Move the object node in the map tree.
	void MoveObjNodesInTree(const LinkList<ObjectPtr> &moveObjs,
							Object *pNewParent, bool updateViews = true);

	// Shifts center of object.  Counter adjust all components.
	// void TranslateObjectCenter(Object &obj, const Vector3d &deltaVec);

	void ObjectsMove(const LinkList<ObjectPtr> &mObjs,
					 const TransSpace &operateSpace,
					 const Vector3d &mVec, bool updateViews = true);

	void ObjectsRotate(const LinkList<ObjectPtr> &rObjs, 
					   const TransSpace &operateSpace,
					   const Vector3d &rotPt, float rotAngle,
					   bool updateViews = true);

	void GetObjectsScaleBoundBox(const LinkList<ObjectPtr> &sObjs,
								 const TransSpace &operateSpace,
								 Vector3d &minBoundVec,
								 Vector3d &maxBoundVec) const;

	void GetObjectsScaleBoundBox(Vector3d &minBoundVec,
								 Vector3d &maxBoundVec,
								 const TransSpace &operateSpace) const;
	Object &ObjectsScaleBegin(const LinkList<ObjectPtr> &sObjs,
							  const TransSpace &operateSpace,
							  const Vector3d &basisVec);
	void ObjectsScaleChange(const LinkList<ObjectPtr> &sObjs,
							const Vector3d &newScaleVec,
							bool updateViews = true);
	void ObjectsScaleEnd(const LinkList<ObjectPtr> &sObjs,
						 bool updateViews = true);

	// Brush Manipulation.
	void BrushManipulateBegin(Object &obj);
	Geometry *BrushManipulateEnd(bool updateViews = true);

	void FaceMove(const GPolygon *facePtr, const Vector3d &deltaVec,
				  const TransSpace &operateSpace, bool updateViews = true);
	void EdgeMove(const Edge3d *edgePtr, const Vector3d &deltaVec,
				  const TransSpace &operateSpace, bool updateViews = true);
	void VertexMove(int vIndex, const Vector3d &deltaVec,
					const TransSpace &operateSpace, bool updateViews = true);

	Geometry *BrushReplace(Object &brushObj, Geometry &newBrush,
						   bool updateViews = true);

	// Hollow Brush.
	bool HollowBrush(Object &brushObj, bool inward, float thickness = 8.0f,
					 bool updateViews = true);

	// CSG Subtraction...
	bool CSGSubtract(Object &cuttee, const LinkList<ObjectPtr> &cutters,
					 bool updateViews = true);

	// CSG Intersection...
	Object *CSGIntersect(Object &scope, const LinkList<ObjectPtr> &nodes,
						 bool updateViews = true);

	// Plane clipping.
	void PlaneClipObjects(Plane &cutingPlane, const LinkList<ObjectPtr> &sObjs,
						  bool updateViews = true);

	// Flips an object across the given plane.
	void FlipObjects(const LinkList<ObjectPtr> &sObjs, const Plane &flipPlane,
					 bool updateViews = true);

	// Grouping
	void GroupObjects(Object &newGroup, const LinkList<ObjectPtr> &objPtrs,
					  bool updateViews = true);
	void UngroupObjects(Object &disbandGroup, bool updateViews = true);

	// Texture stuff.
	static bool TextureApplyObj(Object &obj);
	void TextureApplyObjs(LinkList<ObjectPtr> &objs, char *name,
						  bool updateViews = true);
	void TextureApplyFace(Object &texObj, int faceIndex, char *name,
						  bool updateViews = true);
	void TextureManipulate(Object &texObj, int faceIndex, int xOff, int yOff,
						   float rotAng, float xScale, float yScale, bool texLock,
						   bool updateViews = true);
	void ModifyTexSurfAttrib(Object &texObj, int faceIndex, UINT newVal,
							 bool updateViews = true);
	void ModifyTexValAttrib(Object &texObj, int faceIndex, UINT newVal,
							bool updateViews = true);
	void ModifyContentAttrib(Object &opObj, UINT newVal,
							 bool updateViews = true);

	// Entity handling
	void EntitySetKey(Entity *ent, const char *key, const char *arg,
					  bool updateViews = true);
	void EntityApply(Object *obj, Entity *ent, bool updateViews = true);

	//===== Locking =====
	// bool LockObject(Object &obj) { return true; }
	// void UnlockObject(Object &obj) { }

	Game *GetGame(void) { return game; }
	EntList *GetEntList(void) { return entList; }
	char *GetPalName(void) { return palName; }

	void ExportWad(char *filename);
	void SpecifyWads(void);
	static bool MarkTexsUsed(Object &obj);

protected:
	bool LoadDocHeader(LPCTSTR pathName, char *gameName, char *entName, char *palName);
	bool SaveDocHeader(LPCTSTR pathName, LPCTSTR gameName, LPCTSTR entName, LPCTSTR palName);

	Object *rootObject, *holdManipObj;
	Geometry *holdBrush;
	bool brushManipInit;

	Game *game;
	EntList *entList;
	char palName[32];

	static Texture *applyTexture;

#ifdef _SHAREWARE_DEMO_
	int demoBrushCount;
#endif
};

class QooleView {
public:
	QooleView();
	virtual ~QooleView();

	void RegisterDocument(QooleDoc *pDoc);

	virtual void OnUpdate(LPARAM lHint, Object *pScope) {};

protected:
	QooleDoc *pQDoc;

	DECLARE_DYNAMIC(QooleView)
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QOOLEDOC_H__8B1BCCFA_FCC8_11D1_8E6B_004005310168__INCLUDED_)
