///////////////////////////////////////////////////////////
//  NifUtilMaterial.h
//  Definitions of Havok materials from Nix.xml
//  Created on:      09-June-2012 06:31:33
//  Original author: Skyfox
///////////////////////////////////////////////////////////

#if !defined(NIFUTILMATERIAL_H)
#define NIFUTILMATERIAL_H

#include <string>
#include <map>
#include <vector>

#define	NCU_NUM_DEFAULT_MATERIAL	3741512247	//  SKY_HAV_MAT_STONE

//  used namespaces
using namespace std;

namespace NifUtility
{
	struct NifUtlMaterial
	{
		unsigned int	_code;
		string			_name;
		string			_defName;
	};


	class NifUtlMaterialList
	{
		public:
						NifUtlMaterialList();
			virtual		~NifUtlMaterialList();

			/**
			 * get list of materials
			 */
			map<string, NifUtlMaterial>& getMaterialMap();

			/**
			 * check for existing material definition and return related code; 0 if unknown
			 */
			unsigned int getMaterialCode(string matDefName);

			/**
			 * Initialize material map
			 */
			void initializeMaterialMap(string pathToXML);

			/**
			 * Get list of user messages
			 */
			virtual vector<string>& getUserMessages();

			/**
			 * Reset material map
			 */
			virtual void reset();

		protected:
			/**
			 * log messages for user
			 */
			vector<string> _userMessages;

			/**
			 * map of materials, key: definition as string, value: NifUtlMaterial
			 */
			map<string, NifUtlMaterial> _materialMap;
	};
}
#endif // !defined(NIFUTILMATERIAL_H)
