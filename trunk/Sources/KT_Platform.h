// Joseph Onesto
// Early Platform v2 header
// Modified by Cris Serrano, Alex Jonic

#ifndef __KT_PLATFORM_H
#define __KT_PLATFORM_H

#include "Frog.h"
#include "KT_Enemies.h"

#define DEBUG_PLATFORM_SIZE 192.0f
#define PLATFORM_SPEED 10.0f
#define PLATFORM_RESET_TIME 2.0f

namespace Webfoot
{
	enum PlatformType {
		NORMAL,
		SLIPPERY,
		SPRING,
		CRUMBLE,
		PATROL
	};

    class KT_Platform;
    
    class KT_Platform
    {
      public:
        KT_Platform();
        void Init();
		void Init(PlatformType, PatrolType);
        void Deinit();
        void Update(unsigned int, Point2F);
        void Draw();
        void PlayerCollide();   // Player is colliding with platform
		void ReverseDirection(bool);

		void SetType(PlatformType);
		void SetType(int);

		void ResetCrumblePlatform();
		void HandleCrumblePlatform(float);
		void FadeCrumblingPlatform(float);

		const PlatformType GetType();
		const Point2F GetVelocity();

		float horizontalSpeed = (float) (FrogMath::Random() % 201 + 50); // Gets a random speed for moving platforms

		// IDK what we plan on using to get values of the platform
		// adding reference to platform length for collision -Jeremy
		const float GetPlatformSize();
        
        // Coordinates of the platform within the level
        Point2F position;
		Point2F p1, p2; // Additional positions for testing

        // Length (x) and height (y) of the platform
        Point2F dimensions;
        
		bool moving;
        // Does the platform disappear after the player jumps off of it?
		bool crumblesOnContact = false;
        // Is the platform a spring?
		bool isSpring = false;
		// Is the platform slippery?
		bool isSlippery = false;

		bool isActive = true;

        // Determines the time for the platform to be available before it drops
        // (Think donut platforms in Mario)
        // Set to -1 if platform never drops
        float dropTimer;
		float crumbleStart;
		float crumbleEnd;
        
		// For downward movement
		//Point2F velocity; // Velocity is based on player movement
        Image* sprite;
		Sprite* animatedSprite; // Too lazy to just convert the images to sprites - Alex

		// The patrol enemy on the platform
		Patrol* patrol;
	protected:
		Point2F velocity;

		// What type of platform is it?
		PlatformType type;
		ColorRGBA8 debugPlatformColor;
	};
}          
#endif