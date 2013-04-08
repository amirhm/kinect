//
//  AlgoArmAxisMatcher.h
//  NiteAndHandAttributes
//
//  Created by Timothy Prepscius on 3/27/13.
//  Copyright (c) 2013 Timothy Prepscius. All rights reserved.
//

#ifndef __NiteAndHandAttributes__AlgoArmAxisMatcher__
#define __NiteAndHandAttributes__AlgoArmAxisMatcher__

#include <vector>

class AlgoArmAxisMatcher {

public:
	static const double defaultWeights[20][2];
	static const int defaultNumWeights;

	std::vector<std::pair<float, float> > middleAndWeights;

protected:
	void testData (const std::vector<std::vector<float> > &data, bool correct);
	void printRow (const std::vector<float> &row);
	float findAverage (const std::vector<float> &v);
	int findNumNotInDirection (const std::vector<float> &v, float o, float d);
	std::pair<float, float> findLeastIntersection (const std::vector<float> &lhs, const std::vector<float> &rhs);
	std::pair<float, float> findWeightedMiddle (const std::vector<float> &lhs, const std::vector<float> &rhs);
	std::vector<float> findValues (const std::vector< std::vector<float> > &data, int at);

public:
	float valueAt (const std::vector<float> &segments, int i);
	float match (const std::vector<float> &segments);
	void generateWeightValues (const char *pointingCsv, const char *closedCsv);
	
	AlgoArmAxisMatcher ();
	AlgoArmAxisMatcher (double *weights, int size);
	
} ;

#endif /* defined(__NiteAndHandAttributes__AlgoArmAxisMatcher__) */
