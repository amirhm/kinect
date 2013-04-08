//
//  HandDetector_AlgoArmAxisDepthIntersection.cpp
//  NiteAndHandAttributes
//
//  Created by Timothy Prepscius on 3/27/13.
//  Copyright (c) 2013 Timothy Prepscius. All rights reserved.
//

#include "HandDetector_AlgoArmAxisDepthIntersection.h"
#include "wm5/Wm5Mathematics.h"
#include "CoordConverter.h"
#include <OpenNI.h>

HandDetector_AlgoArmAxisDepthIntersection::HandDetector_AlgoArmAxisDepthIntersection() :
	handStateMatcher((double *)AlgoArmAxisMatcher::defaultWeights, AlgoArmAxisMatcher::defaultNumWeights/2)
{

}

std::vector<float> HandDetector_AlgoArmAxisDepthIntersection::NO_RESULT() const
{
	std::vector<float> result;
	result.push_back(0);
	result.push_back(0);
	
	return result;
}

const nite::UserId *HandDetector_AlgoArmAxisDepthIntersection::getMapIdFor(const nite::UserMap& map, const Vector2i &p)
{
	return map.getPixels() + ((map.getWidth() * p.Y()) + (p.X()));
}

std::vector<Wm5::Vector3f> HandDetector_AlgoArmAxisDepthIntersection::getPixelsInArea (
	const openni::VideoFrameRef &frame,
	const Wm5::Vector3f &center,
	float radius,
	const nite::UserMap& map,
	const CoordConverter &converter
)
{
	Vector2i frameCenter = converter.toFrame(center);
	Vector2i ld = converter.toFrame(center+Vector3f(radius,0,0));
	Vector2i ud = converter.toFrame(center+Vector3f(0,radius,0));
	Vector2i fd = converter.toFrame(center+Vector3f(0,0,radius));
	
	int r = max(
		(ld-frameCenter).Length(),
		(ud-frameCenter).Length(),
		(fd-frameCenter).Length()
	);
	
	Vector2i lt(frameCenter.X() - r, frameCenter.Y() - r);
	Vector2i rb(frameCenter.X() + r, frameCenter.Y() + r);
	lt.X() = std::min(std::max(0,lt.X()), frame.getWidth());
	lt.Y() = std::min(std::max(0,lt.Y()), frame.getHeight());
	rb.X() = std::min(std::max(0,rb.X()), frame.getWidth());
	rb.Y() = std::min(std::max(0,rb.Y()), frame.getHeight());
	
	std::vector<Wm5::Vector3f> pixels;
	for (int x=lt.X(); x<rb.X(); ++x)
	{
		for (int y=lt.Y(); y<rb.Y(); ++y)
		{
			Vector2i p(x,y);
			
			// restrict to the pixels the Nite has determined are the character
//			if (getMapIdFor(map, p)==0)
//				continue;
				
			pixels.push_back(converter.toNite(p));
		}
	}
	
	return pixels;
}

void HandDetector_AlgoArmAxisDepthIntersection::detectHandState (
	const std::vector<Vector3f> &pixels,
	const Vector3f &elbow, const Vector3f &hand, const Vector3f &edgeHand, float searchWidth,
	const Vector3f &armAxis, const Vector3f &armAxisUp, const Vector3f &armAxisLeft,
	const CoordConverter &coordinateConverter, ComputationDebugger *debugger,
	float anticipatedPeak, float &peakOut, float &handStateOut,
	std::vector<int> &intersectingPixels
)
{
	float dv = (edgeHand - hand).Length() * 2.0f;
	int numSegments = 40;
	float segment = dv/numSegments;
	
	if (debugger)
	{
		for (auto &pixel : pixels)
			debugger->circle(coordinateConverter.toFrame(pixel), 1, Vector3i(0,255,0));
	}
	
	std::vector<float> intersectionAreas;
	for (int i=0; i<numSegments; ++i)
	{
		Vector3f center = edgeHand + i * segment * (-armAxis);
		float intersectionArea = 0;
		Wm5::Box3f segmentBox = Wm5::Box3f(
			center,
			-armAxis, armAxisLeft, armAxisUp,
			segment/2.0, searchWidth/2.0, searchWidth/2.0
		);
		
		std::vector<Wm5::Vector3f> vertices;
		vertices.resize(8);
		
		if (debugger)
			debugger->circle(coordinateConverter.toFrame(center), 2, Vector3i(255,255,255));
			
		segmentBox.ComputeVertices(vertices.data());
		
		if (debugger)
		{
			for (int i=0; i<7; ++i)
				debugger->line(
					coordinateConverter.toFrame(vertices[i]),
					coordinateConverter.toFrame(vertices[i+1]),
					Vector3i(255,255,255),
					2
				);
		}
		
		for (int i=0; i<pixels.size(); ++i)
		{
			auto &pixel = pixels[i];
			Wm5::Sphere3f pixelSphere(pixel, 0.1);
			Wm5::IntrBox3Sphere3f intersector(segmentBox, pixelSphere);
			if (intersector.Test())
			{
				if (debugger)
					debugger->circle(coordinateConverter.toFrame(pixel), 1, Vector3i(255,255,0));
					
				intersectingPixels.push_back(i);
				intersectionArea += 0.1;
			}
		}
		
		intersectionAreas.push_back(intersectionArea);
	}
	
	handStateOut = handStateMatcher.match(intersectionAreas);
	// log it

/*
	static int frame = 0;
	std::cout << frame++;

	bool first = true;
	for (float f : intersectionAreas)
		std::cout << "," << f;
	std::cout << "\n";
*/
}

std::vector<float> HandDetector_AlgoArmAxisDepthIntersection::getHandState(
	nite::UserTracker &userTracker,
	nite::UserTrackerFrameRef &userTrackerFrame,
	const nite::UserData& user,
	ComputationDebugger *debugger
)
{
	openni::VideoFrameRef depthFrame = userTrackerFrame.getDepthFrame();
	if (!depthFrame._getFrame())
		return NO_RESULT();
		
	if (user.getSkeleton().getState() != nite::SKELETON_TRACKED)
		return NO_RESULT();
		
	const nite::UserMap& userLabels = userTrackerFrame.getUserMap();

	CoordConverter coordinateConverter(userTracker, depthFrame);

	std::vector<std::pair<const nite::SkeletonJoint *, const nite::SkeletonJoint *> > arms;
	arms.push_back(
		std::pair<const nite::SkeletonJoint *, const nite::SkeletonJoint *>(
			&user.getSkeleton().getJoint(nite::JOINT_RIGHT_ELBOW), &user.getSkeleton().getJoint(nite::JOINT_RIGHT_HAND)
		)
	);
	arms.push_back(
		std::pair<const nite::SkeletonJoint *, const nite::SkeletonJoint *>(
			&user.getSkeleton().getJoint(nite::JOINT_LEFT_ELBOW), &user.getSkeleton().getJoint(nite::JOINT_LEFT_HAND)
		)
	);
	std::vector<float> results;
	for (auto &arm : arms)
	{
		const nite::SkeletonJoint &lelbow = *arm.first;
		const nite::SkeletonJoint &lhand = *arm.second;

		Wm5::Vector3f elbow(lelbow.getPosition().x, lelbow.getPosition().y, lelbow.getPosition().z);
		Wm5::Vector3f hand(lhand.getPosition().x, lhand.getPosition().y, lhand.getPosition().z);
		
		if (debugger)
		{
			debugger->circle(coordinateConverter.toFrame(elbow), 3, Vector3i(0,0,255));
			debugger->circle(coordinateConverter.toFrame(hand), 3, Vector3i(0,0,255));
			debugger->line(coordinateConverter.toFrame(hand), coordinateConverter.toFrame(hand), Vector3i(0,128,255), 2);
		}
			
		Vector3f armAxis = hand - elbow;
		armAxis.Normalize();
		
		Vector3f armLeft = armAxis.Cross(Wm5::Vector3f::UNIT_Y);
		armLeft.Normalize();
		
		Vector3f armUp = armAxis.Cross(armLeft);
		armUp.Normalize();
		
		float searchOnArm = (hand-elbow).Length()/2.0f;
		float searchWidth = searchOnArm/1.2;
		
		Wm5::Vector3f edgeHandSearch = hand + (searchOnArm * armAxis);
		// should transform the sphere of the search into an ellipse on the screen
		// but for later.

		if (debugger)
		{
			debugger->circle(coordinateConverter.toFrame(edgeHandSearch), 4, Vector3i(0,0,255));
			debugger->line(coordinateConverter.toFrame(hand), coordinateConverter.toFrame(edgeHandSearch), Vector3i(0,200,255), 2);
		}
		
		std::vector<Vector3f> pixels = getPixelsInArea(depthFrame, hand, searchOnArm, userLabels, coordinateConverter);
		
		float lastPeak = 0;
		float peakOut;
		float handStateOut;
		
		std::vector<int> intersectingPixels;
		detectHandState(
			pixels, elbow, hand, edgeHandSearch, searchWidth, armAxis, armUp, armLeft, coordinateConverter, debugger,
			lastPeak, peakOut, handStateOut,
			intersectingPixels
		);
		
		results.push_back(handStateOut);
		
		if (handStateOut > 0)
		{
			if (debugger)
			{
				for (auto i : intersectingPixels)
				{
					auto &pixel = pixels[i];
					debugger->circle(coordinateConverter.toFrame(pixel), 1, Vector3i(0,0,255));
				}
			}
		}
	}
	
	return results;
}




