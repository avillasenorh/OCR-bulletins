#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/***********************************************************************
* Function: g56_epic
************************************************************************
************************************************************************
************************************************************************
***********************************************************************/

#define INT_UNDEF -12345
#define FLT_UNDEF -12345.0

#define DEGREE 176

int trim(char s[]);
/*int sindex(char s[], char t[]);*/
int sindex(char *s, char *t);

int g56_epic(char *line, int *month, int *day, int *hour, int *minute,
             int *second, float *lat, float *lon, int *lat_acc, int *lon_acc, float *rdepth, char *data)
{
	static char *month_name[13] = {
		"illegal month",
		"Jan.", "Feb.", "March", "April", "May", "June",
		"July", "Aug.", "Sept.", "Oct.", "Nov.", "Dec."

	};

	int	n;
	int	n1;
	char	mn[12];
	char	aux[20];

	int	i, j;
	int	ird;
	int	rdepth_acc;

	int	ideg, imin;
	int	ilat, ilon;
	float	rlat, rlon;
	char	deg[20], min[20];

	*month = *day = *hour = *minute = *second = INT_UNDEF;
	*lat = *lon = *rdepth = FLT_UNDEF;
	*lat_acc = *lon_acc = 0;
	data[0] = '\0';
	
	/* this does not work when e.g. 5ls. instead of 51s. (reads 5 seconds instead of error)
	n = sscanf(line, "%s %dd. %dh. %dm. %ds.", mn, day, hour, minute, second);
	fprintf(stderr, "%s %dd. %dh. %dm. %ds.\n", mn, *day, *hour, *minute, *second);
	if (n != 5) {
		fprintf(stderr, "Error reading origin time: %s\n", line);
		return -1;
	}
	*/

	sscanf(line, "%s", mn);
	for (i = 1; i < 13; i++) {
		if (strcmp(mn, month_name[i]) == 0) {
			*month = i;
			break;
		}
	}
	if (*month == INT_UNDEF) {
                fprintf(stderr, "get_hypo error: ");
                fprintf(stderr, "month name not found%s\n%s", mn, line);
                return 1;
	}

	/* decode day */
	i = 0;
	while(! isspace(line[i])) /* skip first word = month name */
		i++;
	while(isspace(line[i])) /* skip blanks. next field = day  */
		i++;
	j = 0;
	while(isdigit(line[i]))
		aux[j++] = line[i++];
	aux[j] = '\0';
	if (j < 1 || j > 2 || line[i] != 'd' || line[i+1] != '.') {
                fprintf(stderr, "get_hypo error: ");
                fprintf(stderr, "wrong format for day\n%s", line);
                return 1;
	}
	sscanf(aux, "%d", day);
	i += 2;
	/* decode hour */
	while(isspace(line[i])) /* skip blanks. next field = hour  */
		i++;
	j = 0;
	while(isdigit(line[i]))
		aux[j++] = line[i++];
	aux[j] = '\0';
	if (j < 1 || j > 2 || line[i] != 'h' || line[i+1] != '.') {
                fprintf(stderr, "get_hypo error: ");
                fprintf(stderr, "wrong format for hour\n%s", line);
                return 1;
	}
	sscanf(aux, "%d", hour);
	i += 2;
	/* decode minute */
	while(isspace(line[i])) /* skip blanks. next field = minute  */
		i++;
	j = 0;
	while(isdigit(line[i]))
		aux[j++] = line[i++];
	aux[j] = '\0';
	if (j < 1 || j > 2 || line[i] != 'm' || line[i+1] != '.') {
                fprintf(stderr, "get_hypo error: ");
                fprintf(stderr, "wrong format for minute\n%s", line);
                return 1;
	}
	sscanf(aux, "%d", minute);
	i += 2;
	/* decode second */
	while(isspace(line[i])) /* skip blanks. next field = second  */
		i++;
	if (sindex(line, "Epicentre") == i) {
		;
                /*fprintf(stderr, "get_hypo warning: ");
                fprintf(stderr, "no seconds in origin time\n%s", line);*/
	} else {
		j = 0;
		while(isdigit(line[i]))
			aux[j++] = line[i++];
		aux[j] = '\0';
		if (j < 1 || j > 2 || line[i] != 's' || line[i+1] != '.') {
			fprintf(stderr, "get_hypo error: ");
			fprintf(stderr, "wrong format for second\n%s", line);
			return 1;
		}
		sscanf(aux, "%d", second);
		i += 2;
	}

	/* Decode latitude */

	/* i is now character position in line */

	i = sindex(line, "Epicentre");
        if (n1 == -1) {
                fprintf(stderr, "get_hypo: Unable to decode Epicentre line\n");
                fprintf(stderr, "%s", line);
                return 1;
        }
	/* position i just after "Epicentre" */
	i += strlen("Epicentre");

	while (isspace(line[i]))
		i++;

	j = 0;
	while(isdigit(line[i]))
		aux[j++] = line[i++];
	if (j < 1) {
		fprintf(stderr, "g56_epic error: ");
		fprintf(stderr, "invalid character in latitude:\n%s", line);
		exit(1);
	}

	/* degree character and decimal point */

	if ((unsigned char)line[i] == DEGREE || line[i] == 'o' || line[i] == '.' || line[i] == '-') {
		aux[j++] = '.';
		i++;
	} else {
		fprintf(stderr, "g56_epic warning: ");
		fprintf(stderr, "degree character missing in latitude:\n%s", line);
	}

	/* advance blank spaces after degree, if any */
	while (isspace(line[i]))
		i++;
	*lat_acc = 0;
	while(isdigit(line[i])) {
		aux[j++] = line[i++];
		(*lat_acc)++;
	}
	if (*lat_acc == 0) {
		fprintf(stderr, "g56_epic warning: ");
		fprintf(stderr, "latitude has no fractionary part!:\n%s", line);
	}
	aux[j] = '\0';

	/* Special case: degrees and minutes */

	if (line[i] == '\'') {
		j = ideg = 0;
		while(aux[j] != '.') {
			deg[ideg++] = aux[j++];
		}
		deg[ideg] = '\0';
		sscanf(deg, "%d", &ilat);
		j++;
		imin = 0;
		while(isdigit(aux[j])) {
			min[imin++] = aux[j++];
		}
		min[imin] = '\0';
		sscanf(min, "%f", &rlat);
		*lat = (float)ilat + rlat/60.;
		*lat_acc = 8;
		i++;
	} else {
		sscanf(aux, "%f", lat);
	}

	if (line[i] == 'N') {
		i++;
	} else if (line[i] == 'S') {
		*lat = -(*lat);
		i++;
	} else if (isspace(line[i])) {
		if (*lat == 0.0) {
			fprintf(stderr, "g56_epic warning: ");
			fprintf(stderr, "lat = 0.0 !!:\n%s", line);
			i++;
		} else {
			fprintf(stderr, "g56_epic error: ");
			fprintf(stderr, "N-S character missing:\n%s", line);
			exit(1);
		}
	} else {
		fprintf(stderr, "g56_epic error: ");
		fprintf(stderr, "Invalid N-S character: %c\n%s", line[i], line);
		exit(1);
	}

	/* Final point after N-S (no space allowed between N-S and point) */
	if (line[i] == '.') {
		i++;
	} else {
		fprintf(stderr, "g56_epic error: ");
		fprintf(stderr, "No point after latitude\n%s", line);
		exit(1);
	}

	/* Decode longitude */

	while (isspace(line[i]))
		i++;

	j = 0;
	aux[0] = '\0';
	while(isdigit(line[i]))
		aux[j++] = line[i++];
	if (j < 1) {
		fprintf(stderr, "g56_epic error: ");
		fprintf(stderr, "invalid character in longitude:\n%s", line);
		exit(1);
	}

	/* degree character and decimal point */

	if ((unsigned char)line[i] == DEGREE || line[i] == 'o' || line[i] == '.' || line[i] == '-') {
		aux[j++] = '.';
		i++;
	} else {
		fprintf(stderr, "g56_epic warning: ");
		fprintf(stderr, "degree character missing in longitude:\n%s", line);
	}

	/* advance blank spaces after degree, if any */
	while (isspace(line[i]))
		i++;
	*lon_acc = 0;
	while(isdigit(line[i])) {
		aux[j++] = line[i++];
		(*lon_acc)++;
	}
	if (*lon_acc == 0) {
		fprintf(stderr, "g56_epic warning: ");
		fprintf(stderr, "longitude has no fractionary part!:\n%s", line);
	}
	aux[j] = '\0';

	/* Special case: degrees and minutes */

	if (line[i] == '\'') {
		j = ideg = 0;
		while(aux[j] != '.') {
			deg[ideg++] = aux[j++];
		}
		deg[ideg] = '\0';
		sscanf(deg, "%d", &ilon);
		j++;
		imin = 0;
		while(isdigit(aux[j])) {
			min[imin++] = aux[j++];
		}
		min[imin] = '\0';
		sscanf(min, "%f", &rlon);
		*lon = (float)ilon + rlon/60.;
		*lon_acc = 8;
		i++;
	} else {
		sscanf(aux, "%f", lon);
	}

	if (line[i] == 'E') {
		i++;
	} else if (line[i] == 'W') {
		*lon = -(*lon);
		i++;
	} else if (isspace(line[i])) {
		if (*lon == 0.0 || *lon == 180.0) {
			fprintf(stderr, "g56_epic warning: ");
			fprintf(stderr, "lon = 0 or lon = 180 !!:\n%s", line);
			i++;
		} else {
			fprintf(stderr, "g56_epic error: ");
			fprintf(stderr, "E-W character missing:\n%s", line);
			exit(1);
		}
	} else {
		fprintf(stderr, "g56_epic error: ");
		fprintf(stderr, "Invalid E-W character: %c\n%s", line[i], line);
		exit(1);
	}

	/* Final point after E-W (no space allowed between E-W and point) */
	if (line[i] == '.') {
		i++;
	} else {
		fprintf(stderr, "g56_epic error: ");
		fprintf(stderr, "No point after longitude\n%s", line);
		exit(1);
	}

	/* verify that both accuracies (lat and lon) are identical */

	if (*lat_acc != *lon_acc) {
		fprintf(stderr, "g56_epic warning: ");
		fprintf(stderr, "Different accuracy for latitude and longitude\n%s", line);
		if (*lat_acc > *lon_acc)
			*lon_acc = *lat_acc;
		else if (*lat_acc < *lon_acc)
			*lat_acc = *lon_acc;
	}

	/* Find if depth is in epicenter line */
	while (isspace(line[i]))
		i++;

	ird = 0;
	if ((ird=sindex(line, "Depth of focus")) >= 0 && strstr(line, "km.") == NULL) {
		if (ird != i) {
			fprintf(stderr, "g56_epic error: ");
			fprintf(stderr, "Depth must follow epicenter inmediately\n%s", line);
			exit(1);
		}
		i += strlen("Depth of focus");
		/* decode h(R) or rdepth */
		while (isspace(line[i]) || line[i] == '=')
			i++;
		j = 0;
		aux[0] = '\0';
		if (line[i] != '0') {
			fprintf(stderr, "g56_epic error: ");
			fprintf(stderr, "Depth must start with a 0\n%s", line);
			exit(1);
		} else {
			aux[j++] = line[i++];
		}
		if (line[i] == '0') {
			aux[j++] = '.';
		} else if (line[i] == '-' || line[i] == '.') {
			aux[j++] = '.';
			i++;
		} else {
			fprintf(stderr, "g56_epic error: ");
			fprintf(stderr, "invalid character in depth of focus\n%s", line);
			exit(1);
		}
		rdepth_acc = 0;
		while (isdigit(line[i])) {
			aux[j++] = line[i++];
			rdepth_acc++;
		}
		if (rdepth_acc < 3) {
			fprintf(stderr, "g56_epic error: ");
			fprintf(stderr, "depth accuracy must be at least 3 digits\n%s", line);
			exit(1);
		}
		aux[j] = '\0';
		sscanf(aux, "%f", rdepth);
		if (line[i] == 'R')
			i++;
		if (line[i] == '.') {
			i++;
		} else if (isspace(line[i])) {
			fprintf(stderr, "g56_epic warning: ");
			fprintf(stderr, "period after depth of focus missing:\n%s", line);
		} else {
			fprintf(stderr, "g56_epic error: ");
			fprintf(stderr, "invalid character after depth of focus: %c\n%s", line[i], line);
			exit(1);
		}
	} else if ((ird=sindex(line, "Focus at Base of Superficial Layers.")) >= 0) {
		if (ird != i) {
			fprintf(stderr, "g56_epic error: ");
			fprintf(stderr, "Depth must follow epicenter inmediately\n%s", line);
			exit(1);
		}
		i += strlen("Focus at Base of Superficial Layers.");
		*rdepth = 0.0;
	}
	while (isspace(line[i]))
		i++;
	j = 0;
	while(line[i] != '\0')
		data[j++] = line[i++];
	data[j] = '\0';
	trim(data);

	return 0;
}
