#include <stdio.h>
#include <string.h>
#include <math.h>

#define MAXLINE 96
#define NINT(x) (int)floor((x)+0.5)

/*******************************************************************************
nrc       = next record category/format
year      = reference year
month     = reference month
day       = day number
hour      = hours
minute    = minutes
second    = seconds
sec_acc   = precision of time
#agency number
#prime estimate flag
lat       = event latitude
lat_acc   = precision of latitude
lon       = event longitude
lon_acc   = precision of longitude
depth     = focal depth
depth_acc = precision of depth
#magnitude
#magnitude (end of range)?
#precision of magnitude
#type of magnitude
#number of magnitude observations
#standard error for magnitude
#precision of standard error
igreg   = geographycal region number
isreg   = seismic region number
nobs    = number of observations
#standard deviation of one observation
#precision of standard deviation
#number of standard deviation observations
*******************************************************************************/

int w_isc1(char *s, int nrc, int year, int month, int day, int hour, int minute,
  float second, int sec_acc, float lat, int lat_acc, float lon, int lon_acc,
  float depth, int depth_acc, int igreg, int isreg, int nobs)
{
	int	agency = 0;
	char	flag = 'A';
	char	aux[MAXLINE];

	/* origin time, coordinates, and focal depth */
	sprintf(aux, "%2d%2d%4d%2d%2d%2d%2d%4d%2d%3d%c%7d%2d%8d%2d%4d%2d",
	1, nrc, year, month, day, hour, minute, NINT(second*100), sec_acc, agency, flag,
	NINT(lat*10000), lat_acc, NINT(lon*10000), lon_acc, NINT(depth*10), depth_acc);
	strcpy(s, aux);

	/* ignore all magnitude information
	 write only undefined precision for magnitude and magnitude standard error */
	strcat(s, "        99         99");

	/* geographic/seismic region, and number of observations */
	sprintf(aux, "%4d%3d%4d", igreg, isreg, nobs);
	strcat(s, aux);

	return 0;
}
