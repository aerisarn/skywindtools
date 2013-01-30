#include "..\Common\stdafx.h"
#include "Configuration.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace NifUtility;


Configuration::Configuration()
	:	_matScanTag        ("SkyrimHavokMaterial"),
		_matScanName       ("SKY_HAV_"),
		_collMaterial      (3741512247),
		_matHandling       (0),
		_vertColHandling   (0),
		_collTypeHandling  (1),
		_colorWireframe    (0xFFFFFFFF),
		_colorWireCollision(0xFFFFFF00),
		_colorBackground   (0xFF200020),
		_colorSelected     (0xFFFF00FF),
		_upTangentSpace    (true),
		_reorderProperties (true),
		_dxShowTexture     (true),
		_dxShowWireframe   (false),
		_dxShowColorWire   (false),
		_dxForceDDS        (false),
		_hasFile           (false)
{
	_matScanPrefix.push_back("Material");
	_matScanIgnore.push_back("Unknown");
}

Configuration::~Configuration()
{
	if (!_hasFile)		write();
}

bool Configuration::readAttribute(const string& content, const string tag, vector<string>& attribute, unsigned int& offsetOut, unsigned int offsetIn)
{
	string			tString;
	unsigned int	offset(offsetIn);

	attribute.clear();

	while (readAttribute(content, tag, tString, offset, offset))
	{
		attribute.push_back(tString);
	}

	return !attribute.empty();
}

bool Configuration::readAttribute(const string& content, const string tag, string& attribute, unsigned int& offsetOut, unsigned int offsetIn)
{
	size_t	posStart(offsetIn);
	size_t	posEnd  (offsetIn);
	bool	isOK    (false);

	posStart = content.find(tag, posStart);
	if (posStart != string::npos)
	{
		posStart += tag.length();
		posEnd = content.find(tag, posStart);
		if (posEnd != string::npos)
		{
			attribute = content.substr(posStart, posEnd - posStart - 2);
			offsetOut = posEnd + tag.length();
			isOK      = true;
		}
	}

	return isOK;
}

bool Configuration::readAttribute(const string& content, const string tag, DWORD& attribute, unsigned int& offsetOut, unsigned int offsetIn)
{
	size_t	posStart(offsetIn);
	size_t	posEnd  (offsetIn);
	bool	isOK    (false);

	posStart = content.find(tag, posStart);
	if (posStart != string::npos)
	{
		posStart += tag.length();
		posEnd = content.find(tag, posStart);
		if (posEnd != string::npos)
		{
			istringstream	tStream(content.substr(posStart, posEnd - posStart - 2).c_str());
			tStream >> hex >> attribute;
			offsetOut = posEnd + tag.length();
			isOK      = true;
		}
	}

	return isOK;
}

bool Configuration::readAttribute(const string& content, const string tag, int& attribute, unsigned int& offsetOut, unsigned int offsetIn)
{
	size_t	posStart(offsetIn);
	size_t	posEnd  (offsetIn);
	bool	isOK    (false);

	posStart = content.find(tag, posStart);
	if (posStart != string::npos)
	{
		posStart += tag.length();
		posEnd = content.find(tag, posStart);
		if (posEnd != string::npos)
		{
			attribute = atoi(content.substr(posStart, posEnd - posStart - 2).c_str());
			offsetOut = posEnd + tag.length();
			isOK      = true;
		}
	}

	return isOK;
}

bool Configuration::readAttribute(const string& content, const string tag, bool& attribute, unsigned int& offsetOut, unsigned int offsetIn)
{
	size_t	posStart(offsetIn);
	size_t	posEnd  (offsetIn);
	bool	isOK    (false);

	posStart = content.find(tag, posStart);
	if (posStart != string::npos)
	{
		posStart += tag.length();
		posEnd = content.find(tag, posStart);
		if (posEnd != string::npos)
		{
			attribute = (atoi(content.substr(posStart, posEnd - posStart - 2).c_str()) == 1);
			offsetOut = posEnd + tag.length();
			isOK      = true;
		}
	}

	return isOK;
}

bool Configuration::read(const string fileName)
{
	ifstream		iStr   (fileName.c_str());
	string			content;
	string			search;
	unsigned int	offset (0);

	//  file opened successfully
	if (iStr.is_open())
	{
		while (iStr.good())
		{
			//iStr >> content;
			getline(iStr, content);

			if (content.empty())	continue;

			//  fetch attributes
			readAttribute(content, "PathSkyrim>", _pathSkyrim, offset);
			readAttribute(content, "PathNifXML>", _pathNifXML, offset);
			readAttribute(content, "PathTemplate>", _pathTemplate, offset);
			readAttribute(content, "MatHandling>", _matHandling, offset);
			readAttribute(content, "VertexColorHandling>", _vertColHandling, offset);
			readAttribute(content, "UpdateTangentSpace>", _upTangentSpace, offset);
			readAttribute(content, "ReorderProperties>", _reorderProperties, offset);
			readAttribute(content, "CollTypeHandling>", _collTypeHandling, offset);
			readAttribute(content, "CollMaterial>", _collMaterial, offset);
			readAttribute(content, "LastTexture>", _lastTexture, offset);
			readAttribute(content, "LastTemplate>", _lastTemplate, offset);
			readAttribute(content, "DirSource>", _dirSource, offset);
			readAttribute(content, "DirDestination>", _dirDestination, offset);
			readAttribute(content, "DirCollision>", _dirCollision, offset);

			readAttribute(content, "MatScanTag>", _matScanTag, offset);
			readAttribute(content, "MatScanName>", _matScanName, offset);
			readAttribute(content, "MatScanPrefix>", _matScanPrefix, offset);
			readAttribute(content, "MatScanIgnore>", _matScanIgnore, offset);

			readAttribute(content, "ShowTexture>", _dxShowTexture, offset);
			readAttribute(content, "ShowWireframe>", _dxShowWireframe, offset);
			readAttribute(content, "ShowColorWire>", _dxShowColorWire, offset);
			readAttribute(content, "ForceDDS>", _dxForceDDS, offset);
			readAttribute(content, "DirTexturePath>", _dirTexturePath, offset);
			readAttribute(content, "ColorWireframe>", _colorWireframe, offset);
			readAttribute(content, "ColorWireCollision>", _colorWireCollision, offset);
			readAttribute(content, "ColorBackground>", _colorBackground, offset);
			readAttribute(content, "ColorSelected>", _colorSelected, offset);



		}  //  while (iStr.good())

		//  close file
		iStr.close();

		_hasFile = true;

	}  //  if (oStr.is_open())

	//  remember file name
	_configName = fileName;

	return _hasFile;
}

bool Configuration::write()
{
	if (_configName.empty())	return false;

	return write(_configName);
}

bool Configuration::write(const string fileName)
{
	ofstream	oStr(fileName.c_str(), ios::out | ios::trunc);
	bool		isOK(false);

	//  file opened successfully
	if (oStr.is_open())
	{
		oStr << "<Config>";
		oStr << "<PathSkyrim>" << _pathSkyrim << "</PathSkyrim>";
		oStr << "<PathNifXML>" << _pathNifXML << "</PathNifXML>";
		oStr << "<PathTemplate>" << _pathTemplate << "</PathTemplate>";
		oStr << "<LastTexture>" << _lastTexture << "</LastTexture>";
		oStr << "<LastTemplate>" << _lastTemplate << "</LastTemplate>";
		oStr << "<DirSource>" << _dirSource << "</DirSource>";
		oStr << "<DirDestination>" << _dirDestination << "</DirDestination>";
		oStr << "<DirCollision>" << _dirCollision << "</DirCollision>";
		oStr << "<MatHandling>" << _matHandling << "</MatHandling>";
		oStr << "<VertexColorHandling>" << _vertColHandling << "</VertexColorHandling>";
		oStr << "<UpdateTangentSpace>" << (_upTangentSpace ? 1 : 0) << "</UpdateTangentSpace>";
		oStr << "<ReorderProperties>" << (_reorderProperties ? 1 : 0) << "</ReorderProperties>";
		oStr << "<CollTypeHandling>" << _collTypeHandling << "</CollTypeHandling>";
		oStr << "<CollMaterial>" << _collMaterial << "</CollMaterial>";

		oStr << "<MaterialScan>";
		oStr << "<MatScanTag>" << _matScanTag << "</MatScanTag>";
		oStr << "<MatScanName>" << _matScanName << "</MatScanName>";
		oStr << "<MatScanPrefixList>";
		for (vector<string>::iterator pIter(_matScanPrefix.begin()), pEnd(_matScanPrefix.end()); pIter != pEnd; ++pIter)
		{
			oStr << "<MatScanPrefix>" << *pIter << "</MatScanPrefix>";
		}
		oStr << "</MatScanPrefixList>";
		oStr << "<MatScanIgnoreList>";
		for (vector<string>::iterator pIter(_matScanIgnore.begin()), pEnd(_matScanIgnore.end()); pIter != pEnd; ++pIter)
		{
			oStr << "<MatScanIgnore>" << *pIter << "</MatScanIgnore>";
		}
		oStr << "</MatScanIgnoreList>";
		oStr << "</MaterialScan>";

		oStr << "<DirectXView>";
		oStr << "<ShowTexture>" << _dxShowTexture << "</ShowTexture>";
		oStr << "<ShowWireframe>" << _dxShowWireframe << "</ShowWireframe>";
		oStr << "<ShowColorWire>" << _dxShowColorWire << "</ShowColorWire>";
		oStr << "<ForceDDS>" << _dxForceDDS << "</ForceDDS>";
		oStr << "<ColorWireframe>" << hex << _colorWireframe << "</ColorWireframe>";
		oStr << "<ColorWireCollision>" << hex << _colorWireCollision << "</ColorWireCollision>";
		oStr << "<ColorBackground>" << hex << _colorBackground << "</ColorBackground>";
		oStr << "<ColorSelected>" << hex << _colorSelected << "</ColorSelected>";
		oStr << "<TexturePathList>";
		for (vector<string>::iterator pIter(_dirTexturePath.begin()), pEnd(_dirTexturePath.end()); pIter != pEnd; ++pIter)
		{
			oStr << "<DirTexturePath>" << *pIter << "</DirTexturePath>";
		}
		oStr << "</TexturePathList>";
		oStr << "</DirectXView>";


		oStr << "</Config>";

		//  close file
		oStr.close();

		isOK = true;

	}  //  if (oStr.is_open())

	return isOK;
}

