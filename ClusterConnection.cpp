#include "ClusterConnection.h"
#include <iostream>

CClusterConnection::CClusterConnection(int Cluster1, int Cluster2, bool b1LeftOf2)
{
	m_Cluster1 = Cluster1;
	m_Cluster2 = Cluster2;
	m_b1LeftOf2 = b1LeftOf2;
}

bool CClusterConnection::IsSame(int Cluster1, int Cluster2, bool b1LeftOf2)
{
	if ((Cluster1 == m_Cluster1) && (Cluster2 == m_Cluster2) && (b1LeftOf2 == m_b1LeftOf2))
	{
		return true;
	}

	if ((Cluster1 == m_Cluster2) && (Cluster2 == m_Cluster1) && (b1LeftOf2 != m_b1LeftOf2))
	{
		return true;
	}

	return false;
}

bool CClusterConnection::IsOpposite(int Cluster1, int Cluster2, bool b1LeftOf2)
{
	if ((Cluster1 == m_Cluster1) && (Cluster2 == m_Cluster2) && (b1LeftOf2 != m_b1LeftOf2))
	{
		return true;
	}

	if ((Cluster1 == m_Cluster2) && (Cluster2 == m_Cluster1) && (b1LeftOf2 == m_b1LeftOf2))
	{
		return true;
	}

	return false;
}

int CClusterConnection::Get1()
{
	return m_Cluster1;
}

int CClusterConnection::Get2()
{
	return m_Cluster2;
}

bool CClusterConnection::Is1LeftOf2()
{
	return m_b1LeftOf2;
}