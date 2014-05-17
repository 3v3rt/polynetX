#ifndef __DETECTOR_H__
#define __DETECTOR_H__

#include "Point.h"
#include "Link.h"
#include "ClusterConnection.h"

#include <iostream>
#include <vector>
#include <map>

#include <math.h>

using namespace std;

class Detector
{
public:

	void SetData(vector<CPnt> Points, vector<CLink> Links, vector<double> Size);
	void Analyze(int Direction);

	bool IsPercolating();
	bool IsNetwork();
	bool IsSelfConnected();

private:
	void ColorAllParticles();
	void DeactivatePeriodicLinks(int direction);
	void ColorLoop();
	int  CalculateColorSum();
	void IdentifyGroups(vector<vector<CPnt*>>* pGroups);
	bool FindSelfConnectedCluster();
	void IdentifyInterClusterConnection(int Direction, vector<CClusterConnection>* pClusterConnection);
	bool KillDeadEndClusters(vector<CClusterConnection>* pClusterConnection);
	bool BurnTest(vector<CClusterConnection>* pClusterConnection);
	size_t CountClusters(vector<CClusterConnection>* pClusterConnection);
	void PercolationCheck(int Direction);

private:
	vector<CPnt> m_Points;
	vector<CLink> m_Links;
	vector<double> m_Size;

	bool m_Percolating;
	bool m_Network;
	bool m_SelfConnected;
};

#endif