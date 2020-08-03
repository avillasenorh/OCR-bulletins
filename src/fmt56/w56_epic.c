#include <math.h>
#include <stdio.h>
#include <string.h>

#define ABS(x) ( (x > 0) ? x : -x)

#define MAXLINE 128
#define INT_UNDEF -12345
#define FLT_UNDEF -12345.0
#define NINT(x) (int)floor((x)+0.5)
#define DEGREE 176

int w56_epic(char *card, int month, int day, int hour, int minute, int second,
		float lat, float lon, int lat_acc, int lon_acc, float rdepth, char *data)
{
	static char *month_name[13] = {
		"illegal month",
		"Jan.", "Feb.", "March", "April", "May", "June",
		"July", "Aug.", "Sept.", "Oct.", "Nov.", "Dec."

	};
	int	i;
	char	ns[3], ew[3];
	char	aux[MAXLINE];
	int	ilat, ilon;
	int	rlat, rlon;

	if (lat > 0.0) {
		strcpy(ns, "N.");
	} else if (lat < 0.0) {
		strcpy(ns, "S.");
	} else {
		strcpy(ns, "  ");
	}
	if (lon > 0.0) {
		strcpy(ew, "E.");
	} else if (lon < 0.0) {
		strcpy(ew, "W.");
	} else {
		strcpy(ew, "  ");
	}
	/* T0 */
	if (second == INT_UNDEF) {
		sprintf(card, "%s %dd. %dh. %dm.",
		month_name[month], day, hour, minute);
	} else {
		sprintf(card, "%s %dd. %dh. %dm. %ds.",
		month_name[month], day, hour, minute, second);
	}
	/* Epicentre */
	if (lat_acc == 0) {
		sprintf(aux, "  Epicentre %3.0f%-2s %4.0f%-2s", ABS(lat), ns, ABS(lon), ew);
	} else if (lat_acc == 1) {
		sprintf(aux, "  Epicentre %5.1f%-2s %6.1f%-2s", ABS(lat), ns, ABS(lon), ew);
	} else if (lat_acc == 2) {
		sprintf(aux, "  Epicentre %6.2f%-2s %7.2f%-2s", ABS(lat), ns, ABS(lon), ew);
	} else if (lat_acc == 8) {
		lat = ABS(lat);
		lon = ABS(lon);
		ilat = (int)lat;
		rlat = NINT(60.*(lat-ilat));
		ilon = (int)lon;
		rlon = NINT(60.*(lon-ilon));
		sprintf(aux, "  Epicentre %2d%c%2d\'%-2s %3d%c%2d\'%-2s",
		ilat, DEGREE, rlat, ns, ilon, DEGREE, rlon, ew);
	} else {
		sprintf(aux, "  Epicentre %f%-2s %f%-2s", ABS(lat), ns, ABS(lon), ew);
	}
	strcat(card, aux);
	/* Depth of focus */
	if (rdepth == FLT_UNDEF) {
		;
	} else {
		if (rdepth == 0.0) {
			strcpy(aux, "  Focus at Base of Superficial Layers.");
		} else {
			sprintf(aux, "  Depth of focus %5.3f.", rdepth);
		}
		strcat(card, aux);
	}
	/* comment string after epicenter */
	if (strlen(data) > 0) {
		sprintf(aux, "  %s\n", data);
	} else {
		strcpy(aux, "\n");
	}
	strcat(card, aux);

	return 0;
}
