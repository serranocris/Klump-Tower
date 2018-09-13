#include "Klump.h"


namespace Webfoot{
	Klump::Klump(){ // This is the best method in the entire game
		sprite = NULL;
	}

	void Klump::Init(){
		// 1024
		
		sprite = frog_new Sprite();
		sprite->VisibleSet(true);
		sprite->Init("Sprites/Sprite", "KlumpIntro"); //file name/json file, sprite name
		sprite->ScaleSet(Point2F::Create(0.45f, 0.45f));
		sprite->AnimationEnqueue("KlumpMovementHair");

		randKlump = FrogMath::Random() % 100;
		randKlump += 100;
		//DebugPrintf("Position to throw: %i", randKlump);

		position = Point2F::Create(theScreen->WidthGet() / 2.0f, theScreen->HeightGet() / 2.0f);
		topPadding = 25;
		position.y = topPadding;
		bounces = 0;
		velocity.x = SPEED;
		velocity.y = 250.0f;
		bossFightSwoop = 400.0f;
		hits = 0;
	}

	void Klump::Deinit(){
		if (sprite){
			sprite->Deinit();
			frog_delete sprite;
			sprite = NULL;
		}
	}

	void Klump::Draw(){

		/*
		mysprite->Draw();
		*/

		sprite->Draw();
		
		/*theScreen->LineDraw(Point2F::Create(position.x, position.y), Point2F::Create(position.x + DEBUG_SIZE, position.y), COLOR_RGBA8_GREEN);
		theScreen->LineDraw(Point2F::Create(position.x + DEBUG_SIZE, position.y), Point2F::Create(position.x + DEBUG_SIZE, position.y + DEBUG_SIZE), COLOR_RGBA8_GREEN);
		theScreen->LineDraw(Point2F::Create(position.x + DEBUG_SIZE, position.y + DEBUG_SIZE), Point2F::Create(position.x, position.y + DEBUG_SIZE), COLOR_RGBA8_GREEN);
		theScreen->LineDraw(Point2F::Create(position.x, position.y + DEBUG_SIZE), Point2F::Create(position.x, position.y), COLOR_RGBA8_GREEN);
		*/
	}

	void Klump::Update(unsigned int dt, boolean bossFight){
		float dtSeconds = (float)dt / 1000.0f;
		position.x += velocity.x * dtSeconds;

		if (position.x > ((float)theScreen->WidthGet()) - DEBUG_SIZE){
			position.x = (float)theScreen->WidthGet() - DEBUG_SIZE;
			TurnAround();
		}
		else if (position.x < 0){
			position.x = 0;
			TurnAround();
		}

		if (bossFight){
			position.y += velocity.y * dtSeconds;
			if (position.y >= bossFightSwoop && hits < 3){
				position.y = bossFightSwoop;
				velocity.y *= -1;
			}
			if (position.y <= topPadding){
				position.y = topPadding;
				velocity.y *= -1;
			}
		}
		sprite->Update(dt);
		sprite->PositionSet(position);
	}

	void Klump::TurnAround(){
		
		velocity.x *= -1.0f;

		AddBounceCount();

		if (bounces > maxBounces){
			//DebugPrintf("bounces are now > maxBounces\n");
			positionThrow = true;
		}

		if (positionThrow == true){ 
			
			if (position.x > randKlump)
			{

				ThrowHazard();
				toss = true;
				//DebugPrintf("toss is true\n");
				positionThrow = false;
			}
		}
		/*
		DebugPrintf("TurnAround() complete\n");
		DebugPrintf("Bounces = %i\n", BounceCountGet());
		*/
	}

	int Klump::AddBounceCount(){
		bounces++;
		return bounces;
	}

	int Klump::BounceCountGet(){
		return bounces;
	}

	void Klump::ThrowHazard(){
		
		/*
		DebugPrintf("ThrowHazard attempt, randKlump = %i \n", randKlump);
		DebugPrintf("klump position.x = %i\n", position.x);
		*/
		/*
		if (position.x == 100 + randKlump)
		{
			toss = true;
			DebugPrintf("toss is true\n");
		}
		*/
		
		ResetBounces();
	}

	void Klump::ResetBounces(){
		//DebugPrintf("Bounces reset\n");
		toss = false;
		bounces = 0;
	}

	int Klump::GetX(){
		return position.x;
	}

	int Klump::GetY(){
		return position.y;
	}

	void Klump::TakeDamage(int score){
		hits++;
		//sprite->AnimationClear();
		if (hits == 1){
			sprite->AnimationSet("KlumpGetHitOnce");
			sprite->AnimationEnqueue("KlumpMovementHair");
		}
		else if (hits == 2){
			sprite->AnimationSet("KlumpGetHit");
			sprite->AnimationEnqueue("KlumpMovement");
		}
		else{
			sprite->AnimationSet("KlumpFinalHit");
			velocity.x = 0;
			velocity.y = 100;
			DebugPrintf("You Win!\n");
			DebugPrintf("Score: %d\n", score);
		}
	}

	int Klump::GetSize(){
		return DEBUG_SIZE;
	}
}

