//Nick Berg
#include "Frog.h"
#include "MainMenu.h"
#include "MainGame.h"
#include "MainUpdate.h"

using namespace Webfoot;

/// Which interface should be shown for this state.
#define GUI_LAYER_NAME "MainMenu"

MainMenu MainMenu::instance;



//-----------------------------------------------------------------------------

void MainMenu::Init()
{
   Inherited::Init();
   exitingGame = false;
   //Loads image and sets position of image
   image = theImages->Load("Klump Tower 8bit");
   background = theImages->Load("Backgrounds/background");
   s = theSounds->Load("altPlaceholder");
   s->Play();
   position.y = 300.00;
   position.x = theScreen->WidthGet() / 2.00;

   bgpos.y = 0.0f;
   bgpos.x = 0.0f;

}

//-----------------------------------------------------------------------------

void MainMenu::OnGUILayerInit(LayerWidget*)
{
   // Set up the GUI callbacks.
   PressButtonWidget::OnClickRegister(GUI_LAYER_NAME ".Play", OnPlayClick);
   PressButtonWidget::OnClickRegister(GUI_LAYER_NAME ".Exit", OnExitClick);
}

//-----------------------------------------------------------------------------

void MainMenu::Deinit()
{
   if(exitingGame)
      theMainUpdate->Exit();

   //Unloads image when not in use
   if (image)
   {
	   theImages->Unload(image);
	   image = NULL;
   }

   if (background)
   {
	   theImages->Unload(background);
	   background = NULL;
   }

   if (s){
	   s->Stop();
	   theSounds->Unload(s);
   }

   Inherited::Deinit();
}




//-----------------------------------------------------------------------------
//Draw method draws the image onto the screen
void MainMenu::Draw()
{
	background->Draw(bgpos);

	image->Draw(position - (Point2F::Create(image->SizeGet()) / 2.0f));	
}

//-----------------------------------------------------------------------------

const char* MainMenu::GUILayerNameGet()
{
   return GUI_LAYER_NAME;
}

//-----------------------------------------------------------------------------

void MainMenu::OnPlayClick(PressButtonWidget*, void*)
{
   if(!theStates->StateChangeCheck() && !theMainUpdate->ExitingCheck())
   {
      theMainMenu->StateChangeTransitionBegin(true);
      theStates->Push(theMainGame);
   }
}

//-----------------------------------------------------------------------------

void MainMenu::OnExitClick(PressButtonWidget*, void*)
{
   if(!theStates->StateChangeCheck() && !theMainUpdate->ExitingCheck())
   {
      theMainMenu->StateChangeTransitionBegin(true);
      theMainMenu->exitingGame = true;
      theStates->Pop();
   }
}

//-----------------------------------------------------------------------------
