// Alex Jonic, Jeremy Green, Kyle Kovack

#ifndef __PLAYER_H
#define __PLAYER_H

#include "Frog.h"
#include "KT_Platform.h"
#include "KT_Enemies.h"
#include "Klump.h"
#include <vector>

#define PLAYER_SPEED 400.0f//Slowed player down a bit
#define PLAYER_HORIZANTAL_MOVEMENT 250.0f //Thought the player moved too much to the right or left with one button click
#define JUMP_HEIGHT 400.0f //slowed player down to a resanable jump
#define JUMP_HEIGHT_MODIFIER 1.5f // Used when the player jumps off a spring platform
#define GRAVITY 398.0f //Default 500 Reduced to 398 for better control
#define DEBUG_BOX_SIZE 65

namespace Webfoot {

	enum AnimationState {
		IDLE,
		MOVING_LEFT,
		MOVING_RIGHT,
		JUMPING,
		HIT,
		_TOTAL
	};

	class Player;

	class Player {
	public:
		Player();
		void Init(int, int, int);
		void Deinit();
		void Update(unsigned int, std::vector<KT_Platform*>, boolean);
		void Draw();

		bool HandleInput();
		void HandleAnimation(AnimationState);

		bool CheckCollision(float, std::vector<KT_Platform*>);
		bool CheckPatrolCollision(std::vector<Patrol*>);
		bool CheckHazardCollision(std::vector<Hazard*>);
		bool IsAlive();
		bool CheckKlumpHit(Klump*);
		bool CheckHitByKlump(Klump*);

		bool CheckSquareCollision(int left, int right, int top, int bottom);

		Point2F const GetLocalPosition();
		Point2F GetGlobalPosition(int levelHeight);
		Point2F const GetVelocity();
		int const GetGlobalOffset();
		int GetScore();
		void Dance(unsigned int, boolean first);

		Point2F p1, p2, p3, p4;
		Point2F GOposition;
		Point2F HEposition;

		boolean atMaxHeight;
		float maxHeight;

	protected:
		bool grounded; // Used to detect whether or not the player is on a platform
		bool isAlive;

		float friction; // Changed depending on if the player is on a slippery platform or not
						// Default friction is 0.9
		//bool jumps;
		Point2F velocity;
		Point2F localPosition;
		Point2F prevLocalPosition;
		Sprite* sprite;
		Image* GameOver;
		Image* HitEsc;
		int score;
		int j;
		int jumps;
		int powerUpTime;
		int power;
		int soundCD;
		
		int globalOffset;
		GamepadGeneric* gamepad;

		KT_Platform* collidedPlatform; // The platform we're currently colliding with

		float spriteScale;
	};
}

#endif