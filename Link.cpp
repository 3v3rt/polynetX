#include "Link.h"

CLink::CLink(int p1, int p2)
{
	m_Point1 = p1;
	m_Point2 = p2;
	m_bActive = true;
}

void CLink::Activate()
{
	m_bActive = true;
}

void CLink::Deactivate()
{
	m_bActive = false;
}

bool CLink::Active()
{
	return m_bActive;
}

int CLink::P1()
{
	return m_Point1;
}

int CLink::P2()
{
	return m_Point2;
}
