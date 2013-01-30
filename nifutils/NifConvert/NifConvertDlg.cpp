// NifConvertDlg.cpp : implementation file
//

#include "..\Common\stdafx.h"
#include "NifConvert.h"
#include "NifConvertDlg.h"
#include "..\Common\Configuration.h"
#include "..\Common\FDFileHelper.h"
#include "..\Common\NifConvertUtility.h"

#include "..\Common\NifCollisionUtility.h"
#include "..\Common\version.h"




using namespace NifUtility;

extern	CNifConvertDlg		dlg;
extern NifUtlMaterialList		glMaterialList;
extern Configuration		glConfig;

//  ToolTip definitions
struct FDToolTipText
{
	int		_uid;
	string	_text;
};

static FDToolTipText	glTTText[] = {{IDC_CHECK_TANGENTSPACE, "Calculate normals and binormals in NiTriShapeData"},
{IDC_CHECK_NITRISHAPEPROPERTIES, "Make sure properties of NiTriShape are in valid order"},
{IDC_RADIO_VCADD, "Add default vertext colors of 0xffffff in case of set flag SLSF2_Vertex_Colors"},
{IDC_RADIO_VCREMOVE, "Remove flag SLSF2_Vertex_Colors in case of missing vertex colors"},
{IDC_COMBO_TEXTURE, "Define path to texture files written to BSShaderTextureSet"},
{IDC_COMBO_TEMPLATE, "Define NIF file used as template for converting and adding collision data"},
{IDC_EDIT_INPUT, "Path to source NIF-file to be converted"},
{IDC_EDIT_OUTPUT, "Path to destination NIF-file"},
{IDC_BUTTON_INPUT, "Choose source NIF-file to be converted"},
{IDC_BUTTON_OUTPUT, "Choose destination NIF-file"},
{IDC_BUTTON_TEMPLATE, "Choose path to template files and scan recursively"},
{IDC_BUTTON_TEXTURE, "Choose path to Skyrim and scan Data/Textures recursively"},
{IDC_RICHEDIT_LOG, "Some log output"},
{-1, ""}
};

//  static wrapper function
void logCallback(const int type, const char* pMessage)
{
	dlg.logMessage(type, pMessage);
}

// CNifConvertDlg dialog

CNifConvertDlg::CNifConvertDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNifConvertDlg::IDD, pParent)
{
	//m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CNifConvertDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RICHEDIT_LOG, m_logView);
}

BEGIN_MESSAGE_MAP(CNifConvertDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_INPUT, &CNifConvertDlg::OnBnClickedButtonInput)
	ON_BN_CLICKED(IDC_BUTTON_OUTPUT, &CNifConvertDlg::OnBnClickedButtonOutput)
	ON_BN_CLICKED(IDOK, &CNifConvertDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_TEMPLATE, &CNifConvertDlg::OnBnClickedButtonTemplate)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE, &CNifConvertDlg::OnBnClickedButtonTexture)
	ON_COMMAND(ID_DEFAULT_SAVESETTINGS, &CNifConvertDlg::OnDefaultSavesettings)
	ON_COMMAND(ID_DEFAULT_RELOADDIRECTORIES, &CNifConvertDlg::OnDefaultReloaddirectories)
	ON_BN_CLICKED(IDC_BUTTON_DIRINPUT, &CNifConvertDlg::OnBnClickedButtonInput3)
	ON_BN_CLICKED(IDC_BUTTON_DIROUTPUT, &CNifConvertDlg::OnBnClickedButtonOutput3)
	//ON_EN_CHANGE(IDC_EDIT_INPUT, &CNifConvertDlg::OnEnChangeEditInput)

	ON_EN_CHANGE(IDC_EDIT_OUTPUT, &CNifConvertDlg::OnEnChangeEditOutput)
	ON_BN_CLICKED(IDOK2, &CNifConvertDlg::OnBnClickedOk2)
	ON_BN_CLICKED(IDC_CHECK_USE_CONVERTED, &CNifConvertDlg::OnBnClickedCheckUseConverted)
END_MESSAGE_MAP()


void CNifConvertDlg::parseDir(CString path, set<string>& directories, bool doDirs)
{
	CFileFind   finder;
	BOOL        result(FALSE);

	result = finder.FindFile(path + _T("\\*.*"));

	while (result)
	{
		result = finder.FindNextFileW();

		if (finder.IsDots())    continue;
		if (finder.IsDirectory() && doDirs)
		{
			CString   newDir(finder.GetFilePath());
			CString   tDir = newDir.Right(newDir.GetLength() - newDir.Find(_T("\\Textures\\")) - 1);

			directories.insert(CStringA(tDir).GetString());

			parseDir(newDir, directories);
		}
		else if (!finder.IsDirectory() && !doDirs)
		{
			CString   newDir(finder.GetFilePath());
			CString   tDir = newDir.Right(newDir.GetLength() - path.GetLength() - 1);

			directories.insert(CStringA(tDir).GetString());
		}
	}
}

// CNifConvertDlg message handlers

BOOL CNifConvertDlg::PreTranslateMessage(MSG* pMsg)
{
//	m_toolTip.RelayEvent(pMsg);
	//TODO enable tool tip
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CNifConvertDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	GetDlgItem(IDOK2)                                       ->EnableWindow(TRUE);
	((CButton*) GetDlgItem(IDC_RADIO_VCREMOVE))            ->SetCheck(BST_CHECKED);
	((CButton*) GetDlgItem(IDC_CHECK_TANGENTSPACE))        ->SetCheck(BST_CHECKED);
	((CButton*) GetDlgItem(IDC_CHECK_NITRISHAPEPROPERTIES))->SetCheck(BST_CHECKED);

	//  prepare log view
	CHARFORMAT	cf = { 0 };

	cf.cbSize    = sizeof(cf);
	cf.dwMask    = CFM_FACE | CFM_SIZE | CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE | CFM_STRIKEOUT | CFM_PROTECTED;
	cf.dwEffects = 0;
	cf.yHeight   = 127;
	lstrcpy(cf.szFaceName, _T("Small Fonts"));

	m_logView.SetDefaultCharFormat(cf);
	m_logView.SetBackgroundColor(FALSE, RGB(0x00, 0x00, 0x00));
	m_logView.SetReadOnly       (TRUE);

	//Todo reenable  prepare tool tips
	/*
	if (m_toolTip.Create(this, TTS_BALLOON))
	{
		for (short i(0); glTTText[i]._uid != -1; ++i)
		{
			m_toolTip.AddTool(GetDlgItem(glTTText[i]._uid), CString(glTTText[i]._text.c_str()));
		}

		m_toolTip.Activate(TRUE);
	}*/
	
	//  scan path and fill combo boxes
	OnDefaultReloaddirectories();

	//  set title
	char	cbuffer[100];

	sprintf(cbuffer, "NifConMerge v%d.%d.%d.%04d", FD_VERSION, FD_SUBVERSION, FD_REVISION, FD_BUILD);
	SetWindowText(CString(cbuffer));

	return TRUE;  // return TRUE  unless you set the focus to a control
}

//  If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CNifConvertDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CNifConvertDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CNifConvertDlg::OnBnClickedButtonInput()
{
	if (m_fileNameAry[0].IsEmpty())
	{
		m_fileNameAry[0] = glConfig._dirSource.c_str();
	}
	m_fileNameAry[0] = FDFileHelper::getFileOrFolder(m_fileNameAry[0], L"Nif Files (*.nif)|*.nif||", L"nif", false, true);
	GetDlgItem(IDC_EDIT_INPUT)->SetWindowText(m_fileNameAry[0]);
	if (!m_fileNameAry[0].IsEmpty() && !m_fileNameAry[1].IsEmpty())
	{
		GetDlgItem(IDOK)->EnableWindow(TRUE);
	}
}

void CNifConvertDlg::OnBnClickedButtonOutput()
{
	if (m_fileNameAry[1].IsEmpty())
	{
		m_fileNameAry[1] = glConfig._dirDestination.c_str();
	}



	m_fileNameAry[1] = FDFileHelper::getFileOrFolder(m_fileNameAry[1], L"Nif Files (*.nif)|*.nif||", L"nif", true);

}

void CNifConvertDlg::OnBnClickedButtonTemplate()
{
	CString		pathTmpl(glConfig._pathTemplate.c_str());

	//  get new path to templates
	pathTmpl = FDFileHelper::getFileOrFolder(pathTmpl + _T("\\"), L"*.nif (*.nif)|*.nif||", L"nif", false, true);

	//  if path given - reload templates
	if (!pathTmpl.IsEmpty())
	{
		CComboBox*	pCBox = (CComboBox*) GetDlgItem(IDC_COMBO_TEMPLATE);
		set<string> directories;

		//  reset selection box
		pCBox->ResetContent();

		//  parse directory
		parseDir(pathTmpl, directories, false);

		//  in case of existing data
		if (directories.size() > 0)
		{
			//  add files to selection box
			for (set<string>::iterator tIter = directories.begin(); tIter != directories.end(); tIter++)
			{
				if ((*tIter).rfind(".nif") == string::npos)		continue;
				pCBox->AddString(CString((*tIter).c_str()));
			}
			pCBox->SetCurSel(0);

			//  set new path to config
			glConfig._pathTemplate = CStringA(pathTmpl).GetString();

			//  reset last choosen template
			glConfig._lastTemplate = *directories.begin();
		}
	}
}

void CNifConvertDlg::OnBnClickedButtonTexture()
{
	CString		pathTmpl(glConfig._pathSkyrim.c_str());

	//  get new path to templates
	pathTmpl = FDFileHelper::getFileOrFolder(pathTmpl + _T("\\"), L"TESV.exe (TESV.exe)|TESV.exe||", L"exe", false, true);

	//  if path given - reload templates
	if (!pathTmpl.IsEmpty())
	{
		CComboBox*	pCBox = (CComboBox*) GetDlgItem(IDC_COMBO_TEXTURE);
		set<string> directories;

		//  reset selection box
		pCBox->ResetContent();

		//  set new path to config
		glConfig._pathSkyrim = CStringA(pathTmpl).GetString();

		//  add subdirectory
		pathTmpl += "\\Data\\Textures";

		//  parse directory
		parseDir(pathTmpl, directories);

		//  in case of existing data
		if (directories.size() > 0)
		{
			//  add files to selection box
			for (set<string>::iterator tIter = directories.begin(); tIter != directories.end(); tIter++)
			{
				pCBox->AddString(CString((*tIter).c_str()));
			}
			pCBox->SetCurSel(0);

			//  reset last choosen template
			glConfig._lastTexture = *directories.begin();
		}
	}
}

void CNifConvertDlg::OnBnClickedOk()
{
	NifConvertUtility		ncUtility;
	NifCollisionUtility     ncColUtility(glMaterialList);

	//TODO
	unsigned short		ncReturn(NCU_OK);

	//  copy strings from input
	GetDlgItem(IDC_EDIT_INPUT)    ->GetWindowTextW(m_fileNameAry[0]);
	GetDlgItem(IDC_EDIT_OUTPUT)   ->GetWindowTextW(m_fileNameAry[1]);
	GetDlgItem(IDC_COMBO_TEMPLATE)->GetWindowTextW(m_fileNameAry[2]);
	GetDlgItem(IDC_COMBO_TEXTURE) ->GetWindowTextW(m_texturePath);

	//  set path
	ncUtility.setTexturePath((CStringA(m_texturePath)).GetString());

	//  set callback for log info
	ncUtility.setLogCallback(logCallback);

	//  set flags
	ncUtility.setVertexColorHandling  ((VertexColorHandling) (GetCheckedRadioButton(IDC_RADIO_VCREMOVE, IDC_RADIO_VCADD) - IDC_RADIO_VCREMOVE));
	ncUtility.setUpdateTangentSpace   (((CButton*) GetDlgItem(IDC_CHECK_TANGENTSPACE))->GetCheck() != FALSE);
	ncUtility.setReorderProperties    (((CButton*) GetDlgItem(IDC_CHECK_NITRISHAPEPROPERTIES))->GetCheck() != FALSE);

	//  convert nif
	std::string src = (CStringA(m_fileNameAry[0])).GetString();
	std::string dst = (CStringA(m_fileNameAry[1])).GetString();
	std::string templ = (CStringA(CString(glConfig._pathTemplate.c_str()) + L"\\" + m_fileNameAry[2])).GetString();
	ncReturn = ncUtility.convertShape(src, dst, templ);
	if (ncReturn != NCU_OK)
	{
		logMessage(NCU_MSG_TYPE_ERROR, "NifConverter returned code: " + ncReturn);
	}
	else
	{
		logMessage(NCU_MSG_TYPE_ERROR, "Nif converted successfully");
	}
	logMessage(NCU_MSG_TYPE_INFO, "- - - - - - - - - -");
}

void CNifConvertDlg::logMessage(const int type, const char* pMessage)
{
	CHARFORMAT	charFormat;
	COLORREF	color       (RGB(0x00, 0xD0, 0x00));
	CString		text        (pMessage);
	int			lineCountOld(m_logView.GetLineCount());
	int			tType       (type);

	//  special handling of type settings
	if (pMessage[0] == '^')
	{
		tType = atoi(pMessage+1);
		text  = (pMessage+2);
	}

	//  append of newline necessary?
	if (pMessage[strlen(pMessage) - 1] != '\n')
	{
		text += _T("\r\n");
	}

	//  get color by type
	switch (tType)
	{
	case NCU_MSG_TYPE_ERROR:
		{
			color = RGB(0xFF, 0x00, 0x00);
			break;
		}

	case NCU_MSG_TYPE_WARNING:
		{
			color = RGB(0xFF, 0xFF, 0x00);
			break;
		}

	case NCU_MSG_TYPE_TEXTURE:
		{
			color = RGB(0x50, 0x50, 0xFF);
			break;
		}

	case NCU_MSG_TYPE_TEXTURE_MISS:
		{
			color = RGB(0xC0, 0x50, 0xFF);
			break;
		}

	case NCU_MSG_TYPE_SUB_INFO:
		{
			color = RGB(0x00, 0x60, 0x00);
			break;
		}
	}

	//  character format
	charFormat.cbSize      = sizeof(charFormat);
	charFormat.dwMask      = CFM_COLOR;
	charFormat.dwEffects   = 0;
	charFormat.crTextColor = color;

	//  select  nothing, set format and append new text
	m_logView.SetSel(-1, -1);
	m_logView.SetSelectionCharFormat(charFormat);
	m_logView.ReplaceSel(text);

	//  scroll to end of text
	m_logView.LineScroll(m_logView.GetLineCount() - lineCountOld);
}

void CNifConvertDlg::OnContextMenu(CWnd *pWnd, CPoint point)
{
	CMenu*	pPopUp(NULL);
	CMenu	menuBar;

	menuBar.LoadMenu(IDR_MENU_POPUP);

	pPopUp = menuBar.GetSubMenu(0);
	pPopUp->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
}

void CNifConvertDlg::OnDefaultSavesettings()
{
	//  copy strings from input
	GetDlgItem(IDC_EDIT_INPUT)    ->GetWindowTextW(m_fileNameAry[0]);
	GetDlgItem(IDC_EDIT_OUTPUT)   ->GetWindowTextW(m_fileNameAry[1]);
	GetDlgItem(IDC_EDIT_DIRINPUT)    ->GetWindowTextW(m_fileNameAry[3]);
	GetDlgItem(IDC_EDIT_DIROUTPUT)   ->GetWindowTextW(m_fileNameAry[4]);
	GetDlgItem(IDC_COMBO_TEMPLATE)->GetWindowTextW(m_fileNameAry[2]);
	GetDlgItem(IDC_COMBO_TEXTURE) ->GetWindowTextW(m_texturePath);

	//  write last used options
	glConfig._vertColHandling   = (GetCheckedRadioButton(IDC_RADIO_VCREMOVE, IDC_RADIO_VCADD) - IDC_RADIO_VCREMOVE);
	glConfig._upTangentSpace    = (((CButton*) GetDlgItem(IDC_CHECK_TANGENTSPACE))->GetCheck() != FALSE);
	glConfig._reorderProperties = (((CButton*) GetDlgItem(IDC_CHECK_NITRISHAPEPROPERTIES))->GetCheck() != FALSE);
	glConfig._lastTexture       = CStringA(m_texturePath).GetString();
	glConfig._lastTemplate      = CStringA(m_fileNameAry[2]).GetString();
	glConfig._dirSource         = CStringA(m_fileNameAry[0].Left(m_fileNameAry[0].ReverseFind('\\') + 1)).GetString();
	glConfig._dirDestination    = CStringA(m_fileNameAry[1].Left(m_fileNameAry[1].ReverseFind('\\') + 1)).GetString();
	glConfig._dirBatchSource         = CStringA(m_fileNameAry[3]).GetString();
	glConfig._dirBatchDestination    = CStringA(m_fileNameAry[4]).GetString();

	//  write last config
	glConfig.write();
}

void CNifConvertDlg::OnDefaultReloaddirectories()
{
	logMessage(NCU_MSG_TYPE_INFO, "scanning directories...");

	//  re-scan texture path
	CComboBox*	pCBox = (CComboBox*) GetDlgItem(IDC_COMBO_TEXTURE);
	CString		pathTmpl(glConfig._pathSkyrim.c_str());
	set<string> directories;

	//  reset selection box
	pCBox->ResetContent();

	//  add subdirectory
	pathTmpl += "\\Data\\Textures";

	//  parse directory
	parseDir(pathTmpl, directories);

	//  in case of existing data
	if (directories.size() > 0)
	{
		//  add files to selection box
		for (set<string>::iterator tIter = directories.begin(); tIter != directories.end(); tIter++)
		{
			pCBox->AddString(CString((*tIter).c_str()) + _T("\\"));
		}
		pCBox->SelectString(-1, CString(glConfig._lastTexture.c_str()));

		//  make sure one entry is selected
		GetDlgItem(IDC_COMBO_TEXTURE) ->GetWindowTextW(pathTmpl);
		if (pathTmpl.IsEmpty())
		{
			pCBox->SetCurSel(0);
		}

		//  reset last choosen template
		GetDlgItem(IDC_COMBO_TEXTURE)->GetWindowTextW(pathTmpl);
		glConfig._lastTexture = CStringA(pathTmpl).GetString();
	}

	//  re-scan templates
	pCBox    = (CComboBox*) GetDlgItem(IDC_COMBO_TEMPLATE);
	pathTmpl = glConfig._pathTemplate.c_str();
	directories.clear();

	//  reset selection box
	pCBox->ResetContent();

	//  parse directory
	parseDir(pathTmpl, directories, false);

	//  in case of existing data
	if (directories.size() > 0)
	{
		//  add files to selection box
		for (set<string>::iterator tIter = directories.begin(); tIter != directories.end(); tIter++)
		{
			if ((*tIter).rfind(".nif") == string::npos)		continue;
			pCBox->AddString(CString((*tIter).c_str()));
		}
		pCBox->SelectString(-1, CString(glConfig._lastTemplate.c_str()));

		//  make sure one entry is selected
		GetDlgItem(IDC_COMBO_TEMPLATE) ->GetWindowTextW(pathTmpl);
		if (pathTmpl.IsEmpty())
		{
			pCBox->SetCurSel(0);
		}

		//  reset last choosen template
		GetDlgItem(IDC_COMBO_TEMPLATE)->GetWindowTextW(pathTmpl);
		glConfig._lastTemplate = CStringA(pathTmpl).GetString();
	}

	logMessage(NCU_MSG_TYPE_INFO, "scan done.");
}


void CNifConvertDlg::OnBnClickedButtonInput3()
{

	// TODO: Add your control notification handler code here
	TCHAR        dir[MAX_PATH];
	BROWSEINFO   bi = { 0 };
	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
	BOOL foundDir = false;

	if(pidl) {
		foundDir = SHGetPathFromIDList(pidl, dir);
	}
	if(foundDir) {
		GetDlgItem(IDC_EDIT_DIRINPUT)->SetWindowText(dir);
	}
	m_fileNameAry[3] = dir;

}


void CNifConvertDlg::OnBnClickedButtonOutput3()
{

	//DWORD options;
	//fileDialog->GetOptions(&options);
	//fileDialog->SetOptions(FOS_PICKFOLDERS);
	TCHAR        dir[MAX_PATH];
	BROWSEINFO   bi = { 0 };
	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
	BOOL foundDir = false;

	if(pidl) {
		foundDir = SHGetPathFromIDList(pidl, dir);
	}
	if(foundDir) {
		GetDlgItem(IDC_EDIT_DIROUTPUT)->SetWindowText(dir);
	}
	m_fileNameAry[4] = dir;

}





void CNifConvertDlg::OnEnChangeEditOutput( )
{

}

void CNifConvertDlg::processFilesInDir(TCHAR input_root[MAX_PATH], TCHAR output_root[MAX_PATH], 
									   NifConvertUtility& ncUtility, std::string templ, std::string col_templ,
									   NifCollisionUtility& ncColUtility, bool addCollision,
									   bool useConvertedNif)
{
	unsigned short		ncReturn(NCU_OK);
	HANDLE hFile;
	WIN32_FIND_DATA FindFileData;

	HANDLE testFile;
	WIN32_FIND_DATA testFindFileData;

	TCHAR find_file_query[MAX_PATH];
	TCHAR find_template[MAX_PATH];
	TCHAR find_col_template[MAX_PATH];

	StringCchCopy(find_template, MAX_PATH, input_root);
	StringCchCat(find_template, MAX_PATH, TEXT("\\template.nif"));                         
	hFile = FindFirstFile(find_template, &FindFileData);
	if (hFile != INVALID_HANDLE_VALUE) {
		templ = CStringA(find_template).GetString();
	}

	StringCchCopy(find_col_template, MAX_PATH, input_root);
	StringCchCat(find_col_template, MAX_PATH, TEXT("\\template_col.nif"));                         
	hFile = FindFirstFile(find_col_template, &FindFileData);
	if (hFile != INVALID_HANDLE_VALUE) {
		col_templ = CStringA(find_col_template).GetString();
	}

	//contains C:/inputs
	StringCchCopy(find_file_query, MAX_PATH, input_root);
	//want to iterate over C:/inputs/*.nif
	StringCchCat(find_file_query, MAX_PATH, TEXT("\\*"));

	//assert valid input dir
	hFile = FindFirstFile(find_file_query, &FindFileData);
	if (hFile == INVALID_HANDLE_VALUE) {
		logMessage(NCU_MSG_TYPE_ERROR, "invalid src (sub)dir");    
		return;
	}


	do
	{
		//massage into a std::str containing full path to file/folder.
		std::string src = (CStringA(input_root)).GetString();
		src.append("\\");
		src.append((CStringA(FindFileData.cFileName)).GetString());

		//same for output
		std::string dst = (CStringA(output_root)).GetString();
		dst.append("\\");
		dst.append((CStringA(FindFileData.cFileName)).GetString());

		if(!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			
			if(addCollision) {
				if(src.find("_col.nif") != std::string::npos) {
					//skip _col files
					continue;
				}
			}
			//assert path to src is valid
			CString s1(src.c_str());
			testFile = FindFirstFile(s1, &testFindFileData);
			if (testFile == INVALID_HANDLE_VALUE) {
				logMessage(NCU_MSG_TYPE_ERROR, (src + " is invalid file").c_str() );   
				continue;
			}
			//assert its a nif file
			CString ext = PathFindExtension(CString(src.c_str()));
			if (  ext.Compare(L".nif") ) {
				logMessage(NCU_MSG_TYPE_ERROR, (src + " is not a nif file").c_str() );  
				continue;
			}

			//skip templates
			if(CStringA(FindFileData.cFileName).Compare("template.nif") == 0) {
				logMessage(NCU_MSG_TYPE_ERROR, "Skipping template");
				continue;
			}
			if(CStringA(FindFileData.cFileName).Compare("template_col.nif") == 0) {
				logMessage(NCU_MSG_TYPE_ERROR, "Skipping template_col.nif");
				continue;
			}

			

			//convert it, hope dst exists...
			try {
				ncReturn = ncUtility.convertShape(src, dst, templ);
				if (ncReturn != NCU_OK) {
					logMessage(NCU_MSG_TYPE_ERROR, "NifConverter returned code: " + ncReturn);
				} else {
					logMessage(NCU_MSG_TYPE_ERROR, CStringA(src.c_str()) + " converted successfully");
				}
			} catch (const runtime_error& error) {			
				logMessage(NCU_MSG_TYPE_ERROR, "NifConverter crashed with error: " + CStringA(error.what()) );
				continue;
			} catch(exception e) {
				logMessage(NCU_MSG_TYPE_ERROR, "NifConverter Internal error: " + CStringA(e.what()) );
				continue;
			}

			if(ncReturn == NCU_OK && addCollision) {
				std::string collisionSrc = dst;

				if(useConvertedNif) {
					//make a copy of the conversion(dst) and convert
					collisionSrc.insert(collisionSrc.length()-4, "_temp", 5);
					CopyFile(CString(dst.c_str()), CString(collisionSrc.c_str()), false);		
				} else {
					collisionSrc = src;
					collisionSrc.insert(collisionSrc.length()-4, "_col", 4);									
				}

				try {
					ncReturn = ncColUtility.addCollision(collisionSrc, dst, col_templ);
					if(addCollision && (useConvertedNif || removeCollisionSrc) ) {
						DeleteFile( CString(collisionSrc.c_str()));
					}													   
				} catch (const runtime_error& error) {
					logMessage(NCU_MSG_TYPE_ERROR, "NifCollisionConverter crashed with error: " + CStringA(error.what()) );
				} catch(exception e) {
					logMessage(NCU_MSG_TYPE_ERROR, "NifCollisionConverter Internal error: " + CStringA(e.what()) );
				}

				if (ncReturn != NCU_OK)
				{
					logMessage(NCU_MSG_TYPE_ERROR, "NifCollisionConverter returned code: " + ncReturn );
				}
				else
				{
					logMessage(NCU_MSG_TYPE_ERROR, CStringA(collisionSrc.c_str()) + " added collision successfully");
				}
			}
		}
		else { //is directory

			CString dir_name = FindFileData.cFileName;
			//skip the . and .. dirs to avoid infinite recursion
			if (  dir_name.Compare(L".") == 0 || dir_name.Compare(L"..") == 0) {
				continue;
			}

			//recurse into subdir
			TCHAR input_subdir[MAX_PATH];
			StringCchCopy(input_subdir, MAX_PATH, CString(src.c_str()));

			TCHAR output_subdir[MAX_PATH];
			StringCchCopy(output_subdir, MAX_PATH, CString(dst.c_str()));

			//does subdir exist?
			CString s1(dst.c_str());
			testFile = FindFirstFile(s1, &testFindFileData);
			if (testFile == INVALID_HANDLE_VALUE) {
				//create subdir
				CreateDirectory(output_subdir, NULL);
			}

			processFilesInDir(input_subdir, output_subdir, ncUtility, templ, col_templ, ncColUtility, addCollision, useConvertedNif);

		}
	}while(FindNextFile(hFile, &FindFileData)); //keep on trucking through files
}

void CNifConvertDlg::OnBnClickedOk2()
{
	NifConvertUtility		ncUtility;
	NifCollisionUtility		ncColUtility(glMaterialList);
	unsigned short		ncReturn(NCU_OK);
	
	//TODO: promote to protected, remove func params
	bool addCollision = false;
	bool useConvertedNif = false;
	removeCollisionSrc = false;

	//  copy strings from input
	GetDlgItem(IDC_EDIT_DIRINPUT)    ->GetWindowTextW(m_fileNameAry[3]);
	GetDlgItem(IDC_EDIT_DIROUTPUT)   ->GetWindowTextW(m_fileNameAry[4]);
	GetDlgItem(IDC_COMBO_TEMPLATE)->GetWindowTextW(m_fileNameAry[2]);
	GetDlgItem(IDC_COMBO_TEXTURE) ->GetWindowTextW(m_texturePath);
	if(((CButton*) GetDlgItem(IDC_CHECK_DO_COLLISION))->GetCheck() ) {
		addCollision = true;
	}

	if(((CButton*) GetDlgItem(IDC_CHECK_REMOVE_CONVERTED))->GetCheck() ) {
		removeCollisionSrc = true;
	}

	if(((CButton*) GetDlgItem(IDC_CHECK_USE_CONVERTED))->GetCheck() ) {
		useConvertedNif = true;
	}

	//  set path
	ncUtility.setTexturePath((CStringA(m_texturePath)).GetString());

	//  set callback for log info
	ncUtility.setLogCallback(logCallback);
	ncColUtility.setLogCallback(logCallback);

	//  set flags
	ncUtility.setVertexColorHandling  ((VertexColorHandling) (GetCheckedRadioButton(IDC_RADIO_VCREMOVE, IDC_RADIO_VCADD) - IDC_RADIO_VCREMOVE));
	ncUtility.setUpdateTangentSpace   (((CButton*) GetDlgItem(IDC_CHECK_TANGENTSPACE))->GetCheck() != FALSE);
	ncUtility.setReorderProperties    (((CButton*) GetDlgItem(IDC_CHECK_NITRISHAPEPROPERTIES))->GetCheck() != FALSE);

	TCHAR input_root[MAX_PATH];
	TCHAR output_root[MAX_PATH];

	StringCchCopy(input_root, MAX_PATH, m_fileNameAry[3]);
	StringCchCopy(output_root, MAX_PATH, m_fileNameAry[4]);

	if(!lstrcmp(input_root, L"")) {
		logMessage(NCU_MSG_TYPE_ERROR, "no src dir");     
		return;
	}
	if(!lstrcmp(output_root, L"")) {
		logMessage(NCU_MSG_TYPE_ERROR, "no dst dir");     
		return;
	}
	if(!lstrcmp(m_fileNameAry[2], L"")) {
		//check for template in input_root
		HANDLE hFile;
		WIN32_FIND_DATA FindFileData;
		TCHAR find_template[MAX_PATH];

		StringCchCopy(find_template, MAX_PATH, input_root);
		StringCchCat(find_template, MAX_PATH, TEXT("\\template.nif"));                         
		hFile = FindFirstFile(find_template, &FindFileData);
		if (hFile == INVALID_HANDLE_VALUE) {
			logMessage(NCU_MSG_TYPE_ERROR, "no template");     
			return;
		}
	}
	//TODO why was this ehre?
	//std::string templ = (CStringA(CString(glConfig._pathTemplate.c_str()) + L"\\" + m_fileNameAry[2])).GetString();
	std::string templ = CStringA(m_fileNameAry[2]).GetString();
	processFilesInDir(input_root, output_root, ncUtility, templ, "", ncColUtility, addCollision, useConvertedNif);
}

void CNifConvertDlg::OnBnClickedCheckTangentspace2()
{
	// TODO: Add your control notification handler code here
}


void CNifConvertDlg::OnBnClickedCheckUseConverted()
{
	// TODO: Add your control notification handler code here
}
