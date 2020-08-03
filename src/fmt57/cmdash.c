#include <stdio.h>

/***********************************************************************
* Function: cmdash
************************************************************************
* Purpose: deletes all blank spaces from the beginning of the character
*          string s
************************************************************************
* Returns: index of first non-blank character (-1 if not found)
***********************************************************************/

#define MDASH	'\227'

int cmdash(char *s)
{
	int	i = 0;
	int	k = 0;

	for (i = 0; s[i] != '\0'; i++)
		if (s[i] == MDASH) {
			s[i] = '-';
			k++;
		} else if (s[i] == ',') {
			s[i] = '.';
		}

	return k;
}
