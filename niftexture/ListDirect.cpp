// Считываем директории
#include <iostream>
#include <string>
#include <direct.h>
#include <vector>
#include "Shlobj.h"
#include "Strsafe.h"

using namespace::std;

vector<string> listFilesInDirectory(string directoryName)
{
	vector<string> listFileNames;
	vector<string> DirectReestor;
	vector<string> SubDirectReestor;

    WIN32_FIND_DATA FindFileData;
	string directoryNameEyE = directoryName + "*";

    HANDLE hFind = FindFirstFile(directoryNameEyE.c_str(), &FindFileData);

	// Верхний уровень (Higher level)
	if (string(FindFileData.cFileName) != "." && string(FindFileData.cFileName) != ".." && (FindFileData.dwFileAttributes == 16))
	DirectReestor.push_back(string (FindFileData.cFileName) + "\\");
    while (FindNextFile(hFind, &FindFileData))
	if (string(FindFileData.cFileName) != "." && string(FindFileData.cFileName) != ".." && (FindFileData.dwFileAttributes == 16)) {
			DirectReestor.push_back(string (FindFileData.cFileName) + "\\");}
	listFileNames.push_back(directoryName);

	if (DirectReestor.size() != 0)
	{
	// Нижний уровень (Lower level)
	for (unsigned int i = 0; i < DirectReestor.size(); i++)
	{
		string TempDir = directoryName + DirectReestor[i] + "*";
		
		HANDLE hFind = FindFirstFile(TempDir.c_str(), &FindFileData);
	if (string(FindFileData.cFileName) != "." && string(FindFileData.cFileName) != ".." && (FindFileData.dwFileAttributes == 16))
	SubDirectReestor.push_back(string (FindFileData.cFileName) + "\\");
    while (FindNextFile(hFind, &FindFileData))
	if (string(FindFileData.cFileName) != "." && string(FindFileData.cFileName) != ".." && (FindFileData.dwFileAttributes == 16))
		SubDirectReestor.push_back(string (FindFileData.cFileName) + "\\");
	for (unsigned int SbDr = 0; SbDr < SubDirectReestor.size(); SbDr++)
			DirectReestor.push_back(DirectReestor[i] + SubDirectReestor[SbDr]);
	
	 SubDirectReestor.clear();
	}

	for (unsigned int i = 0; i < DirectReestor.size(); i++) {
		listFileNames.push_back(directoryName + DirectReestor[i]);
	}
	}

    return listFileNames;
}

WIN32_FIND_DATA EmptyFindFileData;

vector<string> listFiles(vector<string> FileList, string Finding) {

	vector<string> FileListReturn;

	for (unsigned int i = 0; i < FileList.size(); i++) {
		string TempString;
		TempString = FileList[i];
		TempString += "*" + Finding; 

		 WIN32_FIND_DATA FindFileData;
		FindFileData = EmptyFindFileData;
		HANDLE hFind = FindFirstFile(TempString.c_str(), &FindFileData);
		// Strange checking, but work
		size_t found = ((string)FindFileData.cFileName).find("ММММ");
		if (found == string::npos && string(FindFileData.cFileName) != "")
		{
		FileListReturn.push_back(FileList[i] + FindFileData.cFileName);
		while (FindNextFile(hFind, &FindFileData))
		FileListReturn.push_back(FileList[i] + FindFileData.cFileName);
		}
	}
	return FileListReturn;
}