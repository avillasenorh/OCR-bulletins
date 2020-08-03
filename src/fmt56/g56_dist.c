#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define FLT_UNDEF -12345.0

int g56_dist(char *line, float *delta)
{
	int	i, j;
	char	sdist[10];
	int	idelta;

	*delta = FLT_UNDEF;

	i = 0;	/* index for line */

	/* if first character of string is not a digit [0-9] exit function */

	if (! isdigit(line[i])) {
		fprintf(stderr, "g56_dist error: ");
		fprintf(stderr, "first character of distance must be a number:\n%s", line);
		return -1;
	}

	/* Distance:
	value from 0 to 180 degrees; precision - 1/10 of a degree */

	/* Common errors: S instead of 5 (or 8) and i instead of 1 */
	j = i;
	while (! isspace(line[j])) {
		if (line[j] == 'i') line[j] = '1';
		if (line[j] == 'S') line[j] = '5';
		j++;
	}


	/* Find first non-digit character of epicentral distance */

	j = 0;	/* index for sdist */
	while (isdigit(line[i])) {
		sdist[j++] = line[i++];
	}
	if (j > 4) {
		fprintf(stderr, "g56_dist error: ");
		fprintf(stderr, "wrong epicentral distance:\n%s", line);
		return -1;
	}

	/* Find decimal point (or common OCR errors) and ignore */
	if (line[i] == '.' || line[i] == '-' || line[i] == ':' || line[i] == '\'') {
		i++;
		if (isdigit(line[i]) && isspace(line[i+1])) {
			sdist[j++] = line[i++];
			sdist[j] = '\0';
		} else if (line[i] == 'i' && isspace(line[i+1])) {
			sdist[j++] = '1';
			i++;
			sdist[j] = '\0';
		} else {
			fprintf(stderr, "g56_dist error: ");
			fprintf(stderr, "wrong format for epicentral distance:\n%s", line);
			return -1;
		}
	} else if (isspace(line[i])) {
		sdist[j] = '\0';
	} else {
		fprintf(stderr, "g56_dist error: ");
		fprintf(stderr, "invalid character in epicentral distance:\n%s", line);
		return -1;
	}

	sscanf(sdist, "%d", &idelta);
	if (idelta > 1800 || idelta < 0) {
		fprintf(stderr, "g56_dist error: ");
		fprintf(stderr, "epicentral distance out of range:\n%s", line);
		return -1;
	}
	*delta = (float)idelta/10.0;

	/* now i indicates position of tab between delta and azimuth
	or first blank space between delta and azimuth */

	while(isspace(line[i]))
		i++;
	if (! isdigit(line[i]) && line[i] != 'S') {
		fprintf(stderr, "g56_dist error: ");
		fprintf(stderr, "first character of azimuth must be a number:\n%s", line);
		return -1;
	}


	return i;
}
