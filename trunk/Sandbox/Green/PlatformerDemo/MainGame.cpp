#include "Frog.h"
#include "MainGame.h"
#include "MainUpdate.h"

using namespace Webfoot;

#define GRAVITY 400.0F
#define JUMP_INTENSITY 300.0F
#define MOVE_SPEED 250.0F
#define ARRAY_SIZE 10

MainGame MainGame::instance;

//==============================================================================

/// Main GUI
#define GUI_LAYER_NAME "MainGame"

//-----------------------------------------------------------------------------

MainGame::MainGame()
{
	player = NULL;
	player2 = NULL;
}

//-----------------------------------------------------------------------------

void MainGame::Init()
{
   Inherited::Init();

   // Create and initialize the player.
   player = frog_new Player();
   player->Init("PlayerRight", "PlayerLeft");

   player2 = frog_new Player();
   player2->Init("Player2Right", "Player2Left");

   // Platforms are expressed as Point4F objects
   // x,y = coordinates of first point
   // z,w = coordinates of second point
   //
   // Set first platform as ground
   float groundHeight = theScreen->HeightGet() / 4.0 * 3.0;
   platforms[0].x = 0;
   platforms[0].y = groundHeight;
   platforms[0].z = theScreen->WidthGet();
   platforms[0].w = groundHeight;

   // Set the rest of the platforms at a fixed height, fixed size, and random x values
   // x = a random point in the first (screen width - platform width) of the screen horizontally
   // y = a set point (index * platform vertical spacing) above the ground
   // z = a point (platform width) to the right of the x value;
   // w = the y value;
   //
   float platformWidth = theScreen->WidthGet() / 4.0;
   if (platformWidth > theScreen->WidthGet())
   {
	   platformWidth = theScreen->WidthGet();
   }
   float platformVerticalSpacing = 40.0;
   
   for (int i = 1; i < ARRAY_SIZE; i++)
   {
	   platforms[i].x = rand() % (int)(theScreen->WidthGet() - platformWidth);
	   platforms[i].y = groundHeight - (i * platformVerticalSpacing);
	   platforms[i].z = platforms[i].x + (platformWidth);
	   platforms[i].w = platforms[i].y;
   }
}

//-----------------------------------------------------------------------------

void MainGame::Deinit()
{
   // Deinitialize and delete the players.
	if (player)
	{
		player->Deinit();
		frog_delete player;
		player = NULL;
	}
   
	if (player2)
	{
		player2->Deinit();
		frog_delete player2;
		player2 = NULL;
	}

   Inherited::Deinit();
}

//-----------------------------------------------------------------------------

const char* MainGame::GUILayerNameGet()
{
   return GUI_LAYER_NAME;
}

//-----------------------------------------------------------------------------

void MainGame::Update()
{
   Inherited::Update();

   unsigned int dt = theClock->LoopDurationGet();

   player->Update(dt, platforms);
   player2->Update(dt, platforms);

   // Return to the previous menu if the escape key is pressed.
   if(!theStates->StateChangeCheck() && theKeyboard->KeyJustPressed(KEY_ESCAPE))
   {
      theMainGame->StateChangeTransitionBegin(true);
      theStates->Pop();
   }

   // Check for player 1 key presses
   if (theKeyboard->KeyJustPressed(KEY_UP))
   {
	   player->Jump();
   }
   if (theKeyboard->KeyJustPressed(KEY_DOWN))
   {
	   player->Drop();
   }
   if (theKeyboard->KeyJustPressed(KEY_LEFT))
   {
	   player->LeftPressed();
   }
   // Check for player 1 key releases
   if (theKeyboard->KeyJustReleased(KEY_LEFT))
   {
	   player->LeftReleased();
   }
   if (theKeyboard->KeyJustPressed(KEY_RIGHT))
   {
	   player->RightPressed();
   }
   if (theKeyboard->KeyJustReleased(KEY_RIGHT))
   {
	   player->RightReleased();
   }


   // Check for player 2 key presses
   if (theKeyboard->KeyJustPressed(KEY_W))
   {
	   player2->Jump();
   }
   if (theKeyboard->KeyJustPressed(KEY_S))
   {
	   player2->Drop();
   }
   if (theKeyboard->KeyJustPressed(KEY_A))
   {
	   player2->LeftPressed();
   }
   // Check for player 2 key releases
   if (theKeyboard->KeyJustReleased(KEY_A))
   {
	   player2->LeftReleased();
   }
   if (theKeyboard->KeyJustPressed(KEY_D))
   {
	   player2->RightPressed();
   }
   if (theKeyboard->KeyJustReleased(KEY_D))
   {
	   player2->RightReleased();
   }
}

//-----------------------------------------------------------------------------

void MainGame::Draw()
{
	// Draw platforms
	for (int i = 0; i < ARRAY_SIZE; i++)
	{
		theScreen->LineDraw(Point2F::Create(platforms[i].x, platforms[i].y), Point2F::Create(platforms[i].z, platforms[i].w), COLOR_RGBA8_WHITE);
	}

	// Draw players
	player->Draw();
	player2->Draw();
}

//==============================================================================

Player::Player()
{
   // Initialize pointers to NULL for safety.
   imageRight = NULL;
   imageLeft = NULL;
}

//------------------------------------------------------------------------------

void Player::Init(char* rightImageName, char* leftImageName)
{
	// Load the images of the player.
	imageRight = theImages->Load(rightImageName);
	imageLeft = theImages->Load(leftImageName);

   // Start the player in the middle of the bottom of the screen.
   position = Point2F::Create(theScreen->SizeGet() / 2);

   // Set the player with a y velocity of 0
   yVelocity = 0;

   // Set the player facing right
   right = true;

   // Set player to not moving
   movingRight = false;
   movingLeft = false;
}

//------------------------------------------------------------------------------

void Player::Deinit()
{
   // Unload the image of the player.
   if(imageRight)
   {
      theImages->Unload(imageRight);
      imageRight = NULL;
   }

   if (imageLeft)
   {
	   theImages->Unload(imageLeft);
	   imageLeft = NULL;
   }
}

//------------------------------------------------------------------------------

void Player::Update(unsigned int dt, Point4F platformArray[ARRAY_SIZE])
{
   // Get a floating point number for the duration of the last frame in seconds.
   float dtSeconds = (float)dt / 1000.0f;

   // Update the y velocity based on gravity
   yVelocity += GRAVITY * dtSeconds;

   // Update the y position of the player.
   position.y += yVelocity * dtSeconds;

   // Only move the player horizontally if one direction is pressed
   if (movingRight && !movingLeft)
   {
	   position.x += MOVE_SPEED * dtSeconds;
   }
   if (movingLeft && !movingRight)
   {
	   position.x -= MOVE_SPEED * dtSeconds;
   }


   // The position of the player corresponds to its center.  We want to keep the
   // whole player on-screen, so figure out the area within which the center must
   // stay.
   Point2F playerSize = Point2F::Create(imageRight->SizeGet());
   Point2F halfPlayerSize = playerSize / 2.0f;
   Box2F playerArea = {halfPlayerSize.x, halfPlayerSize.y,
      (float)theScreen->WidthGet() - playerSize.x,
      (float)theScreen->HeightGet() - playerSize.y};

   // If the player has gone too far in any direction, stop it at the edge of the screen

   // See if it's too far right.
   if(position.x > playerArea.MaxXGet())
      position.x = playerArea.MaxXGet();

   // See if it's too far left.
   if(position.x < playerArea.x)
      position.x = playerArea.x;

   /// See if it lands on a platform. If so, reset y velocity
   // Checks:
   // 1) If the right side of the player is to the right of the left side of the platform
   // 2) If the left side of the player is to the left of the right side of the platform
   // 3) If the bottom of the player is below the platform
   // 4) If the bottom of the player was above the platform before the last movement (with a landing buffer)
   //
   float landingBuffer = 0.01F;
   for (int i = 0; i < ARRAY_SIZE; i++)
   {
	   if (position.x + halfPlayerSize.x > platformArray[i].x &&
		   position.x - halfPlayerSize.x < platformArray[i].z &&
		   position.y + halfPlayerSize.y > platformArray[i].y &&
		   position.y + halfPlayerSize.y - (yVelocity * dtSeconds) <= platformArray[i].y + landingBuffer)
	   {
		   position.y = platformArray[i].y - halfPlayerSize.y;
		   yVelocity = 0.0F;
	   }
   }
}

//------------------------------------------------------------------------------

void Player::Draw()
{
   // The center of the player is in the center of the image, so use an offset.
	if (right)
	{
		imageRight->Draw(position - (Point2F::Create(imageRight->SizeGet()) / 2.0f));
	}
	else
	{
		imageLeft->Draw(position - (Point2F::Create(imageLeft->SizeGet()) / 2.0f));
	}
}


// If the players y velocity if close enough to 0 (almost garunteed to be on a platform) the player is allowed to jump
// Very small potential for a bug:
// If the y velocity at the peak of a jump happens to be within 0.00001 of 0 (not likely) and the player makes a frame-perfect jump they will get a double jump
// Can be fixed with a boolean contantly updating whether or not the player is on a platform (probably not necessary)
//
void Player::Jump()
{
	if (std::abs(yVelocity) < 0.00001)
	{
		yVelocity = -JUMP_INTENSITY;
	}
}

void Player::Drop()
{
	yVelocity = JUMP_INTENSITY;
}


// Fuctions called based on key presses
// Sets facing direction and moving direction
//
void Player::RightPressed()
{
	right = true;
	movingRight = true;
}

void Player::RightReleased()
{
	if (movingLeft)
	{
		right = false;
	}
	movingRight = false;
}

void Player::LeftPressed()
{
	right = false;
	movingLeft = true;
}

void Player::LeftReleased()
{
	if (movingRight)
	{
		right = true;
	}
	movingLeft = false;
}

//------------------------------------------------------------------------------