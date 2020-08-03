#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define FLT_UNDEF -12345.0

#define TRUE 1
#define FALSE 0

int get_distaz(char *line, float *delta, float *azim)
{
	int	i, j;
	int	ldist = FALSE;
	char	sdist[10];
	char	sazim[10];

	*delta = *azim = FLT_UNDEF;

	i = 0;	/* index for line */
	while (isspace(line[i]))
		i++;

	/* if first non-blank character after station name is not a
	digit [0-9] exit function */

	if (! isdigit(line[i]))
		return -1;

	/* Distance:
	value from 0 to 180 degress; precision - 1/100 of a degree */
	j = 0;	/* index for sdist */

	/* Integer part of distance */

	while (isdigit(line[i]))
		sdist[j++] = line[i++];

	/* Find decimal point (and fix common errors) */
	if (line[i] == '.' || line[i] == '-' || line[i] == ':') {
		sdist[j++] = '.';
		i++;
	} else if (line[i] == ' ') {
		if (isdigit(line[i+1])) {
			sdist[j++] = '.';
			i++;
		} else if (line[i+1] == '.') {
			sdist[j++] = '.';
			i += 2;
		} else {
			return -1;
		}
	} else if (line[i] == '\t' && j >= 3) {
		i -= 2;
		j -= 2;
		sdist[j++] = '.';
	} else {
		return -1;
	}

	/* Fractionary part (2 digits) */

	if (isdigit(line[i]) && isdigit(line[i+1]) && 
	(line[i+2] == '\t' || line[i+2] == ' ')) {
			sdist[j++] = line[i++];
			sdist[j++] = line[i++];
	} else {
		return -1;
	}
	sdist[j] = '\0';
	sscanf(sdist, "%f", delta);
	if (*delta > 180.0 || *delta < 0.0) {
		return -1;
	}

	/* now i indicates position of tab between delta and azimuth
	or first blank space between delta and azimuth */

	while(isspace(line[i]))
		i++;


	/* Azimuth:
	value from 0 to 360 degress; precision - 1/10 of a degree */
	j = 0;	/* index for sazim */

	if (! isdigit(line[i]))
		return -1;

	/* Integer part of azimuth */

	while (isdigit(line[i])) {
		sazim[j++] = line[i++];
	}
	if (j > 3) {
		fprintf(stderr, "get_distaz error: ");
		fprintf(stderr, "wrong azimuth: %s\n", line);
		return -1;
	}

	/* Find decimal point (and fix common errors) */
	if (line[i] == '.' || line[i] == '-' || line[i] == ':') {
		sazim[j++] = '.';
		i++;
	} else if (line[i] == ' ') {
		if (isdigit(line[i+1])) {
			sazim[j++] = '.';
			i++;
		} else if (line[i+1] == '.') {
			sazim[j++] = '.';
			i += 2;
		} else {
			return -1;
		}
	} else {
		return -1;
	}

	/* Fractionary part (1 digits) */
	/* considers the case when there is not phase data after azimuth */

	if (isdigit(line[i]) && (line[i+1] == '\t' || line[i+1] == '\n' || line[i+1] == '\r')) {
			sazim[j++] = line[i++];
	} else {
		return -1;
	}
	sazim[j] = '\0';

	sazim[j] = '\0';
	sscanf(sazim, "%f", azim);
	if (*azim > 360.0 || *azim < 0) {
		return -1;
	}

	/*fprintf(stderr, "**%s**%s**\n", sdist, sazim);*/

	if (line[i] == '\n')
		return i;
	else
		return i+1;
}
