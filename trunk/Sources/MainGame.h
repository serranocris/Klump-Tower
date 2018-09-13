// Cris Serrano
// Alex Jonic

#ifndef __MAINGAME_H__
#define __MAINGAME_H__

#include "Frog.h"
#include "MenuState.h"
#include "Player.h"
#include "TileHandler.h"
#include "KT_Platform.h"
#include "Level.h"
#include "KT_Enemies.h"
#include "Klump.h"

namespace Webfoot {

class Player;
class Cloud;
class Ball;
class KT_Platform;
class Cat;
class Klump;
class Hazard;

class Cloud{
public:
	Cloud();
	void Init();
	void Deinit();
	void Update(unsigned int dt, Point2F backgroundVelocity);
	void Draw();
	void ResetCloud();
protected:
	Point2F position;
	Point2F velocity;
	Image* image;
	float speed;
};

//==============================================================================

class MainGame : public MenuState
{
public:
   typedef MenuState Inherited;

   MainGame();
   virtual ~MainGame() {};
   
   virtual void Init();
   virtual void Deinit();

   /// Call this on every frame to update the positions.
   virtual void Update();
   /// Call this on every frame to draw the images.
   virtual void Draw();

   static MainGame instance;

protected:
   /// Returns the name of the GUI layer
   virtual const char* GUILayerNameGet();

   Player* player;

   Klump* klump;

   std::vector<Cat*> cats;
   std::vector<Hazard*> hazards;

   Cloud clouds[2];

   Level level;
   Image* background;
};

MainGame* const theMainGame = &MainGame::instance;

//==============================================================================

/// A bouncing ball
class Ball
{
public:
   Ball();
   
   /// Initialize the ball
   void Init();
   /// Clean up the ball
   void Deinit();

   /// Make any changes for the given frame.  'dt' is the amount of time that
   /// has passed since the last frame, in milliseconds.
   void Update(unsigned int dt);
   /// Draw the ball.
   void Draw();

protected:
   /// Appearance of the ball.
   Image* image;
   /// Current position of the ball.
   Point2F position;
   /// Current velocity of the ball.
   Point2F velocity;
};
//==============================================================================



} //namespace Webfoot {

#endif //#ifndef __MAINGAME_H__
