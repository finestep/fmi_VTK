// VTKDialog.cpp : implementation file
//

#include "stdafx.h"
#include "volumevisMFC.h"
#include "VTKDialog.h"
#include "afxdialogex.h"


// VTKDialog dialog

IMPLEMENT_DYNAMIC(VTKDialog, CDialog)

VTKDialog::VTKDialog(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_VTKDIALOG, pParent)
{

}

VTKDialog::~VTKDialog()
{
}

void VTKDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(VTKDialog, CDialog)
END_MESSAGE_MAP()


// VTKDialog message handlers
