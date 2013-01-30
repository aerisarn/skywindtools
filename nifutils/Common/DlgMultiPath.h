#pragma once

#include "..\Common\stdafx.h"
#include <vector>

using namespace std;

// DlgMultiPath dialog
class DlgMultiPath : public CDialog
{
	private:
		vector<string>	_pathList;

		DECLARE_DYNAMIC(DlgMultiPath)

	protected:
		virtual	void	DoDataExchange      (CDataExchange* pDX);    // DDX/DDV support
		virtual BOOL	OnInitDialog        ();
		virtual	void	OnOK                ();
		afx_msg void	OnBnClickedBtnDown  ();
		afx_msg void	OnBnClickedBtnUp    ();
		afx_msg void	OnBnClickedBtnRemove();
		afx_msg void	OnBnClickedBtnAdd   ();

		DECLARE_MESSAGE_MAP()

	public:
						DlgMultiPath(vector<string> pathList, CWnd* pParent = NULL);   // standard constructor
		virtual			~DlgMultiPath();
				void	getPathList(vector<string>& pathList);
};
