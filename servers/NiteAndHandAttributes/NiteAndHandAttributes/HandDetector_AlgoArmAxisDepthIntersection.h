//
//  HandDetector_AlgoArmAxisDepthIntersection.h
//  NiteAndHandAttributes
//
//  Created by Timothy Prepscius on 3/27/13.
//  Copyright (c) 2013 Timothy Prepscius. All rights reserved.
//

#ifndef __NiteAndHandAttributes__HandDetector_AlgoArmAxisDepthIntersection__
#define __NiteAndHandAttributes__HandDetector_AlgoArmAxisDepthIntersection__

#include "CoordConverter.h"
#include "AlgoArmAxisMatcher.h"

class HandDetector_AlgoArmAxisDepthIntersection
{
public:
	std::vector<float> NO_RESULT () const;
	AlgoArmAxisMatcher handStateMatcher;

protected:
	const nite::UserId *getMapIdFor(const nite::UserMap& map, const Vector2i &p);

	std::vector<Wm5::Vector3f> getPixelsInArea (
		const openni::VideoFrameRef &frame,
		const Wm5::Vector3f &center,
		float radius,
		const nite::UserMap& map,
		const CoordConverter &converter
	);

	void detectHandState (
		const std::vector<Vector3f> &pixels,
		const Vector3f &elbow, const Vector3f &hand, const Vector3f &edgeHand, float searchWidth,
		const Vector3f &armAxis, const Vector3f &armAxisUp, const Vector3f &armAxisLeft,
		const CoordConverter &coordinateConverter, ComputationDebugger *debugger,
		float anticipatedPeak, float &peakOut, float &handStateOut,
		std::vector<int> &intersectingPixels
	);
	
public:
	HandDetector_AlgoArmAxisDepthIntersection ();

	std::vector<float> getHandState(
		nite::UserTracker &userTracker,
		nite::UserTrackerFrameRef &userTrackerFrame,
		const nite::UserData& user, ComputationDebugger *debugger
	);
	
} ;

#endif /* defined(__NiteAndHandAttributes__HandDetector_AlgoArmAxisDepthIntersection__) */
