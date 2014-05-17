#ifndef __MAIN_H__
#define __MAIN_H__

#include <iostream>
#include <vector>
#include <map>

#include <math.h>


#include "Point.h"
#include "Link.h"
#include "ClusterConnection.h"
#include "Detector.h"
#include "Generator.h"
#include "Mersenne.h"

using namespace std;

bool ReadData(vector<CPnt>* pPoints, vector<CLink>* pLinks, string PosFile, string LinkFile);
void ColorAllParticles(vector<CPnt>* pPoints);
void DeactivatePeriodicLinks(int direction, vector<double>* pSize, vector<CPnt>* pPoints, vector<CLink>* pLinks);
void ColorLoop(vector<CPnt>* pPoints, vector<CLink>* pLinks);
int  CalculateColorSum(vector<CPnt>* pPoints);
void IdentifyGroups(vector<vector<CPnt*>>* pGroups, vector<CPnt>* pPoints);
bool FindSelfConnectedCluster(vector<CPnt>* pPoints, vector<CLink>* pLinks);
void IdentifyInterClusterConnection(int Direction, vector<CClusterConnection>* pClusterConnection, vector<CPnt>* pPoints, vector<CLink>* pLinks);
bool KillDeadEndClusters(vector<CClusterConnection>* pClusterConnection);
bool BurnTest(vector<CClusterConnection>* pClusterConnection);
size_t CountClusters(vector<CClusterConnection>* pClusterConnection);

#endif