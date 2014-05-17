#ifndef __LINK_H__
#define __LINK_H__

class CLink
{
public:
	CLink(int p1, int p2);

	void Activate();
	void Deactivate();
	bool Active();
	int P1();
	int P2();

private:
	int m_Point1;
	int m_Point2;
	bool m_bActive;
};

#endif