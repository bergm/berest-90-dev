
#include "helper.h"


using namespace Tools;

int Tools::date2doy(int day, int month)
{
	static const int daysInMonth[] = { 0, 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30};
	int doy = 0;
	for(int m = 1; m <= month; m++)
		doy += daysInMonth[m];
	doy += day;
	return doy;
}

std::pair<int, int> Tools::doy2date(int doy)
{
	static const int daysInMonth[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	int m = 1;
	for(; doy-daysInMonth[m]>0; m++)
		doy -= daysInMonth[m];

	return std::make_pair(doy, m);
}
