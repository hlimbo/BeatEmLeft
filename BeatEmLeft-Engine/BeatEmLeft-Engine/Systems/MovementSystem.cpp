#include "MovementSystem.h"
#include "ECS.h"
#include "EntitySystem.h"
#include "../Components/Transform.h"
#include "../Components/Kinematic.h"
#include "../Input/KeyboardController.h"

#include "../Utility/CollisionQuery.h"

#include <vector>

using namespace std;

//use this function to check if a coordinate is between minCoordinate and maxCoordinate
bool isOnLineSegment_in(float coordinate, float minCoordinate, float maxCoordinate)
{
	return coordinate > minCoordinate && coordinate < maxCoordinate;
}

bool isOverlapping_in(const BoxCollider& b1, const BoxCollider& b2)
{
	return b1.position.x < b2.position.x + b2.width &&
		b1.position.x + b1.width > b2.position.x &&
		b1.position.y < b2.position.y + b2.height &&
		b1.position.y + b1.height > b2.position.y;
}

bool isOverlappingX_in(const BoxCollider& b1, const BoxCollider& b2)
{
	return b1.position.x < b2.position.x + b2.width &&
		b1.position.x + b1.width > b2.position.x;
}

bool isOverlappingY_in(const BoxCollider& b1, const BoxCollider& b2)
{
	return	b1.position.y < b2.position.y + b2.height &&
		b1.position.y + b1.height > b2.position.y;
}

//Note: speed != velocity (velocity = speed * direction)
//custom functionality of how stuff should accelerate
float accelerate_in(float currentSpeed, float maxSpeed, float deltaTime, float deltaSpeed)
{
	//note: currentSpeed * deltaSpeed * deltaTime is the acceleration
	//acceleration.x = currentSpeed * deltaSpeed * deltaTime * direction.x;
	//acceleration.y = currentSpeed * deltaSpeed * deltaTime * direction.y;
	return currentSpeed > maxSpeed ? maxSpeed : currentSpeed + (currentSpeed * deltaSpeed * deltaTime);
}

//temp function
//Vect2 GetAcceleration(const Kinematic* kinematic,float deltaTime)
//{
//	return Vect2(kinematic->direction.x, kinematic->direction.y) * (kinematic->currentSpeed * kinematic->accelFactor * deltaTime);
//}

MovementSystem::MovementSystem(ECS* ecs)
{
	this->ecs = ecs;
	transforms = &(ecs->transforms);
	kinematics = &(ecs->kinematics);
}

void MovementSystem::Init()
{
	//temp code
	int playerID = ecs->entitySystem.GetIDs("Player").at(0);
	keyboard = ecs->GetKeyboard(playerID);
}


//something to think about... should kinematic movement decide
//which spritesheet animations to play or do button presses decide
//which spritesheet animations to play

//use this to play around with speed,velocity,acceleration,and direction based
//on the user input states.
void MovementSystem::UpdateKinematics(float deltaTime)
{
	//temp code
	//int playerID = ecs->entitySystem.GetIDs("Player").at(0);
	//keyboard = ecs->GetKeyboard(playerID);
	//Kinematic* kinematic = ecs->kinematics.GetComponent(playerID);
	//Transform* transform = ecs->transforms.GetComponent(playerID);

	//this loop is what allow flexibility of the system
	//that is whenever I register keyboard control to another entity
	//the amount of effort to do so is minimal!
	vector<int> entityIDs = ecs->entitySystem.GetIDs();
	for (vector<int>::iterator it = entityIDs.begin();it != entityIDs.end();++it)
	{
		keyboard = ecs->GetKeyboard(*it);
		Kinematic* kinematic = ecs->kinematics.GetComponent(*it);
		Transform* transform = ecs->transforms.GetComponent(*it);

		if (keyboard == nullptr || kinematic == nullptr || transform == nullptr)
			continue;

		//TOP DOWN movement controller
		//if (keyboard != nullptr && transform != nullptr && kinematic != nullptr)
		//{
		//	if (keyboard->KeyReleased("left") &&
		//		keyboard->KeyReleased("right") &&
		//		keyboard->KeyReleased("up") &&
		//		keyboard->KeyReleased("down"))
		//	{
		//		kinematic->direction = Vect2(0.0f, 0.0f);
		//		kinematic->currentSpeed = kinematic->minSpeed;
		//	}
		//	else if (keyboard->KeyHeld("left") && keyboard->KeyHeld("right"))
		//	{
		//		kinematic->direction.x = 0.0f;
		//		kinematic->currentSpeed = kinematic->minSpeed;
		//	}
		//	else
		//	{
		//		float ds = kinematic->accelFactor;
		//		if (keyboard->KeyPressed("left"))
		//		{
		//			kinematic->direction.x = -1.0f;
		//			kinematic->currentSpeed = accelerate_in(kinematic->currentSpeed, kinematic->maxSpeed, deltaTime, ds);
		//			//kinematic->velocity.x = -kinematic->currentSpeed * deltaTime;
		//		}
		//		else if (keyboard->KeyHeld("left"))
		//		{
		//			kinematic->direction.x = -1.0f;
		//			kinematic->currentSpeed = accelerate_in(kinematic->currentSpeed, kinematic->maxSpeed, deltaTime, ds);
		//			//kinematic->velocity.x = -kinematic->currentSpeed * deltaTime;
		//		}

		//		if (keyboard->KeyPressed("right"))
		//		{
		//			kinematic->direction.x = 1.0f;
		//			kinematic->currentSpeed = accelerate_in(kinematic->currentSpeed, kinematic->maxSpeed, deltaTime, ds);
		//			//kinematic->velocity.x = kinematic->currentSpeed * deltaTime;
		//		}
		//		else if (keyboard->KeyHeld("right"))
		//		{
		//			kinematic->direction.x = 1.0f;
		//			kinematic->currentSpeed = accelerate_in(kinematic->currentSpeed, kinematic->maxSpeed, deltaTime, ds);
		//			//kinematic->velocity.x = kinematic->currentSpeed * deltaTime;
		//		}

		//		if (keyboard->KeyPressed("up"))
		//		{
		//			kinematic->direction.y = -1.0f;
		//			kinematic->currentSpeed = accelerate_in(kinematic->currentSpeed, kinematic->maxSpeed, deltaTime, ds);
		//			//kinematic->velocity.y = -kinematic->currentSpeed * deltaTime;
		//		}
		//		else if (keyboard->KeyHeld("up"))
		//		{
		//			kinematic->direction.y = -1.0f;
		//			kinematic->currentSpeed = accelerate_in(kinematic->currentSpeed, kinematic->maxSpeed, deltaTime, ds);
		//			//kinematic->velocity.y = -kinematic->currentSpeed * deltaTime;
		//		}

		//		if (keyboard->KeyPressed("down"))
		//		{
		//			kinematic->direction.y = 1.0f;
		//			kinematic->currentSpeed = accelerate_in(kinematic->currentSpeed, kinematic->maxSpeed, deltaTime, ds);
		//			//kinematic->velocity.y = kinematic->currentSpeed * deltaTime;
		//		}
		//		else if (keyboard->KeyHeld("down"))
		//		{
		//			kinematic->direction.y = 1.0f;
		//			kinematic->currentSpeed = accelerate_in(kinematic->currentSpeed, kinematic->maxSpeed, deltaTime, ds);
		//			//kinematic->velocity.y = kinematic->currentSpeed * deltaTime;
		//		}

		//		//temp code handling diagonals (only applies to keyboard)
		//		//up-left
		//		if (kinematic->direction.x == -1.0f && kinematic->direction.y == -1.0f)
		//		{
		//			kinematic->direction = Vect2(cosf(5 * (float)M_PI / 4), sinf(-(float)M_PI / 4));
		//		}
		//		//up-right
		//		else if (kinematic->direction.x == 1.0f && kinematic->direction.y == -1.0f)
		//		{
		//			kinematic->direction = Vect2(cosf((float)M_PI / 4), sinf(-(float)M_PI / 4));
		//		}
		//		//down-left
		//		else if (kinematic->direction.x == -1.0f && kinematic->direction.y == 1.0f)
		//		{
		//			kinematic->direction = Vect2(cosf(5 * (float)M_PI / 4), sinf((float)M_PI / 4));
		//		}
		//		//down-right
		//		else if (kinematic->direction.x == 1.0f && kinematic->direction.y == 1.0f)
		//		{
		//			kinematic->direction = Vect2(cosf((float)M_PI / 4), sinf((float)M_PI / 4));
		//		}

		//	}

		//	kinematic->velocity = Vect2(kinematic->direction.x, kinematic->direction.y) * kinematic->currentSpeed * deltaTime;
		//}


		//Platformer movement controller
		if (keyboard->KeyPressed("left"))
		{
			kinematic->direction.x = -1.0f;
			kinematic->currentSpeed = kinematic->minSpeed;
		}

		if (keyboard->KeyHeld("left"))
		{
			float newSpeed = kinematic->currentSpeed + GetDeltaSpeed(kinematic) * deltaTime;
			kinematic->currentSpeed = (newSpeed > kinematic->maxSpeed) ? kinematic->maxSpeed : newSpeed;
		}

		if (keyboard->KeyPressed("right"))
		{
			kinematic->direction.x = 1.0f;
			kinematic->currentSpeed = kinematic->minSpeed;
		}

		if (keyboard->KeyHeld("right"))
		{
			float newSpeed = kinematic->currentSpeed + GetDeltaSpeed(kinematic) * deltaTime;
			kinematic->currentSpeed = (newSpeed > kinematic->maxSpeed) ? kinematic->maxSpeed : newSpeed;
		}

		if (keyboard->KeyReleased("left") && keyboard->KeyReleased("right"))
		{
			kinematic->direction.x = 0.0f;
			kinematic->currentSpeed = kinematic->minSpeed;
		}

		//if both left and right keys are held and/or pressed don't move
		if (keyboard->KeyHeld("left") && keyboard->KeyHeld("right"))
		{
			kinematic->direction.x = 0.0f;
		}

		//jumping
		if (keyboard->KeyPressed("space"))
		{
		//	puts("press");
			kinematic->currentJumpTime = 0.0f;		
		}
		else if (keyboard->KeyHeld("space") && kinematic->currentJumpTime < kinematic->maxJumpTime)
		{
			//puts("held");
			//printf("kinematic jump time: %f\n", kinematic->currentJumpTime);
			kinematic->currentJumpTime += deltaTime;
			kinematic->direction.y = -1.0f;
			kinematic->jumpSpeed += kinematic->jumpSpeed * kinematic->jumpFactor * deltaTime;
			if (kinematic->jumpSpeed > kinematic->maxJumpSpeed)
				kinematic->jumpSpeed = kinematic->maxJumpSpeed;
		}
		
		if (keyboard->KeyReleased("space"))
		{
			//printf("current jumpTime: %f\n", kinematic->currentJumpTime);
			//puts("released");
			kinematic->jumpSpeed = kinematic->minJumpSpeed;
			kinematic->direction.y = 0.0f;
		}

		if (kinematic->currentJumpTime >= kinematic->maxJumpTime)
		{
		//	printf("current jumpTime max: %f\n", kinematic->currentJumpTime);
			//puts("released");
			kinematic->jumpSpeed = kinematic->minJumpSpeed;
			kinematic->direction.y = 0.0f;
			kinematic->currentJumpTime = kinematic->maxJumpTime;
		}

		kinematic->gravity += kinematic->gravity * kinematic->gravityFactor * deltaTime;
		if (kinematic->gravity > kinematic->maxGravity)
			kinematic->gravity = kinematic->maxGravity;

		//printf("gravity: %f\n", kinematic->gravity);

		//if (kinematic->direction.y == 0.0f && kinematic->jumpSpeed * kinematic->direction.y * deltaTime != 0.0f)
		//{
		//	printf("this result: %f,%f,%f\n", kinematic->jumpSpeed, kinematic->direction.y, deltaTime);
		//}

		kinematic->velocity.y = (kinematic->gravity * deltaTime) + (kinematic->jumpSpeed * kinematic->direction.y * deltaTime);
		kinematic->velocity.x = kinematic->direction.x * kinematic->currentSpeed * deltaTime;
	}

}

//call after update positions
void MovementSystem::CheckForCircleCollisions(float deltaTimeInMS)
{
	//temp code
	int playerID = ecs->entitySystem.GetIDs("Player").at(0);
	Transform* pt = transforms->GetComponent(playerID);
	CircleCollider* pc = ecs->circleColliders.GetComponent(playerID);
	Kinematic* pk = kinematics->GetComponent(playerID);

	if (pt == nullptr || pc == nullptr || pk == nullptr)
		return;

	Vect2 newP(pt->position);

	vector<int> tileIDs = ecs->entitySystem.GetIDs("Tile");
	for (vector<int>::iterator it = tileIDs.begin();it != tileIDs.end();++it)
	{
		Transform* tile = transforms->GetComponent(*it);
		CircleCollider* circle = ecs->circleColliders.GetComponent(*it);

		if (tile == nullptr || circle == nullptr)
			continue;

		float distSqr = VectMath::DistSqr(newP, tile->position);
		float sumRadii = pc->radius + circle->radius;
		float sumRadiiSqr = sumRadii * sumRadii;
		if (distSqr < sumRadiiSqr)
		{
			float dist = VectMath::Dist(newP, tile->position);

			float pen = dist - sumRadii;
			Vect2 normal = VectMath::Normalize(newP, tile->position);
			Vect2 penVector = normal * pen;
			//printf("pen vector: (%f,%f)\n", penVector.x, penVector.y);
			newP = newP + penVector;
			pt->position = newP;
		}
	}

}

//tries to predict the right amount of velocity required to not overlap with a tile
void MovementSystem::CheckForCollisions(float deltaTimeInMS)
{
	//temp code
	int playerID = ecs->entitySystem.GetIDs("Player").at(0);
	Transform* pt = transforms->GetComponent(playerID);
	BoxCollider* pb = ecs->boxColliders.GetComponent(playerID);
	Kinematic* pk = kinematics->GetComponent(playerID);

	if (pt == nullptr || pb == nullptr || pk == nullptr)
		return;
		//temp
		//Vect2 tempOffset(7.0f, 1.0f);

		Vect2 newP(pt->position + pk->velocity);
		Vect2 oldP(pt->position);
		Vect2 deltaP(newP - oldP);

		vector<int> tileIDs = ecs->entitySystem.GetIDs("Tile");
		for (vector<int>::iterator it = tileIDs.begin();it != tileIDs.end();++it)
		{
			Transform* tile = transforms->GetComponent(*it);
			BoxCollider* box = ecs->boxColliders.GetComponent(*it);

			//skip this tile if it does not have these components
			if (tile == nullptr || box == nullptr)
				continue;

			//temp
			//BoxCollider playerBox;
			//playerBox.position = pt->position + pk->velocity;
			//playerBox.width = pb->width;
			//playerBox.height = pb->height;

			//CollisionQuery::GetOverlappedBox(playerBox, *box);


			float timeX = 0.0f;
			float timeY = 0.0f;

			//check left or right side of tile
			if (CollisionQuery::IsOnLineSegment(oldP.y, tile->position.y, tile->position.y + box->height)
				|| CollisionQuery::IsOnLineSegment(oldP.y + pb->height, tile->position.y, tile->position.y + box->height))
			{
				//player moving to the right
				if (deltaP.x > 0.0f)
				{
					timeX = CollisionQuery::GetContactTime(oldP.x + pb->width, newP.x + pb->width, tile->position.x);
				//	printf("timeX: %f\n", timeX);

					//predicting how much delta velocity is needed this frame
					//in order to not overlap against a non-moving entity with a box collider attached
					if (timeX >= 0 && timeX <= deltaTimeInMS)
					{
						float adjustedVelX = deltaP.x * timeX;
						float contactX = (oldP.x + pb->width) + adjustedVelX;
						float cy = oldP.y + (deltaP.y * timeX);
						if (newP.x + pb->width > contactX)
						{
							//printf("contact pt: (%f, %f)\n", contactX, cy);
							pk->velocity.x = adjustedVelX;
						}
			
					}
				}
				else if (deltaP.x < 0.0f) //player moving to the left
				{
					//right side tile
					timeX = CollisionQuery::GetContactTime(oldP.x, newP.x, tile->position.x + box->width);
					//printf("timeX2: %f\n", timeX);

					if (timeX >= 0 && timeX <= deltaTimeInMS)
					{
						float contactX2 = oldP.x + (deltaP.x * timeX);

						if (newP.x < contactX2)
						{
							float adjustedVelX = deltaP.x * timeX;
							pk->velocity.x = adjustedVelX;
						}
					}
				}
			}

			//check top or bottom side of tile
			if (CollisionQuery::IsOnLineSegment(oldP.x, tile->position.x, tile->position.x + box->width)
				|| CollisionQuery::IsOnLineSegment(oldP.x + pb->width, tile->position.x, tile->position.x + box->width))
			{
				//check top side of tile
				//if player is moving downwards
				if (deltaP.y > 0.0f)
				{
					timeY = CollisionQuery::GetContactTime(oldP.y + pb->height, newP.y + pb->height, tile->position.y);

					if (timeY >= 0 && timeY <= deltaTimeInMS)
					{
						//Note: deltaP.y * timeY is how much the player would need to move to touch the tile.
						float contactY = (oldP.y + pb->height) + (deltaP.y * timeY);
						if (newP.y + pb->height > contactY)
						{
						//	printf("timeY: %f\n", timeY);
							//change the velocity instead of correcting the position
							float adjustedVelY = deltaP.y * timeY;
							pk->velocity.y = adjustedVelY;
								
							//not sure if gravity should be changed here...
							pk->gravity = pk->minGravity;
						}

					}

				}
				//check bottom side of tile
				else if (deltaP.y < 0.0f)
				{
					timeY = CollisionQuery::GetContactTime(oldP.y, newP.y, tile->position.y + box->height);

					if (timeY >= 0 && timeY <= deltaTimeInMS)
					{
						float contactY2 = oldP.y + (deltaP.y * timeY);
						if (newP.y < contactY2)
						{
						//	printf("timeY2: %f\n", timeY);
							float adjustedVelY = deltaP.y * timeY;
							pk->velocity.y = adjustedVelY;
						}
					}
				}
			}

		}
}

//will try to find any collisions that resulted in an overlap
//if there exists any collision overlaps, correct the entity's position
//Usage: Call this function after UpdatePositions has been called
void MovementSystem::CorrectCollisionOverlaps(float deltaTimeInMS)
{
	//temp code
	int playerID = ecs->entitySystem.GetIDs("Player").at(0);
	Transform* pt = transforms->GetComponent(playerID);
	BoxCollider* pb = ecs->boxColliders.GetComponent(playerID);
	Kinematic* pk = kinematics->GetComponent(playerID);

	if (pt == nullptr || pb == nullptr || pk == nullptr)
		return;

	/*
		The difference between this function and CheckForCollisions
		is that the player entity does not need to be moving to
		correct the overlap collision
	*/

	Vect2 newP(pt->position);

	vector<int> tileIDs = ecs->entitySystem.GetIDs("Tile");
	for (vector<int>::iterator it = tileIDs.begin();it != tileIDs.end();++it)
	{
		Transform* tile = transforms->GetComponent(*it);
		BoxCollider* box = ecs->boxColliders.GetComponent(*it);
		if (tile == nullptr || box == nullptr)
			continue;

		//diagonals - only needed if the game is a top-down shooter or rpg where you are allowed to moved diagonally
		//if (isOnLineSegment_in(newP.y, tile->position.y, tile->position.y + box->height))
		//{
		//	//top left corner of player 
		//	if (isOnLineSegment_in(newP.x, tile->position.x, tile->position.x + box->width))
		//	{
		//		//puts("inside box top left");
		//		float diffY = tile->position.y + box->height - newP.y;
		//		float diffX = tile->position.x + box->width - newP.x;
		//		newP.y += diffY;
		//		newP.x += diffX;

		//		pt->position = newP;
		//	}

		//	//top right corner of player
		//	else if (isOnLineSegment_in(newP.x + pb->width, tile->position.x, tile->position.x + box->width))
		//	{
		//		puts("inside box top right");
		//		float diffY = tile->position.y + box->height - newP.y;
		//		float diffX = newP.x + pb->width - tile->position.x;
		//		
		//		newP.y += diffY;
		//		newP.x -= diffX;

		//		pt->position = newP;
		//	}
		//}
		//else if (isOnLineSegment_in(newP.y + pb->height, tile->position.y, tile->position.y + box->height))
		//{
		//	//bottom left of player
		//	if (isOnLineSegment_in(newP.x, tile->position.x, tile->position.x + box->width))
		//	{
		//		puts("inside box bottom left");
		//		float diffY = newP.y + pb->height - tile->position.y;
		//		float diffX = tile->position.x + box->width - newP.x;
		//		newP.y -= diffY;
		//		newP.x += diffX;

		//		pt->position = newP;
		//	}
		//	//bottom right of player
		//	else if (isOnLineSegment_in(newP.x + pb->width, tile->position.x, tile->position.x + box->width))
		//	{
		//		puts("inside box bottom right");
		//		float diffY = newP.y + pb->height - tile->position.y;
		//		float diffX = newP.x + pb->width - tile->position.x;
		//		newP.y -= diffY;
		//		newP.x -= diffX;

		//		pt->position = newP;
		//	}
		//	
		//}

		//temporary checks
		//if (pt->position.y + pb->height == box->position.y)
		//{
		//	puts("matching!!");
		//	pt->position.y -= 1.0f;
		//}

		//this check ensures if boxes that are of the same size as the tiles or bigger
		//properly get their positions corrected after overlap
		if (CollisionQuery::IsOverlapping(*pb, *box))
		{
			//Vect2 normal = VectMath::Normalize(pk->velocity);
			Vect2 oldPos(pt->position - pk->velocity);
			Vect2 pen = pt->position - tile->position;

			Vect2 deltaPos(pt->position - oldPos);

			//this fixes the problem of boxcolliders of equal or greater size to be able to pass through
			if (deltaPos.x > 0.0f)
			{
				pt->position.x += (pen.x + pb->width) * -1.0f;
				//printf("contact2 (%f, %f)\n", pt->position.x, pt->position.y);
				pk->velocity.x = 0.0f;
			}
			else if (deltaPos.x < 0.0f)
			{
				pen.x = box->position.x + box->width - pt->position.x;
				pt->position.x += pen.x;
				pk->velocity.x = 0.0f;
			}

			//this causes the player to teleport away
			//if (deltaPos.y > 0.0f)
			//{
			//	printf("cy: %f\n", pt->position.y + pen.y);
			//	//pt->position.y += (pen.y + pb->height) * -1.0f;
			//}
			//else if (deltaPos.y < 0.0f)
			//{
			//	//pen.y = pt->position.y - (box->position.y + box->width);
			//	//pt->position.y -= (pen.y) * 1.0f;
			//}
		}


		//make sure player boxcollider position is in sync with player transform position
		pb->position = pt->position;
	}
}

void MovementSystem::CheckForSlopes(float deltaTimeInMS)
{
	//temp
	int playerID = ecs->entitySystem.GetIDs("Player").at(0);
	Transform* playerTransform = transforms->GetComponent(playerID);
	BoxCollider* playerBox = ecs->boxColliders.GetComponent(playerID);
	Kinematic* playerKinematic = ecs->kinematics.GetComponent(playerID);

	if (playerTransform == nullptr || playerBox == nullptr || playerKinematic == nullptr)
		return;

	//use bottom right corner of collider
	Vect2 oldP(playerTransform->position.x + playerBox->width,playerTransform->position.y + playerBox->height);
	Vect2 newP(oldP + playerKinematic->velocity);
	Vect2 deltaP(newP - oldP);

	//rising slopes
	vector<int> entityIDs = ecs->entitySystem.GetIDs("Slope");
	for (vector<int>::iterator it = entityIDs.begin();it != entityIDs.end();++it)
	{

		Transform* transform = transforms->GetComponent(*it);
		SlopeCollider* slope = ecs->slopes.GetComponent(*it);

		if (transform == nullptr || slope == nullptr)
			continue;

		//commenting this out means the slopes are sticky (no jumping)
	/*	if (!CollisionQuery::IsOnLineSegmentInclusive(newP.y, transform->position.y, transform->position.y + slope->height))
			continue;*/

		//calculate slope - box collider collisions
		if (deltaP.x > 0) //climb up
		{
			//check if the old position this frame is on the slope tile
			if (CollisionQuery::IsOnLineSegmentInclusive(oldP.x, transform->position.x, transform->position.x + slope->width))
			{
				//calculate position x on slope
				//c.x = t.x + t.w * k
				//k = (p.y - t.y ) / t.h 

				float k = (oldP.y - transform->position.y) / slope->height;
				//printf("k: %f\n", k);
				float cx = transform->position.x + (slope->width * (1- k));
				if (newP.x > cx && CollisionQuery::IsOnLineSegmentInclusive(newP.y,transform->position.y,transform->position.y + slope->height))
				{
					//push y position upwards (calculate new y position on slope)
					float k2 = (newP.x - transform->position.x) / slope->width;
					float newY = transform->position.y + (slope->height * (1- k2));
					playerTransform->position.y = newY - playerBox->height;
					playerKinematic->velocity.y = 0.0f;
					//temporarily turn off gravity here
			/*		playerKinematic->gravity = 0.0f;
					playerKinematic->gravityFactor = 0.0f;
					playerKinematic->maxGravity = 0.0f;
					playerKinematic->minGravity = 0.0f;*/

				//	printf("k2: %f\n", k2);
				//	printf("newY: %f\n", newY);
				}
				else
				{
				//	printf("newP.x: %f\n", newP.x);
				//	printf("cx: %f\n", cx);
				}
			}
		}
		else if (deltaP.x < 0) // climb down
		{
			//check if the old position this frame is on the slope tile
			if (CollisionQuery::IsOnLineSegmentInclusive(oldP.x - playerBox->width, transform->position.x, transform->position.x + slope->width))	
			{
				//calculate position x on slope
				//c.x = t.x + t.w * k
				//k = (p.y - t.y ) / t.h

				float k = (oldP.y - transform->position.y) / slope->height;
				//printf("k: %f\n", k);
				float cx = transform->position.x + (slope->width * (1 - k));
				if (newP.x < cx && CollisionQuery::IsOnLineSegmentInclusive(newP.y, transform->position.y, transform->position.y + slope->height))
				{

					//push y position downwards (calculate new y position on slope)
					float k2 = (newP.x - transform->position.x) / slope->width;
					float newY = transform->position.y + (slope->height * (1 - k2));
					playerTransform->position.y = newY - playerBox->height;
					playerKinematic->velocity.y = 0.0f;

				}
			}
		}  
		else if (deltaP.y > 0 && deltaP.x == 0)
		{
			//maintain position on the slope
			
			if (CollisionQuery::IsOnLineSegmentInclusive(oldP.x, transform->position.x, transform->position.x + slope->width))
			{
				//calculate cy (assumption transform->position.x < oldP.x < transform->position.x + slope->width)
				float k = (oldP.x - transform->position.x) / slope->width;
				float cy = transform->position.y + (slope->height * (1 - k));
				if (newP.y > cy)
				{
					playerKinematic->velocity.y = cy - oldP.y;
					//float peny = newP.y - cy;
					//playerTransform->position.y -= peny;
				}
			}

		}

	}

	Vect2 oldP2(playerTransform->position.x,playerTransform->position.y + playerBox->height);
	Vect2 newP2(oldP2 + playerKinematic->velocity);
	Vect2 deltaP2(newP2 - oldP2);

	//descending slopes
	vector<int> entityIDs2 = ecs->entitySystem.GetIDs("SlopeRev");
	for (vector<int>::iterator it = entityIDs2.begin();it != entityIDs2.end();++it)
	{
		Transform* transform = transforms->GetComponent(*it);
		SlopeCollider* slope = ecs->slopes.GetComponent(*it);

		if (transform == nullptr || slope == nullptr)
			continue;

		if (deltaP2.x < 0) //go up
		{
			if (CollisionQuery::IsOnLineSegmentInclusive(oldP2.x, transform->position.x, transform->position.x + slope->width))
			{
				float k = (oldP2.y - transform->position.y) / slope->height;
				printf("k: %f\n", k);
				float cx = transform->position.x + (slope->width * k);
				if (cx > newP2.x && CollisionQuery::IsOnLineSegmentInclusive(newP2.y, transform->position.y, transform->position.y + slope->height))
				{
					//push y position upwards (calculate new y position on slope)
					float k2 = (newP2.x - transform->position.x) / slope->width;
					float newY = transform->position.y + (slope->height * k2);
					playerTransform->position.y = newY - playerBox->height;
					playerKinematic->velocity.y = 0.0f;
				}

			}
		}
		else if (deltaP2.x > 0)//go down
		{
			//check if the old position this frame is on the slope tile
			if (CollisionQuery::IsOnLineSegmentInclusive(oldP2.x + playerBox->width, transform->position.x, transform->position.x + slope->width))
			{
				//calculate position x on slope
				//c.x = t.x + t.w * k
				//k = (p.y - t.y ) / t.h

				float k = (oldP2.y - transform->position.y) / slope->height;
				//printf("k: %f\n", k);
				float cx = transform->position.x + (slope->width * (k));
				if (cx < newP2.x && CollisionQuery::IsOnLineSegmentInclusive(newP2.y, transform->position.y, transform->position.y + slope->height))
				{

					//push y position downwards (calculate new y position on slope)
					float k2 = (newP2.x - transform->position.x) / slope->width;
					float newY = transform->position.y + (slope->height * (k2));
					playerTransform->position.y = newY - playerBox->height;
					playerKinematic->velocity.y = 0.0f;

				}
			}
		}
		else if (deltaP2.x == 0 && deltaP2.y > 0) //maintain position on slope when standing still
		{
			if (CollisionQuery::IsOnLineSegmentInclusive(oldP2.x, transform->position.x, transform->position.x + slope->width))
			{
				//calculate cy (assumption transform->position.x < oldP.x < transform->position.x + slope->width)
				float k = (oldP2.x - transform->position.x) / slope->width;
				float cy = transform->position.y + (slope->height * (k));
				if (newP2.y > cy)
				{
					playerKinematic->velocity.y = cy - oldP2.y;
					//float peny = newP.y - cy;
					//playerTransform->position.y -= peny;
				}
			}
		}
	}
}

void MovementSystem::UpdatePositions(float deltaTime)
{

	//this is all what we want to do here!
	vector<int> entityIDs = ecs->entitySystem.GetIDs("Player");
	for (vector<int>::iterator it = entityIDs.begin();it != entityIDs.end();++it)
	{
		Transform* transform = transforms->GetComponent(*it);
		Kinematic* kinematic = kinematics->GetComponent(*it);

		if (transform != nullptr && kinematic != nullptr)
		{
			transform->position = transform->position + kinematic->velocity;
		}

		BoxCollider* box = ecs->boxColliders.GetComponent(*it);
		if (box != nullptr &&  transform != nullptr)
		{
			box->position = transform->position;
		}
	}
}

float MovementSystem::GetDeltaSpeed(Kinematic* kinematic)
{
	return kinematic->currentSpeed * kinematic->accelFactor;
}
