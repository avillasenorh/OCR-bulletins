/*******************************************************************************
* Function: linetype
********************************************************************************
* Purpose: identifies type of line a returns a line type code (int)
********************************************************************************
* Possible code values:
		-1 = Unknown (possibly an error)
		0 = Header line (year and page number)
		1 = Origin, epicentre and depth
		2 = Depth of focus (only for "abnormal" events)
		3 = First constant line (A=, B=, C=, delta=, h=)
		4 = Second constant line (D=, E=, G=, H=, K=)
		5 = Standard error line (does not exist for events prior to 1957)
		6 = First table header (Az. P. 0-C. S. O-C. Supp. L.)
		7 = Second table header (o o m. s. s. m. s. s. m. s. m.)
		8 = Station phase list
*******************************************************************************/

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define DEGREE 176

int g56_ltype(char *line, int plt)
{
	static char *months[] = {
		"January", "February", "March", "April", "May", "June",
		"July", "August", "September", "October", "November", "December",
		"Jan.", "Feb.", "Mar.", "Apr.", "May.", "Jun.",
		"Jul.", "Aug.", "Sept.", "Oct.", "Nov.", "Dec."
	};

	char	aux[20];
	int	i;
	int	lt = -1;

	sscanf(line, "%s", aux);
	if (line[0] == '1' && line[1] == '9' && isdigit(line[2]) && isdigit(line[3])) {
		lt = 0;
		i = 4;
		while(isspace(line[i]))
			i++;
			if (! isdigit(line[i])) lt = 8;
	} else if (strstr(line, "Epicentre") != NULL) {
		lt = 8;
		for (i = 0; i < 24; i++) {
			if (strcmp(aux, months[i]) == 0) {
				lt = 1;
				break;
			}
		}
	/* make sure not to select a epicenter line (or a suggested solution) */
	/* Also "Tables for depth of ... used */
	/* For 1943-1956 depth is always in epicenter line
	} else if (strstr(line, "Depth of focus") != NULL) {
		lt = 2;
	} else if (strstr(line, "Focus at Base") != NULL) {
		lt = 2;
	*/
	} else if (line[0] == 'A' && (line[1] == '=' || line[2] == '=')) {
		lt = 3;
	} else if (line[0] == 'D' && (line[1] == '=' || line[2] == '=')) {
		lt = 4;
	/* SE line does not exist prior to 1957
	} else if (strstr(line, "SE=") != NULL || strstr(line, "SE-") != NULL ) {
		lt = 5;
	*/
	} else if (strstr(line, "Az.") != NULL) {
		lt = 6;
	} else if ((unsigned char)line[0] == DEGREE || (line[0] == 'm' && line[1] == '.')) {
		if (plt == 6) lt = 7;
	} else if (strstr(line, "Additional") != NULL) {
		lt = 9;
	} else if (strcmp(aux, "Continued") == 0) {
		lt = 10;
	} else if (strstr(line, "For Notes") != NULL) {
		lt = 12;
	} else if (strstr(line, "Long waves") != NULL) {
		lt = 13;
	} else {
		lt = 8;
		for (i = 0; i < 24; i++) {
			if (strcmp(aux, months[i]) == 0) {
				lt = 11;
				break;
			}
		}
	}


	/* Based on plt (previous line type), some line types are
	expected.  Check that this logic applies.  If not, write
	warning to stderr */

	return lt;
}
