#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FLT_UNDEF -12345.0

int g56_az(char *line, float *az)
{
	int	i, j;
	char	saz[10];
	int	iaz;

	*az = FLT_UNDEF;

	i = 0;	/* index for line */

        /* Common errors: S instead of 5 (or 8) and i instead of 1 */
        j = i;
        while (! isspace(line[j])) {
                if (line[j] == 'i') line[j] = '1';
                if (line[j] == 'S') line[j] = '5';
                j++;
        }

	/* if first character of string is not a digit [0-9] exit function */

	if (! isdigit(line[i])) {
		fprintf(stderr, "g56_az error: ");
		fprintf(stderr, "first character of azimuth must be a number:\n%s", line);
		exit(1);
	}

	/* Azimuth: value from 0 to 180 degrees; precision = 1 degree.
	Important: for very short distances azimuth can be not defined!! */
	j = 0;	/* index for saz */

	/* Find first non-digit character */

	while (isdigit(line[i])) {
		saz[j++] = line[i++];
	}
	if (j > 3) {
		fprintf(stderr, "g56_az error: ");
		fprintf(stderr, "wrong azimuth:\n%s", line);
		exit(1);
	}
	if (! isspace(line[i])) {
		fprintf(stderr, "g56_az error: ");
		fprintf(stderr, "invalid character in azimuth:%c\n%s", line[i], line);
		exit(1);
	} else {
		saz[j] = '\0';
	}

	sscanf(saz, "%d", &iaz);
	if (iaz > 360 || iaz < 0) {
		fprintf(stderr, "g56_az error: ");
		fprintf(stderr, "azimuth out of range:\n%s", line);
		exit(1);
	}
	*az = (float)iaz;

	/* now i indicates position of space between azimuth
	and first character of P phase*/

	while(isspace(line[i]))
		i++;

	/* check that first character of P phase is a valid character
	(or a common error such as 'j') */

	if (isdigit(line[i]) || line[i] == '(' || line[i] == 'e' ||
	   line[i] == 'i' || line[i] == 'j' || line[i] == '-') {
		return i;
	} else {
		fprintf(stderr, "g56_az error: ");
		fprintf(stderr, "invalid first character of P phase: %c\n%s", line[i], line);
		exit(1);
	}
}
