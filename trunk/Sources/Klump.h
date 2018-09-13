// Cris Serrano

#ifndef __KLUMP_H
#define __KLUMP_H

#include "KT_Enemies.h"
#include "Frog.h"

#define DEBUG_SIZE 150
#define SPEED 125.0f

namespace Webfoot{

	class Klump;

	class Klump{
	public:
		Klump();
		void Init(); // Used to start on any given platform
		void Deinit();
		void Update(unsigned int, boolean);
		void Draw();

		void ThrowHazard();
		void ResetBounces();
		int randKlump;
		int GetX();
		int GetY();
		void TakeDamage(int);
		int GetSize();

		bool toss;
		void TurnAround();
		bool positionThrow = false;
		int AddBounceCount();
		Sprite* sprite;
		int bounces;
		int maxBounces = 4;
		int BounceCountGet();
		Point2F position;
		Point2F velocity;
		float bossFightSwoop;
		float topPadding;
		int hits;
	};

}
#endif