#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FLT_UNDEF -12345.0

#define TRUE 1
#define FALSE 0

int g56_l(char *line, int index, int debug, int *lphase, int *next_phase,
		char *onset, float *lmin, int *lparenthesis)

{
	int	i, j;
	char	aux[20];
	int	imin;

	/* initialize function arguments */

	*lmin = FLT_UNDEF;
	*onset = ' ';
	*lphase = FALSE;
	*next_phase = TRUE;
	*lparenthesis = FALSE;

	/* Check that we are not at the end of the line,
	and first character is valid */
	i = index;

	while (isspace(line[i]))
		i++;

	if (line[i] == '\0') {
		fprintf(stderr, "g56_l error: ");
		fprintf(stderr, "no phase read (end of line):\n%s", line);
		*lphase = FALSE;
		*next_phase = FALSE;
		return -1;
	} else if (isdigit(line[i]) || line[i] == '(' || line[i] == 'e' ||
	           line[i] == 'i'  || line[i] == 'j') {
		*lphase = TRUE;
	} else if (line[i] == '-') {
		/*
		fprintf(stderr, "g56_l warning: ");
		fprintf(stderr, "phase not present:\n%s", line);
		*/
		*lphase = FALSE;
	} else {
		fprintf(stderr, "g56_l error: ");
		fprintf(stderr, "invalid character at beginning of phase: %c\n%s", line[i], line);
		*lphase = FALSE;
		return -1;
	}

	/* if no phase (first character is '-') advance to next phase
	(should be the end of the line!) */

	if (! *lphase) {
		i++;
		if (! isspace(line[i])) {
			fprintf(stderr, "g56_l error: ");
			fprintf(stderr, "invalid character after m-dash:\n%s", line);
			return -1;
		}
		/* advance until next non-blank character */
		while (isspace(line[i]))
			i++;
		/* verify if we are at the end of the line */
		if (line[i] != '\0') {
			fprintf(stderr, "g56_l error: ");
			fprintf(stderr, "characters after end of (absent) L phase:\n%s", line);
			exit(1);
		} else {
			*next_phase = FALSE;
			return i;
		}
	}


	/* opening parenthesis */

	if (line[i] == '(') {
		*lparenthesis = TRUE;
		i++;
	}
	while (isspace(line[i]))
		i++;

	/* onset */

	if (line[i] == 'j') line[i] = 'i';
	if (line[i] == 'o') line[i] = 'e';
	if (line[i] == 'e' || line[i] == 'i') {
		*onset = line[i];
		i++;
	}

	/* minutes: float with 1/10 min precission */

minutes:
	while (isspace(line[i]))
		i++;
	if (!isdigit(line[i])) {
		fprintf(stderr, "g56_l error: ");
		fprintf(stderr, "invalid character in minutes for L phase:\n%s", line);
		return -1;
	}

        /* Find first non-digit character of L minutes */
	j = 0;
	aux[0] = '\0';
	while(isdigit(line[i])) {
		aux[j++] = line[i++];
	}
	aux[j] = '\0';
        /* error 01 = e1 */
	if (aux[0] == '0' && j > 1) {
		aux[0] = ' ';
		*onset = 'e';
	}


        if (j > 4) {
                fprintf(stderr, "g56_l error: ");
                fprintf(stderr, "wrong L minutes:\n%s", line);
                return -1;
        }

        /* Find decimal point (or common OCR errors) and ignore */
        if (line[i] == '.' || line[i] == '-' || line[i] == ':' || line[i] == '\'') {
                i++;
                if (isdigit(line[i])) {
                        aux[j++] = line[i++];
                        aux[j] = '\0';
		} else if (line[i] == 'i') {
                        aux[j++] = '1';
			i++;
                        aux[j] = '\0';
		} else if (line[i] == 'S') {
                        aux[j++] = '5';
			i++;
                        aux[j] = '\0';
                } else {
                        fprintf(stderr, "g56_l error: ");
                        fprintf(stderr, "wrong tenths of minutes for L:\n%s", line);
                        return -1;
                }
        } else if (line[i] == ')') {
                aux[j] = '\0';
        } else if (isspace(line[i])) {
                aux[j] = '\0';
        } else {
                fprintf(stderr, "g56_l error: ");
                fprintf(stderr, "invalid character in or after L minutes:\n%s", line);
                return -1;
        }

	/* common error: 1 instead of i */
	if (strlen(aux) == 1) {
		if (aux[0] == '1')
			*onset = 'i';
		else if (aux[0] == '0')
			*onset = 'e';
		goto minutes;	
	}
        sscanf(aux, "%d", &imin);
        if (imin > 999 || imin < 0) {
                fprintf(stderr, "g56_l error: ");
                fprintf(stderr, "L minutes out of range:\n%s", line);
                return -1;
        }
        *lmin = (float)imin/10.0;

	/* now i is just after the last number of L minutes:
	- \n
	- closing parenthesis
	- blank space?
	*/

	/* advance blank spaces if any */
	if (isspace(line[i]))
		while (isspace(line[i]))
			i++;

	/* closing parenthesis */

	if (*lparenthesis) {
		if (line[i] == ')') {
			i++;
			while (isspace(line[i]))
				i++;
		} else {
			fprintf(stderr, "g56_l error: ");
			fprintf(stderr, "closing parenthesis not present for L phase:\n%s", line);
			return -1;
		}
	}

	/* now i should point to first non-blank character after L phase */

	if (line[i] == '\0')
		*next_phase = FALSE;
	else {
		*next_phase = TRUE;
		fprintf(stderr, "g56_l error: ");
		fprintf(stderr, "extra characters after L phase:\n%s", line);
		return -1;
	}

	return i;
}
