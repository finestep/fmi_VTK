
// volumevisMFCView.h : interface of the CvolumevisMFCView class
//

#pragma once


class CvolumevisMFCView : public CView
{
protected: // create from serialization only
	CvolumevisMFCView();
	DECLARE_DYNCREATE(CvolumevisMFCView)

// Attributes
public:
	CvolumevisMFCDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	void OnInitialUpdate();
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CvolumevisMFCView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	COleDropTarget dropTarget;
// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in volumevisMFCView.cpp
inline CvolumevisMFCDoc* CvolumevisMFCView::GetDocument() const
   { return reinterpret_cast<CvolumevisMFCDoc*>(m_pDocument); }
#endif

