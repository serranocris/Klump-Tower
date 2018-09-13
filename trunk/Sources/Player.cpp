// Alex Jonic, Jeremy Green, Kyle Kovack, Cris Serrano, Nick Berg

#include "Player.h"

using namespace Webfoot;

Player::Player(){
	sprite = NULL;
	gamepad = NULL;
	isAlive = true;
}

// Player init takes x and y variables to put on the first platform
void Player::Init(int posX, int posY, int levelHeight){
	// Just initializing this stuff to 0; feel free to change. Kyle's Reminder-> this will change when tiles are in  planning on having it spawn on the tile thats generated.
	localPosition.x = (float) (posX + 96); // 96 moves the player to the middle
	localPosition.y = (float) (posY - DEBUG_BOX_SIZE); // starts player on top of first platform
	prevLocalPosition.y = (float) theScreen->HeightGet();
	prevLocalPosition.x = 0;
	score = 0;
	soundCD = 0;
	velocity = Point2F::Create(0.0f, 0.0f);
	globalOffset = 0;
	jumps = 1;
	powerUpTime = 0;
	//Don't know where to obtain the values for the nativeDeviceIndex. Also unsure what const void* joycap is wanting, so I inserted NULL - Nick
	//gamepad = frog_new GamepadGeneric();
	//gamepad->Init(gamepad->AxisCountGet(), gamepad->ButtonCountGet(), gamepad->HatCountGet(), 0001, NULL, HEAP_DEFAULT);

	GOposition.y = 350.00;
	GOposition.x = theScreen->WidthGet() / 2.00;

	HEposition.y = 650.00;
	HEposition.x = theScreen->WidthGet() / 2.00;

	atMaxHeight = false;
	maxHeight = theScreen->HeightGet() / 5.0f * 2.0f;

	p1 = localPosition;
	p2 = localPosition;

	p1.x = DEBUG_BOX_SIZE / 2;
	p1.y = DEBUG_BOX_SIZE / 2;

	p2.x = p1.x + DEBUG_BOX_SIZE;
	p2.y = p1.y;

	p3.x = p2.x;
	p3.y = p1.y + DEBUG_BOX_SIZE;

	p4.x = p1.x;
	p4.y = p3.y;

	//DebugPrintf("localPosition(x): %i\n", localPosition.x);
	//DebugPrintf("localPosition(y): %i\n", localPosition.y);

	//DebugPrintf("p1(x): %i\n", p1.x);
	//DebugPrintf("p1(y): %i\n", p1.y);

	//Will need to move Sprites folder into graphics folder and number the sprites in a 001 style format- Nick
	sprite = frog_new Sprite();
	sprite->VisibleSet(true);
	sprite->Init("Sprites/Sprite", "Male");
	sprite->ScaleSet(Point2F::Create(0.35f, 0.35f));

	GameOver = theImages->Load("GameOverOutline");
	HitEsc = theImages->Load("HitEsc");
}

void Player::Deinit(){
	if (gamepad){
		gamepad->Deinit();
		frog_delete gamepad;
		gamepad = NULL;
	}

	if (sprite){
		sprite->Deinit();
		frog_delete sprite;
		sprite = NULL;
	}

	if (collidedPlatform){
		collidedPlatform = NULL;
	}

	if (GameOver)
	{
		theImages->Unload(GameOver);
		GameOver = NULL;
	}
}

void Player::Update(unsigned int dt, std::vector<KT_Platform*> platforms, boolean ignoreMaxHeight){
	float dtSeconds = (float)dt / 1000.0f;
	//Lets me see is the controller is connected or not - Nick
	/*bool connected = gamepad->ConnectedCheck();
	if (connected == false){
	DebugPrintf("Not Connected");
	}*/
	if (!HandleInput() && velocity.y < 1.0f && velocity.y > -1.0f){
		//HandleAnimation(AnimationState::IDLE);
		sprite->AnimationSet("Male1Idle");
		sprite->AnimationQueueClear();
	}

	velocity += Point2F::Create(0.0f, GRAVITY) * dtSeconds; // Gravity


	localPosition += velocity * dtSeconds;

	velocity.x *= friction; // friction; Only affects horizontal movement.

	//Terminal velocity of the sprite

	if (velocity.y > JUMP_HEIGHT){
		velocity.y = JUMP_HEIGHT;
	}

	if (velocity.y < -JUMP_HEIGHT * JUMP_HEIGHT_MODIFIER){
		velocity.y = -JUMP_HEIGHT * JUMP_HEIGHT_MODIFIER;
	}

	/*if (localPosition.y <= maxHeight && velocity.y < 0 && !ignoreMaxHeight){
		atMaxHeight = true;
		localPosition.y = maxHeight;
	}
	else{
		atMaxHeight = false;
	}*/
	
	soundCD = soundCD - 1;
	/// See if it lands on a platform. If so, reset y velocity
	if (CheckCollision(dtSeconds, platforms) && collidedPlatform != NULL && collidedPlatform->isActive){
		localPosition.y = collidedPlatform->position.y - DEBUG_BOX_SIZE/*.y*/ / 2.0f;
		//jumps = true;
		jumps = 1;

		// If the platform is moving, then we'll set the player's x-velocity to the platform's horizontal movement speed.
		if (collidedPlatform->moving){
			// If it's not slippery, our velocity will match the platform. Otherwise, we'll just have it slide out from under the player's feet.
			if (!collidedPlatform->isSlippery){
				velocity.x = collidedPlatform->GetVelocity().x;
			}
		}

		// If the platform is slippery, we'll change the player's friction
		if (collidedPlatform->isSlippery){
			friction = 1.0f; // BUG: this affects the player until they land on a new platform.
		}
		else {
			friction = 0.9f;
		}

		// If the platform is a spring, then we'll shoot the player up into the air
		if (collidedPlatform->isSpring){
			velocity.y = -velocity.y * JUMP_HEIGHT_MODIFIER;
		}
		else {
			velocity.y = 0.0F;
		}

		if (collidedPlatform->crumblesOnContact){
			if (collidedPlatform->crumbleStart < 0){
				collidedPlatform->crumbleStart = (float)theClock->TickCountGet();
			}
		}
		
	}

	else {
		if (friction != 0.9f){
			friction = 0.9f;
		}		
	}

	// Just setting up generic constraints.
	// Obviously position.y constraints will be altered later.
	// Constraints MUST go AFTER the velocities have been applied to the position, otherwise we get glitchy movement.

	if (localPosition.x < 0){
		localPosition.x = 0.0f;
	}
	else if (localPosition.x + DEBUG_BOX_SIZE > theScreen->WidthGet()){
		isAlive = false;
	}

	// Redundant
	if (localPosition.y < 0){
		localPosition.y = 0.0f;
		velocity.y = 0.0f; // Stopping player from "sticking" to top of screen.
	}

	// If player hits max height it stops
	if (localPosition.y <= maxHeight && !ignoreMaxHeight){
		atMaxHeight = true;
		localPosition.y = maxHeight;
		globalOffset += (int)(velocity.y * dtSeconds);
		//DebugPrintf("Global Offset: %d\n", globalOffset);
	}
	else{
		atMaxHeight = false;
	}

	if (localPosition.y + DEBUG_BOX_SIZE > theScreen->HeightGet()){
		localPosition.y = (float)theScreen->HeightGet() - DEBUG_BOX_SIZE;
		velocity.y = 0.0f;
		isAlive = false;
	}

	//Basic Scoring system. Nick
	//Score increases for each jump.
	if (localPosition.y < prevLocalPosition.y){
		score = score + 25;
		prevLocalPosition = localPosition;
	//	DebugPrintf("Score: %d\n", score);
	}
	else{
		prevLocalPosition = localPosition;
	}


	p1 = localPosition;
	p2 = localPosition;

	p1.x -= DEBUG_BOX_SIZE / 2 - 15;
	p1.y = localPosition.y + DEBUG_BOX_SIZE / 2 - DEBUG_BOX_SIZE;

	p2.x = p1.x + DEBUG_BOX_SIZE - 35;
	p2.y = p1.y;

	p3.x = p2.x;
	p3.y = p1.y + DEBUG_BOX_SIZE - 10;

	p4.x = p1.x;
	p4.y = p3.y;
	if (localPosition.y > 1003){
		isAlive = false;
		localPosition.x = theScreen->WidthGet() / 2;
	}
	sprite->Update(dt);
	sprite->PositionSet(localPosition + Point2F::Create(0.0f,5.0f));
}

void Player::Draw(){

	/*
	place holder box for the sprite will need to implement the sprite at some point.  Talk to M to see about menu and character selection.
	*/

	sprite->Draw();
	/*
	theScreen->LineDraw(Point2F::Create(localPosition.x - DEBUG_BOX_SIZE / 2, localPosition.y - DEBUG_BOX_SIZE / 2), Point2F::Create(localPosition.x + DEBUG_BOX_SIZE / 2, localPosition.y - DEBUG_BOX_SIZE / 2), COLOR_RGBA8_GREEN);
	theScreen->LineDraw(Point2F::Create(localPosition.x + DEBUG_BOX_SIZE / 2, localPosition.y - DEBUG_BOX_SIZE / 2), Point2F::Create(localPosition.x + DEBUG_BOX_SIZE / 2, localPosition.y + DEBUG_BOX_SIZE / 2), COLOR_RGBA8_GREEN);
	theScreen->LineDraw(Point2F::Create(localPosition.x + DEBUG_BOX_SIZE / 2, localPosition.y + DEBUG_BOX_SIZE / 2), Point2F::Create(localPosition.x - DEBUG_BOX_SIZE / 2, localPosition.y + DEBUG_BOX_SIZE / 2), COLOR_RGBA8_GREEN);
	theScreen->LineDraw(Point2F::Create(localPosition.x - DEBUG_BOX_SIZE / 2, localPosition.y + DEBUG_BOX_SIZE / 2), Point2F::Create(localPosition.x - DEBUG_BOX_SIZE / 2, localPosition.y - DEBUG_BOX_SIZE / 2), COLOR_RGBA8_GREEN);
	
	theScreen->LineDraw(p1, p2, COLOR_RGBA8_RED, 1.0, 0.0);
	theScreen->LineDraw(p2, p3, COLOR_RGBA8_RED, 1.0, 0.0);
	theScreen->LineDraw(p3, p4, COLOR_RGBA8_RED, 1.0, 0.0);
	theScreen->LineDraw(p4, p1, COLOR_RGBA8_RED, 1.0, 0.0);
	*/
	
	if (isAlive == false){
		GameOver->Draw(GOposition - (Point2F::Create(GameOver->SizeGet()) / 2.0f));
		HitEsc->Draw(HEposition - (Point2F::Create(HitEsc->SizeGet()) / 2.0f));
	}
	
}

// Handles the player's input. Called in Update()
bool Player::HandleInput(){
	bool returnVal = false;
	if (jumps == 1){
		jumps = 0;
		if (theKeyboard->KeyJustPressed(KeyCode::KEY_UP) || theKeyboard->KeyJustPressed(KeyCode::KEY_W)){
			if (power == 1 || power == 3 && powerUpTime > 0){
				velocity = Point2F::Create(velocity.x, -JUMP_HEIGHT * 1.5);
				powerUpTime - 1;
			}
			else{
				velocity = Point2F::Create(velocity.x, -JUMP_HEIGHT);
			}
			Sound* s = theSounds->Load("Jump");
			s->Play();
			returnVal = true;
		}
	}
	
	if (theKeyboard->KeyPressed(KeyCode::KEY_RIGHT) || theKeyboard->KeyPressed(KeyCode::KEY_D)){
		if (power == 2 || power == 3 && powerUpTime > 0){
			velocity = Point2F::Create(PLAYER_HORIZANTAL_MOVEMENT * 1.5, velocity.y);
			powerUpTime - 1;
		}
		else{
			velocity = Point2F::Create(PLAYER_HORIZANTAL_MOVEMENT, velocity.y);
		}
		HandleAnimation(AnimationState::MOVING_RIGHT);
		//sprite->AnimationEnqueue("Male1WalkRight");
		returnVal = true;
	}

	if (theKeyboard->KeyPressed(KeyCode::KEY_LEFT) || theKeyboard->KeyPressed(KeyCode::KEY_A)){
		if (power == 2 || power == 3 && powerUpTime > 0){
			velocity = Point2F::Create(-PLAYER_HORIZANTAL_MOVEMENT * 1.5, velocity.y);
			powerUpTime - 1;
		}
		else{
			velocity = Point2F::Create(-PLAYER_HORIZANTAL_MOVEMENT, velocity.y);
		}
		//sprite->AnimationEnqueue("Male1WalkLeft");
		HandleAnimation(AnimationState::MOVING_LEFT);
		returnVal = true;
	}

	// Getting down input wont be necessary unless we want to implement a "fast fall"
	if (theKeyboard->KeyJustPressed(KeyCode::KEY_DOWN) || theKeyboard->KeyJustPressed(KeyCode::KEY_S)){
		velocity = Point2F::Create(velocity.x, PLAYER_SPEED);
		returnVal = true;
	}

	return returnVal;
}

// Returns the position of the player
Point2F const Player::GetLocalPosition(){
	return localPosition;
}

// To get the player's global position, we have to do:
// (level.GetLevelHeight() - (int)theScreen->SizeGet().y) + player->GetGlobalOffset() + (int)player->GetLocalPosition().y)
// We get the level's height, and subtract the screen's size (1024) from it. The player will be at the bottom of the screen (1024), so that counters the initial subtraction.
// This is necessary, because we want to get the player's position in the world. Their position in local space contributes to their position in the world.
// Example: (4096 - 1024) + 0 + 1004 = 4076
// The above example calculates where the player will be positioned at y:1004 locally (aka, where the player starts (since they are 20 pixels tall currently)
Point2F Player::GetGlobalPosition(int levelHeight){
	return Point2F::Create(localPosition.x, (float) ((levelHeight - (int)theScreen->HeightGet()) + globalOffset + (int)localPosition.y));
}

// Returns the velocity of the player
Point2F const Player::GetVelocity(){
	return velocity;
}

int const Player::GetGlobalOffset(){
	return globalOffset;
}

// Checks:
// 1) If the right side of the player is to the right of the left side of the platform
// 2) If the left side of the player is to the left of the right side of the platform
// 3) If the bottom of the player is below the platform
// 4) If the bottom of the player was above the platform before the last movement (with a landing buffer)

// Returns true if a platform is being touched by the player. Sets collidedPlatform to the current platform the player is touching.
// Returns false if the player is not in contact with a platform, and sets the collidedPlatform to null.
bool Player::CheckCollision(float dtSeconds, std::vector<KT_Platform*> platforms){
	float landingBuffer = 0.01F;
	for (int i = 0; i < platforms.size(); i++){
		if (localPosition.x + (DEBUG_BOX_SIZE - 15)/*.x*/ / 2.0f > platforms[i]->position.x &&
			localPosition.x - (DEBUG_BOX_SIZE - 15)/*.x*/ / 2.0f < platforms[i]->position.x + platforms[i]->GetPlatformSize() &&
			localPosition.y + DEBUG_BOX_SIZE/*.y*/ / 2.0f > platforms[i]->position.y &&
			localPosition.y + DEBUG_BOX_SIZE/*.y*/ / 2.0f - (velocity.y * dtSeconds) <= platforms[i]->position.y + landingBuffer)
			
		{
			collidedPlatform = platforms[i];
			grounded = true;
			return true;
		}
	}
	collidedPlatform = NULL;
	grounded = false;
	return false;
}

bool Player::CheckPatrolCollision(std::vector<Patrol*> patrols){
	for (int i = 0; i < patrols.size(); i++){
		// The collisions aren't based on the player's collision box. This is a little problematic.
		if (CheckSquareCollision(patrols[i]->p1.x, patrols[i]->p2.x, patrols[i]->p1.y, patrols[i]->p4.y)){
			//DebugPrintf("Hit!");
			// How far and in which direction we throw the player when they hit an enemy
			velocity = Point2F::Create(patrols[i]->GetDirection() * 750.0f, -25.0f);
			if (soundCD <= 0){
				Sound* s;
				if (patrols[i]->GetType() == PatrolType::CAT){
					s = theSounds->Load("mew1");
				}
				else {
					s = theSounds->Load("Chirp");
				}
				s->Play();
				soundCD = 100;
			}
			return true;
		}

	}
	return false;
}

//Checks collision of hazards and applies powerups

bool Player::CheckHazardCollision(std::vector<Hazard*> hazards){
	for (int i = 0; i < hazards.size(); i++){
		if (localPosition.x > hazards[i]->p1.x &&
			localPosition.x < hazards[i]->p2.x &&
			localPosition.y > hazards[i]->p1.y &&
			localPosition.y < hazards[i]->p4.y &&
			hazards[i]->isAlive){

			//DebugPrintf("Hit");
			
			if (hazards[i]->CheckPowerUp() == 1){
				power = FrogMath::Random() % 3;
				powerUpTime = 5000;
				if (soundCD <= 0){
					Sound* s = theSounds->Load("GenericUpgradeObtained");
					s->Play();
					soundCD = 100;
				}
				
			}
			else{
				velocity.y *= 5.0f;
				velocity.x *= 2.0f;
				if (soundCD <= 0){
					Sound* s = theSounds->Load("DmgTakenMale1");
					s->Play();
					soundCD = 100;
				}
			}

			hazards[i]->isAlive = false;
			return true;
		}
	}
	return false;
}

void Player::HandleAnimation(AnimationState anim){
	switch (anim){
	case AnimationState::IDLE:
		break;
	case AnimationState::MOVING_RIGHT:
		sprite->AnimationSet("Male1WalkRight");
		break;
	case AnimationState::MOVING_LEFT:
		sprite->AnimationSet("Male1WalkLeft");
		break;
	case AnimationState::JUMPING:
		break;
	case AnimationState::HIT:
		break;
	default:
		break;
	}
	sprite->PausedSet(false);
}

bool Player::IsAlive(){
	return isAlive;
}

bool Player::CheckSquareCollision(int left, int right, int top, int bottom){
	return (p1.x <= right &&
		p2.x >= left &&
		p1.y <= bottom &&
		p4.y >= top);
}

bool Player::CheckKlumpHit(Klump* klump){
	
	/*
	if (localPosition.x + DEBUG_BOX_SIZE / 2.0f >= klump->position.x
		&& localPosition.x - DEBUG_BOX_SIZE / 2.0f <= klump->position.x + klump->GetSize()
		&& localPosition.y + DEBUG_BOX_SIZE / 2.0f >= klump->position.y
		&& localPosition.y - DEBUG_BOX_SIZE / 2.0f <= klump->position.y + klump->GetSize() / 2.0f){
		*/
	if (CheckSquareCollision(klump->position.x, klump->position.x + klump->GetSize(), klump->position.y, klump->position.y + klump->GetSize())){
		// Player bounces up and away from Klump
		Point2F bounceVelocity = Point2F::Create(100, -500);
		// Bounce left if on left side of Klump
		if (localPosition.x < klump->position.x + klump->sprite->SizeGet(0).x / 2.0f)
			bounceVelocity.x *= -1;
		velocity = bounceVelocity;
		
		return true;
	}
	return false;
}

bool Player::CheckHitByKlump(Klump* klump){
	if (localPosition.x + DEBUG_BOX_SIZE / 2.0f >= klump->position.x
		&& localPosition.x - DEBUG_BOX_SIZE / 2.0f <= klump->position.x + klump->GetSize()
		&& localPosition.y + DEBUG_BOX_SIZE / 2.0f >= klump->position.y + klump->GetSize() / 2.0f
		&& localPosition.y - DEBUG_BOX_SIZE / 2.0f <= klump->position.y + klump->GetSize()){
		isAlive = false;
		return true;
	}
	return false;
}

int Player::GetScore(){
	return score;
}

void Player::Dance(unsigned int dt, boolean first){
	if (first)
		sprite->AnimationEnqueue("Male1Final");
	sprite->Update(dt);
	sprite->Draw();
}