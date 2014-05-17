#ifndef __POINT_H__
#define __POINT_H__

class CPnt
{
public:
	CPnt(double x, double y, double z, int Index);

	int Color();
	void Color(int c);

	double Pos(int axis);

private:
	int m_Color;
	int m_Index;
	double m_X;
	double m_Y;
	double m_Z;
};

#endif