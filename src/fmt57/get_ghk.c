/***********************************************************************
* Function: get_ghk.c
************************************************************************
* Purpose: reads  constants G, H, K, and HT from character string
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

int get_ghk(char *line, float *g, float *h, float *k, float *ht)
{
	int	nerr = 0;
	int	n;
	int	i;

	*g = *h = *k = *ht = FLT_UNDEF;

	for (i = 0; i < strlen(line); i++) {
		if (line[i] == 'G' || line [i] == 'H' ||
		line[i] == 'K' || line [i] == 'T')
			if (line[i+1] == '-') line[i+1] = '=';
	}

	if (line[0] == 'G' && line[1] == '=') {
		*g = *h = *k = *ht = 0.;
	} else {
		nerr = -1;
	}

	/*
	n = sscanf(line, "G=%f H=%f K=%f   HT=%f", g, h, k, ht);

	if (n < 4) {
		fprintf(stderr, "get_ghk error: Error reading this card:\n");
		fprintf(stderr, "%s\n", line);
		nerr = -1;
	}
	*/

	/*fprintf(stderr, "%d %f %f %f %f\n", n, *g, *h, *k, *ht);*/
	
	return nerr;
}
