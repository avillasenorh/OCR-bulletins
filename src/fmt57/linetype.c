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
		3 = First constant line (A=, B=, C=, ...)
		4 = Second constant line (G=, H=, K=, HT=)
		5 = Standard error line
		6 = First table header (DELTA AZ. P 0-C ...)
		7 = Second table header (DEG. DEG. M S S ...)
		8 = Station phase list
*******************************************************************************/

#include <ctype.h>
#include <stdio.h>
#include <string.h>

int linetype(char *line, int plt)
{
	int	lt = -1;

	if (strstr(line, "PAGE") != NULL) {
		lt = 0;
	} else if (strstr(line, "EPICENTRE") != NULL) {
		lt = 1;
	} else if (strstr(line, "DEPTH OF FOCUS") != NULL) {
		lt = 2;
	} else if (line[0] == 'A' && (line[1] == '=' || line[1] == '-')) {
		lt = 3;
	} else if (line[0] == 'G' && (line[1] == '=' || line[1] == '-')) {
		lt = 4;
	} else if (strstr(line, "SE=") != NULL || strstr(line, "SE-") != NULL ) {
		lt = 5;
	} else if (strstr(line, "DELTA") != NULL) {
		lt = 6;
	} else if (strstr(line, "DEG.") != NULL) {
		lt = 7;
	} else if (isupper(line[0])) {
		lt = 8;
	}


	/* Based on plt (previous line type), some line types are
	expected.  Check that this logic applies.  If not, write
	warning to stderr */

	return(lt);
}
