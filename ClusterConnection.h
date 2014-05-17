#ifndef __CLUSTER_CONNECTION_H__
#define __CLUSTER_CONNECTION_H__

class CClusterConnection
{	
public:
	CClusterConnection(int Cluster1, int Cluster2, bool b1LeftOf2);
	
	bool IsSame(int Cluster1, int Cluster2, bool b1LeftOf2);
	bool IsOpposite(int Cluster1, int Cluster2, bool b1LeftOf2);
	int Get1();
	int Get2();
	bool Is1LeftOf2();

private:
	int m_Cluster1;
	int m_Cluster2;
	bool m_b1LeftOf2;
};

#endif