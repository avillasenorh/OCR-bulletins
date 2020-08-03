/***********************************************************************
* Function: get_hypo
************************************************************************
************************************************************************
************************************************************************
***********************************************************************/
#include <stdio.h>
#include <string.h>

#define MAXLINE 128
#define INT_UNDEF -12345
#define FLT_UNDEF -12345.0

int sindex(char s[], char t[]);

int get_hypo(char *line, int *month, int *day, int *hour, int *minute,
             int *second, float *lat, float *lon, float *depth)
{
	char *month_name[13] = {
		"illegal month",
		"JANUARY", "FEBRUARY", "MARCH", "APRIL", "MAY", "JUNE",
		"JULY", "AUGUST", "SEPTEMBER", "OCTOBER", "NOVEMBER",
		"DECEMBER"
	};
	int	nerr = 0;
	int	n;
	int	n1, n2;
	char	mn[12];
	char	sepic[16];

	int	i;

	*month = *day = *hour = *minute = *second = INT_UNDEF;
	*lat = *lon = *depth = FLT_UNDEF;
	
	n = sscanf(line, "%s %d %d.H %d.M %d.S", mn, day, hour, minute, second);

	if (n != 5) {
		fprintf(stderr, "Error reading origin time: %s\n", line);
		return -1;
	}

	for (i = 1; i < 13; i++) {
		if (strcmp(mn, month_name[i]) == 0) {
			*month = i;
			break;
		}
	}

	/*fprintf(stderr, "%s %d %d %d %d %d\n",
	mn, *month, *day, *hour, *minute, *second);*/

	if (*month == INT_UNDEF) {
		nerr = 1;
		return(nerr);
	}

	/* Decode latitude and longitude */

	n1 = sindex(line, "EPICENTRE");
	n2 = sindex(line, "DEPTH");
	/*fprintf(stderr, "%d %d\n", n1, n2);*/
	if (n1 == -1 || n2 == -1) {
		fprintf(stderr, "get_hypo: Unable to decode EPICENTRE line\n");
		fprintf(stderr, "%s", line);
		return(1);
	}

	strncpy(sepic, &line[n1+9], n2-(n1+9));
        sepic[n2-(n1+9)] = '\0';
	/*fprintf(stderr, "**%s**\n", sepic);*/

	sscanf(sepic, "%f%f", lat, lon);
	/*fprintf(stderr, "%f %f\n", *lat, *lon);*/


	sscanf(&line[n2+6], "%f.KM", depth);
	/*fprintf(stderr, "%f\n", *depth);*/

	return nerr;
}
