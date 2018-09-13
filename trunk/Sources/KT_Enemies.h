// Cris Serrano, Alex Jonic

#ifndef __KT_ENEMIES_H
#define __KT_ENEMIES_H

#include "Frog.h"
//#include "Player.h"

namespace Webfoot{
	
	enum PatrolType {
		CAT,
		BURB
	};

	class Patrol;

	class Patrol{
	public:
		Patrol();
		void Init();
		void Init(int, int, PatrolType); // Used to start on any given platform
		void Init(Point2F);
		void Deinit();
		void Update(unsigned int, Point2F, Point2F, float);
		void Draw();

		bool TurnAround();

		Sprite* sprite;
		Point2F platformPosition;
		Point2F position;
		Point2F velocity;
		Point2F p1, p2, p3, p4; // Used for collision

		const int GetDirection();

		const PatrolType GetType();
	protected:
		float patrolSpeed;
		float spriteScale;
		int direction;
		Point2F halfSprite;
		PatrolType type;
	};


	class Hazard;

	class Hazard{
	public:
		Hazard();
		void Init();
		void Deinit();
		void Update(unsigned int, Point2F);
		void Draw();
		void SetX(int);
		void SetY(int);
		int offScreen = FrogMath::Random() % + 3000;
		Point2F p1, p2, p3, p4;

		int CheckPlayerCollision(Point2F playerP1, Point2F playerP2, Point2F playerP3, Point2F playerP4);
		// Can't do this, otherwise we're circle-referencing Player.h
		// Player.h references KT_Platform.h which references KT_Enemies.h which references Player.h
		// Could you pass a player's position instead?
		//int CheckPlayerCollision(Player* player);
		int CheckPowerUp();

		int powerupChance;
		bool powerUp;

		void Throw(int, int);

		Sprite* sprite;
		Point2F position;
		Point2F velocity;

		float rotation;

		bool isAlive;

	};
}

#endif