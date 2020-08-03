#include <stdio.h>
#include <string.h>

int w_isc7(char *s, int nrc, int year, int month, char *station)
{
	sprintf(s, "%2d%2d%4d%2d%2d%-84s",7, nrc, year, month, 1, station);

	return 0;
}
