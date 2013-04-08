//
//  Types.h
//  NiteAndHandAttributes
//
//  Created by Timothy Prepscius on 3/27/13.
//  Copyright (c) 2013 Timothy Prepscius. All rights reserved.
//

#ifndef NiteAndHandAttributes_Types_h
#define NiteAndHandAttributes_Types_h

#include "wm5/Wm5Mathematics.h"
#include <OpenNI.h>

typedef Wm5::Vector3f Vector3f;
typedef Wm5::Vector2f Vector2f;

class Vector2i
{
	int v[2];
public:
	
	Vector2i ()
	{
		v[0] = v[1] = 0;
	}

	Vector2i(int x, int y)
	{
		v[0] = x;
		v[1] = y;
	}
		
	int &X ()
	{
		return v[0];
	}
	
	int &Y ()
	{
		return v[1];
	}
	
	const int &X () const
	{
		return v[0];
	}
	
	const int &Y () const
	{
		return v[1];
	}
	
	int &operator[](int i)
	{
		return v[i];
	}

	const int &operator[](int i) const
	{
		return v[i];
	}
	
	Vector2i operator -(const Vector2i &rhs)
	{
		return Vector2i(v[0]-rhs.v[0], v[1]-rhs.v[1]);
	}
	
	Vector2i operator +(const Vector2i &rhs)
	{
		return Vector2i(v[0]+rhs.v[0], v[1]+rhs.v[1]);
	}
	
	int Length()
	{
		return sqrt(v[0]*v[0] + v[1]+v[1]);
	}

} ;

class Vector3i
{
	int v[3];
public:
	
	Vector3i ()
	{
		v[0] = v[1] = v[2] = 0;
	}

	Vector3i(int x, int y, int z)
	{
		v[0] = x;
		v[1] = y;
		v[2] = z;
	}
		
	int &X ()
	{
		return v[0];
	}
	
	int &Y ()
	{
		return v[1];
	}
	
	int &Z ()
	{
		return v[2];
	}
	
	const int &X () const
	{
		return v[0];
	}
	
	const int &Y () const
	{
		return v[1];
	}
	
	const int &Z () const
	{
		return v[2];
	}
	
	int &operator[](int i)
	{
		return v[i];
	}

	const int &operator[](int i) const
	{
		return v[i];
	}
	
} ;

class ComputationDebugger
{
public:
	virtual ~ComputationDebugger () {}
	
	virtual void beginFrame(openni::VideoFrameRef in) = 0;
	virtual void circle (const Vector2i &center, int radius, const Vector3i &color) = 0;
	virtual void line (const Vector2i &p1, const Vector2i &p2, const Vector3i &color, int width) = 0;
	virtual void endFrame () = 0;
	virtual void show () = 0;
};

template<typename V>
V max(V v1, V v2, V v3)
{
	return std::max(std::max(v1, v2),v3);
}

template<typename V>
V max(V v1, V v2, V v3, V v4)
{
	return std::max(std::max(std::max(v1, v2),v3), v4);
}

#endif
