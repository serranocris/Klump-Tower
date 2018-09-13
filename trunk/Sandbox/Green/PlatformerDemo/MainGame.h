#ifndef __MAINGAME_H__
#define __MAINGAME_H__

#include "Frog.h"
#include "MenuState.h"
#include <string.h>
#include <vector>

#define ARRAY_SIZE 10

namespace Webfoot {

class Player;

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

   /// The players.
   Player* player;
   Player* player2;

   // Array of platforms
   Point4F platforms[ARRAY_SIZE];
};

MainGame* const theMainGame = &MainGame::instance;

//==============================================================================

/// A player
class Player
{
public:
   Player();
   
   /// Initialize the player
   // imageNamePrefix does not include "left" and "right"
   void Init(char* rightImageName, char* leftImageName);
   /// Clean up the player
   void Deinit();

   /// Make any changes for the given frame.  'dt' is the amount of time that
   /// has passed since the last frame, in milliseconds.
   void Update(unsigned int dt, Point4F platformArray[ARRAY_SIZE]);
   /// Draw the player
   void Draw();

   void Jump();
   void Drop();

   void RightPressed();
   void RightReleased();
   void LeftPressed();
   void LeftReleased();

protected:
   /// Appearance of the player.
   Image* imageRight;
   Image* imageLeft;
   /// Current position of the player.
   Point2F position;
   /// Current y velocity of the player.
   float yVelocity;
   // Indicates whether the player is facing right or not
   bool right;

   // Indicates whether the left or right keys are pressed
   bool movingRight;
   bool movingLeft;
};

//==============================================================================

} //namespace Webfoot {

#endif //#ifndef __MAINGAME_H__
