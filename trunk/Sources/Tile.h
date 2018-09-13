// Alex Jonic
// This is a tile: a (typically) 32x32 sprite that will make up the background

#ifndef __TILE_H
#define __TILE_H

#include "Frog.h"

namespace Webfoot{
	class Tile;
	class Tile{
	public:
		Tile();
		Tile(Point2F);
		void Init(Point2F);
		void Init(Point2F, char*);
		void Deinit();
		void Update(unsigned int);
		void Update(unsigned int, Point2F); // Used for moving the tile.
		void Draw();

		Point2F const GetPosition();
		void SetPosition(Point2F);
		void SetPosition(int, int);
	protected:
		Image* sprite;
		Point2F position;

	};
}

#endif