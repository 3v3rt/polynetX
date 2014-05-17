#include "Detector.h"

using namespace std;


void Detector::SetData(vector<CPnt> Points, vector<CLink> Links, vector<double> Size)
{
	m_Points = Points;
	m_Links = Links;
	m_Size = Size;

	m_SelfConnected = false;
	m_Percolating = false;
	m_Network = false;
}

void Detector::Analyze(int Direction)
{
	DeactivatePeriodicLinks(Direction);
	ColorAllParticles();
	ColorLoop();

	if (FindSelfConnectedCluster())
	{
		m_Network = true;
		m_Percolating = true;
		m_SelfConnected = true;
		return;
	}

	vector<vector<CPnt*>> Groups;
	IdentifyGroups(&Groups);

	vector<CClusterConnection> InterClusterConnections;

	PercolationCheck(Direction);

	try
	{
		IdentifyInterClusterConnection(Direction, &InterClusterConnections);
	}
	catch (...)
	{
		m_Network = true;
		return;
	}

	KillDeadEndClusters(&InterClusterConnections);

	if (BurnTest(&InterClusterConnections))
	{
		m_Network = true;
	}
	else
	{
		m_Network = false;
	}	
}

bool Detector::IsNetwork()
{
	return m_Network;
}

bool Detector::IsPercolating()
{
	return m_Percolating;
}

bool Detector::IsSelfConnected()
{
	return m_SelfConnected;
}


void Detector::ColorAllParticles()
{
	for (unsigned int i = 0; i < m_Points.size(); i++)
	{
		m_Points[i].Color(1000000 + i);
	}
}

void Detector::DeactivatePeriodicLinks(int Direction)
{
	for (vector<CLink>::iterator iLink = m_Links.begin(); iLink != m_Links.end(); ++iLink)
	{
		if (fabs(m_Points[iLink->P1()].Pos(Direction) - m_Points[iLink->P2()].Pos(Direction)) > (0.5 * m_Size[Direction]))
		{
			iLink->Deactivate();
		}
	}
}


void Detector::ColorLoop()
{
	map<int, int> ColorMap;
	//map<int, int> ColorMapMap;
	int ClusterCount = 1;

	// Do a two step mapping
	for (vector<CLink>::iterator iLink = m_Links.begin(); iLink != m_Links.end(); ++iLink)
	{		
		if (iLink->Active())
		{			
			int Color1 = m_Points[iLink->P1()].Color();
			int Color2 = m_Points[iLink->P2()].Color();
			int MaxColor = Color1 > Color2 ? Color1 : Color2;

			if (Color1 < Color2)
			{
				m_Points[iLink->P1()].Color(Color2);
			}
			else
			{
				m_Points[iLink->P2()].Color(Color1);
			}

			// None found			
			if ((ColorMap.find(Color1) == ColorMap.end()) && (ColorMap.find(Color2) == ColorMap.end()))
			{
				ColorMap[MaxColor] = ClusterCount;
				ClusterCount++;
			} 
			// Only Color2 found
			else if ((ColorMap.find(Color1) == ColorMap.end()) && (ColorMap.find(Color2) != ColorMap.end()))
			{
				if (Color1 > Color2)
				{
					ColorMap[Color1] = ColorMap[Color2];
				}
			}			
			// Only Color1 found
			else if ((ColorMap.find(Color1) != ColorMap.end()) && (ColorMap.find(Color2) == ColorMap.end()))
			{
				if (Color2 > Color1)
				{
					ColorMap[Color2] = ColorMap[Color1];
				}
			}
			// Both found, conflict, resolve by going all the way down and merging to lowest value
			else  if ((ColorMap.find(Color1) != ColorMap.end()) && (ColorMap.find(Color2) != ColorMap.end()))
			{
				int Lowest1 = ColorMap[Color1];

				while ((ColorMap.find(Lowest1) != ColorMap.end()) && (Lowest1 != ColorMap[Lowest1]))
				{
					Lowest1 = ColorMap[Lowest1];
				}

				int Lowest2 = ColorMap[Color2];

				while ((ColorMap.find(Lowest2) != ColorMap.end()) && (Lowest2 != ColorMap[Lowest2]))
				{
					Lowest2 = ColorMap[Lowest2];
				}

				if (Lowest1 < Lowest2)
				{
					ColorMap[Lowest2] = Lowest1;
				}
				else
				{
					ColorMap[Lowest1] = Lowest2;
				}
			}
		}
	}

	//int i = 0;

	for (vector<CPnt>::iterator iPoint = m_Points.begin(); iPoint != m_Points.end(); ++iPoint)
	{		
		//i++;

		if (ColorMap.find(iPoint->Color()) != ColorMap.end())
		{	
			int NewColor = ColorMap[iPoint->Color()];

			while ((ColorMap.find(NewColor) != ColorMap.end()) && (NewColor != ColorMap[NewColor]))
			{
				NewColor = ColorMap[NewColor];
			}

			iPoint->Color(NewColor);
		}
	}
}

int Detector::CalculateColorSum()
{
	int Sum = 0;

	for (vector<CPnt>::iterator iIt = m_Points.begin(); iIt != m_Points.end(); ++iIt)
	{
		Sum += iIt->Color();
	}

	return Sum;
}

bool Detector::FindSelfConnectedCluster()
{
	for (vector<CLink>::iterator iLink = m_Links.begin(); iLink != m_Links.end(); ++iLink)
	{
		if (!iLink->Active())
		{
			if (m_Points[iLink->P1()].Color() ==  m_Points[iLink->P2()].Color())
			{
				return true;
			}
		}
	}

	return false;
}

void Detector::IdentifyGroups(vector<vector<CPnt*>>* pGroups)
{
	map<int, int> Mapping;

	int GroupCount = 0;

	for (size_t i = 0; i < m_Points.size(); i++)
	{
		if (Mapping.find(m_Points[i].Color()) == Mapping.end())
		{
			Mapping[m_Points[i].Color()] = GroupCount;
			GroupCount++;
		}
	}

	pGroups->resize(GroupCount);

	for (int i = 0; i < m_Points.size(); i++)
	{
		// Add all point pointers to the right group
		(*pGroups)[Mapping[m_Points[i].Color()]].push_back(&(m_Points[i]));
		// Replace the point color with the groupID
		m_Points[i].Color(Mapping[m_Points[i].Color()]);
	}
}

void Detector::PercolationCheck(int Direction)
{
	std::map<int, int> StickLeft;
	std::map<int, int> StickRight;

	// iterate over all links, inactive links are for the direction we're testing
	for (vector<CLink>::iterator iLink = m_Links.begin(); iLink != m_Links.end(); ++iLink)
	{
		if (iLink->Active())
			continue;

		int ClusterA = m_Points[iLink->P1()].Color();		
		int ClusterB = m_Points[iLink->P2()].Color();

		double PosA = m_Points[iLink->P1()].Pos(Direction);
		double PosB = m_Points[iLink->P2()].Pos(Direction);

		bool bALeftOfB = (PosA > PosB);

		if (bALeftOfB) // So A stick to the right, B to the left
		{
			StickLeft[ClusterB] = 1;
			StickRight[ClusterA] = 1;
		}
		else
		{
			StickLeft[ClusterA] = 1;
			StickRight[ClusterB] = 1;
		}
	}

	// Now check for overlap in the two stick maps
	for (std::map<int, int>::iterator iStick = StickLeft.begin(); iStick != StickLeft.end(); ++iStick)
	{
		if (StickRight.find(iStick->first) != StickRight.end())
		{
			m_Percolating = true;
		}
	}
}

void Detector::IdentifyInterClusterConnection(int Direction, vector<CClusterConnection>* pClusterConnection)
{
	for (vector<CLink>::iterator iLink = m_Links.begin(); iLink != m_Links.end(); ++iLink)
	{
		if (iLink->Active())
			continue;

		int ClusterA = m_Points[iLink->P1()].Color();		
		int ClusterB = m_Points[iLink->P2()].Color();

		double PosA = m_Points[iLink->P1()].Pos(Direction);
		double PosB = m_Points[iLink->P2()].Pos(Direction);

		bool bALeftOfB = (PosA > PosB);

		bool bFound = false;

		for (std::vector<CClusterConnection>::iterator iCluster = pClusterConnection->begin(); iCluster != pClusterConnection->end(); ++iCluster)
		{
			if (iCluster->IsSame(ClusterA, ClusterB, bALeftOfB))
			{
				bFound = true;
				break;
			}

			if (iCluster->IsOpposite(ClusterA, ClusterB, bALeftOfB)) // network!
			{
				throw "network";
			}
		}

		if (!bFound)
		{
			pClusterConnection->push_back(CClusterConnection(ClusterA, ClusterB, bALeftOfB));
		}
	}
}

bool Detector::KillDeadEndClusters(vector<CClusterConnection>* pClusterConnection)
{
	bool bRemovedSome = false;

	vector<CClusterConnection> NonDeadEnded;
	map<int, int> ConnectionCount;

	for (vector<CClusterConnection>::iterator iConn = pClusterConnection->begin(); iConn != pClusterConnection->end(); ++iConn)
	{
		if (ConnectionCount.find(iConn->Get1()) == ConnectionCount.end())
		{
			ConnectionCount[iConn->Get1()] = 0;
		}

		ConnectionCount[iConn->Get2()]++;
		if (ConnectionCount.find(iConn->Get2()) == ConnectionCount.end())
		{
			ConnectionCount[iConn->Get2()] = 0;
		}

		ConnectionCount[iConn->Get2()]++;
	}

	for (vector<CClusterConnection>::iterator iConn = pClusterConnection->begin(); iConn != pClusterConnection->end(); ++iConn)
	{
		if ((ConnectionCount[iConn->Get1()] > 1) && (ConnectionCount[iConn->Get2()] > 1))
		{
			NonDeadEnded.push_back(*iConn);
		}
	}

	if (NonDeadEnded.size() < pClusterConnection->size())
	{
		KillDeadEndClusters(&NonDeadEnded);
	}

	pClusterConnection->clear();

	for (int i = 0; i < NonDeadEnded.size(); i++)
	{
		pClusterConnection->push_back(NonDeadEnded[i]);
	}

	return bRemovedSome;
}

bool Detector::BurnTest(vector<CClusterConnection>* pClusterConnection)
{
	std::map<int, int> BurnLocation;

	if (pClusterConnection->size() == 0)
	{
		return false;
	}

	BurnLocation[(*pClusterConnection)[0].Get1()] = 0;

	while (CountClusters(pClusterConnection) != BurnLocation.size())
	{
		bool bNewBurn = false;

		for (vector<CClusterConnection>::iterator iConn = pClusterConnection->begin(); iConn != pClusterConnection->end(); ++iConn)
		{	
			// 1 is burning
			if ((BurnLocation.find(iConn->Get1()) != BurnLocation.end()) &&
				(BurnLocation.find(iConn->Get2()) == BurnLocation.end()))
			{
				BurnLocation[iConn->Get2()] = BurnLocation[iConn->Get1()] + (iConn->Is1LeftOf2() ? 1 : -1);
				bNewBurn = true;
			}			
			// 2 is burning
			else if ((BurnLocation.find(iConn->Get1()) == BurnLocation.end()) &&
					 (BurnLocation.find(iConn->Get2()) != BurnLocation.end()))
			{
				BurnLocation[iConn->Get1()] = BurnLocation[iConn->Get2()] + (iConn->Is1LeftOf2() ? -1 : 1);
				bNewBurn = true;
			}
			// Both are burning, check for network
			else if ((BurnLocation.find(iConn->Get1()) != BurnLocation.end()) &&
				     (BurnLocation.find(iConn->Get2()) != BurnLocation.end()))
			{
				if (iConn->Is1LeftOf2())
				{
					if ((BurnLocation[iConn->Get1()] + 1) != (BurnLocation[iConn->Get2()]))
					{// Network!
						return true;
					}
				}
				else
				{
					if ((BurnLocation[iConn->Get1()] - 1) != (BurnLocation[iConn->Get2()]))
					{// Network!
						return true;
					}
				}
			}
		}

		if (!bNewBurn)
		{
			for (vector<CClusterConnection>::iterator iConn = pClusterConnection->begin(); iConn != pClusterConnection->end(); ++iConn)
			{
				if ((BurnLocation.find(iConn->Get1()) == BurnLocation.end()) &&
					(BurnLocation.find(iConn->Get2()) == BurnLocation.end()))
				{
					BurnLocation[iConn->Get1()] = 0;
					break;
				}
			}
		}
	}

	return false;
}

size_t Detector::CountClusters(vector<CClusterConnection>* pClusterConnection)
{
	map<int,int> Exist;

	for (vector<CClusterConnection>::iterator iConn = pClusterConnection->begin(); iConn != pClusterConnection->end(); ++iConn)
	{
		Exist[iConn->Get1()] = 1;
		Exist[iConn->Get2()] = 1;
	}

	return Exist.size();
}