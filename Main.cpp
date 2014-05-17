#include "Main.h"

using namespace std;

//#define SZX 10
//#define SZY 10
//#define SZZ 10
#define LNKFILE "link.txt"
#define POSFILE "pos.txt"



int main1(int argc, char** argv)
{
	FILE* fpMersenne = fopen("mersenne.seed", "rb");

	if (fpMersenne != NULL)
	{
		unsigned long seed;
		fread(&seed, sizeof(unsigned long), 1, fpMersenne);
		fclose(fpMersenne);
		init_genrand(seed);
	}

	FILE* pPos = fopen(argv[1], "rt");

	double Pos[3];
	std::vector<CPnt> Points;

	int Ctr = 0;

	double Max = 0;

	while (fscanf(pPos, "%lf %lf %lf", &(Pos[0]), &(Pos[1]), &(Pos[2])) == 3)
	{
		Points.push_back(CPnt(Pos[0], Pos[1], Pos[2], Ctr));
		Ctr++;

		if (Pos[0] > Max)
		{
			Max = Pos[0];
		}

		if (Pos[1] > Max)
		{
			Max = Pos[1];
		}

		if (Pos[2] > Max)
		{
			Max = Pos[2];
		}
	}

	fclose(pPos);

	std::vector<CLink> Links;

	FILE* pLnk = fopen(argv[2], "rt");

	int Link[2];

	while (fscanf(pLnk, "%d %d", &(Link[0]), &(Link[1])) == 2)
	{
		Links.push_back(CLink(Link[0], Link[1]));
	}

	fclose(pLnk);


	std::vector<double> Box;
	Box.push_back(Max + 0.0001);
	Box.push_back(Max + 0.0001);
	Box.push_back(Max + 0.0001);

	Detector detector;

	detector.SetData(Points, Links, Box);

	detector.Analyze(0);
	printf("X %s\n", detector.IsNetwork() ? "Y" : "N");
	detector.Analyze(1);
	printf("Y %s\n", detector.IsNetwork() ? "Y" : "N");
	detector.Analyze(2);
	printf("Z %s\n", detector.IsNetwork() ? "Y" : "N");

	return 0;
}

int main2(int argc, char** argv)
{
	FILE* fpMersenne = fopen("mersenne.seed", "rb");

	if (fpMersenne != NULL)
	{
		unsigned long seed;
		fread(&seed, sizeof(unsigned long), 1, fpMersenne);
		fclose(fpMersenne);
		init_genrand(seed);
	}

	if (argc != 7)
	{
		fprintf(stderr, "usage:\n polynetx NP X Y Z (g|p) iter\n");
		return -1;
	}

	int ParticleCount = atoi(argv[1]);

	vector<double> SystemSize;
	SystemSize.push_back(atof(argv[2]));
	SystemSize.push_back(atof(argv[3]));
	SystemSize.push_back(atof(argv[4]));

	int IterationCount = atoi(argv[6]);

	int PercStats[8] = {0,0,0,0,0,0,0,0};
	int NetStats[8] = {0,0,0,0,0,0,0,0};
	int SelfStats[8] = {0,0,0,0,0,0,0,0};

	for (int i = 0; i < IterationCount; i++)
	{
		vector<CPnt> Points;
		vector<CLink> Links;

		Generator g;

		if (argv[5][0] == 'p')
			g.GeneratePoly(ParticleCount, &Points, &Links, SystemSize, 0.1);
		else if (argv[5][0]=='g')
			g.GenerateGrid(ParticleCount, &Points, &Links, SystemSize);
		else
		{
			fprintf(stderr, "last arg should be p or g\n");
			return -1;
		}

		int NetStat = 0;
		int SelfStat = 0;
		int PercStat = 0;

		for (int Direction = 0; Direction < 3; Direction++)
		{
			Detector detector;
			detector.SetData(Points, Links, SystemSize);

			detector.Analyze(Direction);

			if (detector.IsPercolating())
			{
				PercStat += 1 << Direction;
			}

			if (detector.IsNetwork())
			{
				NetStat += 1 << Direction;
			}

			if (detector.IsSelfConnected())
			{
				SelfStat += 1 << Direction;
			}

			if (!detector.IsPercolating() && detector.IsNetwork())
			{
				fprintf(stderr, "Consistency fail!\n");
			}

			if (detector.IsSelfConnected() && !detector.IsNetwork())
			{
				fprintf(stderr, "Consistency fail!\n");
			}
		}

		PercStats[PercStat]++;
		NetStats[NetStat]++;
		SelfStats[SelfStat]++;
	}

	for (int i = 0; i < 8; i++)
	{
		cout << i << " " << PercStats[i] << " " << NetStats[i] << " " << SelfStats[i] << endl;
	}

	fpMersenne = fopen("mersenne.seed", "wb");
	unsigned long r = genrand_int32();
	fwrite(&r, sizeof(unsigned long), 1, fpMersenne);
	fclose(fpMersenne);
}

int main(int argc, char** argv)
{
	if (argc == 3)
	{
		return main1(argc, argv);
	}

	return main2(argc, argv);
}

//int main(int argc, char** argv)
//{
//	if (argc != 6)
//	{
//		fprintf(stderr, "usage:\n polynetx pos link\n");
//		return -1;
//	}
//
//	vector<double> SystemSize;
//
//	SystemSize.push_back(atof(argv[3]));
//	SystemSize.push_back(atof(argv[4]));
//	SystemSize.push_back(atof(argv[5]));
//
//	for (int Direction = 0; Direction < 3; Direction++)
//	{
//		vector<CPnt> Points;
//		vector<CLink> Links;
//
//		if (!ReadData(&Points, &Links, std::string(argv[1]), std::string(argv[2])))
//		{
//			cout << "Could not read data" << endl;
//			return -1;
//		}
//
//		DeactivatePeriodicLinks(Direction, &SystemSize, &Points, &Links);
//		ColorAllParticles(&Points);
//		ColorLoop(&Points, &Links);
//
//		if (FindSelfConnectedCluster(&Points, &Links))
//		{
//			cout << "Network in direction " << Direction << " " << " SC" << endl;
//			continue;
//		}
//
//		vector<vector<CPnt*>> Groups;
//		IdentifyGroups(&Groups, &Points);
//
//		//cout << Groups.size() << endl;
//
//		vector<CClusterConnection> InterClusterConnections;
//		IdentifyInterClusterConnection(Direction, &InterClusterConnections, &Points, &Links);
//		KillDeadEndClusters(&InterClusterConnections);
//
//		if (BurnTest(&InterClusterConnections))
//		{
//			cout << "Network in direction " << Direction << " BURN" << endl;
//		}
//		else
//		{
//			cout << "No Network in direction " << Direction << endl;
//		}
//	}
//}

bool ReadData(vector<CPnt>* pPoints, vector<CLink>* pLinks, std::string PosFile, std::string LinkFile)
{
	FILE* fp = fopen(PosFile.c_str(), "r");

	if (!fp)
	{
		return false;
	}

	double x,y,z;

	int Index = 0;

	while (fscanf(fp, "%lf %lf %lf\n", &x, &y, &z) == 3)
	{
		pPoints->push_back(CPnt(x, y, z, Index));
		Index++;
	}

	fclose(fp);

	fp = fopen(LinkFile.c_str(), "r");

	if (!fp)
	{
		return false;
	}

	int p1, p2;

	while (fscanf(fp, "%d %d\n", &p1, &p2) == 2)
	{
		pLinks->push_back(CLink(p1, p2));
	}

	fclose(fp);

	return true;
}

//void ColorAllParticles(vector<CPnt>* pPoints)
//{
//	for (unsigned int i = 0; i < pPoints->size(); i++)
//	{
//		(*pPoints)[i].Color(1000000 + i);
//	}
//}
//
//void DeactivatePeriodicLinks(int direction, vector<double>* pSize, vector<CPnt>* pPoints, vector<CLink>* pLinks)
//{
//	for (vector<CLink>::iterator iLink = pLinks->begin(); iLink != pLinks->end(); ++iLink)
//	{
//		if (fabs((*pPoints)[iLink->P1()].Pos(direction) - (*pPoints)[iLink->P2()].Pos(direction)) > (0.5 * (*pSize)[direction]))
//		{
//			iLink->Deactivate();
//		}
//	}
//}
//
//
//void ColorLoop(vector<CPnt>* pPoints, vector<CLink>* pLinks)
//{
//	//vector<vector<CPnt*>> Neighbours;
//
//	//Neighbours.resize(pPoints->size());
//
//	//for (vector<CLink>::iterator iLink = pLinks->begin(); iLink != pLinks->end(); ++iLink)
//	//{
//	//	Neighbours[iLink->P1()].push_back(&((*pPoints)[iLink->P2()]));
//	//	Neighbours[iLink->P2()].push_back(&((*pPoints)[iLink->P1()]));
//	//}
//
//	map<int, int> ColorMap;
//	map<int, int> ColorMapMap;
//	int ClusterCount = 1;
//	// Do a two step mapping
//	for (vector<CLink>::iterator iLink = pLinks->begin(); iLink != pLinks->end(); ++iLink)
//	{		
//		if (iLink->Active())
//		{			
//			int Color1 = (*pPoints)[iLink->P1()].Color();
//			int Color2 = (*pPoints)[iLink->P2()].Color();
//			int MaxColor = Color1 > Color2 ? Color1 : Color2;
//
//			if (Color1 < Color2)
//			{
//				(*pPoints)[iLink->P1()].Color(Color2);
//			}
//			else
//			{
//				(*pPoints)[iLink->P2()].Color(Color1);
//			}
//
//			// None found
//			if ((ColorMap.find(Color1) == ColorMap.end()) && (ColorMap.find(Color2) == ColorMap.end()))
//			{
//				ColorMap[MaxColor] = ClusterCount;
//				ClusterCount++;
//			} 
//			// Only Color2 found
//			else if ((ColorMap.find(Color1) == ColorMap.end()) && (ColorMap.find(Color2) != ColorMap.end()))
//			{
//				if (Color1 > Color2)
//				{
//					ColorMap[Color1] = ColorMap[Color2];
//				}
//			}			
//			// Only Color1 found
//			else if ((ColorMap.find(Color1) != ColorMap.end()) && (ColorMap.find(Color2) == ColorMap.end()))
//			{
//				if (Color2 > Color1)
//				{
//					ColorMap[Color2] = ColorMap[Color1];
//				}
//			}
//			// Both found, conflict, resolve by going all the way down and merging to lowest value
//			else  if ((ColorMap.find(Color1) != ColorMap.end()) && (ColorMap.find(Color2) != ColorMap.end()))
//			{
//				int Lowest1 = ColorMap[Color1];
//
//				while ((ColorMap.find(Lowest1) != ColorMap.end()) && (Lowest1 != ColorMap[Lowest1]))
//				{
//					Lowest1 = ColorMap[Lowest1];
//				}
//
//				int Lowest2 = ColorMap[Color2];
//
//				while ((ColorMap.find(Lowest2) != ColorMap.end()) && (Lowest2 != ColorMap[Lowest2]))
//				{
//					Lowest2 = ColorMap[Lowest2];
//				}
//
//				if (Lowest1 < Lowest2)
//				{
//					ColorMap[Lowest2] = Lowest1;
////					ColorMap[Lowest2] = ColorMap[Lowest1];
//				}
//				else
//				{
//					ColorMap[Lowest1] = Lowest2;
////					ColorMap[Lowest1] = ColorMap[Lowest2];
//				}
//
////DEBUG
////for (map<int, int>::iterator iMap = ColorMap.begin(); iMap != ColorMap.end(); ++iMap)
////{
////	if (iMap->first == 0)
////		printf("0 as key\n");
////	if (iMap->second >= 1000000)
////		printf("> 1000000 as value\n");
////}
////END DEBUG
//
//			}
//		}
//	}
//
//printf("CC %d\n", ClusterCount);
//
//	int i = 0;
//
//	for (vector<CPnt>::iterator iPoint = pPoints->begin(); iPoint != pPoints->end(); ++iPoint)
//	{		
//		//DEBUG
//		//continue;
//		//
//		i++;
//
//		if (ColorMap.find(iPoint->Color()) != ColorMap.end())
//		{	
//			int NewColor = ColorMap[iPoint->Color()];
//
//			while ((ColorMap.find(NewColor) != ColorMap.end()) && (NewColor != ColorMap[NewColor]))
//			{
//				NewColor = ColorMap[NewColor];
//			}
//
//			iPoint->Color(NewColor);//ColorMap[iPoint->Color()]);
//		}
//	}
//
////	cout << ClusterCount << endl;		
//	
//
//	//int NewCheckSum = -2;
//	//int OldCheckSum = CalculateColorSum(pPoints);
//
//	//while (NewCheckSum != OldCheckSum)
//	//{
//	//	for (vector<CLink>::iterator iLink = pLinks->begin(); iLink != pLinks->end(); ++iLink)
//	//	{
//	//		if (iLink->Active())
//	//		{
//	//			int P1 = iLink->P1();
//	//			int P2 = iLink->P2();
//
//	//			if ((*pPoints)[P1].Color() < (*pPoints)[P2].Color())
//	//			{
//	//				(*pPoints)[P1].Color((*pPoints)[P2].Color());
//	//			}
//	//			else if ((*pPoints)[P1].Color() > (*pPoints)[P2].Color())
//	//			{
//	//				(*pPoints)[P2].Color((*pPoints)[P1].Color());
//	//			}
//	//		}
//	//	}
//
//	//	OldCheckSum = NewCheckSum;
//	//	NewCheckSum = CalculateColorSum(pPoints);
//	//}
//}
//
//int CalculateColorSum(vector<CPnt>* pPoints)
//{
//	int Sum = 0;
//
//	for (vector<CPnt>::iterator iIt = pPoints->begin(); iIt != pPoints->end(); ++iIt)
//	{
//		Sum += iIt->Color();
//	}
//
//	return Sum;
//}
//
//bool FindSelfConnectedCluster(vector<CPnt>* pPoints, vector<CLink>* pLinks)
//{
//	for (vector<CLink>::iterator iLink = pLinks->begin(); iLink != pLinks->end(); ++iLink)
//	{
//		if (!iLink->Active())
//		{
//			if ((*pPoints)[iLink->P1()].Color() ==  (*pPoints)[iLink->P2()].Color())
//			{
//				return true;
//			}
//		}
//	}
//
//	return false;
//}
//
//void IdentifyGroups(vector<vector<CPnt*>>* pGroups, vector<CPnt>* pPoints)
//{
//	map<int, int> Mapping;
//
//	int GroupCount = 0;
//
//	for (size_t i = 0; i < pPoints->size(); i++)
//	{
//		if (Mapping.find((*pPoints)[i].Color()) == Mapping.end())
//		{
//			Mapping[(*pPoints)[i].Color()] = GroupCount;
//			GroupCount++;
//		}
//	}
//
//	pGroups->resize(GroupCount);
//
//	for (int i = 0; i < pPoints->size(); i++)
//	{
//		// Add all point pointers to the right group
//		(*pGroups)[Mapping[(*pPoints)[i].Color()]].push_back(&((*pPoints)[i]));
//		// Replace the point color with the groupID
//		(*pPoints)[i].Color(Mapping[(*pPoints)[i].Color()]);
//	}
//}
//
//void IdentifyInterClusterConnection(int Direction, vector<CClusterConnection>* pClusterConnection, vector<CPnt>* pPoints, vector<CLink>* pLinks)
//{
//	for (vector<CLink>::iterator iLink = pLinks->begin(); iLink != pLinks->end(); ++iLink)
//	{
//		if (iLink->Active())
//			continue;
//
//		int ClusterA = (*pPoints)[iLink->P1()].Color();		
//		int ClusterB = (*pPoints)[iLink->P2()].Color();
//
//		double PosA = (*pPoints)[iLink->P1()].Pos(Direction);
//		double PosB = (*pPoints)[iLink->P2()].Pos(Direction);
//
//		bool bALeftOfB = (PosA > PosB);
//
//		bool bFound = false;
//
//		for (std::vector<CClusterConnection>::iterator iCluster = pClusterConnection->begin(); iCluster != pClusterConnection->end(); ++iCluster)
//		{
//			if (iCluster->IsSame(ClusterA, ClusterB, bALeftOfB))
//			{
//				bFound = true;
//				break;
//			}
//
//			if (iCluster->IsOpposite(ClusterA, ClusterB, bALeftOfB)) // network!
//			{
//				throw "network";
//			}
//		}
//
//		if (!bFound)
//		{
//			pClusterConnection->push_back(CClusterConnection(ClusterA, ClusterB, bALeftOfB));
//		}
//	}
//}
//
//bool KillDeadEndClusters(vector<CClusterConnection>* pClusterConnection)
//{
//	bool bRemovedSome = false;
//
//	vector<CClusterConnection> NonDeadEnded;
//	map<int, int> ConnectionCount;
//
//	for (vector<CClusterConnection>::iterator iConn = pClusterConnection->begin(); iConn != pClusterConnection->end(); ++iConn)
//	{
//		if (ConnectionCount.find(iConn->Get1()) == ConnectionCount.end())
//		{
//			ConnectionCount[iConn->Get1()] = 0;
//		}
//
//		ConnectionCount[iConn->Get2()]++;
//		if (ConnectionCount.find(iConn->Get2()) == ConnectionCount.end())
//		{
//			ConnectionCount[iConn->Get2()] = 0;
//		}
//
//		ConnectionCount[iConn->Get2()]++;
//	}
//
//	for (vector<CClusterConnection>::iterator iConn = pClusterConnection->begin(); iConn != pClusterConnection->end(); ++iConn)
//	{
//		if ((ConnectionCount[iConn->Get1()] > 1) && (ConnectionCount[iConn->Get2()] > 1))
//		{
//			NonDeadEnded.push_back(*iConn);
//		}
//	}
//
//	if (NonDeadEnded.size() < pClusterConnection->size())
//	{
//		KillDeadEndClusters(&NonDeadEnded);
//	}
//
//	pClusterConnection->clear();
//
//	for (int i = 0; i < NonDeadEnded.size(); i++)
//	{
//		pClusterConnection->push_back(NonDeadEnded[i]);
//	}
//
//	return bRemovedSome;
//}
//
//bool BurnTest(vector<CClusterConnection>* pClusterConnection)
//{
//	std::map<int, int> BurnLocation;
//
//	if (pClusterConnection->size() == 0)
//	{
//		return false;
//	}
//
//	BurnLocation[(*pClusterConnection)[0].Get1()] = 0;
//
//	while (CountClusters(pClusterConnection) != BurnLocation.size())
//	{
//		bool bNewBurn = false;
//
//		for (vector<CClusterConnection>::iterator iConn = pClusterConnection->begin(); iConn != pClusterConnection->end(); ++iConn)
//		{	
//			// 1 is burning
//			if ((BurnLocation.find(iConn->Get1()) != BurnLocation.end()) &&
//				(BurnLocation.find(iConn->Get2()) == BurnLocation.end()))
//			{
//				BurnLocation[iConn->Get2()] = BurnLocation[iConn->Get1()] + (iConn->Is1LeftOf2() ? 1 : -1);
//				bNewBurn = true;
//			}			
//			// 2 is burning
//			else if ((BurnLocation.find(iConn->Get1()) == BurnLocation.end()) &&
//					 (BurnLocation.find(iConn->Get2()) != BurnLocation.end()))
//			{
//				BurnLocation[iConn->Get1()] = BurnLocation[iConn->Get2()] + (iConn->Is1LeftOf2() ? -1 : 1);
//				bNewBurn = true;
//			}
//			// Both are burning, check for network
//			else if ((BurnLocation.find(iConn->Get1()) != BurnLocation.end()) &&
//				     (BurnLocation.find(iConn->Get2()) != BurnLocation.end()))
//			{
//				if (iConn->Is1LeftOf2())
//				{
//					if ((BurnLocation[iConn->Get1()] + 1) != (BurnLocation[iConn->Get2()]))
//					{// Network!
//						return true;
//					}
//				}
//				else
//				{
//					if ((BurnLocation[iConn->Get1()] - 1) != (BurnLocation[iConn->Get2()]))
//					{// Network!
//						return true;
//					}
//				}
//			}
//		}
//
//		if (!bNewBurn)
//		{
//			for (vector<CClusterConnection>::iterator iConn = pClusterConnection->begin(); iConn != pClusterConnection->end(); ++iConn)
//			{
//				if ((BurnLocation.find(iConn->Get1()) == BurnLocation.end()) &&
//					(BurnLocation.find(iConn->Get2()) == BurnLocation.end()))
//				{
//					BurnLocation[iConn->Get1()] = 0;
//					break;
//				}
//			}
//		}
//	}
//
//	return false;
//}
//
//size_t CountClusters(vector<CClusterConnection>* pClusterConnection)
//{
//	map<int,int> Exist;
//
//	for (vector<CClusterConnection>::iterator iConn = pClusterConnection->begin(); iConn != pClusterConnection->end(); ++iConn)
//	{
//		Exist[iConn->Get1()] = 1;
//		Exist[iConn->Get2()] = 1;
//	}
//
//	return Exist.size();
//}