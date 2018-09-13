#include "Frog.h"
#include "MainGame.h"
#include "MainUpdate.h"
#include <string>
#include <iostream>
#include <fstream>

using namespace std;
using namespace Webfoot;

/// Speed of the ball along a given axis in pixels per second.
#define BALL_AXIS_SPEED 400.0f

MainGame MainGame::instance;

//==============================================================================

/// Main GUI
#define GUI_LAYER_NAME "MainGame"

//-----------------------------------------------------------------------------

MainGame::MainGame()
{
	DebugPrintf("Constructed.");
}

//-----------------------------------------------------------------------------

void MainGame::Init()
{
	map[400][15];
	Inherited::Init();
	for (int i = 0; i < 400; i++)
	{
		for (int j = 0; j < 15; j++)
		{
			map[i][j] = Block::Create(0, ' ', Point2F::Create(0, 0));
		}
	}
	DebugPrintf("Loading level...");
	LoadLevel(1.1);
	DebugPrintf("Complete");
	music->Play(0, true);
	globalOffset = 0;
	jumpTimer = 0;
	mario.Init();
}

//-----------------------------------------------------------------------------

void MainGame::Deinit()
{   
	Inherited::Deinit();
	music->Stop();
	theSounds->Unload(music);
	music = NULL;
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

	// Return to the previous menu if the escape key is pressed.
	if (!theStates->StateChangeCheck() && theKeyboard->KeyJustPressed(KEY_ESCAPE))
	{
		theMainGame->StateChangeTransitionBegin(true);
		theStates->Pop();
	}

	mario.velocity = 0;
	// Move Mario Right
	if (theKeyboard->KeyPressed(KEY_RIGHT))
	{
		mario.velocity = 1;
		mario.facingRight = true;
		mario.sprite = theImages->Load("Mario/MarioRunR");
		if (theKeyboard->KeyPressed(KEY_RSHIFT) || theKeyboard->KeyPressed(KEY_LSHIFT))
		{
			mario.localPosition.x += 14;
		}
		else
		{
			mario.localPosition.x += 14;
		}
		if (mario.localPosition.x + mario.sprite->WidthGet() / 2 > theScreen->WidthGet() / 2)
		{
			mario.localPosition.x = theScreen->WidthGet() / 2 - mario.sprite->WidthGet() / 2;
			globalOffset += 14;
		}
	}

	// Move Mario Left
	else if (theKeyboard->KeyPressed(KEY_LEFT))
	{
		mario.velocity = -1;
		mario.facingRight = false;
		mario.sprite = theImages->Load("Mario/MarioRunL");
		if (theKeyboard->KeyPressed(KEY_RSHIFT) || theKeyboard->KeyPressed(KEY_LSHIFT))
		{
			mario.localPosition.x -= 14;
			if (mario.localPosition.x < 0)
				mario.localPosition.x = 0;
		}
		else
		{
			mario.localPosition.x -= 14;
			if (mario.localPosition.x < 0)
				mario.localPosition.x = 0;
		}
		if (globalOffset <= 0)
			globalOffset = 0;
	}

	else if (theKeyboard->KeyPressed(KEY_DOWN))
	{
		mario.localPosition.y += 8;
	}

	// Mario Stands
	else
	{
		if (mario.facingRight)
		{
			mario.sprite = theImages->Load("Mario/MarioR");
		}
		else
		{
			mario.sprite = theImages->Load("Mario/MarioL");
		}
	}

	// Mario Jump
	if (theKeyboard->KeyPressed(KEY_UP) && mario.onGround)
	{
		mario.onGround = false;
		if (mario.health > 1)
			mario.sfx = theSounds->Load("SFX/superjump");
		else
			mario.sfx = theSounds->Load("SFX/jump");
		jumpTimer = 350;
		// mario.sfx->Play(0, false);
		if (mario.facingRight)
		{
			mario.sprite = theImages->Load("Mario/MarioJumpR");
		}
		else
		{
			mario.sprite = theImages->Load("Mario/MarioJumpL");
		}
	}
	else if(!theKeyboard->KeyPressed(KEY_UP))
	{
		jumpTimer = 0;
	}

	if (jumpTimer > 0)
	{
		mario.localPosition.y -= std::sqrt(jumpTimer) / 1.1f;
		jumpTimer -= theClock->LoopDurationGet();
	}
	else
	{
		mario.localPosition.y += 10;
	}

	// Check Mario Collision
	float x = (mario.localPosition.x + globalOffset) / 64.0;
	float y = 15.0 - (mario.localPosition.y + 40.0) / 64.0;

#pragma region Vertical Collisions

	// Standing directly on blocks
	if ((map[(int)x + 1][(int)y - 1].solid || !map[(int)x + 1][(int)y - 1].empty) && mario.localPosition.y + mario.sprite->HeightGet() > map[(int)x + 1][(int)y - 1].location.y)
	{
		mario.onGround = true;
		DebugPrintf("Standing A\n");
		mario.localPosition.y = map[(int)x + 1][(int)y - 1].location.y - mario.sprite->HeightGet();

		x = (mario.localPosition.x + globalOffset) / 64.0;
		y = 15.0 - (mario.localPosition.y + 40.0) / 64.0;
	}

	// Mario overhang off left of blocks, standing on.
	if (x - (int)x > 0.290f)
	{
		// Stand on block
		if (!mario.onGround && (map[(int)x + 2][(int)y - 1].solid || !map[(int)x + 2][(int)y - 1].empty) && mario.localPosition.y + mario.sprite->HeightGet() > map[(int)x + 2][(int)y - 1].location.y && !map[(int)x+2][(int)y].solid)
		{
			DebugPrintf("Standing B\n");
			mario.onGround = true;
			mario.localPosition.y = map[(int)x + 2][(int)y - 1].location.y - mario.sprite->HeightGet();

			x = (mario.localPosition.x + globalOffset) / 64.0;
			y = 15.0 - (mario.localPosition.y + 40.0) / 64.0;
		}
		else if (map[(int)x + 2][(int)y].solid)
		{
			DebugPrintf("Right\n");
			mario.localPosition.x = map[(int)x + 2][(int)y].location.x - globalOffset - mario.sprite->WidthGet();

			x = (mario.localPosition.x + globalOffset) / 64.0;
			y = 15.0 - (mario.localPosition.y + 40.0) / 64.0;
		}

		// Hit block from below
		if (!mario.onGround && map[(int)x + 2][(int)y].solid)
		{
			DebugPrintf("Hit bottom\n");
			mario.localPosition.y = map[(int)x + 2][(int)y].location.y + 65;
			map[(int)x + 2][(int)y].Hit(mario.health);
			jumpTimer = 0;
			mario.localPosition.y += 1;

			x = (mario.localPosition.x + globalOffset) / 64.0;
			y = 15.0 - (mario.localPosition.y + 40.0) / 64.0;
		}
	}

	// Ceiling blocks
	if (!mario.onGround && (map[(int)x + 1][(int)y].solid || !map[(int)x + 1][(int)y].empty))
	{
		DebugPrintf("Hit bottom\n");
		mario.localPosition.y = map[(int)x + 1][(int)y].location.y + 65;
		map[(int)x + 1][(int)y].Hit(mario.health);
		jumpTimer = 0;
		mario.localPosition.y += 1;

		x = (mario.localPosition.x + globalOffset) / 64.0;
		y = 15.0 - (mario.localPosition.y + 40.0) / 64.0;
	}
#pragma endregion

#pragma region Horizontal Collisions

	// From right
	if ((map[(int)x+2][(int)y].solid || !map[(int)x + 2][(int)y].empty) && x - (int)x >= 0.25f)
	{
		DebugPrintf("Right\n");
		mario.localPosition.x = map[(int)x+2][(int)y].location.x - globalOffset - mario.sprite->WidthGet();

		x = (mario.localPosition.x + globalOffset) / 64.0;
		y = 15.0 - (mario.localPosition.y + 40.0) / 64.0;
	}

	// Overhang above
	if ((map[(int)x + 2][(int)y-1].solid || !map[(int)x + 2][(int)y - 1].empty)&& (int)x + 1 - x <= 0.75f)
	{
		DebugPrintf("Right\n");
		mario.localPosition.x = map[(int)x + 2][(int)y-1].location.x - globalOffset - mario.sprite->WidthGet();

		x = (mario.localPosition.x + globalOffset) / 64.0;
		y = 15.0 - (mario.localPosition.y + 40.0) / 64.0;
	}

	// From left
	if (map[(int)x+1][(int)y].solid || !map[(int)x + 1][(int)y].empty)
	{
		DebugPrintf("Left\n");
		mario.localPosition.x = map[(int)x+2][(int)y].location.x - globalOffset;

		x = (mario.localPosition.x + globalOffset) / 64.0;
		y = 15.0 - (mario.localPosition.y + 40.0) / 64.0;
	}

	// From left w/ overhang
	if (map[(int)x + 1][(int)y-1].solid || !map[(int)x + 1][(int)y - 1].empty)
	{
		DebugPrintf("Left\n");
		mario.localPosition.x = map[(int)x + 2][(int)y-1].location.x - globalOffset;

		x = (mario.localPosition.x + globalOffset) / 64.0;
		y = 15.0 - (mario.localPosition.y + 40.0) / 64.0;
	}
#pragma endregion

	x = (mario.localPosition.x + globalOffset) / 64.0;
	y = 15.0 - (mario.localPosition.y + 40.0) / 64.0;

	/* DebugPrintf(to_string(x).c_str());
	DebugPrintf(" ");
	DebugPrintf(to_string(y).c_str());
	DebugPrintf("\n"); */
}

//-----------------------------------------------------------------------------

void MainGame::Draw()
{
	background->Draw(Point2F::Create(0, 0));
	for (int i = 0; i < 400; i++)
	{
		for (int j = 0; j < 15; j++)
		{
			if (map[i][j].id != ' ')
				map[i][j].Draw(globalOffset);
		}
	}
	mario.Draw();
}

void MainGame::LoadLevel(float level)
{
	string fileName = "C:/";
	fileName += to_string(level);
	fileName += ".txt";
	ifstream levelFile;
	levelFile.open("Levels/1.1.txt");
	string line;
	int pallette = 0;
	background = theImages->Load("background_black");
	getline(levelFile, line);
	switch (line[0])
	{
	case 'O':
		pallette = 0;
		background = theImages->Load("background_blue");
		music = theSounds->Load("Music/overworld");
		break;
	case 'U':
		pallette = 1;
		break;
	case 'C':
		pallette = 2;
		break;
	}
	int index = 0;
	while (!levelFile.eof())
	{
		getline(levelFile, line);
		if (line[0] != '-')
		{
			for (int i = 0; i < 15; i++)
			{
				map[index][i] = Block::Create(pallette, line[i], Point2F::Create(index, i));
			}
		}
		index++;
	}
	if (index < 400)
		for (int i = index; i < 400; i++)
		{
			for (int j = 0; j < 15; j++)
			{
				map[i][j] = Block::Create(pallette, ' ', Point2F::Create(i, j));
			}
		}
}

// ============================================================

Block::Block()
{
	id = ' ';
}

Block Block::Create(int p, char c, Point2F P)
{
	Block b = Block();
	b.id = c;
	b.location = Point2F::Create(64 * (P.x - 1), theScreen->HeightGet() - (40 + 64 * P.y));
	b.empty = false;
	b.solid = true;
	b.powerSprite = theImages->Load("empty");
	b.effect = NULL;
	string pallette;
	switch (c)
	{
		// Empty block
		case ' ':
			b.empty = true;
			b.solid = false;
			b.sprite = theImages->Load("empty");
			break;

		// Ground block
		case 'G':
			switch (p)
			{
				case 0:
					b.sprite = theImages->Load("Overworld/Ground");
					break;

				case 1:
					b.sprite = theImages->Load("Underground/Ground");
					break;

				case 2:
					b.sprite = theImages->Load("Castle/Ground");
					break;
			}
			break;

		// ? Block with Powerup
		case '?':
			b.effect = theSounds->Load("SFX/powerfromblock");
			switch (p)
			{
			case 0:
				b.sprite = theImages->Load("Overworld/QBlock");
				break;

			case 1:
				b.sprite = theImages->Load("Underground/QBlock");
				break;

			case 2:
				b.sprite = theImages->Load("Castle/QBlock");
				break;
			}
			break;

		// ? Block with Coin
		case '/':
			b.effect = theSounds->Load("SFX/coin");
			switch (p)
			{
			case 0:
				b.sprite = theImages->Load("Overworld/QBlock");
				break;

			case 1:
				b.sprite = theImages->Load("Underground/QBlock");
				break;

			case 2:
				b.sprite = theImages->Load("Castle/QBlock");
				break;
			}
			break;

		// Castle
		case 'C':
			b.solid = false;
			switch (p)
			{
			case 0:
				b.sprite = theImages->Load("Overworld/Castle");
				break;

			case 1:
				b.sprite = theImages->Load("Underground/Castle");
				break;

			case 2:
				b.sprite = theImages->Load("Castle/Castle");
				break;
			}
			break;

		// Coin
		case '0':
			b.solid = false;
			switch (p)
			{
			case 0:
				b.sprite = theImages->Load("Overworld/Coin");
				break;

			case 1:
				b.sprite = theImages->Load("Underground/Coin");
				break;

			case 2:
				b.sprite = theImages->Load("Castle/Coin");
				break;
			}
			break;

		// Brick
		case 'B':
			switch (p)
			{
			case 0:
				b.sprite = theImages->Load("Overworld/Brick");
				break;

			case 1:
				b.sprite = theImages->Load("Underground/Brick");
				break;

			case 2:
				b.sprite = theImages->Load("Castle/Brick");
				break;
			}
			break;

		// Block
		case 'L':
			switch (p)
			{
			case 0:
				b.sprite = theImages->Load("Overworld/Block");
				break;

			case 1:
				b.sprite = theImages->Load("Underground/Block");
				break;

			case 2:
				b.sprite = theImages->Load("Castle/Block");
				break;
			}
			break;

		// Flagpole
		case 'F':
			b.solid = false;
			b.sprite = theImages->Load("Flagpole");
			break;

		// Pipe Top Left
		case '1':
			switch (p)
			{
			case 0:
				b.sprite = theImages->Load("Overworld/Pipe1");
				break;

			case 1:
				b.sprite = theImages->Load("Underworld/Pipe1");
				break;

			case 2:
				b.sprite = theImages->Load("Castle/Pipe1");
				break;
			}
			break;

		// Pipe Top Right
		case '2':
			switch (p)
			{
			case 0:
				b.sprite = theImages->Load("Overworld/Pipe2");
				break;

			case 1:
				b.sprite = theImages->Load("Underworld/Pipe2");
				break;

			case 2:
				b.sprite = theImages->Load("Castle/Pipe2");
				break;
			}
			break;

		// Pipe Bottom Left
		case '3':
			switch (p)
			{
			case 0:
				b.sprite = theImages->Load("Overworld/Pipe3");
				break;

			case 1:
				b.sprite = theImages->Load("Underworld/Pipe3");
				break;

			case 2:
				b.sprite = theImages->Load("Castle/Pipe3");
				break;
			}
			break;

		// Pipe Bottom Left
		case '4':
			switch (p)
			{
			case 0:
				b.sprite = theImages->Load("Overworld/Pipe4");
				break;

			case 1:
				b.sprite = theImages->Load("Underworld/Pipe4");
				break;

			case 2:
				b.sprite = theImages->Load("Castle/Pipe4");
				break;
			}
			break;

		// Small Cloud
		case 'c':
			b.solid = false;
			b.empty = true;
			b.sprite = theImages->Load("Overworld/ShortCloud");
			break;

		// Short Bush
		case 'b':
			b.solid = false;
			b.empty = true;
			b.sprite = theImages->Load("Overworld/ShortBush");
			break;

		// Small Hill
		case 'h':
			b.solid = false;
			b.empty = true;
			b.sprite = theImages->Load("Overworld/SmallHill");
			break;

		// Long Cloud
		case 'd':
			b.solid = false;
			b.empty = true;
			b.sprite = theImages->Load("Overworld/LongCloud");
			break;

		// Long Bush
		case 'a':
			b.solid = false;
			b.empty = true;
			b.sprite = theImages->Load("Overworld/LongBush");
			break;

		// Large Hill
		case 'i':
			b.solid = false;
			b.empty = true;
			b.sprite = theImages->Load("Overworld/LargeHill");
			break;

		// Invisible 1up Block
		case 'I':
			b.effect = theSounds->Load("SFX/powerfromblock");
			b.solid = false;
			b.sprite = theImages->Load("empty");
			break;
	}
	return b;
}

void Block::Draw(int offset)
{
	Point2F drawLocation = location - Point2F::Create(offset, 0);
	if(id != ' ')
		sprite->Draw(drawLocation);
}

void Block::Hit(int health)
{
	switch (id)
	{
		case '?':
			effect->Play(0, false);
			effect = theSounds->Load("SFX/bump");
			sprite = theImages->Load("Overworld/UsedQBlock");
			break;

		case '/':
			effect = theSounds->Load("SFX/coin");
			effect->Play(0, false);
			effect = theSounds->Load("SFX/bump");
			sprite = theImages->Load("Overworld/UsedQBlock");
			break;

		case 'B':
			if (health > 1)
			{
				effect = theSounds->Load("SFX/break");
				effect->Play(0, false);
				solid = false;
				empty = true;
				sprite = theImages->Load("Empty");
			}
			else
			{
				effect = theSounds->Load("SFX/bump");
				effect->Play(0, false);
			}
			break;

		case 'I':
			effect->Play(0, false);
			empty = false;
			solid = true;
			sprite = theImages->Load("Overworld/UsedQBlock");
			break;

		default:
			break;
	}
}

void Mario::Init()
{
	localPosition = Point2F::Create(401, 728);
	sprite = theImages->Load("Mario/MarioR");
	facingRight = true;
	health = 1;
}

void Mario::Deinit()
{
	if (sprite)
	{
		theImages->Unload(sprite);
	}
}

void Mario::Draw()
{
	sprite->Draw(localPosition);
}