
// MainFrm.h : interface of the CMainFrame class
//

#pragma once
#include "Interactor.h"
#include "VolVis.h"

namespace fmiVis {
	struct nbsMetadata;
}
class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CToolBar          m_wndToolBar;
	CStatusBar        m_wndStatusBar;

	vtkSmartPointer<vtkRenderer> ren;
	vtkMFCWindow* dialog;


	vtkWin32OpenGLRenderWindow* renWin;

	vtkSmartPointer<vtkRenderWindowInteractor> iren;

	vtkSmartPointer<fmiVis::VisualizationInteractor> style;


	fmiVis::ViewportManager vm;

	nbsMetadata meta;

	fmiVis::Widgets3D widgets;


// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	void SpawnVTK();

	DECLARE_MESSAGE_MAP()

};


