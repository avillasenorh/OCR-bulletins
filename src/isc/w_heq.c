#include <stdio.h>
#include <math.h>

#define NINT(x) (int)floor((x)+0.5)

/*******************************************************************************
* Function: w_heq.c
*******************************************************************************/

int w_heq(char *s, int year, int month, int day, int hour, int minute, float second,
  float lat, float lon, float depth, float mag, float imag, int ntel, float iscdepth,
  int igreg, int ndep)
{
	sprintf(s, " HEQ  %2d %2d %2d  %2d %2d %5.2f  %7.3f%8.3f %5.1f %3.1f%2d%3d%5.1f%3d%3d",
	year-1900, month, day, hour, minute, second, lat, lon, depth, mag, NINT(10.0*imag),
	ntel, iscdepth, igreg, ndep);

	return 0;
}
