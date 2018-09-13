// Alex Jonic featuring work by Cris Serrano

#include "PlatformHandler.h"

using namespace Webfoot;

void PlatformHandler::Init(int topPlatform, int levelHeight){
	platformCount = (levelHeight / 50);
	//DebugPrintf("%d", platformCount);
	GeneratePlatforms(topPlatform, levelHeight);
}

void PlatformHandler::Deinit(){
	for (int i = 0; i < platforms.size(); i++){
		if (platforms[i] != NULL){
			platforms[i]->Deinit();
			frog_delete platforms[i];
			platforms[i] = NULL;
		}
	}
	platforms.clear();
	patrols.clear();
}

void PlatformHandler::Update(unsigned int dt, Point2F velocity){
	for (int i = 0; i < platforms.size(); i++){
		//if (platforms[i]->isActive){
		platforms[i]->Update(dt, velocity);
		//}
	}
}

void PlatformHandler::Draw(){
	for (int i = 0; i < platforms.size(); i++){
		if (platforms[i]->isActive){
			platforms[i]->Draw();
		}
	}
}

// For balancing purposes, the higher up we go, the more patrol platforms and such we want.
// We could try to find a way to do that.

void PlatformHandler::GeneratePlatforms(int topPlatform, int levelHeight){
	int distance = FrogMath::Random() % 100 + 50; // Used to increase Y position of platforms
	bool firstPass = true; //Used to set the x position for the first platform
	//int randomPlacement = rand() % 556; // FrogMath's random is better than the standard library, let it be known
	int randomFrog = FrogMath::Random() % 556; // Places the first platform
	int randomChoice = FrogMath::Random() % 1; // Returns true or false
	int platPositionX = 0; // Used to store a platforms position for later
	int platPositionY = 0;
	int maximumX = FrogMath::Random() % MAX_X; // Gets a number that cannot be placed any further than specified
	int maximumRepos = FrogMath::Random() % REPOSITION; // Repositions platform if it is of screen
	int randomChance = FrogMath::Random() % 1; // Another true or false variable
	int makeMove;
	int movingOffset = 0; // Used to position the moving platforms
	int firstPlatX = 0; // Used to put the player on the first platform
	int firstPlatY = 0;
	int platformRng = 0;
	PlatformType platformType = PlatformType::NORMAL;
	PatrolType patrolType = PatrolType::CAT;

	for (int i = 0; i < platformCount + 3; i++){
		platforms.push_back(frog_new KT_Platform());
		

		/*if (i > topPlatform){
			platformType = PlatformType::CRUMBLE;
		}*/

		// If we're past the 10th platform, we'll set a special platform type.
		if (i > 10){
			// Roll to see what platform type we get.
			// Essentially this is coded like rolling a 100 sided die.
			// If we get 5 or less, we'll have a patrol platform. 5-15 and we'll have a crumbling platform
			// etc..
			// If anyone has a better way of handling this, feel free to change it.
			int difficultyMod = 0;
			if (i > platformCount - (platformCount / 3.0f)){
				difficultyMod = 20;
				patrolType = PatrolType::BURB;
			}
			else if (i > platformCount / 2.0f){
				difficultyMod = 10;
				//DebugPrintf("EU!!! DIFFICULTY! \n");
			}
			platformRng = FrogMath::Random() % 100;
			if (platformRng < 5 + difficultyMod){
				platformType = PlatformType::PATROL;
			}
			else if (platformRng < 10 + difficultyMod){
				platformType = PlatformType::CRUMBLE;
			}
			else if (platformRng < 20 + difficultyMod) {
				platformType = PlatformType::SLIPPERY;
			}
			else if (platformRng < 40){
				platformType = PlatformType::SPRING;
			}
			else {
				platformType = PlatformType::NORMAL;
			}
		}

		if (i >= platformCount - 3){
			platformType = PlatformType::CRUMBLE;
			platforms[i]->position.y = platPositionY - 20;
			if (i == platformCount - 3){
				platforms[i]->position.x = theScreen->WidthGet() / 2.0f - (platforms[i]->GetPlatformSize() / 2.0f);
			}
			else if (i == platformCount - 2){
				platforms[i]->position.x = theScreen->WidthGet() / 3.0f - platforms[i]->GetPlatformSize();
			}
			else if (i == platformCount - 1) {
				platforms[i]->position.x = theScreen->WidthGet() - (theScreen->WidthGet() / 3.0f) - platforms[i]->GetPlatformSize();
			}
		}

		if (platPositionY < -levelHeight + (theScreen->HeightGet() / 3.0f)){
			//DebugPrintf("Platform %d above line: %d %d\n", i, platPositionY, -levelHeight);
			platformType = PlatformType::CRUMBLE;
		}
		
		maximumX = FrogMath::Random() % MAX_X;
		maximumRepos = FrogMath::Random() % REPOSITION;
		randomChance = FrogMath::Random() % 1;
		makeMove = FrogMath::Random() % 9; // 10% chance of making the platform move

		// This if statement places the first platform within screen bounds
		if (firstPass){
			platforms[i]->position.x = (float)randomFrog; //748 is how wide the screen is, 192 is the width of the platform, 748 - 192 = 556
			firstPass = false; // Sets to false so the rest of the platforms can be created
			firstPlatX = (int)platforms[i]->position.x;
			firstPlatY = (int)platforms[i]->position.y;
		}

		// Gives every platform except the first one a 10% chance of being able to move left and right
		else if (makeMove == 1){
			if (movingOffset > 200){
				movingOffset = -200;
			}
			platforms[i]->position.x = theScreen->WidthGet() / 2.0f;// +movingOffset;
			platforms[i]->ReverseDirection(true);
			movingOffset += MOVING_INCREMENT; // This places platforms so that they do not all move in unison
		}

		//=========================================================================================
		// Make new platforms here in an else if block - Cris


		//=========================================================================================

		else if (!firstPass && i < platformCount){
			if (makeMove != 1){
				if (randomChance == 1){ // If true, a platform is placed to the right
					platforms[i]->position.x = (float)(platPositionX + maximumX); // Uses the last platform's X position, and adds on the extra units
				}
				else if (randomChance == 0){ // Placed left
					platforms[i]->position.x = (float)(platPositionX - maximumX); // Last X position - extra units
				}
			}

			if (platforms[i]->position.x < 0){ // If the platform is too left, move right
				platforms[i]->position.x = 0.0f; // Reset platform to side of screen.
				platforms[i]->position.x += maximumRepos; // Gets a random number to reposition the platform
			}
			else if (platforms[i]->position.x + platforms[i]->GetPlatformSize() > theScreen->WidthGet()){ // If too far right, move left
				platforms[i]->position.x = theScreen->WidthGet() - platforms[i]->GetPlatformSize(); // Reset platform to side of screen
				platforms[i]->position.x -= maximumRepos; // Repositions platform
			}
		}

		platPositionX = (int)platforms[i]->position.x; // Saves this platform's position to use again later
		platPositionY = (int)platforms[i]->position.y;

		// if (platforms[i])
		platforms[i]->position.y += distance; // Inreases Y to move platforms up
		distance -= FrogMath::Random() % 100 + 75; // Changed to negative from positive to create the platforms above - Cris

		// We init it after we've set up everything, otherwise there are a few weird "gotchyas"
		platforms[i]->Init(platformType, patrolType);
		
		if (platformType == PlatformType::PATROL){
			patrols.push_back(platforms[i]->patrol);
		}
	}
}

const std::vector<KT_Platform*> PlatformHandler::GetPlatforms(){
	return platforms;
}

const std::vector<Patrol*> PlatformHandler::GetPatrols(){
	return patrols;
}