// Alex Jonic, Cris Serrano

#include "Frog.h"
#include "MainGame.h"
#include "MainUpdate.h"
#include "Player.h"
#include "KT_Platform.h"
#include "TileHandler.h"
#include <stdlib.h>
#include "FrogMath.h"
#include "Klump.h"

using namespace Webfoot;

/// Speed of the ball along a given axis in pixels per second.
#define BALL_AXIS_SPEED 400.0f
#define NUM_HAZARDS 5
#define TOP_PLATFORM 20
int ii = 0;

MainGame MainGame::instance;

//==============================================================================

/// Main GUI
#define GUI_LAYER_NAME "MainGame"

//-----------------------------------------------------------------------------

MainGame::MainGame()
{
   player = NULL;
   klump = NULL;
   background = NULL;
}

//-----------------------------------------------------------------------------

void MainGame::Init()
{
	Inherited::Init();

	level.Init(TOP_PLATFORM);

	for (int i = 0; i < NUM_HAZARDS; i++){
		hazards.push_back(frog_new Hazard());
		hazards[i]->Init();
	}

	//DebugPrintf("Platform Count: %d", level.GetPlatforms().size());

	player = frog_new Player();
	player->Init(level.GetPlatforms()[0]->position.x, level.GetPlatforms()[0]->position.y, level.GetLevelHeight());

	klump = frog_new Klump();
	klump->Init();

	clouds[0] = Cloud();
	clouds[0].Init();

	clouds[1] = Cloud();
	clouds[1].Init();

	background = theImages->Load("Backgrounds/background");

}

//-----------------------------------------------------------------------------

void MainGame::Deinit()
{
	if (background){
		theImages->Unload(background);
		background = NULL;
	}

	clouds[0].Deinit();
	clouds[1].Deinit();

   if (player){
	   player->Deinit();
	   frog_delete player;
	   player = NULL;
   }

   if (klump != NULL){
	   klump->Deinit();
	   frog_delete klump;
	   klump = NULL;
   }

   /* Test
   if (hazard != NULL){
	   hazard->Deinit();
	   frog_delete hazard;
	   hazard = NULL;
   }
   */
   
   for (int i = 0; i < NUM_HAZARDS; i++){
	   if (hazards[i] != NULL){
		   hazards[i]->Deinit();
		   frog_delete hazards[i];
		   hazards[i] = NULL;
	   }
   }
 
   level.Deinit();
   
   /*
   if (kt_platform)
   {
	   kt_platform->Deinit();
	   frog_delete kt_platform;
	   kt_platform = NULL;
   }
   */



   hazards.clear();

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

   boolean bossFight = level.GetPlatforms()[level.GetPlatforms().size()-3]->position.y >= theScreen->HeightGet() / 2.0; //TOP_PLATFORM]->position.y >= theScreen->HeightGet() / 2.0f; //l
   if (player->IsAlive() && klump->hits < 3){
	   player->Update(dt, level.GetPlatforms(), bossFight);
	   player->CheckPatrolCollision(level.GetPatrols());
	   player->CheckHazardCollision(hazards);
	   if (player->CheckKlumpHit(klump)){
		   klump->TakeDamage(player->GetScore());
		   DebugPrintf("Klump hit\n");
	   }
	   player->CheckHitByKlump(klump);
   }
   else if (klump->hits == 3){
	   player->Dance(dt, true);
	   klump->hits++;
   }
   else if (klump->hits > 3){
	   player->Dance(dt, false);
   }
   klump->Update(dt, bossFight);

   

   // Background velocity used to move tiles and platforms.
   // Background velocity set to 0 unless player is at max height,
   // in which case backgroundVelocity.y is set to -player->GetVelocity().y
   Point2F backgroundVelocity = Point2F::Create(0.0F, 0.0F);
   if (player->atMaxHeight)
	   backgroundVelocity.y = -player->GetVelocity().y;
   // KT_Platforms Array for Cris
   
   for (int i = 0; i < NUM_HAZARDS; i++){
	   hazards[i]->Update(dt, backgroundVelocity);
	   
	   
	   
	   if (klump->toss == true){
		   
		   /*
		   hazards[i]->SetY(klump->GetY());
		   hazards[i]->SetX(klump->GetX());
		   */

		   
	   }
	   

	   if (i > 5){
		   i = 0;
	   }
   }
   level.Update(dt, backgroundVelocity);

   

   // global position of camera: 
   // level.GetLevelHeight() + player->GetGlobalOffset()
   // This will get the distance the camera has moved up from the bottom of the screen.

   //DebugPrintf("Player Global Coordinates: (%d, %d)\n", (int)player->GetGlobalPosition(level.GetLevelHeight()).x, (int)player->GetGlobalPosition(level.GetLevelHeight()).y);\

   // Return to the previous menu if the escape key is pressed.

   if (!theStates->StateChangeCheck() && theKeyboard->KeyJustPressed(KEY_ESCAPE))
   {
	   theMainGame->StateChangeTransitionBegin(true);
	   theStates->Pop();
   }

   clouds[0].Update(dt,backgroundVelocity);
   clouds[1].Update(dt,backgroundVelocity);
}

//-----------------------------------------------------------------------------

void MainGame::Draw()
{
	background->Draw();
	clouds[0].Draw();
	level.Draw();
	player->Draw();
	klump->Draw();
	clouds[1].Draw();

	for (int i = 0; i < NUM_HAZARDS; i++){
		hazards[i]->Draw();
	}

}

//==============================================================================

Ball::Ball()
{
   // Initialize pointers to NULL for safety.
   image = NULL;
}

//------------------------------------------------------------------------------

void Ball::Init()
{
   // Load the image of the ball.
   image = theImages->Load("Ball");

   // Start the ball in the middle of the screen.
   position = Point2F::Create(theScreen->SizeGet() / 2);

   // The ball will initially be moving down and to the right.
   velocity.Set(BALL_AXIS_SPEED, BALL_AXIS_SPEED);
}

//------------------------------------------------------------------------------

void Ball::Deinit()
{
   // Unload the image of the ball.
   if(image)
   {
      theImages->Unload(image);
      image = NULL;
   }
}

//------------------------------------------------------------------------------

void Ball::Update(unsigned int dt)
{
   // Get a floating point number for the duration of the last frame in seconds.
   float dtSeconds = (float)dt / 1000.0f;

   // Update the position of the ball.
   position += velocity * dtSeconds;

   // The position of the ball corresponds to its center.  We want to keep the
   // whole ball on-screen, so figure out the area within which the center must
   // stay.
   Point2F ballSize = Point2F::Create(image->SizeGet());
   Point2F halfBallSize = ballSize / 2.0f;
   Box2F ballArea = {halfBallSize.x, halfBallSize.y,
      (float)theScreen->WidthGet() - ballSize.x,
      (float)theScreen->HeightGet() - ballSize.y};

   // If the ball has gone too far in any direction, make sure its velocity
   // will bring it back.

   // See if it's too far right.
   if((position.x > ballArea.MaxXGet()) && (velocity.x > 0.0f))
      velocity.x *= -1.0f;

   // See if it's too far left.
   if((position.x < ballArea.x) && (velocity.x < 0.0f))
      velocity.x *= -1.0f;

   // See if it's too far down.
   if((position.y > ballArea.MaxYGet()) && (velocity.y > 0.0f))
      velocity.y *= -1.0f;

   // See if it's too far up.
   if((position.y < ballArea.y) && (velocity.y < 0.0f))
      velocity.y *= -1.0f;
}

//------------------------------------------------------------------------------

void Ball::Draw()
{
   // The center of the ball is in the center of the image, so use an offset.
   image->Draw(position - (Point2F::Create(image->SizeGet()) / 2.0f));
}

//------------------------------------------------------------------------------

Cloud::Cloud(){
	image = NULL;
}

void Cloud::Init(){

	image = theImages->Load("cloud");
	ResetCloud();
}

void Cloud::Deinit(){
	if (image){
		theImages->Unload(image);
		image = NULL;
	}
}

void Cloud::Update(unsigned int dt, Point2F backgroundVelocity){
	float dtSeconds = (float)dt / 1000.0f;
	position += (velocity + backgroundVelocity) * dtSeconds;

	if (position.x + image->WidthGet() < 0){
		ResetCloud();
	}

	if (position.y > theScreen->HeightGet()){
		ResetCloud();
	}
}

void Cloud::Draw(){
	image->Draw(position);
}

void Cloud::ResetCloud(){
	position.x = theScreen->WidthGet() + image->WidthGet() + 10;
	position.y = FrogMath::Random() % theScreen->HeightGet();
	speed = FrogMath::Random() % 100 + 50;
	velocity = Point2F::Create(-speed, 0.0f);
}