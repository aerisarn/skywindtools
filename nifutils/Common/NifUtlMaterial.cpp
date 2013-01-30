#include "NifUtlMaterial.h"
//typedef unsigned long DWORD;
#include <WTypes.h>

#include "..\Common\Configuration.h"

#include <iostream>
#include <fstream>

//  used namespaces
using namespace NifUtility;

extern Configuration	glConfig;

/*---------------------------------------------------------------------------*/
NifUtlMaterialList::NifUtlMaterialList()
{
}

/*---------------------------------------------------------------------------*/
NifUtlMaterialList::~NifUtlMaterialList()
{
}

/*---------------------------------------------------------------------------*/
map<string, NifUtlMaterial>& NifUtlMaterialList::getMaterialMap()
{
	return _materialMap;
}

/*---------------------------------------------------------------------------*/
unsigned int NifUtlMaterialList::getMaterialCode(string matDefName)
{
	return (_materialMap.count(matDefName) > 0) ? _materialMap[matDefName]._code : 0;
}

/*---------------------------------------------------------------------------*/
void NifUtlMaterialList::initializeMaterialMap(string pathToXML)
{
	ifstream	streamIn;
	char		cbuffer[10000] = {0};
	char		txtTag [200]   = {0};
	char		txtName[200]   = {0};
	bool		isMaterialSection(false);

	//  reset old settings
	reset();

	//  open nif.xml
	streamIn.open(pathToXML.c_str(), ifstream::in);

	sprintf(cbuffer, "^%cOpening '%s': %s", (streamIn.good() ? '0' : '2'), pathToXML.c_str(), (streamIn.good() ? "OK" : "FAILED"));
	_userMessages.push_back(cbuffer);

	sprintf(txtTag, "<enum name=\"%s\" storage=\"uint\">", glConfig._matScanTag.c_str());
	sprintf(txtName, "name=\"%s", glConfig._matScanName.c_str());

	//  on valid input
	while (streamIn.good())
	{
		//  read next row
		streamIn.getline(cbuffer, 10000);

		//  search start of material definition
		if (strstr(cbuffer, txtTag) != NULL)
		{
			isMaterialSection = true;
		}

		//  row within material definition section
		if (isMaterialSection)
		{
			//  valid material definition for SKYrim?
			if ((strstr(cbuffer, "<option value=\"") != NULL) &&
				(strstr(cbuffer, txtName) != NULL))
			{
				NifUtlMaterial	matNew;
				char*			pStart(strstr(cbuffer, "value="));
				char*			pEnd  (strstr(cbuffer, "\" name="));
				bool			doSkip(false);

				//  read material code
				matNew._code = (unsigned int) atof(pStart + 7);

				//  parse and read definition name
				pStart = pEnd + 8;
				pEnd   = strstr(pStart, "\">");
				*pEnd  = 0;
				matNew._defName = pStart;

				//  parse and read user readable name
				pStart = pEnd + 2;
				pEnd   = strstr(pStart, "</option>");
				*pEnd  = 0;

				//  parse skipping entries
				for (vector<string>::iterator pIter(glConfig._matScanIgnore.begin()), pEnd(glConfig._matScanIgnore.end()); pIter != pEnd; ++pIter)
				{
					if (strstr(pStart, pIter->c_str()) != NULL)
					{
						doSkip = true;
						break;
					}
				}
				if (doSkip)		continue;

				//  parse prefix list
				for (vector<string>::iterator pIter(glConfig._matScanPrefix.begin()), pEnd(glConfig._matScanPrefix.end()); pIter != pEnd; ++pIter)
				{
					if (strncmp(pStart, pIter->c_str(), pIter->length()) == 0)
					{
						pStart += pIter->length();	//  skip leading prefix
						break;
					}
				}

				//  skip leading spaces
				for (; *pStart == ' '; ++pStart);

				matNew._name = pStart;

				//  append material to map
				_materialMap[matNew._defName] = matNew;

				_userMessages.push_back("^4added: " + matNew._defName + " => '" + matNew._name + "'");
			}
			//  early break at end of material definition
			else if (strstr(cbuffer, "</enum>") != NULL)
			{
				break;
			}
		}  //  if (isMaterialSection)
	}  //  while (streamIn.good())

	sprintf(cbuffer, "^%cmaterials found: %d", ((_materialMap.size() > 0) ? '0' : '1'), _materialMap.size());
	_userMessages.push_back(cbuffer);

	streamIn.close();
}

/*---------------------------------------------------------------------------*/
vector<string>& NifUtlMaterialList::getUserMessages()
{
	return _userMessages;
}

/*---------------------------------------------------------------------------*/
void NifUtlMaterialList::reset()
{
	_userMessages.clear();
	_materialMap.clear();
}

