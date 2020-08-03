#include <stdio.h>

/***********************************************************************
* Function: mdash
************************************************************************
* Purpose: substitutes all m-dash characters in string s by '-'
************************************************************************
* Returns: number of m-dash characters substituted in string s
***********************************************************************/

#define MDASH	'\227'

int mdash(char *s)
{
	int	i = 0;
	int	k = 0;

	for (i = 0; s[i] != '\0'; i++)
		if (s[i] == MDASH) {
			s[i] = '-';
			k++;
		}

	return k;
}
