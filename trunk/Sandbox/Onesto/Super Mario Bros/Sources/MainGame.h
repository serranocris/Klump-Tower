#ifndef __MAINGAME_H__
#define __MAINGAME_H__

#include "Frog.h"
#include "MenuState.h"
#include <iostream>

namespace Webfoot {

	class Block;
	class Mario;

	class Mario
	{
	public:
		Point2F localPosition;
		Image* sprite;
		Sound* sfx;
		int health;
		int lives;
		int score;
		int velocity;
		bool facingRight;
		bool onGround;

		void Init();
		void Deinit();

		void Draw();
	};

	class Block
	{
	public:
		Block();
		static Block Create(int p, char c, Point2F P);

		char id;
		bool empty;
		bool solid;
		Point2F location;
		Image* sprite;
		Image* powerSprite;
		Sound* effect;

		void Draw(int offset);
		void Hit(int health);
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

	   virtual void Update();
	   virtual void Draw();

	   virtual void LoadLevel(float level);

	   static MainGame instance;
	   int globalOffset;
	   int jumpTimer;
	   Sound* music;
	   Image* background;
	   Block map[400][15];
	   Mario mario;

	protected:
	   /// Returns the name of the GUI layer
	   virtual const char* GUILayerNameGet();

	};

	MainGame* const theMainGame = &MainGame::instance;

	//==============================================================================

} //namespace Webfoot {

#endif //#ifndef __MAINGAME_H__
