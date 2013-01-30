#pragma once

class FDFileHelper
{
  private:
    static  void      splitFileName  (const CString path, CString& directory, CString& fileName, CString& extension);

  public:
    static  CString   getFileOrFolder(const CString fileName, CString filter, CString extension, bool saveDialog=false, bool selFolder=false, CString title=_T(""));
};
