#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#include "Options.h"
#include "Main.h"

#include "niflib.h"
#include "obj/NiTexturingProperty.h"
#include "obj/NiSourceTexture.h"
#include "obj/niobject.h"
#include "obj/NiNode.h"
#include "obj/NiGeometry.h"
#include "obj/bsfadenode.h"
#include "obj/NiTriStrips.h"
#include "obj/NiTriShape.h"
#include "obj/NiTriShapeData.h"
#include "obj/NiProperty.h"
#include "obj/NiMaterialProperty.h"
#include "obj/NiAVObject.h"
#include "obj/BSShaderPPLightingProperty.h"
#include "obj/BSShaderTextureSet.h"
#include "obj/BSXFlags.h"
#include "obj/BSLightingShaderProperty.h"
#include "obj/BSLODTriShape.h"

using namespace Niflib;

bool CatchTexture = 0;
bool Changes = 0;

typedef std::map<std::string, std::string> MapType;

vector<string> LogVector;

string ToLower(string str)
{
	std::transform(str.begin(), str.end(), str.begin(), tolower);
	return str;
}
string CutTexture(string tmpPath)
{
	size_t found=tmpPath.find_last_of("\\");
	if (found != string::npos) tmpPath = tmpPath.substr(found + 1);
	found=tmpPath.find_last_of(".");
	tmpPath = tmpPath.substr(0, found);
	return tmpPath;
}

string FindTextures(string TexturePathIn)
{
	CatchTexture = 0;

	string TexturePathOut;
	string TexturesPathSearch;
	
	for (vector<string>::iterator TexturesPathSearch = TexturesPaths.begin(); TexturesPathSearch != TexturesPaths.end(); TexturesPathSearch++)
	{
		if (ToLower(TexturePathIn) == CutTexture(ToLower(*TexturesPathSearch))) 
		{
			TexturePathOut = *TexturesPathSearch;
			CatchTexture = 1;
			Changes = 1;
			break;
		}
	}
	if (CatchTexture == 1)
	{
	size_t found = ToLower(TexturePathOut).find("textures\\");
	TexturePathOut = TexturePathOut.substr(found);
	TexturePathOut = TexturePathOut.substr(0, TexturePathOut.size()-string(InTexturesExtension).size());

	cout << TexturePathIn << " - found" << endl;

	return TexturePathOut;
	}
	else
	{
		cout << TexturePathIn << " - not found" << endl;
		LogVector.push_back(TexturePathIn + " - not found\n");
		return TexturePathIn;
	}
}

void SetTextures(string tmpPath, BSShaderTextureSetRef TextureSet, MapType my_map) {
	string FinaleTexture;
	CatchTexture = 0;
	Changes = 0;

	std::map<std::string, std::string>::const_iterator it;
	for (it = my_map.begin(); it != my_map.end(); ++it)
	{
		if(ToLower(it->first) == ToLower(tmpPath)) {
			CatchTexture = 1;
			Changes = 1;
			FinaleTexture = it->second;

			cout << "match: " << FinaleTexture << endl;
			break;
		} else {
			//cout << "No Match" << endl;
		}
	}
	
	if (CatchTexture == 1) {
		string TexturePathOut = FinaleTexture.substr(0, FinaleTexture.size()-string(InTexturesExtension).size());
		cout << "texture out: " << TexturePathOut << endl;
		TextureSet->SetTexture(0, TexturePathOut + OutTexturesExtension);
		
		if (Normals == 1) {
			TextureSet->SetTexture(1, TexturePathOut + "_n" + OutTexturesExtension);
		}
		else TextureSet->SetTexture(1, "");
	}
}

void SetTexturesObl(string tmpPath, NiSourceTextureRef SourceTexture)
{
	string FinaleTexture;

	tmpPath = CutTexture(tmpPath);
	FinaleTexture = FindTextures(tmpPath);
	if (CatchTexture == 1) SourceTexture->SetExternalTexture(FinaleTexture + OutTexturesExtension);
}


bool updateTangentSpace(NiTriShapeDataRef pDataObj)
{
	vector<Vector3>		vecVertices (pDataObj->GetVertices());
	vector<Vector3>		vecNormals  (pDataObj->GetNormals());
	if (pDataObj->GetUVSetCount() <= 0)
		return false;
	vector<TexCoord>	vecTexCoords(pDataObj->GetUVSet(0));
	vector<Triangle>	vecTriangles(pDataObj->GetTriangles());

	//  check on valid input data
	if (vecVertices.empty() || vecTriangles.empty() || vecNormals.size() != vecVertices.size() || vecVertices.size() != vecTexCoords.size())
	{
		return false;
	}

	//  prepare result vectors
	vector<Vector3>		vecTangents  = vector<Vector3>(vecVertices.size(), Vector3(0.0f, 0.0f, 0.0f));
	vector<Vector3>		vecBiNormals = vector<Vector3>(vecVertices.size(), Vector3(0.0f, 0.0f, 0.0f));

	for (unsigned int t(0); t < vecTriangles.size(); ++t)
	{
		Vector3		vec21(vecVertices[vecTriangles[t][1]] - vecVertices[vecTriangles[t][0]]);
		Vector3		vec31(vecVertices[vecTriangles[t][2]] - vecVertices[vecTriangles[t][0]]);
		TexCoord	txc21(vecTexCoords[vecTriangles[t][1]] - vecTexCoords[vecTriangles[t][0]]);
		TexCoord	txc31(vecTexCoords[vecTriangles[t][2]] - vecTexCoords[vecTriangles[t][0]]);
		float		radius(((txc21.u * txc31.v - txc31.u * txc21.v) >= 0.0f ? +1.0f : -1.0f));

		Vector3		sdir(( txc31.v * vec21[0] - txc21.v * vec31[0] ) * radius,
					     ( txc31.v * vec21[1] - txc21.v * vec31[1] ) * radius,
					     ( txc31.v * vec21[2] - txc21.v * vec31[2] ) * radius);
		Vector3		tdir(( txc21.u * vec31[0] - txc31.u * vec21[0] ) * radius,
					     ( txc21.u * vec31[1] - txc31.u * vec21[1] ) * radius,
					     ( txc21.u * vec31[2] - txc31.u * vec21[2] ) * radius);

		//  normalize
		sdir = sdir.Normalized();
		tdir = tdir.Normalized();

		for (int j(0); j < 3; ++j)
		{
			vecTangents [vecTriangles[t][j]] += tdir;
			vecBiNormals[vecTriangles[t][j]] += sdir;
		}
	}  //  for (unsigned int t(0); t < vecTriangles.size(); ++t)

	for (unsigned int i(0); i < vecVertices.size(); ++i)
	{
		Vector3&	n(vecNormals[i]);
		Vector3&	t(vecTangents [i]);
		Vector3&	b(vecBiNormals[i]);

		if ((t == Vector3()) || (b == Vector3()))
		{
			t[0] = n[1];
			t[1] = n[2];
			t[2] = n[0];

			b = n.CrossProduct(t);
		}
		else
		{
			t = t.Normalized();
			t = (t - n * n.DotProduct(t));
			t = t.Normalized();

			b = b.Normalized();
			b = (b - n * n.DotProduct(b));
			b = (b - t * t.DotProduct(b));
			b = b.Normalized();
		}
	}  //  for (unsigned int i(0); i < vecVertices.size(); ++i)

	//  set tangents and binormals to object
	pDataObj->SetBitangents(vecBiNormals);
	pDataObj->SetTangents  (vecTangents);

	return true;
}

void NifTexturesFunc(string FilePath, MapType my_map) { 
	vector<NiAVObjectRef> NiAVOShapeList;
	vector<NiPropertyRef> PropertyList;
	vector<BSShaderPPLightingPropertyRef> ShaderPPList;
	vector<BSLightingShaderPropertyRef> ShaderList;
	vector<NiTexturingPropertyRef> NiTexturingPropertyList;
	vector<NiSourceTextureRef> NiSourceTexturesList;

	string MainRoot;
	string Game;
	BSFadeNodeRef FadeNode   = NULL;
	NiNodeRef Node = NULL;
	BSShaderTextureSetRef TextureSet = NULL;
	NifInfo   nifInfo;

	string tmpPath;

	if (ReadNifTree(FilePath)->GetType().GetTypeName() == "BSFadeNode") 
	{FadeNode = DynamicCast<BSFadeNode>(ReadNifTree(FilePath, &nifInfo));
	MainRoot = "BSFadeNode";}
	else if (ReadNifTree(FilePath)->GetType().GetTypeName() == "NiNode") 
	{Node = DynamicCast<NiNode>(ReadNifTree(FilePath, &nifInfo));
	MainRoot = "NiNode";}
	else if (ReadNifTree(FilePath)->GetType().GetTypeName() == "BSLeafAnimNode") 
	{Node = DynamicCast<NiNode>(ReadNifTree(FilePath, &nifInfo));
	MainRoot = "NiNode";}
	else cout << "Unknown MainRoot";

	if (nifInfo.version == unsigned int(VER_20_2_0_7) && nifInfo.userVersion == 11 && nifInfo.userVersion2 == 34)  Game = "Fallout";
	else if (nifInfo.version == unsigned int(VER_20_2_0_7) && nifInfo.userVersion == 12 && nifInfo.userVersion2 == 83)  Game = "Skyrim";
	else if (nifInfo.version == unsigned int(VER_20_0_0_5) && nifInfo.userVersion == 11 && nifInfo.userVersion2 == 11)  Game = "Oblivion";
	else cout << "Unknown nif version!" << endl;

// BSXFlags generation
if (GenerateBSXFlags) {
	BSXFlagsRef bsx = new BSXFlags;
	bsx->SetName("BSX");
	bsx->SetData(Flag);
	if (MainRoot == "BSFadeNode") FadeNode->AddExtraData(DynamicCast<NiExtraData>(bsx));
	else if (MainRoot == "NiNode") Node->AddExtraData(DynamicCast<NiExtraData>(bsx));
}

	if (MainRoot == "BSFadeNode") NiAVOShapeList = FadeNode->GetChildren();
	else if (MainRoot == "NiNode") NiAVOShapeList = Node->GetChildren();

	//Very big cycle, ger all textures containers
	for (unsigned int i = 0; i < NiAVOShapeList.size(); i++) 
	{
		string tmpType = NiAVOShapeList[i]->GetType().GetTypeName();
		if (tmpType == "NiTriStrips" || tmpType ==  "NiTriShape" || tmpType == "BSLODTriShape") 
		{
			if (Game == "Fallout") 
			{
			PropertyList = NiAVOShapeList[i]->GetProperties();
			for (unsigned int a = 0; a < PropertyList.size(); a++) 
				if (PropertyList[a]->GetType().GetTypeName() == "BSShaderPPLightingProperty")
				ShaderPPList.push_back(DynamicCast<BSShaderPPLightingProperty>(PropertyList[a]));
			}
			else if (Game == "Skyrim")
			{

				if (DynamicCast<NiTriShape>(NiAVOShapeList[i])!=NULL && 
					DynamicCast<NiTriShape>(NiAVOShapeList[i])->GetData()!=NULL)
				{
					if(updateTangentSpace(DynamicCast<NiTriShapeData>(DynamicCast<NiTriShape>(NiAVOShapeList[i])->GetData())))
					{
						//  enable tangent space
						DynamicCast<NiTriShapeData>(DynamicCast<NiTriShape>(NiAVOShapeList[i])->GetData())->SetTspaceFlag(0x10);
					}
				}
				
				bool Switcher = 0;
				for (int x = 0; Switcher == 0 && x < 2; x++) 
				{
					//Wrong. Accessing a maybe unexistant property
					if (((DynamicCast<NiGeometry>(NiAVOShapeList[i])))!=NULL &&
						((DynamicCast<NiGeometry>(NiAVOShapeList[i]))->GetBSProperty(x))!=NULL &&
						((DynamicCast<NiGeometry>(NiAVOShapeList[i]))->GetBSProperty(x))->GetType().GetTypeName() == "BSLightingShaderProperty")
					{
						ShaderList.push_back(DynamicCast<BSLightingShaderProperty>((DynamicCast<NiGeometry>(NiAVOShapeList[i]))->GetBSProperty(x)));
						//Check for flags

						(DynamicCast<BSLightingShaderProperty>((DynamicCast<NiGeometry>(NiAVOShapeList[i]))->GetBSProperty(x)))->SetShaderFlags1(SkyrimShaderPropertyFlags1(
							(DynamicCast<BSLightingShaderProperty>((DynamicCast<NiGeometry>(NiAVOShapeList[i]))->GetBSProperty(x)))->GetShaderFlags1() | SLSF1_SPECULAR ));

						Switcher = 1;
					}
				}
			}
			else if (Game == "Oblivion")
			{
				PropertyList = NiAVOShapeList[i]->GetProperties();
				for (unsigned int a = 0; a < PropertyList.size(); a++)
				{
					if (PropertyList[a]->GetType().GetTypeName() == "NiTexturingProperty")
					NiTexturingPropertyList.push_back(DynamicCast<NiTexturingProperty>(PropertyList[a]));
				}
			}
		}
		else if (tmpType == "NiNode") {
			for (unsigned int a = 0; a < DynamicCast<NiNode>(NiAVOShapeList[i])->GetChildren().size(); a++)
			{
				NiAVOShapeList.push_back(DynamicCast<NiNode>(NiAVOShapeList[i])->GetChildren().at(a));
			}
		}
	}
	//===========================================
	// Now unicle for each game:
	if (Game == "Skyrim") 
	{
		for (unsigned int i = 0; i < ShaderList.size(); i++) 
		{
			TextureSet = ShaderList[i]->GetTextureSet();
			tmpPath = TextureSet->GetTexture(0);
			SetTextures(tmpPath, TextureSet, my_map);
		}
	}
	else if (Game == "Fallout") 
	{
		for (unsigned int i = 0; i < ShaderPPList.size(); i++) 
		{
			TextureSet = ShaderPPList[i]->GetTextureSet();
			tmpPath = TextureSet->GetTexture(0);
			SetTextures(tmpPath, TextureSet, my_map);
		}
	}
	else if (Game == "Oblivion")
	{
		for (unsigned int i = 0; i < NiTexturingPropertyList.size(); i++) 
		{
			for (int a = 0; a < NiTexturingPropertyList[i]->GetTextureCount(); a++)
			{
			if ((NiTexturingPropertyList[i]->GetTexture(a)).source != 0) NiSourceTexturesList.push_back((NiTexturingPropertyList[i]->GetTexture(a)).source);
			}	
		}
		for (unsigned int i = 0; i < NiSourceTexturesList.size(); i++) 
		{
			SetTexturesObl(NiSourceTexturesList[i]->GetTextureFileName(), NiSourceTexturesList[i]);
		}

	}
	//=====================================
	// Close part + some log
	if (myfile.is_open() == false && Log == 1 && LogVector.size() != 0) myfile.open ("log.txt");
	cout << FilePath;
	if (Changes == 1)
	{
		if (MainRoot == "BSFadeNode") WriteNifTree(FilePath, FadeNode, nifInfo);
		else if (MainRoot == "NiNode") WriteNifTree(FilePath, Node, nifInfo);
		cout << " - Modified" << endl;
		if (LogVector.size() != 0 && Log == 1) myfile << FilePath << " - Miss some textures:\n";
	}
	else 
	{
		cout << " - Skip" << endl;
		if (Log == 1) myfile << FilePath << " - Skip:\n";
	}
	if (LogVector.size() != 0 && Log == 1)
	{
		for (std::vector<std::string>::iterator it = LogVector.begin(); it != LogVector.end(); it++) myfile << (*it);
		myfile.flush();
		LogVector.clear();
	}
	// Clear for the future
	Changes = 0;
}