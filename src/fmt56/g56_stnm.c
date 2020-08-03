#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***********************************************************************
* Function: g56_stnm.c
************************************************************************
* Purpose: extracts station name from ISS bulletin phase card
************************************************************************
* Arguments:
*	line: character array containing phase card (input)
*	stnm: character array containing station name (output)
************************************************************************
* Returns: index of first character (numerical character) of epicentral
*          distance
***********************************************************************/
int trim(char *);

int g56_stnm(char *line, char *stnm, int *ncmp, int cmp[])
{
	int	i, j;
	int	n;
	int	icmp = 0;

	*ncmp = 0;
	cmp[0] = cmp[1] = cmp[2] = 0;

	if (! isupper(line[0])) {
		fprintf(stderr, "g56_stnm error: ");
		fprintf(stderr, "first character must be uppercase letter:\n%s", line);
		exit(1);
	}

	i = 0;
	j = 0;
	while(! isdigit(line[i])) {
		if (j > 0 && isupper(line[i]) && islower(stnm[j-1])) {
			stnm[j++] = ' ';
		}
		stnm[j++] = line[i++];
	}
	stnm[j] = '\0';
	if (isdigit(line[i]) && ! isspace(line[i+1])) {
		/* get component names, and eliminate them from station name */
		for (icmp = 0;; icmp++) {
			trim(stnm);
			n = strlen(stnm);
			if (stnm[n-1] == '.' || stnm[n-1] == '-') {
				if (stnm[n-2] == 'Z' || stnm[n-2] == 'z') {
					cmp[icmp] = 1;
				} else if (stnm[n-2] == 'N') {
					cmp[icmp] = 2;
				} else if (stnm[n-2] == 'E') {
					cmp[icmp] = 3;
				} else if (stnm[n-2] == 'W') {
					cmp[icmp] = 13;
				} else {
					break;
				}
				stnm[n-2] = '\0';
			} else {
				break;
			}
		}
		*ncmp = icmp;
		/* make sure all blanks are ' ' (no tabs, ) */
		n = 0;
		while(stnm[n] != '\0') {
			if (isspace(stnm[n]))
				stnm[n] = ' ';
			n++;
		}
	} else {
		fprintf(stderr, "g56_stnm error: ");
		fprintf(stderr, "cannot find valid epicentral distance:\n%s", line);
		exit(1);
	}
	return i;
}
