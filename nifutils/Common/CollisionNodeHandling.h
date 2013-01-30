///////////////////////////////////////////////////////////
//  CollisionNodeHandling.h
//  Implementation of the Class CollisionNodeHandling
//  Created on:      06-Mai-2012 10:04:40
//  Original author: fdueber
///////////////////////////////////////////////////////////

#if !defined(COLLISIONNODEHANDLING_H)
#define COLLISIONNODEHANDLING_H

namespace NifUtility
{
	/**
	 * handling of collision node
	 * @author fdueber
	 * @version 1.0
	 * @created 06-Mai-2012 10:04:40
	 */
	enum CollisionNodeHandling
	{
		NCU_CN_COLLISION = 0,
		NCU_CN_FALLBACK,
		NCU_CN_SHAPES
	};

}
#endif // !defined(COLLISIONNODEHANDLING_H)
