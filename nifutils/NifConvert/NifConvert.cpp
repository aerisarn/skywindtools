// NifConvert.cpp : Defines the class behaviors for the application.
//

#include "..\Common\stdafx.h"
#include "NifConvert.h"
#include "NifConvertDlg.h"
#include "..\Common\FDFileHelper.h"
#include "..\Common\Configuration.h"

#include "..\Common\NifUtlMaterial.h"


#include <Common/Base/hkBase.h>
#include <Common/Base/System/hkBaseSystem.h>
#include <Common/Base/Memory/System/Util/hkMemoryInitUtil.h>
#include <Common/Base/Memory/Allocator/Malloc/hkMallocAllocator.h>

#include <Common/Base/keycode.cxx>

#ifdef HK_FEATURE_PRODUCT_ANIMATION
#undef HK_FEATURE_PRODUCT_ANIMATION
#endif
#ifndef HK_EXCLUDE_LIBRARY_hkgpConvexDecomposition
#define HK_EXCLUDE_LIBRARY_hkgpConvexDecomposition
#endif
#include <Common/Base/Config/hkProductFeatures.cxx> 

static void HK_CALL errorReport(const char* msg, void* userArgGivenToInit)
{
	printf("%s", msg);
}

#define HK_MAIN_CALL _cdecl


#if defined( HK_ATOM )
extern "C" int __cdecl ADP_Close( void );
#endif

//  used namespaces
using namespace NifUtility;

CNifConvertDlg	dlg;
NifUtlMaterialList		glMaterialList;
Configuration	glConfig;

// CNifConvertApp

BEGIN_MESSAGE_MAP(CNifConvertApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CNifConvertApp construction

CNifConvertApp::CNifConvertApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CNifConvertApp object

CNifConvertApp theApp;


// CNifConvertApp initialization

BOOL CNifConvertApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxInitRichEdit2();

	CStringA	configName;

	GetModuleFileNameA(NULL, configName.GetBuffer(MAX_PATH), MAX_PATH);
	configName.ReleaseBuffer();
	configName.Replace(".exe", ".xml");

	glConfig.read((const char*) configName);

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization



		//  initialize Havok  (500000 bytes of physics solver buffer)
	hkMemoryRouter*		pMemoryRouter(hkMemoryInitUtil::initDefault(hkMallocAllocator::m_defaultMallocAllocator, hkMemorySystem::FrameInfo(500000)));
	hkBaseSystem::init(pMemoryRouter, errorReport);

  LPWSTR* argv;
  int     argc(0);

  argv = CommandLineToArgvW(m_lpCmdLine, &argc);

	//  path to templates - not given or overwritten
	if ((glConfig._pathTemplate.empty()) || (argc >= 2))
	{
		if (argc >= 2)
		{
			glConfig._pathTemplate = (const char*) CStringA(argv[1]);
		}
		else
		{
			glConfig._pathTemplate = (const char*) CStringA(FDFileHelper::getFileOrFolder(_T(""), L"*.nif (*.nif)|*.nif||", L"nif", false, true, _T("Please select template directory")));
		}
	}  //  if ((glConfig._pathTemplate.empty()) || (argc >= 2))

	//  path to Skyrim - not given or overwritten
	if ((glConfig._pathSkyrim.empty()) || ((argc >= 1) && (wcsstr(argv[0], L"NifConvert.exe") == NULL)))
	{
		if ((argc >= 1) && (wcsstr(argv[0], L"NifConvert.exe") == NULL))
		{
			glConfig._pathSkyrim = (const char*) CStringA(argv[0]);
		}
		else
		{
			glConfig._pathSkyrim = (const char*) CStringA(FDFileHelper::getFileOrFolder(_T(""), L"TESV.exe (TESV.exe)|TESV.exe||", L"exe", false, true, _T("Please select SKYRIM directory")));
		}
	}  //  if ((glConfig._pathSkyrim.empty()) || ((argc >= 1) && (wcsstr(argv[0], L"OneClickNifConvert.exe") == NULL)))

		//  nif.xml - not given or overwritten
	if ((glConfig._pathNifXML.empty()) || (argc >= 3))
	{
		if (argc >= 3)
		{
			glConfig._pathNifXML = (const char*) CStringA(argv[2]);
		}
		else
		{
			//TODO why is this needed?
		//	glConfig._pathNifXML = (const char*) CStringA(FDFileHelper::getFileOrFolder(_T(""), L"Nif-XML (nif.xml)|nif.xml||", L"xml", false, false, _T("Please select Nif.xml file")));
		}

	}  //  if ((glConfig._pathNifXML.empty()) || (argc >= 3))

	//  initialize material map
	glMaterialList.initializeMaterialMap(glConfig._pathNifXML);

  LocalFree(argv);


	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK2)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
	//  cleanup Havok
	hkBaseSystem::quit();

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
