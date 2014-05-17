#include "Point.h"

CPnt::CPnt(double x, double y, double z, int Index)
{
	m_X = x;
	m_Y = y;
	m_Z = z;
	m_Index = Index;
}

void CPnt::Color(int c)
{
	m_Color = c;
}

int CPnt::Color()
{
	return m_Color;
}

double CPnt::Pos(int axis)
{
	switch(axis)
	{
	case 0:
		return m_X;
	case 1:
		return m_Y;
	case 2:
		return m_Z;
	default:
		throw "wtf";
	}
}