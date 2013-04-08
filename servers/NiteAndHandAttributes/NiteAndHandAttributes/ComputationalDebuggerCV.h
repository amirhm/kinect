//
//  ComputationalDebuggerCV.h
//  NiteAndHandAttributes
//
//  Created by Timothy Prepscius on 3/27/13.
//  Copyright (c) 2013 Timothy Prepscius. All rights reserved.
//

#ifndef __NiteAndHandAttributes__ComputationalDebuggerCV__
#define __NiteAndHandAttributes__ComputationalDebuggerCV__

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <boost/thread.hpp>

#include "Types.h"
#include <OpenNI.h>

class ComputationalDebuggerCV : public ComputationDebugger
{
protected:
	bool windowCreated;
	cv::Mat depthMat;
	
	boost::mutex mutex;
	
	void ensureWindow();
	
public:
	ComputationalDebuggerCV ();
	virtual ~ComputationalDebuggerCV ();

	virtual void beginFrame (openni::VideoFrameRef in);
	virtual void circle (const Vector2i &center, int radius, const Vector3i &color);
	virtual void line (const Vector2i &p1, const Vector2i &p2, const Vector3i &color, int width);
	virtual void endFrame();
	
	virtual void show();
};

#endif /* defined(__NiteAndHandAttributes__ComputationalDebuggerCV__) */
