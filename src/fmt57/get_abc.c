/***********************************************************************
* Function: get_abc.c
************************************************************************
* Purpose: reads  constants A, B, C, D, and E from character string
*          argument "line".
************************************************************************
************************************************************************
* Returns: Function returns the error condition
*          0 = no error ocurred
*          1 = unable to interpret line
***********************************************************************/
#include <stdio.h>
#include <string.h>

#define MAXLINE 128
#define FLT_UNDEF -12345.0

int get_abc(char *line, float *a, float *b, float *c, float *d, float *e)
{
	int	nerr = 0;
	int	i;
	int	n;

	*a = *b = *c = *d = *e = FLT_UNDEF;

	for (i = 0; i < strlen(line); i++) {
		if (line[i] == 'A' || line [i] == 'B' || line[i] == 'C' ||
		line[i] == 'D' || line [i] == 'E')
			if (line[i+1] == '-') line[i+1] = '=';
	}

        if (line[0] == 'A' && line[1] == '=') {
                *a = *b = *c = *d = *e = 0.;
        } else {
                nerr = -1;
        }

	/*
	n = sscanf(line, "A=%f B=%f C=%f    D=%f E=%f", a, b, c, d, e);

	if (n < 5) {
		fprintf(stderr, "get_abc error: Error reading this card:\n");
		fprintf(stderr, "%s\n", line);
		nerr = -1;
	}
	*/

	/*fprintf(stderr, "%d %f %f %f %f %f\n", n, *a, *b, *c, *d, *e);*/
	
	return nerr;
}
