#include "HandDetector_AlgoOpenCV.h"
#include <opencv/cv.h>

using namespace cv;
using namespace std;

/*******************************************************
 ********************************************************
 *********************HandDetector***********************
 ********************************************************
 *******************************************************/

void HandDetector_AlgoOpenCV::setParams(HandDetector_AlgoOpenCV::Params& p)
{
    param.cannyLb = 50;
    param.cannyUb = 100;
	param.area = p.area;
	param.cosThreshold = p.cosThreshold;
	param.equalThreshold = p.equalThreshold;
	param.r = p.r;
	param.step = p.step;
}

bool HandDetector_AlgoOpenCV::isEqual(double a, double b)
{
	return fabs(a - b) <= param.equalThreshold;
}

double HandDetector_AlgoOpenCV::angle(std::vector<cv::Point>& contour, int pt, int r)
{
	int size = contour.size();
	cv::Point p0=(pt>0)?contour[pt%size]:contour[size-1+pt];
	cv::Point p1=contour[(pt+r)%size];
	cv::Point p2=(pt>r)?contour[pt-r]:contour[size-1-r];
    
	double ux=p0.x-p1.x;
	double uy=p0.y-p1.y;
	double vx=p0.x-p2.x;
	double vy=p0.y-p2.y;
	return (ux*vx + uy*vy)/sqrt((ux*ux + uy*uy)*(vx*vx + vy*vy));
}

signed int HandDetector_AlgoOpenCV::rotation(std::vector<cv::Point>& contour, int pt, int r)
{
	int size = contour.size();
	cv::Point p0=(pt>0)?contour[pt%size]:contour[size-1+pt];
	cv::Point p1=contour[(pt+r)%size];
	cv::Point p2=(pt>r)?contour[pt-r]:contour[size-1-r];
    
	double ux=p0.x-p1.x;
	double uy=p0.y-p1.y;
	double vx=p0.x-p2.x;
	double vy=p0.y-p2.y;
	return (ux*vy - vx*uy);
}

vector<Vec4i> hierarchy;

uint8_t findFirstNonZero (int x, int y, int dx, int dy, cv::Mat &s1)
{
    int w = s1.cols;
    int h = s1.rows;
    
    while((0 <= x && x < w) && (0 <= y && y < h))
    {
        uint8_t v = s1.at<uint8_t>(cv::Point(x,y));
        if (v != 0)
            return v;
        
        x += dx;
        y += dy;
    }
        
    return 0;
}

void removeZeroParts (cv::Mat &s1)
{
    for (int x=0; x<s1.cols; ++x)
    {
        for (int y=0; y<s1.rows; ++y)
        {
            uint8_t &v = s1.at<uint8_t>(cv::Point(x,y));
            if (v == 0)
            {
                v = std::max (
                    std::max(
                        std::max(
                            findFirstNonZero(x, y, -1, 0, s1),
                            findFirstNonZero(x, y, 1, 0, s1)
                        ),
                        std::max(
                            findFirstNonZero(x, y, 0, -1, s1),
                            findFirstNonZero(x, y, 0, 1, s1)
                        )
                     ),
                    std::max(
                         std::max(
                                  findFirstNonZero(x, y, -1, -1, s1),
                                  findFirstNonZero(x, y, 1, 1, s1)
                                  ),
                         std::max(
                                  findFirstNonZero(x, y, 1, -1, s1),
                                  findFirstNonZero(x, y, -1, 1, s1)
                                  )
                         )
                              );
            }
        }
    }
}

void thresholdTest (cv::Mat &s1, uint8_t lb,uint8_t ub, uint8_t nb)
{
    for (int x = 0; x<s1.cols; ++x)
    {
        for (int y = 0; y<s1.rows; ++y)
        {
            uint8_t &v = s1.at<uint8_t>(cv::Point(x,y));
            if (! (lb <= v && v <= ub) )
                v = nb;
        }
    }
}

cv::Point findCentroid (std::vector<cv::Point> &array_points)
{
    
    Rect bRect = cv::boundingRect(array_points);
    
    return cv::Point(
        bRect.x + (bRect.width / 2),
        bRect.y + (bRect.height / 2)
    );
}

void HandDetector_AlgoOpenCV::detect(cv::Mat& mask, cv::Mat &s1, cv::Mat &s2, cv::Mat&s3, cv::Mat& rgb, std::vector<Hand>& hands)
{
	hands.clear();
	vector<vector<Point> > contours;
//	vector<Vec4i> hierarchy;
    
    //		cv::threshold(depthMap, depthMap, 60, 255, cv::THRESH_BINARY);

    // use farthest neihbor to take out part of the image
    s1 = mask.clone();
//    thresholdTest(s1,0, 128,255);
    
    removeZeroParts(s1);
    cv::normalize(s1.clone(), s1, 0, 255, CV_MINMAX, CV_8UC1);
	thresholdTest(s1, 0, 128, 255);

//    cv::GaussianBlur(s2.clone(), s2, Size( 11, 11 ), 0, 0 );
//    cv::medianBlur(s2.clone(), s2, 11);
    
    Mat canny_output;
    Canny( s1, s2, param.cannyLb, param.cannyUb, 3 );
    
    cv::Mat element = cv::getStructuringElement(cv::MORPH_DILATE, cv::Size(4,4));
    cv::morphologyEx( s2, s3, cv::MORPH_CLOSE, element);
    
	findContours(s2, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(0, 0) );
    
    for( int i = 0; i< contours.size(); i++ )
    {
        Scalar color = Scalar( i * 30,i * 30, 255 );
        drawContours( rgb, contours, i, color, 2, 8, hierarchy, 0, Point() );
    }
	    
    if(!contours.empty())
	{
		for(int i=0; i<contours.size(); i++)
		{
            Hand hand;
			if(cv::contourArea(contours[i])<param.area)
                continue;
            if (0)
			{
				cv::Moments m=cv::moments(contours[i]);
				hand.center.x=m.m10/m.m00;
				hand.center.y=m.m01/m.m00;
                
				for(int j = 0; j < contours[i].size(); j+= param.step )
				{
					double cos0 = angle (contours[i], j, param.r);
                    
					if ((cos0 > 0.5)&&(j+param.step<contours[i].size()))
					{
						double cos1 = angle (contours[i], j - param.step, param.r);
						double cos2 = angle (contours[i], j + param.step, param.r);
						double maxCos = std::max(std::max(cos0, cos1), cos2);
						bool equal = isEqual (maxCos , cos0);
						signed int z = rotation (contours[i], j, param.r);
						if (equal == 1 && z<0)
						{
							hand.fingers.push_back(contours[i][j]);
						}
					}
				}
				hand.contour=contours[i];
				hands.push_back(hand);
			}

            vector<Point> &contour = contours[i];
            cv::approxPolyDP(contour, hand.contour, 2, true);
            cv::convexHull(hand.contour, hand.hullP);
            cv::convexHull(hand.contour, hand.hullI);
            if (hand.hullI.size() > 3)
                cv::convexityDefects(hand.contour, hand.hullI, hand.defects);

            for (auto d : hand.defects)
            {
                int startIdx = d.val[0];
                int endIdx = d.val[1];
                int defectPtIdx = d.val[2];
                double depth = (double)d.val[3]/256.0f;
                
                hand.defectsP.push_back(hand.contour[defectPtIdx]);
            }
            
            if (hand.defectsP.size() > 0)
                hand.center = findCentroid(hand.defectsP);
            else
                hand.center = findCentroid(hand.contour);
            
            hands.push_back(hand);
            
		}
	}
}


void drawHands(cv::Mat& image, std::vector<Hand>& hands)
{
	int size = hands.size();
	std::vector<std::vector<cv::Point> > c;
	for(int i = 0; i<size; i++)
	{
        Hand &hand = hands[i];
        
        Scalar color = Scalar( 0,0, 255 );
        cv::Rect rect = boundingRect(hand.contour);
        cv::rectangle(image, rect, color);
        
        if (hand.hullP.size()>0)
        {
            for (int i=0; i<hand.hullP.size()-1; ++i)
                cv::line( image, hand.hullP[i], hand.hullP[i+1],color, 2);
            cv::line( image, hand.hullP[0], hand.hullP.back(),color, 2);
        }
        
 		c.clear();
		c.push_back(hand.contour);
		cv::circle(image, hand.center, 2, cv::Scalar(255, 0, 0), 2);
		int fingersSize = hand.fingers.size();
		for(int j = 0; j < fingersSize; j++)
		{
			cv::circle(image, hand.fingers[j], 2, cv::Scalar(0, 255, 0), 2);
//			cv::line(image, hand.center, hand.fingers[j], cv::Scalar(0, 0, 255), 4);
//            cv::ellipse(image, hand.rect, cv::Scalar(0,128, 128));
		}
        
		for(auto d : hand.defectsP)
		{
			cv::circle(image, d, 3, cv::Scalar(0, 128, 255), 2);
            
        }
//		std::cout<<hand.fingers.size()<<endl;
	}
}