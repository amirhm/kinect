//
//  CoordConverter.h
//  NiteAndHandAttributes
//
//  Created by Timothy Prepscius on 3/27/13.
//  Copyright (c) 2013 Timothy Prepscius. All rights reserved.
//

#ifndef NiteAndHandAttributes_CoordConverter_h
#define NiteAndHandAttributes_CoordConverter_h

#include "Types.h"
#include <OpenNI.h>
#include "NiTE.h"

class CoordConverter
{
	nite::UserTracker &userTracker;
	const openni::VideoFrameRef &frame;
	
public:
	CoordConverter (nite::UserTracker &_userTracker, const openni::VideoFrameRef &_frame) :
		userTracker(_userTracker),
		frame(_frame)
	{
	}
	
	OniStreamHandle getOniStreamHandle () const
	{
		struct Hack {
			Hack *p1,*p2,*p3,*p4,*p5,*p6,*p7,*p8,*p9,*p10,*p11,*p12,*p13,*p14,*p15,*p16;
		};
		
		Hack *hack = (Hack *)(&userTracker);
		OniStreamHandle oniStreamHandle = (OniStreamHandle)(hack->p1->p1->p7);
		return oniStreamHandle;
	}
	
	Vector2i toFrame (const Vector3f &p) const
	{
		float lhxf, lhyf;
		
		userTracker.convertJointCoordinatesToDepth(
			p.X(),
			p.Y(),
			p.Z(),
			&lhxf, &lhyf
		);

		return Vector2i(
			lhxf * frame.getWidth()/frame.getVideoMode().getResolutionX(),
			lhyf * frame.getHeight()/frame.getVideoMode().getResolutionY()
		);
	}
	
	int toFrameDepth(const Vector2i &p) const
	{
		const openni::DepthPixel *data = (const openni::DepthPixel *)frame.getData();
		const openni::DepthPixel *pixel = data + (frame.getWidth() * p.Y() + p.X());
		
		return *pixel;
	}
	
	Vector3f toNite (const Vector3i &p) const
	{
		float wx, wy, wz;
		oniCoordinateConverterDepthToWorld(getOniStreamHandle(), p[0], p[1], p[2], &wx, &wy, &wz);
		
		return Vector3f(wx,wy,wz);
	}
	
	Vector3f toNite (const Vector2i &p) const
	{
		return toNite(Vector3i(p.X(), p.Y(), toFrameDepth(p)));
	}
} ;


#endif

