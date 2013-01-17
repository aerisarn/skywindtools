#include "ListDirect.h"
#include "NifTextures.h"
#include "Options.h"
#include "Main.h"

#include <iostream>
#include <fstream>
#include <string>
#include <direct.h>
#include <vector>
#include <map>
#include "Shlobj.h"
#include "Strsafe.h"
#include <Windows.h>

#include <sstream>

using namespace::std;

vector<string> TexturesPaths;
ofstream myfile;
ofstream mytexturefile;

typedef std::map<std::string, std::string> MapType;

int main() {

    // Input stream
    ifstream InpStream("ddsreplacelist.txt");
	MapType my_map;

    // Define strings to hold the key and corresponding value
    string key, value;

    // Read the input file using getline ('=' is the delimiter)
    while ( getline(InpStream, key, '=') ) {

        // Print what key is read
        cout << "key read: " << key << endl;

        // Create an istringstream instance to parse the key
        // (string to sring conversion)
        istringstream ss_key(key);
        ss_key >> key;

        // Now read the value and print what is read (value is a string)
        getline(InpStream, value);
        cout << "value read: " << value << endl;

        // Create another istringstream instance to parse the value
        istringstream ss_value(value);
       
		my_map.insert(std::pair<string, string>(key, value));
	
		TexturesPaths.push_back(key);
    }

	system("PAUSE");

	char CurrentPath[_MAX_PATH];

	_getcwd(CurrentPath, _MAX_PATH);
	string PathStr = CurrentPath;
	Read_iniFnc(PathStr);

	if (MeshesPath == "Error" || MeshesPath == "0") MeshesPath = PathStr + "\\";
	if (TexturesPath == "Error" || TexturesPath == "0")  TexturesPath = PathStr + "\\";
	
	vector<string> MeshesPaths;

	// Test cycle for meshes
	//for (int i = 0; i < FileList.size(); i++)
	//{
	//	cout << FileList[i] << endl;
	//}
	//cout << "Nif files:" << endl;

	cout << MeshesPath << endl;
	MeshesPaths = listFiles(listFilesInDirectory(MeshesPath), "nif");
	cout << "Get all nifs files" << endl;
	
	cout << "Models count:" << MeshesPaths.size() << endl;
	if (MeshesPaths.size() == 0) {
		cout << "Error, can't find any meshes" << endl;
		system("PAUSE");
		return 1;
	}
	cout << TexturesPath << endl;

	//TexturesPaths = listFiles(listFilesInDirectory(TexturesPath), InTexturesExtension);

	cout << "Catch textures" << endl;
	cout << "Textures count:" << TexturesPaths.size() << endl;
	
	if (TexturesPaths.size() == 0) {
		cout << "Error, can't find any textures" << endl;
		system("PAUSE");
		return 1;
	}

	for (unsigned int i = 0; i < MeshesPaths.size(); i++) {
		//cout << MeshesPaths[i] << endl;
		NifTexturesFunc(MeshesPaths[i], my_map);
	}

	if (myfile.is_open() == true) 
		myfile.close();

	if (mytexturefile.is_open() == true) 
		mytexturefile.close();
		
	system("PAUSE");

	return 0;
}