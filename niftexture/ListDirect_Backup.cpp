#include <iostream>
#include <string>
#include <direct.h>
#include <vector>
#include "Shlobj.h"
#include "Strsafe.h"

using namespace::std;


string wchar_t2string(const wchar_t *wchar)
{
    string str = "";
    int index = 0;
    while(wchar[index] != 0)
    {
        str += (char)wchar[index];
        ++index;
    }
    return str;
}

wchar_t *string2wchar_t(const string &str)
{
    wchar_t wchar[260];
    int index = 0;
    while(index < str.size())
    {
        wchar[index] = (wchar_t)str[index];
        ++index;
    }
    wchar[index] = 0;
    return wchar;
}

vector<string> listFilesInDirectory(string directoryName)
{
	vector<string> listFileNames;
	vector<string> DirectReestor;
	vector<string> SubDirectReestor;

    WIN32_FIND_DATA FindFileData;
	string directoryNameEyE = directoryName + "*";

    wchar_t * FileName = string2wchar_t(directoryNameEyE);
    HANDLE hFind = FindFirstFile(FileName, &FindFileData);

	// Верхний уровень
	if (((wchar_t2string(FindFileData.cFileName)) != ".") && ((wchar_t2string(FindFileData.cFileName)) != "..") && (FindFileData.dwFileAttributes == 16))
	DirectReestor.push_back(wchar_t2string(FindFileData.cFileName) + "\\");
	//if (DirectReestor.size() != 0)
    //listFileNames.push_back(directoryName + DirectReestor.back());
    while (FindNextFile(hFind, &FindFileData))
		if (((wchar_t2string(FindFileData.cFileName)) != ".") && ((wchar_t2string(FindFileData.cFileName)) != "..") && (FindFileData.dwFileAttributes == 16))
		DirectReestor.push_back(wchar_t2string(FindFileData.cFileName) + "\\");
        //listFileNames.push_back(directoryName + DirectReestor.back());

	// Нижний уровень
	for (int i = 0; i < DirectReestor.size(); i++)
	{
		string TempDir = directoryName + DirectReestor[i] + "*";
		
		HANDLE hFind = FindFirstFile(string2wchar_t(TempDir), &FindFileData);
	if (((wchar_t2string(FindFileData.cFileName)) != ".") && ((wchar_t2string(FindFileData.cFileName)) != "..") && (FindFileData.dwFileAttributes == 16))
	SubDirectReestor.push_back(wchar_t2string(FindFileData.cFileName) + "\\");
	//if (SubDirectReestor.size() != 0)
   // listFileNames.push_back(directoryName + DirectReestor[i] + SubDirectReestor.back());
    while (FindNextFile(hFind, &FindFileData))
		if (((wchar_t2string(FindFileData.cFileName)) != ".") && ((wchar_t2string(FindFileData.cFileName)) != "..") && (FindFileData.dwFileAttributes == 16))
		SubDirectReestor.push_back(wchar_t2string(FindFileData.cFileName) + "\\");
		//if (SubDirectReestor.size() != 0)
        //listFileNames.push_back(directoryName + DirectReestor[i] + SubDirectReestor.back());

	for (int SbDr = 0; SbDr < SubDirectReestor.size(); SbDr++)
		{
			DirectReestor.push_back(DirectReestor[i] + SubDirectReestor[SbDr]);
			//cout << (DirectReestor[i] + SubDirectReestor[SbDr]) << endl;
		}
	
	 SubDirectReestor.clear();
	}

	for (int i = 0; i < DirectReestor.size(); i++) {
		listFileNames.push_back(directoryName + DirectReestor[i]);
	}

	//for (int i = 0; i < listFileNames.size(); i++) {
	//	if (listFileNames[i].find(".") != 0)
	//		listFileNames.erase (listFileNames.begin() + i);
	//}
    return listFileNames;
}