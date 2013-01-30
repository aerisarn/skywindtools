// NifConvertDlg.h : header file
//

#pragma once

#include <set>
#include <strsafe.h>
#include "..\Common\NifConvertUtility.h"
#include "..\Common\NifCollisionUtility.h"
using namespace std;

// CNifConvertDlg dialog
class CNifConvertDlg : public CDialog
{
// Construction
public:
	CNifConvertDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_NIFCONVERT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON			m_hIcon;
	CString			m_fileNameAry[5];
	CString			m_texturePath;
	CRichEditCtrl	m_logView;
	CToolTipCtrl	m_toolTip;
	bool removeCollisionSrc;

  // Generated message map functions
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

  virtual void  parseDir(CString path, set<string>& directories, bool doDirs=true);

public:
	afx_msg void OnBnClickedButtonInput();
	afx_msg void OnBnClickedButtonOutput();
	afx_msg void OnBnClickedButtonTemplate();
	afx_msg void OnBnClickedButtonTexture();
	afx_msg void OnBnClickedOk();
	afx_msg void OnContextMenu(CWnd*, CPoint point);
	afx_msg void OnDefaultSavesettings();
	afx_msg void OnDefaultReloaddirectories();

	virtual void logMessage(const int type, const char* pMessage);
	afx_msg void OnBnClickedButtonInput3();
	afx_msg void OnBnClickedButtonOutput3();
	afx_msg void OnEnChangeEditInput();
	void processFilesInDir(TCHAR input_root[MAX_PATH], TCHAR output_root[MAX_PATH], 
									   NifUtility::NifConvertUtility& ncUtility, std::string templ, std::string col_templ, 
									   NifUtility::NifCollisionUtility& ncColUtility , bool addCollision,
									   bool useConvertedNif);
	afx_msg void OnEnChangeEditOutput();
	afx_msg void OnBnClickedOk2();
	afx_msg void OnBnClickedCheckTangentspace2();
	afx_msg void OnBnClickedCheckUseConverted();
};
