#ifndef __PLATFORM_HANDLER_H
#define __PLATFORM_HANDLER_H

#include "Frog.h"
#include "KT_Platform.h"
#include <vector>

#define NUM_PLATFORMS 1000 // Breaks when you have more than 10, causes issues with Tile.cpp (Fixed; Changed array of platforms to vector - Alex)
#define MAX_X 192 // The maximum distance a platform can be placed from the one previous, should never change
#define REPOSITION 556 // Helps reposition the platform
#define MOVING_INCREMENT 50 // Postions the moving platforms by this number

namespace Webfoot {
	class PlatformHandler;

	class PlatformHandler{
	public:
		void Update(unsigned int, Point2F);
		void Init(int, int);
		void Deinit();
		void Draw();

		void GeneratePlatforms(int, int);

		const std::vector<KT_Platform*> GetPlatforms();
		const std::vector<Patrol*> GetPatrols();

	protected:
		std::vector<KT_Platform*> platforms;
		std::vector<Patrol*> patrols;
		int platformCount;
	};
}
#endif