/*******************************************************************************
* Function: ltype.c
********************************************************************************
* Purpose: identifies type of line a returns a line type code (int).
*          Line must be in valid ISS format (e.g. output of fmt56)
********************************************************************************
* Possible code values:
-1 = Unknown (possibly an error)
 0 = Page number and year (year and page number)
 1 = Primary epicenter line (can contain also comments and/or depth of focus)
 2 = Primary depth of focus
 3 = First constant line:  A=, ...
 4 = Second constant line: D=, ...
 5 = Standard error line: not present for events prior to 1957
 6 = First table header (Az. P. 0-C. S. O-C. Supp. L.)
 7 = Second table header (o o m. s. s. m. s. s. m. s. m.)
 8 = Station phase list (tabular or not) or comment
 9 = Additional readings
10 = Continued on next page
11 = Epicenter header for "Undetermined shock" or "Readings also at..."
12 = For notes see next page
13 = Blank line
*******************************************************************************/

#include <ctype.h>
#include <stdio.h>
#include <string.h>

int ltype(char *line, int plt)
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

	/* 0 = Page number */
	if (line[0] == '1' && line[1] == '9' && isdigit(line[2]) && isdigit(line[3])) {
		lt = 0;

	/* 1 = Primary epicenter line (with or without phase data)
	   can also contain comments (e.g. "as in ...") and/or depth of focus
	*/
	} else if (strstr(line, "Epicentre") != NULL) {
		lt = 8;
		for (i = 0; i < 24; i++) {
			if (strcmp(aux, months[i]) == 0) {
				lt = 1;
				break;
			}
		}

	/* 2 = Primary depth of focus
	- Depth of focus, Focus at base
	- Other wording: Tables for depth of xxxx used, Height of focus, ...
	- Make sure this is primary depth of focus line, not additional or suggested
	*/
	} else if (strstr(line, "Depth of focus") != NULL) {
		lt = 2;
	} else if (strstr(line, "Focus at Base") != NULL) {
		lt = 2;

	/* 3 = First constant line */
	} else if (strstr(line, "A=") != NULL && strstr(line, "B=") != NULL) {
		lt = 3;

	/* 4 = Second constant line */
	} else if (strstr(line, "H=") != NULL && strstr(line, "K=") != NULL) {
		lt = 4;

	/* 5 = Standard error line: not present for events prior to 1957
	} else if (strstr(line, "SE=") != NULL) {
		lt = 5;
	*/

	/* 6 = First table header (Az. P. 0-C. S. O-C. Supp. L.) */
	} else if (strstr(line, "Az.") != NULL && strstr(line, "O-C.") != NULL &&
	  strstr(line, "Supp.") != NULL) {
		lt = 6;

	/* 7 = Second table header (o o m. s. s. m. s. s. m. s. m.) */
	} else if (strstr(line, "m.  s.") != NULL) {
		if (plt == 6) lt = 7;

	/* 9 = Additional readings */
	} else if (strstr(line, "Additional") != NULL) {
		lt = 9;

	/* 10 = Continued on next page */
	} else if (strstr(line, "Continued on next page") != NULL) {
		lt = 10;

	/* 12 = For notes see next page */
	} else if (strstr(line, "For Notes see next page") != NULL) {
		lt = 12;
	} else if (strstr(line, "For notes see next page") != NULL) {
		lt = 12;

	/* 13 = blank line */
	} else if (line[0] == '\n') {
		lt = 13;

	/* 8 = Station phase list (tabular or not) or comment */
	} else {
		lt = 8;
	/* 11 = Epicenter header for "Undetermined shock" or "Readings also at..." */
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
