#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***********************************************************************
* Function: asterisk
*
* Purpose: eliminates leading asterisk and converts next character to
*          lowercase
*
***********************************************************************/


void asterisk(char *phase)
{
	int	i;

	if (strlen(phase) == 0) return;

	if (phase[0] == '*') {
		if (isupper(phase[1])) {

			/* convert to lowercase */
			phase[0] = tolower(phase[1]);

			/* shift charaters to the left */
			i = 1;
			while (phase[i+1] != '\0') {
				phase[i] = phase[i+1];
				i++;
			}
			phase[i] = '\0';

		} else {
			fprintf(stderr, "Function asterisk: ");
			fprintf(stderr, "invalid phase: %s\n", phase);
			exit(1);
		}
	}
	return;
}
