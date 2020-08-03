#include <stdio.h>
#include <string.h>

/***********************************************************************
* Function: get_hypo
************************************************************************
************************************************************************
************************************************************************
***********************************************************************/

#define MAXLINE 128
#define INT_UNDEF -12345
#define FLT_UNDEF -12345.0

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
	char	mn[12];
	char	sepic[16];

	int	i;

	*month = *day = *hour = *minute = *second = INT_UNDEF;
	*lat = *lon = *depth = FLT_UNDEF;
	
	n = sscanf(line, "%s %d %d.H %d.M %d.S  EPICENTRE %f%f DEPTH= %fKM",
		mn, day, hour, minute, second, lat, lon, depth);

	if (n != 8) {
		fprintf(stderr, "Function get_hypo.c: ");
		fprintf(stderr, "error reading hypocenter line:\n%s", line);
		nerr = 1;
		return(nerr);
	}

	for (i = 1; i < 13; i++) {
		if (strcmp(mn, month_name[i]) == 0) {
			*month = i;
			break;
		}
	}

	if (*month == INT_UNDEF) {
		nerr = 1;
		return(nerr);
	}

	return(nerr);
}
