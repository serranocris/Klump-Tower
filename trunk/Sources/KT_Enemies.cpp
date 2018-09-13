//Nick Berg
#include "KT_Enemies.h"
#include "FrogMath.h"

#define DEBUG_SIZE 20.0f

using namespace Webfoot;

Patrol::Patrol(){ // This is the best method in the entire game
	sprite = NULL;
	patrolSpeed = 50.0f;
	spriteScale = 0.25f;
	direction = 1;
}

void Patrol::Init(){

}

void Patrol::Init(int x, int y, PatrolType t){

	type = t;
	switch (t){
	case PatrolType::CAT:
		patrolSpeed = 50.0f;
		break;
	case PatrolType::BURB:
		patrolSpeed = 150.0f;
		break;
	default:
		patrolSpeed = 50.0f;
		break;
	}

	sprite = frog_new Sprite();
	sprite->VisibleSet(true);
	if (type == PatrolType::CAT){
		sprite->Init("Sprites/Sprite", "CatRight");
	}
	else {
		sprite->Init("Sprites/Sprite", "BirdLeft");
	}
	sprite->ScaleSet(Point2F::Create(spriteScale, spriteScale));

	halfSprite = (sprite->SizeGet(0) * spriteScale) / 2.0f;

	position.x = (float) x;
	position.y = (float)y - halfSprite.y;
	velocity.x = patrolSpeed;

	p1 = position;
	p2 = position + Point2F::Create(DEBUG_SIZE, 0.0f);
	p3 = p2 + Point2F::Create(0.0f, DEBUG_SIZE);
	p4 = p1 + Point2F::Create(0.0f, DEBUG_SIZE);



}


void Patrol::Init(Point2F pos){
	position = pos;
	position.y += DEBUG_SIZE;
	velocity.x = patrolSpeed;
}

void Patrol::Deinit(){
	SmartDeinitDelete(sprite);
}


void Patrol::Draw(){
	sprite->Draw();
	/*
	theScreen->LineDraw(p1, p2, COLOR_RGBA8_RED);
	theScreen->LineDraw(p2, p3, COLOR_RGBA8_RED);
	theScreen->LineDraw(p3, p4, COLOR_RGBA8_RED);
	theScreen->LineDraw(p4, p1, COLOR_RGBA8_RED);
	*/
}


void Patrol::Update(unsigned int dt, Point2F parentVelocity, Point2F parentPosition, float parentLength){
	float dtSeconds = (float)dt / 1000.0f;
	//mysprite->Update(dt);

	halfSprite = (sprite->SizeGet(dt) * spriteScale) / 2.0f;

	velocity = parentVelocity;
	velocity += Point2F::Create(patrolSpeed * direction, 0.0f);

	position += velocity * dtSeconds;

	if (position.x > parentPosition.x + parentLength - halfSprite.x){
		position.x = parentPosition.x + parentLength - halfSprite.x;
		direction = -std::abs(direction);
		if (type == PatrolType::CAT){
			sprite->AnimationSet("CatLeft");
		}
		else {
			sprite->AnimationSet("BirdLeft");
		}
	}
	else if (position.x < parentPosition.x + halfSprite.x){
		position.x = parentPosition.x + halfSprite.x;
		direction = std::abs(direction);
		if (type == PatrolType::CAT){
			sprite->AnimationSet("CatRight");
		}
		else {
			sprite->AnimationSet("BirdRight");
		}
	}

	sprite->Update(dt);
	sprite->PositionSet(position);

	p1 = position - halfSprite; // ((sprite->SizeGet(dt) * spriteScale) / 2.0f);
	p2 = position + Point2F::Create(halfSprite.x, -halfSprite.y);
	p3 = p2 + Point2F::Create(0.0f, halfSprite.y * 2.0f);
	p4 = p1 + Point2F::Create(0.0f, halfSprite.y * 2.0f);
}

const int Patrol::GetDirection(){
	return direction;
}

const PatrolType Patrol::GetType(){
	return type;
}

Hazard::Hazard(){
	sprite = NULL;
	rotation = 0.0f;
	isAlive = true;
}

void Hazard::Init(){
	// 1024

	int powerupChance = FrogMath::Random(100); //Determines whether hazard dropped is actually a powerup // Nick Berg
	if(powerupChance <= 30) { //Chance for PowerUp is 30%. Will make power up sprite
		powerUp = true;
		
		sprite = frog_new Sprite();
		sprite->VisibleSet(true);
		sprite->Init("Sprites/Sprite", "RandomPowerup"); 
		
	}
	else{
		powerUp = false;
		
		sprite = frog_new Sprite();
		sprite->VisibleSet(true);
		int rnd = FrogMath::Random() % 2;
		if (rnd == 0){
			sprite->Init("Sprites/Sprite", "FallingComputer"); //again, probably want to change
			sprite->ScaleSet(Point2F::Create(1.0f, 1.0f));
		}
		else{
			sprite->Init("Sprites/Sprite", "BBQ");
			sprite->ScaleSet(Point2F::Create(0.45f, 0.45f));
		}
	}

	int randomYPlacement = FrogMath::Random() % 3000 + 2500;
	int randomXPlacement = FrogMath::Random() % 800 + 100;
	position.x = randomXPlacement;
	position.y = -randomYPlacement;
	velocity.y = 111.1f;
	velocity.x = 0.0f;

	/*
	DebugPrintf("X: %f\n", position.x);
	DebugPrintf("Y: %f\n", position.y);
	*/

	p1 = position;
	p2 = position;

	p1.x -= DEBUG_SIZE / 2;
	p1.y = position.y + DEBUG_SIZE;

	p2.x = p1.x + DEBUG_SIZE;
	p2.y = p1.y;

	p3.x = p2.x;
	p3.y = p1.y + DEBUG_SIZE; // - is above + is below


	p4.x = p1.x;
	p4.y = p3.y;

	// DebugPrintf("Init position: %f %f", p1.x, p1.y);

}

void Hazard::Deinit(){
	if (sprite){
		sprite->Deinit();
		frog_delete sprite;
		sprite = NULL;
	}
}

void Hazard::Update(unsigned int dt, Point2F backgroundVelocity){
	float dtSeconds = (float)dt / 1000.0f;
	//SmartDeinitDelete(sprite);
	position += (velocity + backgroundVelocity) * dtSeconds;

	p1 = position;
	p2 = position;


	p1.x += DEBUG_SIZE / 2;
	p1.y -= DEBUG_SIZE / 2;

	//DebugPrintf("X position %f\n", p1.x);
	//DebugPrintf("Y position %f\n", p1.y);

	p2.x = p1.x + DEBUG_SIZE;
	p2.y = p1.y;

	p3.x = p2.x;
	p3.y = p1.y + DEBUG_SIZE; // - is above + is below


	p4.x = p1.x;
	p4.y = p3.y;

	/*
	if (p1.y > 3000){
	p1.y = offScreen;
	}
	*/
	sprite->Update(dt);
	sprite->PositionSet(p1 + ((p3 - p1) / 2));
	sprite->RotationSet(rotation);
	if (!powerUp){
		if (rotation < 359.0f){
			rotation++;
			//DebugPrintf("Rot: %f\n", rotation);
		}
		else {
			rotation = 0.0f;
		}
	}
}

void Hazard::Draw(){

	sprite->Draw();
	//theScreen->LineDraw(Point2F::Create(position.x, position.y), Point2F::Create(position.x + DEBUG_SIZE, position.y), COLOR_RGBA8_GREEN);
	//theScreen->LineDraw(Point2F::Create(position.x + DEBUG_SIZE, position.y), Point2F::Create(position.x + DEBUG_SIZE, position.y + DEBUG_SIZE), COLOR_RGBA8_GREEN);
	//theScreen->LineDraw(Point2F::Create(position.x + DEBUG_SIZE, position.y + DEBUG_SIZE), Point2F::Create(position.x, position.y + DEBUG_SIZE), COLOR_RGBA8_GREEN);
	//theScreen->LineDraw(Point2F::Create(position.x, position.y + DEBUG_SIZE), Point2F::Create(position.x, position.y), COLOR_RGBA8_GREEN);
	//theScreen->LineDraw(p1, p2, COLOR_RGBA8_RED, 1.0, 0.0);
	//theScreen->LineDraw(p2, p3, COLOR_RGBA8_RED, 1.0, 0.0);
	//theScreen->LineDraw(p3, p4, COLOR_RGBA8_RED, 1.0, 0.0);
	//theScreen->LineDraw(p4, p1, COLOR_RGBA8_RED, 1.0, 0.0);
	
}



void Hazard::SetX(int posX){
	position.x = posX;
}

void Hazard::SetY(int posY){
	position.y = posY;
}
//Checks if a hazard is acutally a power up
int Hazard::CheckPowerUp(){
	if (powerUp == true){
		return 1;
	}
	else{
		return 0;
	}
}


int Hazard::CheckPlayerCollision(Point2F playerP1, Point2F playerP2, Point2F playerP3, Point2F playerP4){

	//if 
	if (playerP1.x < p3.x && playerP3.x > p1.x &&
		playerP1.y < p3.y && playerP3.y > p3.y)
	{
		return 1;
	}
}


