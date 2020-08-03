#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INT_UNDEF -12345
#define FLT_UNDEF -12345.0

#define DEGREE 176

#define HKM(hr) 6338.0*(hr) + 33.0

int sindex(char *, char *);

int r_iss_epic(char *s, int *mo, int *dy, int *hr, int *mn, int *sc,
  float *y, float *x, float *z, int *y_acc, int *x_acc, int *z_acc, char *dt)
{
	static char *month_name[13] = {
		"illegal month",
		"Jan.", "Feb.", "March", "April", "May", "June",
		"July", "Aug.", "Sept.", "Oct.", "Nov.", "Dec."
	};

	int	i, j;
	int	index;
	int	nf;
	char	mo_nm[16];
	float	rdepth;
	char	aux[80];
	char	slat[80], slon[80];
	int	ilat, rlat;
	int	ilon, rlon;

	*mo = *dy = *hr = *mn = *sc = INT_UNDEF;
	*y = *x = *z = FLT_UNDEF;
	dt[0] = '\0';

	/* Origin time */
	nf = sscanf(s, "%s %dd. %dh. %dm. %ds.", mo_nm, dy, hr, mn, sc);

	/*
	if (nf != 5) {
		fprintf(stderr, "r_iss_epic error: ");
		fprintf(stderr, "wrong format in origin time:\n%s\n", s);
		exit(1);
	}
	*/

	for (i = 1; i <= 12; i++) {
		if (strcmp(mo_nm, month_name[i]) == 0) {
			*mo = i;
			break;
		}
	}
	if (*mo == INT_UNDEF) {
		fprintf(stderr, "r_iss_epic error: ");
		fprintf(stderr, "wrong month:\n%s\n", mo_nm);
		exit(1);
	}

	/* Latitude and longitude */

	if ((index = sindex(s, "Epicentre")) == -1) {
		fprintf(stderr, "r_iss_epic error: ");
		fprintf(stderr, "wrong format for lat,lon:\n%s\n", s);
		exit(1);
	}
	sscanf(&s[index], "Epicentre %s %s", slat, slon);
	i = j = 0;
	/* integer part of latitude */
	while(isdigit(slat[i]))
		i++;
	/* degree character or decimal point */
	if ((unsigned char)slat[i] == DEGREE || slat[i] == '.') {
		slat[i] = '.';
		i++;
	}
	/* fractional part of latitude or minutes of latitude */
	while(isdigit(slat[i])) {
		i++;
		j++;
	}
	/* decimal latitude or degrees and minutes */
	if (slat[i] == '\'') {
		sscanf(slat, "%d.%d", &ilat, &rlat);
		*y = (float)ilat + (float)rlat/60.;
		*y_acc = 8;
		i++;
	} else {
		sscanf(slat, "%f", y);
		*y_acc = -j;
	}
	/* North, South or Equator */

	if (slat[i] == 'N' && slat[i+1] == '.') { /* North */
		;
	} else if (slat[i] == 'S' && slat[i+1] == '.') { /* South */
		*y = -(*y);
	} else if (*y == 0. && *y_acc == 0) { /* Equator */
		;
	} else {
		fprintf(stderr, "r_iss_epic error: ");
		fprintf(stderr, "wrong format for latitude: %s\n", slat);
		exit(1);
	}

	i = j = 0;
	/* integer part of longitude */
	while(isdigit(slon[i]))
		i++;
	/* degree character or decimal point */
	if ((unsigned char)slon[i] == DEGREE || slon[i] == '.') {
		slon[i] = '.';
		i++;
	}
	/* fractional part of longitude or minutes of longitude */
	while(isdigit(slon[i])) {
		i++;
		j++;
	}
	/* decimal longitude or degrees and minutes */
	if (slon[i] == '\'') {
		sscanf(slon, "%d.%d", &ilon, &rlon);
		*x = (float)ilon + (float)rlon/60.;
		*x_acc = 8;
		i++;
	} else {
		sscanf(slon, "%f", x);
		*x_acc = -j;
	}
	/* East, West or 180 */

	if (slon[i] == 'E' && slon[i+1] == '.') { /* East */
		;
	} else if (slon[i] == 'W' && slon[i+1] == '.') { /* West */
		*x = -(*x);
	} else if (*x == 180. && *x_acc == 0) { /* 180 */
		;
	} else {
		fprintf(stderr, "r_iss_epic error: ");
		fprintf(stderr, "wrong format for longitude: %s\n", slat);
		exit(1);
	}

	if (*x_acc != *y_acc) {
		if (*x == 180. || *y == 0.) {
			;
		} else {
			fprintf(stderr, "r_iss_epic warning: ");
			fprintf(stderr, "precision for latitude and longitude are different.\n%s\n", s);
			if (*x_acc > *y_acc) {
				*y_acc = *x_acc;
			} else {
				*x_acc = *y_acc;
			}
		}
	}

	/* Focal depth */

	if ((index = sindex(s, "Depth")) != -1) {
		if (strstr(s, "Depth of focus") != NULL) {
			sscanf(&s[index], "Depth of focus %s", aux);
			if (strstr(aux, "km.") == NULL) {
				nf = sscanf(aux, "%f", &rdepth);
				*z = 5.0*(float)floor((HKM(rdepth)+2.5)/5.0);
			} else {
				nf = sscanf(aux, "%fkm.", z);
			}
		} else if (strstr(s, "Depth about") != NULL) {
			sscanf(&s[index], "Depth about %s", aux);
			nf = sscanf(aux, "%fkm.", z);
		} else {
			sscanf(&s[index], "Depth %s", aux);
			if (strstr(aux, "km.") != NULL) {
				nf = sscanf(aux, "%fkm.", z);
			} else {
				fprintf(stderr, "r_iss_epic error: ");
				fprintf(stderr, "cannot understand Depth value: %s\n%s\n", aux, s);
				exit(1);
			}
		}
		if (nf == 0) {
			fprintf(stderr, "r_iss_epic error: ");
			fprintf(stderr, "wrong format for depth:\n%s\n", s);
			exit(1);
		}
	} else if (strstr(s, "Focus at Base") != NULL) {
		*z = 33.0;
	}

	return 0;
}
