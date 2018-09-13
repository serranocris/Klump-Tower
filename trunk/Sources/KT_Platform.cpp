// Cris Serrano
// Modified by: Alex Jonic


#include "KT_Platform.h"

using namespace Webfoot;

KT_Platform::KT_Platform(){
	crumbleEnd = -1.0f;
	crumbleStart = -1.0f;
	animatedSprite = NULL;
	sprite = NULL;
	moving = false;
	patrol = NULL;
	isActive = true;
	velocity = Point2F::Create(0.0f, 0.0f);
	position = Point2F::Create(theScreen->WidthGet() / 2.0f, theScreen->HeightGet() / 2.0f);
}

void KT_Platform::Init(){
	// Placing the debug platform in the middle of the screen
	//position = Point2F::Create(theScreen->SizeGet().x / 2.0f, theScreen->SizeGet().y / 2.0f);
	type = PlatformType::NORMAL;
	debugPlatformColor = COLOR_RGBA8_GREEN;
	//velocity = { 0.0f, 20.0f }; // {X speed, y speed}
	
	sprite = theImages->Load("brick_plateform");
}

// Alternative Init() function that sets the type of the platform
void KT_Platform::Init(PlatformType t, PatrolType pt){
	//position = Point2F::Create(theScreen->SizeGet().x / 2.0f, theScreen->SizeGet().y / 2.0f);

	type = t;

	// We'll set the bools depending on the type. Also we'll init various things that might be necessary.
	// Realistically, we don't need the bools, but it might be easier to call them instead of the type 
	// - Alex
	switch (type){

	case PlatformType::CRUMBLE:
		crumblesOnContact = true;
		debugPlatformColor = COLOR_RGBA8_MAGENTA;
		animatedSprite = frog_new Sprite();
		animatedSprite->Init("Sprites/Sprite", "FallingPlatform");
		dropTimer = 1.0f;
		animatedSprite->VisibleSet(true);
		animatedSprite->PausedSet(true);
		/*
		sprite = theImages->Load("shade_plateform"); //right one?
		*/
		break;
	case PlatformType::SPRING:
		isSpring = true;
		debugPlatformColor = COLOR_RGBA8_YELLOW;
		sprite = theImages->Load("shade_plateform"); //also don't know if correct
		break;
	case PlatformType::SLIPPERY:
		isSlippery = true;
		debugPlatformColor = COLOR_RGBA8_ORANGE;
		sprite = theImages->Load("icebrick_plateform");
		break;
	case PlatformType::PATROL:
		debugPlatformColor = COLOR_RGBA8_RED;
		sprite = theImages->Load("brick_plateform"); //check
		patrol = frog_new Patrol();
		patrol->Init((int)position.x + (FrogMath::Random(GetPlatformSize())), (int)position.y, pt);
		break;
	default:
		debugPlatformColor = COLOR_RGBA8_GREEN;
		crumblesOnContact = false;
		isSpring = false;
		isSlippery = false;
		sprite = theImages->Load("brick_plateform");
		break;
	}

}

void KT_Platform::Deinit(){
	if (sprite){
		theImages->Unload(sprite);
	}

	if (patrol){
		patrol->Deinit();
		frog_delete patrol;
		patrol = NULL;
	}

	if (animatedSprite){
		animatedSprite->Deinit();
		frog_delete animatedSprite;
		animatedSprite = NULL;
	}
}

const float KT_Platform::GetPlatformSize(){
	return DEBUG_PLATFORM_SIZE;
}

void KT_Platform::ReverseDirection(bool chance){
	if (chance == true){
		moving = true;

	}
}

void KT_Platform::Draw(){
	// Debug draw
	//theScreen->LineDraw(Point2F::Create(position.x, position.y), Point2F::Create(position.x + DEBUG_PLATFORM_SIZE, position.y), debugPlatformColor);
	if (sprite != NULL){
		sprite->Draw(position);
	}

	if (animatedSprite != NULL){
		animatedSprite->Draw();
	}

	if (type == PlatformType::PATROL && patrol != NULL){
		patrol->Draw();
	}
}

// We might need a prite for this to work
void KT_Platform::Update(unsigned int dt, Point2F v){
	float dtSeconds = (float)dt / 1000.0f;

	velocity.y = v.y;

	HandleCrumblePlatform(dtSeconds);

	if (moving == true){
		//position.x += horizontalSpeed * dtSeconds;
		velocity.x = horizontalSpeed * 100 * dtSeconds;

		if (position.x < 0){
			position.x = 0;
			horizontalSpeed *= -1.0f;
		}
		else if (position.x > ((float)theScreen->WidthGet() - DEBUG_PLATFORM_SIZE)){
			position.x = (float)theScreen->WidthGet() - DEBUG_PLATFORM_SIZE;
			horizontalSpeed *= -1.0f;
		}
	}

	position += velocity * dtSeconds;

	if (position.x < 0){
		position.x = 0;
		horizontalSpeed = std::abs(horizontalSpeed);
	}
	else if (position.x > ((float)theScreen->WidthGet() - DEBUG_PLATFORM_SIZE)){
		position.x = (float)theScreen->WidthGet() - DEBUG_PLATFORM_SIZE;
		horizontalSpeed = -std::abs(horizontalSpeed);
	}

	if (type == PlatformType::PATROL && patrol != NULL){
		patrol->Update(dt, velocity, position, GetPlatformSize());
	}

	if (animatedSprite != NULL){
		animatedSprite->Update(dt);
		animatedSprite->PositionSet(position);
	}

	if (position.y > theScreen->HeightGet()){
		isActive = false;
	}
	
}

// Returns the PlatformType of the KT_Platform
const PlatformType KT_Platform::GetType(){
	return type;
}

// Sets the PlatformType of the KT_Platform
void KT_Platform::SetType(PlatformType t){
	type = t;
}

// Sets the PlatformType of the KT_Platform by passing in the integer of the respective type
void KT_Platform::SetType(int i){
	type = (PlatformType)i;
}

const Point2F KT_Platform::GetVelocity(){
	return velocity;
}

void KT_Platform::HandleCrumblePlatform(float dtSeconds){
	if (crumbleStart > 0 && isActive){
		animatedSprite->PausedSet(false);
		FadeCrumblingPlatform(dtSeconds);
		// Drop Timer is multiplied by 1000.0f because we need to convert it to miliseconds.
		// After the timer is up, determined by when the player lands on the platform, we'll make the platform stop being active
		//if (crumbleStart > 0 && theClock->TickCountGet() > crumbleStart + (dropTimer * 1000.0f)){

		if (animatedSprite->FinishedPlayingCheck()){
			isActive = false;
			crumbleEnd = (float)theClock->TickCountGet();
		}
	}

	if (crumbleEnd > 0 && (float)theClock->TickCountGet() > crumbleEnd + (PLATFORM_RESET_TIME * 1000.0f) && !isActive){
		ResetCrumblePlatform();
	}
}

void KT_Platform::FadeCrumblingPlatform(float dtSeconds){
	ColorRGBA8 color = animatedSprite->ColorGet();
	color.alpha -= (uchar) (std::ceil(255/9) * dtSeconds);
	animatedSprite->ColorSet(color);
	if (color.alpha <= 0){
		animatedSprite->VisibleSet(false);
		color.alpha = 255;
		animatedSprite->ColorSet(color);
	}
}

void KT_Platform::ResetCrumblePlatform(){
	ColorRGBA8 color = animatedSprite->ColorGet();
	color.alpha = 255;
	animatedSprite->ColorSet(color);
	animatedSprite->TimeSet(0);
	animatedSprite->PausedSet(true);
	isActive = true;
	animatedSprite->VisibleSet(true);
	crumbleStart = -1;
	crumbleEnd = -1;
}