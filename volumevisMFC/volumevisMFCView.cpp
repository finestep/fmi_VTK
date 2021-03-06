
// volumevisMFCView.cpp : implementation of the CvolumevisMFCView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "volumevisMFC.h"
#endif

#include "volumevisMFCDoc.h"
#include "volumevisMFCView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CvolumevisMFCView

IMPLEMENT_DYNCREATE(CvolumevisMFCView, CView)

BEGIN_MESSAGE_MAP(CvolumevisMFCView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CvolumevisMFCView construction/destruction

CvolumevisMFCView::CvolumevisMFCView()
{
	// TODO: add construction code here

}

CvolumevisMFCView::~CvolumevisMFCView()
{
}

BOOL CvolumevisMFCView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

void CvolumevisMFCView::OnInitialUpdate() {
	dropTarget.Register(this);
}

// CvolumevisMFCView drawing

void CvolumevisMFCView::OnDraw(CDC* /*pDC*/)
{
	CvolumevisMFCDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// CvolumevisMFCView printing

BOOL CvolumevisMFCView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CvolumevisMFCView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CvolumevisMFCView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CvolumevisMFCView diagnostics

#ifdef _DEBUG
void CvolumevisMFCView::AssertValid() const
{
	CView::AssertValid();
}

void CvolumevisMFCView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CvolumevisMFCDoc* CvolumevisMFCView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CvolumevisMFCDoc)));
	return (CvolumevisMFCDoc*)m_pDocument;
}
#endif //_DEBUG


// CvolumevisMFCView message handlers
