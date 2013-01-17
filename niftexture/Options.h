#include <string>

#ifndef Settings
#define Settings
//Paths
 extern std::string  MeshesPath;
 extern std::string   TexturesPath;

//NifSettings
 //extern std::string  Game;
 extern bool GenerateBSXFlags;
 extern int Flag;

//Textures
 extern std::string InTexturesExtension;
 extern std::string OutTexturesExtension;
 extern bool Normals;

 //Other
 extern bool Log;

 void Read_iniFnc(std::string PathStr);

#endif 