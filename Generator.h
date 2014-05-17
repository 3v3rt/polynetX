#ifndef __GENERATOR_H__
#define __GENERATOR_H__

#include "Point.h"
#include "Link.h"
#include "ClusterConnection.h"

#include <iostream>
#include <vector>
#include <map>
#include <math.h>

using namespace std;

class Generator
{
public:	
	void GenerateGrid(int ParticleCount, vector<CPnt>* pPoints, vector<CLink>* pLinks, vector<double> Dimensions);
	void GeneratePoly(int ParticleCount, vector<CPnt>* pPoints, vector<CLink>* pLinks, vector<double> Dimensions);
	void GeneratePoly(int ParticleCount, vector<CPnt>* pPoints, vector<CLink>* pLinks, vector<double> Dimensions, double ClickChance);
};

#endif