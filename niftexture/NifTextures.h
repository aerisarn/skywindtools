#include <string>
#include <map>

using namespace::std;

typedef std::map<std::string, std::string> MapType;

//void NifTexturesFunc(string FilePath, vector<string> TexturesPaths,char* OutTexturesExtension,char* InTexturesExtension, char* Game,char* GenerateBSXFlags,int Flag);
void NifTexturesFunc(string FilePath, MapType my_map);