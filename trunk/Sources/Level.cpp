#include "Level.h"

using namespace Webfoot;

void Level::Init(int topPlatform){
	tileHandler.Init();
	levelHeight = theScreen->HeightGet() * 4;
	platformHandler.Init(topPlatform, levelHeight);
}

void Level::Deinit(){
	platformHandler.Deinit();
	tileHandler.Deinit();
}

void Level::Update(unsigned int dt, Point2F velocity){
	tileHandler.Update(dt, velocity);
	platformHandler.Update(dt, velocity);
}

void Level::Draw(){
	tileHandler.Draw();
	platformHandler.Draw();
}

int const Level::GetLevelHeight(){
	return levelHeight;
}

std::vector<KT_Platform*> const Level::GetPlatforms(){
	return platformHandler.GetPlatforms();
}

std::vector<Patrol*> const Level::GetPatrols(){
	return platformHandler.GetPatrols();
}