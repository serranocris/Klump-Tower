// Alex Jonic

#ifndef __LEVEL_H__
#define __LEVEL_H__

#include "Frog.h"
#include "KT_Platform.h"
#include "TileHandler.h"
#include "PlatformHandler.h"
#include <vector>

namespace Webfoot {
	class Level{
	public:
		void Update(unsigned int, Point2F);
		void Draw();
		void Init(int);
		void Deinit();

		const int GetLevelHeight();
		const std::vector<KT_Platform*> GetPlatforms();
		const std::vector<Patrol*> GetPatrols();

	protected:
		//std::vector<KT_Platform> platforms;
		PlatformHandler platformHandler;
		TileHandler tileHandler;
		int levelHeight;
	};
}

#endif