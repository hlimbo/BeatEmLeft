#ifndef COLLISION_QUERY_H
#define COLLISION_QUERY_H

#include "../Components/BoxCollider.h"
#include <assert.h>

namespace CollisionQuery
{
	//using a power of 2 value to adjust floating point numerical errors
	const float tolerance = 0.0078125f;

	//use this function to check if a coordinate is between minCoordinate and maxCoordinate
	bool IsOnLineSegment(float coordinate, float minCoordinate, float maxCoordinate)
	{
		return coordinate > minCoordinate && coordinate < maxCoordinate;
	}

	bool IsOnLineSegmentInclusive(float coordinate, float minCoordinate, float maxCoordinate)
	{
		return coordinate >= minCoordinate && coordinate <= maxCoordinate;
	}

	bool IsOverlapping(const BoxCollider& b1, const BoxCollider& b2)
	{
		return b1.position.x < b2.position.x + b2.width &&
			b1.position.x + b1.width > b2.position.x &&
			b1.position.y < b2.position.y + b2.height &&
			b1.position.y + b1.height > b2.position.y;
	}

	bool IsOverlappingX(const BoxCollider& b1, const BoxCollider& b2)
	{
		return b1.position.x < b2.position.x + b2.width &&
			b1.position.x + b1.width > b2.position.x;
	}

	bool IsOverlappingY(const BoxCollider& b1, const BoxCollider& b2)
	{
		return	b1.position.y < b2.position.y + b2.height &&
			b1.position.y + b1.height > b2.position.y;
	}

	//this function will return the time measured in milliseconds
	//when a point reaches a target point in 1D.
	//This is used in movement system to check when an object comes into contact with another object
	/*
		Params:
			oldPoint - the position the point was at before movement
			projectedPoint - the position the point will be at after movement is applied
			targetPoint - the position of where oldPoint needs to be within the frame.
	
		Note: projectedPoint != oldPoint
	*/
	float GetContactTime(float oldPoint, float projectedPoint, float targetPoint)
	{
		assert(oldPoint != projectedPoint);
		float time = fabsf((oldPoint - targetPoint) / (projectedPoint - oldPoint));
		return (time < tolerance) ? 0.0f : time;
	}

	BoxCollider GetOverlappedBox(const BoxCollider& b1, const BoxCollider& b2)
	{
		BoxCollider overlap;
		if (IsOverlapping(b1, b2))
		{
			//calculate minX,minY for overlap's new corner
			//calculate width and height of overlap
			float minX = -9999.0f;
			float minY = -9999.0f;
			float width = -9999.0f;
			float height = -9999.0f;
			if (b1.position.x + b1.width - b2.position.x > 0
				&& b1.position.x + b1.width - b2.position.x < b1.width + b2.width)
			{
				//b1's right edge is touching b2's left edge
				puts("right edge");
				width = b1.position.x + b1.width - b2.position.x;
				minX = b1.position.x + b1.width - width;
			}

			if ((b2.position.x + b2.width) - b1.position.x > 0
				&& (b2.position.x + b2.width) - b1.position.x  < b1.width + b2.width)
			{
				puts("left edge");
				width = b2.position.x + b2.width - b1.position.x;
				minX = b2.position.x + b2.width - width;
			}

			if ((b2.position.y + b2.height) - b1.position.y > 0
				&& (b2.position.y + b2.height) - b1.position.y < b1.height + b2.height)
			{
				puts("top edge");
				printf("b2.position + b2.height: %f\n", b2.position.y + b2.height);
				printf("b1.position.y: %f\n", b1.position.y);
				minY = b1.position.y;
				height = (b2.position.y + b2.height) - b1.position.y;
			}

			if ((b1.position.y + b1.height) - b2.position.y > 0
				&& (b1.position.y + b1.height) - b2.position.y < b1.height + b2.height)
			{
				puts("bottom edge");
				minY = b1.position.y + b1.height;
				height = (b1.position.y + b1.height) - b2.position.y;
			}

			printf("new coords: (%f, %f)\n", minX, minY);
			printf("width: %f | height: %f \n", width, height);
			overlap.position = Vect2(minX, minY);
			overlap.width = width;
			overlap.height = height;
		}
		else
		{
			overlap.position.x = 0.0f;
			overlap.position.y = 0.0f;
			overlap.width = 0.0f;
			overlap.height = 0.0f;
		}

		return overlap;
	}

}


#endif
