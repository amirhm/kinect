//
//  AlgoArmAxisMatcher.cpp
//  NiteAndHandAttributes
//
//  Created by Timothy Prepscius on 3/27/13.
//  Copyright (c) 2013 Timothy Prepscius. All rights reserved.
//

#include "AlgoArmAxisMatcher.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <boost/algorithm/string.hpp>

const double AlgoArmAxisMatcher::defaultWeights[20][2] = {
	{ 0.677728, -0.0157629 },
	{ 1.90215, -0.0607344 },
	{ 2.6274, -0.0946396 },
	{ 2.75641, -0.0965253 },
	{ 2.64198, -0.0801572 },
	{ 2.48271, -0.0544191 },
	{ 2.44831, -0.0250326 },
	{ 2.49415, 0.00122161 },
	{ 2.59245, 0.0233393 },
	{ 2.65912, 0.0417844 },
	{ 2.70088, 0.0518331 },
	{ 2.73014, 0.0586891 },
	{ 2.70772, 0.0626203 },
	{ 2.61022, 0.0631082 },
	{ 2.49479, 0.0600433 },
	{ 2.40194, 0.0559354 },
	{ 2.24467, 0.049937 },
	{ 2.16207, 0.0410478 },
	{ 2.03099, 0.0356675 },
	{ 1.97102, 0.027502 }
} ;

const int AlgoArmAxisMatcher::defaultNumWeights = 20;

// these must be declared somewhere, why I'm not seeing them I don't now.

static float fabs(float f)
{
	if (f < 0)
		return -f;
	return f;
}

static float signOf(float f)
{
	if (f < 0)
		return -1;
	return 1;
}

AlgoArmAxisMatcher::AlgoArmAxisMatcher ()
{

}

AlgoArmAxisMatcher::AlgoArmAxisMatcher (double *weights, int numWeights)
{
	for (int i=0; i<numWeights; ++i)
		middleAndWeights.push_back(std::pair<float,float>(weights[i*2], weights[i*2+1]));
}

void AlgoArmAxisMatcher::printRow (const std::vector<float> &row)
{
	bool first = true;
	for (float f : row)
	{
		if (!first)
			std::cout << ",";
			
		std::cout << f;
		
		first = false;
	}
	
	std::cout << std::endl;
}

float AlgoArmAxisMatcher::valueAt (const std::vector<float> &segments, int at)
{
	int nonZeroOffset;
	for (nonZeroOffset=1; nonZeroOffset<segments.size(); ++nonZeroOffset)
		if (segments[nonZeroOffset]!=0)
			break;
			
	if (nonZeroOffset + at >= segments.size())
		return -1;

	return segments[at+nonZeroOffset];
		
	float totalArea = 0.0f;
	for (int i=nonZeroOffset; i<(at+nonZeroOffset+1) && i<segments.size(); ++i)
		totalArea += segments[i];
	
	return totalArea;
}

float AlgoArmAxisMatcher::match (const std::vector<float> &segments)
{
	float finalValue = 0;
	for (int i=0; i<middleAndWeights.size(); ++i)
	{
		auto &m = middleAndWeights[i];
		float v = valueAt(segments, i);
		if (v == -1)
			break;
			
		float dv = (v - m.first) * m.second;
//		std::cout << "(" << v << "," << m.first << "," << m.second << ":" << dv << ") ";
		finalValue += dv;
		
	}
	
//	std::cout << "\n";
	
	return finalValue;
}

std::vector<std::vector<float> > readFile (const char *fileName)
{

	std::ifstream in(fileName);

	std::vector<std::vector<float> > result;

	while (in)
	{
		char buffer[2048];
		in.getline(buffer, 2048, '\n');
		std::string line = buffer;

		std::vector<std::string> segments;
		boost::split(segments, line, boost::is_any_of(","));
		
		if (segments.empty())
			continue;
			
		std::vector<float> data;
		bool foundNonZero = false;
		
		// get rid of the frame marker
		segments.erase(segments.begin());
		for (auto &s : segments)
		{
			std::istringstream iss(s);
			float f;
			iss >> f;
			if (f != 0 || foundNonZero)
			{
				data.push_back(f);
				foundNonZero = true;
			}
		}
		
		if (!data.empty())
			result.push_back(data);
	}

	return result;
}

void AlgoArmAxisMatcher::testData (const std::vector<std::vector<float> > &data, bool correct)
{
	int failures = 0;
	int frame = 0;

	for (auto &row : data)
	{
		bool result = match(row) > 0;
		bool failure = (result != correct);
		if (failure)
			failures++;

		if (failure)
		{
			printRow(row);
		}
		else
		{
//			printRow(row);
		}
			
		frame++;
	}

	std::cout << "NUM FAILURES: " << failures << " out of " << frame << std::endl;
}

float AlgoArmAxisMatcher::findAverage (const std::vector<float> &v)
{
	float s = 0;
	for (float f : v)
		s += f;
		
	return s/v.size();
}

int AlgoArmAxisMatcher::findNumNotInDirection (const std::vector<float> &v, float o, float d)
{
	int num = 0;
	for (float f : v)
	{
		if (f * d < o)
			num++;
	}
	
	return num;
}

std::pair<float, float> AlgoArmAxisMatcher::findLeastIntersection (const std::vector<float> &lhs, const std::vector<float> &rhs)
{
	std::vector<float> sortedLhs = lhs;
	std::sort(sortedLhs.begin(), sortedLhs.end());
	
	float la = findAverage(lhs);
	float ra = findAverage(rhs);
	float direction = signOf(ra - la);
	
	int bestIndex = 0;
	float bestScore = 0;
	float bestValue = 0;
	for (int i=0; i<sortedLhs.size(); ++i)
	{
		float f = sortedLhs[i];
		
		float scoreL = (float)findNumNotInDirection(lhs, f, direction) / (float)lhs.size();
		float scoreR = (float)findNumNotInDirection(rhs, f, -direction) / (float)rhs.size();
		
		float score = scoreL + scoreR;
		
		if (score > bestScore)
		{
			bestScore = score;
			bestIndex = i;
			bestValue = f;
		}
	}
	
	return std::make_pair(bestValue, direction * bestScore);
}

std::pair<float, float> AlgoArmAxisMatcher::findWeightedMiddle (const std::vector<float> &lhs, const std::vector<float> &rhs)
{
	float la = findAverage(lhs);
	float ra = findAverage(rhs);

	return std::make_pair((ra+la)/2.0f, ra - la);
}

std::vector<float> AlgoArmAxisMatcher::findValues (const std::vector< std::vector<float> > &data, int at)
{
	std::vector<float> values;
	for (auto &row : data)
	{
		float value = valueAt(row, at);
		if (value == -1)
			continue;
		values.push_back(value);
	}
	
	return values;
}

void AlgoArmAxisMatcher::generateWeightValues (const char *pointingCsv, const char *closedCsv)
{
	auto closed = readFile(closedCsv);
	auto pointing = readFile(pointingCsv);

	std::vector<std::pair<float, float> > middleAndWeights;
	float totalWeight = 0;
	
	for (int at=0; at<20; ++at)
	{
		std::vector<float> closedValues, pointingValues;
		closedValues = findValues(closed, at);
		pointingValues = findValues(pointing, at);
		
		auto leastIntersection = findLeastIntersection(closedValues, pointingValues);
		auto weightedMiddle = findWeightedMiddle(closedValues, pointingValues);
		
		auto result = weightedMiddle;
			
		std::cout << "at " << at << " result: " << result.first << "," << result.second << std::endl;
		
		middleAndWeights.push_back(result);
		totalWeight += fabs(result.second);
	}
	
	
	for (auto &middleAndWeight : middleAndWeights)
		middleAndWeight.second = middleAndWeight.second / totalWeight;
	
	this->middleAndWeights = middleAndWeights;
	
	std::cout << "------------------------------------" << std::endl;
	std::cout << "------------------------------------" << std::endl;
	std::cout << "------------------------------------" << std::endl;
	std::cout << "------------------------------------" << std::endl;
	std::cout << "closed" << std::endl;

	testData(closed, false);

	std::cout << "------------------------------------" << std::endl;
	std::cout << "------------------------------------" << std::endl;
	std::cout << "------------------------------------" << std::endl;
	std::cout << "------------------------------------" << std::endl;
	std::cout << "pointing" << std::endl;

	testData(pointing, true);
	
	std::cout << "------------------------------------" << std::endl;
	std::cout << "------------------------------------" << std::endl;
	std::cout << "------------------------------------" << std::endl;
	std::cout << "------------------------------------" << std::endl;
	std::cout << "finalWeights" << std::endl;

	std::cout << "{\n";
	for (auto &e : middleAndWeights)
	{
		std::cout << "\t{ " << e.first << ", " << e.second << " },\n";
	}
	std::cout << "} ";
}

/*
int main (int argc, char *argv[])
{
	AlgoArmAxisMatcher matcher;
	matcher.generateWeightValues(
		"/Users/tprepscius/Desktop/data/point.csv",
		"/Users/tprepscius/Desktop/data/closed.csv"
	);
}
*/
