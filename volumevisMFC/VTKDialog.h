#pragma once


// VTKDialog dialog

class VTKDialog : public CDialog
{
	DECLARE_DYNAMIC(VTKDialog)

public:
	VTKDialog(CWnd* pParent = nullptr);   // standard constructor
	virtual ~VTKDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = VTKDIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
