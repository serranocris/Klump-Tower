// Alex Jonic
// This file handles all of the tile initializing, deinitializing, updating, and drawing.
// Makes a vector of all of the tiles needed to fill the screen during init by grabbing the screen size.

#include "TileHandler.h"

using namespace Webfoot;


TileHandler::TileHandler(){
}

void TileHandler::Init(){
	int n1 = 1;
	int n2 = 2;
	
	// We divide the screen into TILE_SIZExTILE_SIZE tiles.
	screenX = theScreen->WidthGet() / TILE_SIZE;
	screenY = theScreen->HeightGet() / TILE_SIZE;

	// We're starting at -1 to get a strip of tiles above the viewable area.
	for (int y = -1; y < screenY; y++){
		for (int x = 0; x < screenX; x++){
			// Multiply the x and y by 32 to get the actual position of the tile.
			Point2F position = Point2F::Create((float)(x * TILE_SIZE), (float)(y * TILE_SIZE));
			tiles.push_back(frog_new Tile());
			if (x == 0){
				tiles[tiles.size() - 1]->Init(position, "64_brick_leftedge"); // Initializing the tiles
			}
			else if (x == screenX - 1){
				tiles[tiles.size() - 1]->Init(position, "64_brick_rightedge"); // Initializing the tiles
			}
			else {
				tiles[tiles.size() - 1]->Init(position, "64_brick"); // Initializing the tiles
			}
			if (y > -1){
				if (x != 0 && x != screenX - 1){

					if (x == n1 && y == n2){
						tiles[tiles.size() - 1]->Init(position, "64_brick_window_topleft");
					}
					else if (x == n1 + 1 && y == n2){
						tiles[tiles.size() - 1]->Init(position, "64_brick_window_topright");
						n1 += 4;
						if (n1 + 1 >= screenX - 1){
							n1 = 1;
						}
					}
					else if (x == n1 && y == n2 + 1){
						tiles[tiles.size() - 1]->Init(position, "64_brick_window_botleft");
					}
					else if (x == n1 + 1 && y == n2 + 1){
						tiles[tiles.size() - 1]->Init(position, "64_brick_window_botright");
						n1 += 4;
						if (n1 + 1 >= screenX - 1 && y == n2 + 1){
							n1 = 1;
							n2 += 5;
						}
					}
				}
			}
			
		}
	}
}

void TileHandler::Deinit(){

	// Deiniting all of the tiles
	for (int i = 0; i < tiles.size(); i++){
		if (tiles[i] != NULL){
			tiles[i]->Deinit();
			frog_delete tiles[i];
			tiles[i] = NULL;
		}
	}
	tiles.clear();
}

void TileHandler::Update(unsigned int dt, Point2F velocity){
	float dtSeconds = (float)dt / 1000.0f;
	int offset;
	// Iterate through our list of tiles.
	for (int i = 0; i < tiles.size(); i++){

		/*
		else if (theKeyboard->KeyPressed(KeyCode::KEY_MINUS)){
			tiles[i]->Update(dt, Point2F::Create(0.0f, -DEBUG_TILE_SPEED));
		}*/

		tiles[i]->Update(dt, velocity);

		// if the tile has gone off the screen, lets move it to the top.
		if ((int)tiles[i]->GetPosition().y >= theScreen->HeightGet()){
			// The offset is how far off the screen the tile has gone before this "if" statement activates.
			offset = (int)(tiles[i]->GetPosition().y - theScreen->HeightGet());
			// We set the y position to -TILE_SIZE + the offset of the current tile. Otherwise, we end up with gaps.
			// BUG: For some reason, when we multiply the velocity of the tile by dtSeconds, it occasionally creates a 1px gap between some of the rows of tiles. 
			// Could this be because of float rounding? Seems too regular...
			tiles[i]->SetPosition((int)tiles[i]->GetPosition().x, -TILE_SIZE + offset);
		}
		
		// Will bring back tiles that have gone too far up, if we're reversing directions.
		// May be unnecessary, but I'm adding it just to see if it works.
		// BROKEN -- messes up tile placement. Creates lots of overlapping and gaps.
		/*if ((int)tiles[i]->GetPosition().y < -TILE_SIZE){
			tiles[i]->SetPosition((int)tiles[i]->GetPosition().x, theScreen->SizeGet().y);
		}*/

		// Moves the tiles on the screen. Passes in dt and a speed/velocity to move the tiles by
		if (theKeyboard->KeyPressed(KeyCode::KEY_PLUS)){
			tiles[i]->Update(dt, Point2F::Create(0.0f, DEBUG_TILE_SPEED));
		}
	}
}

void TileHandler::Draw(){
	// Iterate through and draw all of the tiles.
	for (int i = 0; i < tiles.size(); i++){
		tiles[i]->Draw();
	}
}