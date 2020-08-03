#include <ctype.h>
#include <stdio.h>
#include <string.h>

/***********************************************************************
* Function: get_stnm.c
************************************************************************
* Purpose: extracts station name from ISS bulletin phase card
************************************************************************
* Arguments:
*	line: character array containing phase card (input)
*	stnm: character array containing station name (output)
************************************************************************
* Returns: index of next character after the tab that marks the end
*   of the station name
***********************************************************************/

int get_stnm(char *line, char *stnm)
{
	int	i = 0;
	int	ierr = 0;

	if (! isupper(line[0])) {
		return -1;
	}

	/* asssumes that station name is separated from next field
	(epicentral distance) by a tab */

	i = 0;
	while(line[i] != '\t') {
		stnm[i] = line[i];
		if (! isupper(line[i]) && ! isspace(line[i])
		&& line[i] != '-' && line[i] != '.') ierr++;
		i++;
	}
	stnm[i] = '\0';

	if (ierr) {
		if (strcmp(stnm, "ALMATA-2") == 0 ||
		strcmp(stnm, "ST. LOUIS 1") == 0 ||
		strcmp(stnm, "ST. LOUIS 2") == 0) {
			;
		} else {
			fprintf(stderr, "get_stnm warning: ");
			fprintf(stderr, "unusal character in station name: %s\n\n", stnm);
		}
	}

	return i+1;
}
