/***********************************************************************
* Function: get_se
************************************************************************
* Purpose: reads standard error from character string argument "line".
************************************************************************
************************************************************************
* Returns: Function returns the standard error (in seconds)
*          If get_se is unable to interpret and read
*          the passed character string, it returns the dummy value
*          -12345.0
***********************************************************************/
#include <stdio.h>
#include <string.h>

#define MAXLINE 128
#define FLT_UNDEF -12345.0

float get_se(char *line)
{
	int	i;
	float	se = FLT_UNDEF;
	char	*line1;

	if ((line1 = strstr(line, "SE")) != NULL) {
		i = strlen("SE") + 1;
		sscanf(&line1[i], "%f", &se);
	}
	return se;
}
