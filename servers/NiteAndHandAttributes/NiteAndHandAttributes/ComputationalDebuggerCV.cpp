//
//  ComputationalDebuggerCV.cpp
//  NiteAndHandAttributes
//
//  Created by Timothy Prepscius on 3/27/13.
//  Copyright (c) 2013 Timothy Prepscius. All rights reserved.
//

#include "ComputationalDebuggerCV.h"

ComputationalDebuggerCV::ComputationalDebuggerCV ()
{
	windowCreated = false;
}

ComputationalDebuggerCV::~ComputationalDebuggerCV()
{

}

void ComputationalDebuggerCV::ensureWindow ()
{
	if (windowCreated)
		return;
		
	cv::namedWindow("depth", 1);
	cv::moveWindow("depth", 0, 00);
	windowCreated = true;
}

void convert_pixel_map(const openni::VideoFrameRef &in, cv::Mat& out)
{
    const void *data = in.getData();
    int sizes[2] = {in.getHeight(), in.getWidth()};

    cv::Mat s1, &s2 = out;
    s1 = cv::Mat(2, sizes, CV_8UC3, (void *)data);
    cv::cvtColor(s1,s2, CV_RGB2BGR);
	
	/*
	const nite::UserId* pLabels = map.getPixels();
	for (int y=0; y<map.getHeight(); ++y)
	{
		for (int x=0;x<map.getWidth(); ++x, ++pLabels)
		{
			cv::Vec3b &v = s2.at<cv::Vec3b>(cv::Point(x,y));
			if (*pLabels == 0)
				v = cv::Vec3b(0,0,0);
		}
	}
	*/
}

void convert_depth_map(const openni::VideoFrameRef &in, cv::Mat& out)
{
    const void *data = in.getData();
    int sizes[2] = {in.getHeight(), in.getWidth()};

    cv::Mat s1, s2, s3;
    s1 = cv::Mat(2, sizes, CV_16UC1, (void*)data);
	cv::normalize(s1, s2, 0, 255, CV_MINMAX, CV_8UC1);
    cv::cvtColor(s2,out, CV_GRAY2BGR);
	
	/*
		const nite::UserId* pLabels = map.getPixels();
	for (int y=0; y<map.getHeight(); ++y)
	{
		for (int x=0;x<map.getWidth(); ++x, ++pLabels)
		{
			uint16_t &v = s1.at<uint16_t>(cv::Point(x,y));
			if (!*pLabels)
				v = 0;
		}
	}
*/
	
//	cv::normalize(s1, out, 0, 255, CV_MINMAX, CV_8UC1);
}

void ComputationalDebuggerCV::beginFrame (openni::VideoFrameRef depth)
{
	mutex.lock();
    convert_depth_map(depth, depthMat);
}

void ComputationalDebuggerCV::circle(const Vector2i &center, int radius, const Vector3i &color)
{
	cv::circle(depthMat, cv::Point(center.X(), center.Y()), radius, cv::Scalar(color[0], color[1], color[2]));
}

void ComputationalDebuggerCV::line(const Vector2i &p1, const Vector2i &p2, const Vector3i &color, int width)
{
	cv::line(depthMat, cv::Point(p1.X(), p1.Y()), cv::Point(p2.X(), p2.Y()), cv::Scalar(color[0], color[1], color[2]));
}

void ComputationalDebuggerCV::endFrame ()
{
	mutex.unlock();
}

void ComputationalDebuggerCV::show ()
{
	cv::Mat copy;
	{
		boost::lock_guard<boost::mutex> guard(mutex);
		copy = depthMat;
	}
	
	ensureWindow();
	if (copy.rows > 0 && copy.cols > 0)
	{
		cv::imshow("depth", copy);
		cv::waitKey(1);
	}
}