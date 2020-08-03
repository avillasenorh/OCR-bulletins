/***********************************************************************
* Function: get_depth
************************************************************************
* Purpose: reads depth of focus from character string argument "line".
*          Depth of focus is expressed in fraction of the Earth radius.
************************************************************************
************************************************************************
* Returns: Function returns the depth of focus (in fraction of the Earth
*          radius).  If depth_of_focus is unable to interpret and read
*          the passed character string, it returns the dummy value
*          -12345.0
***********************************************************************/
#include <stdio.h>
#include <string.h>

#define MAXLINE 128
#define FLT_UNDEF -12345.0

float get_depth(char *line)
{
	int	i;
	float	rdepth = FLT_UNDEF;
	char	*line1;

	if ((line1 = strstr(line, "DEPTH OF FOCUS")) != NULL) {
		i = strlen("DEPTH OF FOCUS") + 1;
		sscanf(&line1[i], "%fR", &rdepth);
		/*fprintf(stderr, "%f %s", rdepth, line1+i);*/
	}
	return rdepth;
}
