
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "volumevisMFC.h"

#include <string>

#include "MainFrm.h"

#include "Interactor.h"

#include "VolVis.h"
#include "nbsMetadata.h"


//#include <vtkAutoInit.h> 
//VTK_MODULE_INIT(vtkRenderingOpenGL2)

using namespace std::string_literals;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame construction/destruction

CMainFrame::CMainFrame():

	ren( vtkSmartPointer<vtkRenderer>::New() ),

	dialog (new vtkMFCWindow(this) ),
	renWin ( dialog->GetRenderWindow()),

	iren (vtkSmartPointer<vtkRenderWindowInteractor>::New()),

	style (vtkSmartPointer<fmiVis::VisualizationInteractor>::New()),


	meta(),

	vm ( fmiVis::ViewportManager{}),
	
	widgets()
{

	fmiVis::InitConfig();
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: Delete these three lines if you don't want the toolbar to be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	//SpawnVTK();

	return 0;
}

void CMainFrame::SpawnVTK()
{
	ren->SetBackground(0.4, 0.4, 0.8);

	// 	auto renWin = vtkSmartPointer<vtkWin32OpenGLRenderWindow>::New();
	// 
	// 
	// 	renWin->SetSize(800, 800);
	// 	renWin->AddRenderer(ren);



	renWin->SetSize(800, 800);

	dialog->GetRenderWindow()->AddRenderer(ren);


	iren->SetRenderWindow(renWin);

	iren->SetInteractorStyle(style);


	ren->Render();



	std::string file = "D:/3D-dataa/201703210327_hirlam_skandinavia_mallipinta.sqd"s;

	std::string surfFile;

	std::string drawParamPath = "D:/3D-dataa/DrawParams";

	meta.init(file);



	widgets = fmiVis::Make3DView(file, surfFile, drawParamPath, meta, vm, iren.Get(), renWin, style.Get());

	style->GetImpl().setVM(&vm);

	iren->Initialize();

	renWin->WindowInitialize();


	dialog->ShowWindow(SW_SHOW);
	dialog->UpdateWindow();


	ren->ResetCamera();

	//vtkOutputWindowDisplayGenericWarningText(renWin->ReportCapabilities());
	//vtkOutputWindowDisplayGenericWarningText("\n");
	//iren->Start();
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
		 | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU;

	return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers

