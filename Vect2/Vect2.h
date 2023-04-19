/*
	File created by Harvey Limbo
	Feel free to use this Vector2 library but there are probably better
	ones out on the internet to use.
	
	Instructions to statically link:
		If using visual studio 2017...
		To link the Vect2.h file
		1. click on Debug
		2. click on [Project] Properties where [Project] is the name of your visual studio project
		e.g. BeatEmLeft-Engine Properties
		3. goto VC++ Directories
		4. Click on the dropdown under Include Directories
		5. Click on edit
		6. A new window will pop up, click on new line (folder with asterisk icon)
		7. type in the location of where Vect2.h and Vect2.lib are located in your file system
		e.g. D:\User\Vect2_Folder
		
		To link the Vect2.lib file
		1. Click on linker under the [Project] Property Pages
		2. click on edit under the Additional Library Directories
		3. click on new line (folder with asterisk icon)
		4. type in the location of where Vect2.lib is located in your file system
		e.g. D:\User\Vect2_Folder
*/

#ifndef VECT2_H
#define VECT2_H
#include <math.h>

struct Vect2
{
	float x, y;

	//Note: Default Vect2 Constructor will always initialize the x and y components to zero ~ design choice
	Vect2() { this->x = 0; this->y = 0; }
	Vect2(float x, float y) { this->x = x; this->y = y; }
	Vect2(const Vect2& copy) { this->x = copy.x; this->y = copy.y; }
	~Vect2() {}

	inline Vect2& operator=(const Vect2& other)
	{
		this->x = other.x;
		this->y = other.y;
		return *this;
	}

	inline Vect2 operator+(const Vect2& other) const
	{
		return Vect2(x + other.x, y + other.y);
	}

	inline Vect2 operator-(const Vect2& other) const
	{
		return Vect2(x - other.x, y - other.y);
	}

	inline Vect2 operator*(const float& scalar)
	{
		return Vect2(x * scalar, y * scalar);
	}

	inline Vect2 operator/(const float& scalar)
	{
		return Vect2(x / scalar, y / scalar);
	}

	inline Vect2 operator/(const float& scalar) const
	{
			return Vect2(x / scalar, y / scalar);
	}

	inline Vect2 operator+=(const Vect2& other)
	{
		this->x += other.x;
		this->y += other.y;
		return *this;
	}

	inline Vect2 operator -=(const Vect2& other)
	{
		this->x -= other.x;
		this->y -= other.y;
		return *this;
	}

	//Keeps both the x and y axes constrained between a min and a max value
	inline Vect2 ClampAxes(float minAxisValue, float maxAxisValue)
	{
		Vect2 v(x, y);
		v.x = (v.x < minAxisValue) ? minAxisValue : (v.x > maxAxisValue) ? maxAxisValue : v.x;
		v.y = (v.y < minAxisValue) ? minAxisValue : (v.y > maxAxisValue) ? maxAxisValue : v.y;
		return v;
	}

	//returns the sign of each component in the Vector
	inline Vect2 GetSigns()
	{
		Vect2 result;
		result.x = (x == 0.0f) ? 0.0f : (x > 0.0f) ? 1.0f : -1.0f;
		result.y = (y == 0.0f) ? 0.0f : (y > 0.0f) ? 1.0f : -1.0f;
		return result;
	}

// 	void PrintValues();

	//Note:: The functions below are typically used for unit testing only
	//returns true if the other vector is an identical copy of the original vector.
	//Note: does not check if this Vector shares the same memory address location as the other Vector
	inline bool operator ==(const Vect2& other) const
	{
		return this->x == other.x && this->y == other.y;
	}

	inline bool operator !=(const Vect2& other) const
	{
		return !(this == &other);
	}

};

namespace VectMath
{
	inline Vect2 Zero() { return Vect2(0.0f, 0.0f); }
	inline Vect2 Right() { return Vect2(1.0f, 0.0f); }
	inline Vect2 Left() { return Vect2(-1.0f, 0.0f); }
	inline Vect2 Up() { return Vect2(0.0f, 1.0f); }
	inline Vect2 Down() { return Vect2(0.0f, -1.0f); }

	inline float DistSqr(const Vect2& v1, const Vect2& v2)
	{
		return (v2.x - v1.x) * (v2.x - v1.x) + (v2.y - v1.y) * (v2.y - v1.y);
	}
	inline float Dist(const Vect2& v1, const Vect2& v2)
	{
		return sqrt(DistSqr(v1, v2));
	}
	inline float Magnitude(const Vect2& v)
	{
		return Dist(Zero(), v);
	}
	
	inline float GetMax(const Vect2& v)
	{
		return v.x > v.y ? v.x : v.y;
	}
	inline float GetMin(const Vect2& v)
	{
		return v.x > v.y ? v.y : v.x;
	}
	inline Vect2 GetAbs(const Vect2& v)
	{
		return Vect2(fabs(v.x), fabs(v.y));
	}

	inline float Dot(const Vect2& v1, const Vect2& v2)
	{
		return v1.x * v2.x + v1.y * v2.y;
	}
	inline float Cross(const Vect2& v1, const Vect2 v2)
	{
		return (v1.x * v2.y) - (v1.y * v2.x);
	}
	inline Vect2 Normalize(const Vect2& v)
	{
		return v / Magnitude(v);
	}
	inline Vect2 Normalize(const Vect2& v1, const Vect2& v2)
	{
		float magnitude = Dist(v1, v2);
		return (v2 - v1) / magnitude;
	}
	inline float AngleBetween(const Vect2& v1, const Vect2& v2)
	{
		return (float)acos(Dot(v1, v2) / (Magnitude(v1) * Magnitude(v2)));
	}
	inline Vect2 Lerp(const Vect2& from, const Vect2& to, float t)
	{
		return from + (to - from) * t;
	}
	inline bool IsRoughlyEqual(const Vect2& v1, const Vect2& v2, float tolerance = 0.001f)
	{
		return fabs(v1.x - v2.x) < tolerance && fabs(v1.y - v2.y) < tolerance;
	}
}

#endif
