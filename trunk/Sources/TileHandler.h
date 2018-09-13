// Alex Jonic
// This class handles the creation, deletion, drawing, and movement of tiles.

#ifndef __TILE_HANDLER_H
#define __TILE_HANDLER_H

#define TILE_SIZE 64
#define DEBUG_TILE_SPEED 500.0f

#include "Frog.h"
#include "Tile.h"
#include <vector>

namespace Webfoot {

	class TileHandler;

	class TileHandler{

	public:
		TileHandler();
		void Init();
		void Deinit();
		void Update(unsigned int, Point2F);
		void Draw();
	protected:
		// Can't use a 2-Dimensional array: arrays cannot be sized by variables
		std::vector<Tile*> tiles;
		int screenX, screenY;
	};

}

#endif