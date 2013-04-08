#ifndef __HAND_DETECTOR_H__
#define __HAND_DETECTOR_H__

#include <iostream>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>

class Hand
{
public:
	Hand(){};
	~Hand(){};
	std::vector<cv::Point> fingers;
	cv::Point center;
	std::vector<cv::Point> contour;
    
    cv::vector<cv::Point> hullP;
    cv::vector<int> hullI;
    std::vector<cv::Vec4i> defects;
    std::vector<cv::Point> defectsP;
    cv::RotatedRect rect;
};

class HandDetector_AlgoOpenCV
{
public:
	struct Params
	{
        int cannyLb, cannyUb;
		int area;
		int r;
		int step;
		double cosThreshold;
		double equalThreshold;
	};
    
    CvMemStorage contour_st;
	CvSeq		contour;	/* Hand contour */
	CvSeq		hull;		/* Hand convex hull */
    
public:
	HandDetector_AlgoOpenCV(){};
	~HandDetector_AlgoOpenCV(){};
    
	void detect(cv::Mat& mask, cv::Mat&, cv::Mat&, cv::Mat&, cv::Mat&rgb, std::vector<Hand>& hands);
	void setParams(Params& p);
private:
	Params param;
	signed int rotation(std::vector<cv::Point>& contour, int pt, int r);
	double angle(std::vector<cv::Point>& contour, int pt, int r);
	bool isEqual(double a, double b);
};

void drawHands(cv::Mat& image, std::vector<Hand>& hands);

#endif