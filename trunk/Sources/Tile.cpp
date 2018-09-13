// Alex Jonic

#include "Tile.h"

using namespace Webfoot;

Tile::Tile(){
	sprite = NULL; //need this here?
}

Tile::Tile(Point2F pos){
	position = pos;
	sprite = NULL;
}

// Use this init function if for testing purposes. It will load in a default tile.
void Tile::Init(Point2F pos){
	position = pos;
	sprite = theImages->Load("64_brick");
	
}

// Use this init function to dictate the sprite of the tile.
void Tile::Init(Point2F pos, char* filename){
	position = pos;
	sprite = theImages->Load(filename);
}

void Tile::Deinit(){
	if (sprite){
		theImages->Unload(sprite);
		sprite = NULL;
	}
}

void Tile::Update(unsigned int dt){

}

// Moves the tile by the velocity passed in.
void Tile::Update(unsigned int dt, Point2F velocity){
	float dtSeconds = (float)dt / 1000.0f;
	position.y += std::floorf(velocity.y * dtSeconds);
}

void Tile::Draw(){
	sprite->Draw(position);
}

Point2F const Tile::GetPosition(){
	return position;
}

void Tile::SetPosition(Point2F pos){
	position = pos;
}

void Tile::SetPosition(int x, int y){
	position.x = (float)x;
	position.y = (float)y;
}
