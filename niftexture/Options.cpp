#include <iostream>
#include <string>

#include "Options.h"

#include <direct.h>
#include <vector>
#include "Shlobj.h"
#include "Strsafe.h"
#include <Windows.h>

using namespace::std;

//Paths
string  MeshesPath;
string   TexturesPath;

//NifSettings
bool GenerateBSXFlags;
int Flag;

//Textures
string InTexturesExtension;
string OutTexturesExtension;
bool Normals;
bool Log;


void Read_iniFnc(string PathStr) 
{
	//Locals variables
	//Paths
	char  MeshesPathChar[256];
	char   TexturesPathChar[256];

	//Textures
	char InTexturesExtensionChar[256];
	char OutTexturesExtensionChar[256];
	//===============================
	
	PathStr += "\\NifTexturesSettings.ini";
	//Paths
	GetPrivateProfileString("Paths","MeshesPath", "Error", MeshesPathChar, 256 , PathStr.c_str());
	GetPrivateProfileString("Paths","TexturesPath", "Error", TexturesPathChar, 256 , PathStr.c_str());
	//NifSettings
	GenerateBSXFlags = (bool)GetPrivateProfileInt("NifSettings","GenerateBSXFlags", 0, PathStr.c_str());
	Flag = GetPrivateProfileInt("NifSettings","Flag", 2, PathStr.c_str());
	//Textures
	GetPrivateProfileString("Textures","InTexturesExtension", ".dds", InTexturesExtensionChar , 256 , PathStr.c_str());
	GetPrivateProfileString("Textures","OutTexturesExtension", ".dds", OutTexturesExtensionChar , 256 , PathStr.c_str());
	Normals = GetPrivateProfileInt("Textures","Normals", 1, PathStr.c_str());
	//Other
	Log = GetPrivateProfileInt("Other","Log", 1, PathStr.c_str());

	//To globals (As strings)
	MeshesPath = string(MeshesPathChar);
	TexturesPath = string(TexturesPathChar);

	//Game = string(GameChar);
	InTexturesExtension = string(InTexturesExtensionChar);
	OutTexturesExtension = string(OutTexturesExtensionChar);

	if (*MeshesPath.rbegin() != char(92) && MeshesPath != "Error" && MeshesPath != "0") MeshesPath += "\\";
	if (*TexturesPath.rbegin() != char(92) && TexturesPath != "Error" && TexturesPath != "0") TexturesPath += "\\";
}