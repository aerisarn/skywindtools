#pragma once

#include <vector>

using namespace std;

namespace NifUtility
{
	class Configuration
	{
		public:
			string			_configName;
			string			_pathSkyrim;
			string			_pathTemplate;
			string			_pathNifXML;
			string			_lastTexture;
			string			_lastTemplate;
			string			_dirSource;
			string			_dirDestination;
			string			_dirBatchSource;
			string			_dirBatchDestination;
			string			_dirCollision;
			string			_matScanTag;
			string			_matScanName;
			vector<string>	_matScanPrefix;
			vector<string>	_matScanIgnore;
			vector<string>	_dirTexturePath;
			DWORD			_colorWireframe;
			DWORD			_colorWireCollision;
			DWORD			_colorBackground;
			DWORD			_colorSelected;
			int				_collMaterial;
			int				_matHandling;
			int				_vertColHandling;
			int				_collTypeHandling;
			bool			_upTangentSpace;
			bool			_reorderProperties;
			bool			_dxShowTexture;
			bool			_dxShowWireframe;
			bool			_dxShowColorWire;
			bool			_dxForceDDS;
			bool			_hasFile;

		private:
			virtual	bool	readAttribute(const string& content, const string tag, vector<string>& attribute, unsigned int& offsetOut, unsigned int offsetIn=0);
			virtual	bool	readAttribute(const string& content, const string tag, string& attribute, unsigned int& offsetOut, unsigned int offsetIn=0);
			virtual	bool	readAttribute(const string& content, const string tag, DWORD& attribute, unsigned int& offsetOut, unsigned int offsetIn=0);
			virtual	bool	readAttribute(const string& content, const string tag, int& attribute, unsigned int& offsetOut, unsigned int offsetIn=0);
			virtual	bool	readAttribute(const string& content, const string tag, bool& attribute, unsigned int& offsetOut, unsigned int offsetIn=0);

		public:
							Configuration();
			virtual			~Configuration();

			virtual	bool	read (const string fileName);
			virtual	bool	write();
			virtual	bool	write(const string fileName);
	};
}
