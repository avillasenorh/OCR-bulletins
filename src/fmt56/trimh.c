#include <stdio.h>

/***********************************************************************
* Function: trimh
************************************************************************
* Purpose: deletes all blank spaces from the beginning of the character
*          string s
************************************************************************
* Returns: index of first non-blank character (-1 if not found)
***********************************************************************/
int trimh(char s[])
{
	int	i, j;
	int	index = -1;

	for (i = 0; s[i] != '\0'; i++)
		if (s[i] != ' ' && s[i] != '\t') {
			index = i;
			break;
		}

	if (index < 0) {
		fprintf(stderr, "Error trimming character string: *%s*\n", s);
		return(index);
	}

	for (i=index, j=0; s[i] != '\0'; i++)
		s[j++] = s[i];
	s[j] = '\0';

	return index;
}
