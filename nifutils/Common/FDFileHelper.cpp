#include "StdAfx.h"
#include "FDFileHelper.h"

/*-------------------------------------------- splitFileName ---------------------------------------*/
void FDFileHelper::splitFileName(const CString path, CString& directory, CString& fileName, CString& extension)
{
  if (!path.IsEmpty())
  {
    char* pChar      (NULL);
    bool  isExtension(false);
    char  cBuffer[5000];

    sprintf_s(cBuffer, 5000, "%s", (const char*) CStringA(path).GetString());

    for (pChar = cBuffer + strlen(cBuffer) - 1; (pChar > cBuffer); --pChar)
    {
      if (*pChar == '.')
      {
        isExtension = true;
        extension   = pChar + 1;
        *pChar      = 0;
      }
      else if (*pChar == '\\')
      {
        fileName = pChar + 1;
        *pChar   = 0;
        break;
      }
    }  //  for (pChar = cBuffer + strlen(cBuffer) - 1; (pChar > cBuffer); --pChar)

    directory = cBuffer;

  }  //  if (!path.IsEmpty())
}

/*-------------------------------------------- getFileName -----------------------------------------*/
CString FDFileHelper::getFileOrFolder(const CString fileName, CString filter, CString extension, bool saveDialog, bool selFolder, CString title)
{
  CString           dirName;
  CString           tmpName;
  CString           extName;
  DWORD             flags  (OFN_NONETWORKBUTTON | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY);

  //  split path into parts
  splitFileName(fileName, dirName, tmpName, extName);

  //  set some flags and default values
  if (saveDialog)   flags |= OFN_OVERWRITEPROMPT;
  if (selFolder)
  {
    tmpName   = "SelectedFolder";
    extension = "";
  }

  //  create dialog
  
  CFileDialog   dlg(saveDialog?FALSE:TRUE, extension, tmpName, flags, filter);

  //  check on empty directory => use default one
  if (dirName.IsEmpty())
  {
    dirName = ".";
  }
  dlg.m_ofn.lpstrInitialDir = dirName;

  //  use title if given
  if (!title.IsEmpty())
  {
	  dlg.m_ofn.lpstrTitle = title.GetString();
  }

  //  open dialog
  if (dlg.DoModal() == IDOK)
  {
    //  choose return value
    if (!selFolder)   return dlg.GetPathName();

    splitFileName(dlg.GetPathName(), dirName, tmpName, extName);
    return dirName;
  }

  //  return old value
  return fileName;
}

