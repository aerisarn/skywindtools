// DlgDefCommands.cpp : implementation file
//

#include "DlgMultiPath.h"
#include "..\OneClickNifConvert\resource.h"
#include "FDFileHelper.h"

/*-------------------------------------------- DYNAMIC ---------------------------------------------*/
IMPLEMENT_DYNAMIC(DlgMultiPath, CDialog)

/*-------------------------------------------- MESSAGE_MAP -----------------------------------------*/
BEGIN_MESSAGE_MAP(DlgMultiPath, CDialog)
	ON_BN_CLICKED(IDC_BTN_ADD, &DlgMultiPath::OnBnClickedBtnAdd)
	ON_BN_CLICKED(IDC_BTN_REMOVE, &DlgMultiPath::OnBnClickedBtnRemove)
	ON_BN_CLICKED(IDC_BTN_UP, &DlgMultiPath::OnBnClickedBtnUp)
	ON_BN_CLICKED(IDC_BTN_DOWN, &DlgMultiPath::OnBnClickedBtnDown)
END_MESSAGE_MAP()

/*-------------------------------------------- DlgMultiPath --------------------------------------*/
DlgMultiPath::DlgMultiPath(vector<string> pathList, CWnd* pParent /*=NULL*/)
	:	CDialog  (IDD_MULTIPATH_DIALOG, pParent),
		_pathList(pathList)
{
}

/*-------------------------------------------- ~DlgMultiPath -------------------------------------*/
DlgMultiPath::~DlgMultiPath()
{
}

/*-------------------------------------------- DoDataExchange --------------------------------------*/
void DlgMultiPath::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

/*-------------------------------------------- OnInitDialog ----------------------------------------*/
BOOL DlgMultiPath::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CListBox*	pList((CListBox*) GetDlgItem(IDC_LIST_PATH));

	for (vector<string>::iterator pIter(_pathList.begin()), pEnd(_pathList.end()); pIter != pEnd; ++pIter)
	{
		pList->AddString(CString(pIter->c_str()));
	}

	return TRUE;
}

/*-------------------------------------------- OnInitDialog ----------------------------------------*/
void DlgMultiPath::OnOK() 
{
	CListBox*	pList((CListBox*) GetDlgItem(IDC_LIST_PATH));
	CString		tString;

	_pathList.clear();

	for (int idx(0), cnt(pList->GetCount()); idx < cnt; ++idx)
	{
		pList->GetText(idx, tString);
		_pathList.push_back(CStringA(tString).GetString());
	}

	CDialog::OnOK();
}

/*-------------------------------------------- getPathList -----------------------------------------*/
void DlgMultiPath::getPathList(vector<string>& pathList)
{
	pathList = _pathList;
}

void DlgMultiPath::OnBnClickedBtnAdd()
{
	CString		dirTexPath(FDFileHelper::getFileOrFolder(L"", L"All Files (*.*)|*.*||", L"*", false, true, _T("Please select texture directory")));

	if (!dirTexPath.IsEmpty())
	{
		((CListBox*) GetDlgItem(IDC_LIST_PATH))->AddString(dirTexPath.GetString());
	}
}

void DlgMultiPath::OnBnClickedBtnRemove()
{
	CListBox*	pList((CListBox*) GetDlgItem(IDC_LIST_PATH));

	pList->DeleteString(pList->GetCurSel());
}

void DlgMultiPath::OnBnClickedBtnUp()
{
	CListBox*	pList     ((CListBox*) GetDlgItem(IDC_LIST_PATH));
	CString		tString;
	int			selItemIdx(pList->GetCurSel());

	if (selItemIdx > 0)
	{
		pList->GetText     (selItemIdx, tString);
		pList->DeleteString(selItemIdx--);
		pList->InsertString(selItemIdx, tString.GetString());
		pList->SelectString(0, tString.GetString());
	}
}

void DlgMultiPath::OnBnClickedBtnDown()
{
	CListBox*	pList     ((CListBox*) GetDlgItem(IDC_LIST_PATH));
	CString		tString;
	int			selItemIdx(pList->GetCurSel());

	if (selItemIdx < (pList->GetCount() - 1))
	{
		pList->GetText     (selItemIdx, tString);
		pList->DeleteString(selItemIdx++);
		pList->InsertString(selItemIdx, tString.GetString());
		pList->SelectString(0, tString.GetString());
	}
}
