#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INT_UNDEF -12345

#define TRUE 1
#define FALSE 0

int valid_phase(char *);

int g56_phase(char *line, int index, int debug, float delta, int nph, int *lphase, int *next_phase,
               char *onset, int *min, int *sec, char *pol, char *pol1,
               int *lres, int *res, char *pres, char *phase, 
               int *lparenthesis, int *lbrackets, int *lbraces)
{
	int	i, j;
	char	aux[20];
	int	isign;
	int	min_index;

	float	dminp[10] = {30., 38., 46., 55., 65., 76., 86., 97., 103., 107.};
	float	dmins[10] = {14., 17., 20., 23., 27., 31., 35., 39.,  43.,  47.};

	/* initialize function arguments */

	*min = INT_UNDEF;
	*sec = INT_UNDEF;
	*res = INT_UNDEF;
	*onset = ' ';
	*pol = ' ';
	*pol1 = ' ';
	*pres = ' ';
	*lphase = FALSE;
	*next_phase = FALSE;
	*lres = FALSE;
	*lparenthesis = FALSE;
	*lbrackets = FALSE;
	*lbraces = FALSE;
	phase[0] = '\0';

	/* Check that we are not at the end of the line,
	and first character is valid */

start:
	i = index;

	while (isspace(line[i]))
		i++;

	if (line[i] == '\0') {
		fprintf(stderr, "g56_phase error: ");
		fprintf(stderr, "no phase read (end of line):\n%s", line);
		*lphase = FALSE;
		*next_phase = FALSE;
		return -1;
	} else if (isdigit(line[i]) || line[i] == '(' || line[i] == 'e' ||
	           line[i] == 'i'  || line[i] == 'j') {
		*lphase = TRUE;
	} else if (line[i] == '-') {
		/*
		fprintf(stderr, "g56_phase warning: ");
		fprintf(stderr, "phase not present:\n%s", line);
		*/
		*lphase = FALSE;
	} else {
		fprintf(stderr, "g56_phase error: ");
		fprintf(stderr, "invalid character at beginning of phase:\n%s", line);
		*lphase = FALSE;
		return -1;
	}

	/* if no phase (first character is '-') advance to next phase */

	if (! *lphase) {
		i++;
		if (! isspace(line[i])) {
			fprintf(stderr, "g56_phase error: ");
			fprintf(stderr, "invalid character after 1st m-dash:\n%s", line);
			return -1;
		}
		/* advance until next non-blank character */
		while (isspace(line[i]))
			i++;
		/* verify if second m-dash present,
		and advance until next non-blank character */
		if (line[i] != '-') {
			fprintf(stderr, "g56_phase warning: ");
			fprintf(stderr, "only one m-dash for this phase:\n%s", line);
		} else {
			i++;
			if (! isspace(line[i])) {
				fprintf(stderr, "g56_phase error: ");
				fprintf(stderr, "invalid character after 2nd m-dash:\n%s", line);
				return -1;
			}
			while (isspace(line[i]))
				i++;
		}

		/* finally verify that next character is valid phase or end of line */
		if (line[i] == '\0') {
			*next_phase = FALSE;
			return i;
		} else if (isdigit(line[i]) || line[i] == '(' || line[i] == 'e' ||
		           line[i] == 'i' || line[i] == 'j' || line[i] == '-') {
			*next_phase = TRUE;
			return i;
		} else {
			fprintf(stderr, "g56_phase error: ");
			fprintf(stderr, "invalid character after this absent phase:\n%s", line);
			return -1;
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

	if (line[i] == 'e' || line[i] == 'i') {
		*onset = line[i];
		i++;
	}
	while (isspace(line[i]))
		i++;

	/* minutes */

	/* common mistake: i instead of 1, S instead of 5 */
	if (line[i] == 'i' && *onset != ' ') line[i] = '1';
	if (line[i+1] == 'i' && *onset != ' ') line[i+1] = '1';
	if (line[i] == 'S') line[i] = '5';
	if (line[i+1] == 'S') line[i+1] = '5';

	if (!isdigit(line[i])) {
		fprintf(stderr, "g56_phase error: ");
		fprintf(stderr, "invalid character in minutes for this phase (1): %c\n%s", line[i], line);
		return -1;
	}
	j = 0;
	aux[0] = '\0';
	while(isdigit(line[i])) {
		aux[j++] = line[i++];
	}
	aux[j] = '\0';
	/* error 0 instead of e */
	if (aux[0] == '0' && j == 1) {
		if (delta >= 10.) {
			line[i-j] = 'e';
			goto start;
		}
	}
	/* error 1 instead of i */
	if (aux[0] == '1' && j == 1) {
		if (delta >= 10.) {
			line[i-j] = 'i';
			goto start;
		}
	}
	/* error 01 = e1 */
	if (aux[0] == '0' && j > 1) {
		aux[0] = ' ';
		*onset = 'e';
	}
	/* error min = 115 instead of i15 */
	if (j == 3 && aux[0] == '1') {
		line[i-j] = 'i';
		goto start;
	}
	if (! isspace(line[i])) {
		fprintf(stderr, "g56_phase error: ");
		fprintf(stderr, "invalid character in minutes for this phase (2): %c\n%s", line[i], line);
		return -1;
	}
	sscanf(aux, "%d", min);
/*	if (*min < 0 || *min > 59) { */
	if (*min < 0 || *min > 99) {
		fprintf(stderr, "g56_phase error: ");
		fprintf(stderr, "minutes out of range for this phase:\n%s", line);
		return -1;
	}
	if (*min == 1) {  /* remember this to fix error "i" <-> "1" */
		min_index = i-1;
	}
	while (isspace(line[i]))
		i++;

	/* seconds */
	/* common mistakes: i instead of 1, S instead of 5 */
	if (line[i] == 'i') line[i] = '1';
	if (line[i+1] == 'i') line[i+1] = '1';
	if (line[i] == 'S') line[i] = '5';
	if (line[i+1] == 'S') line[i+1] = '5';

	if (!isdigit(line[i])) {
		fprintf(stderr, "g56_phase error: ");
		fprintf(stderr, "invalid character in seconds for this phase:\n%s", line);
		return -1;
	}
	j = 0;
	aux[0] = '\0';
	while(isdigit(line[i])) {
		aux[j++] = line[i++];
	}
	aux[j] = '\0';
	sscanf(aux, "%d", sec);
	if (*sec < 0 || *sec > 59) {
		fprintf(stderr, "g56_phase error: ");
		fprintf(stderr, "seconds out of range for this phase:\n%s", line);
		return -1;
	}

	/* polarity and closing parenthesis */

	/* sometimes there is an extra blank space between minutes and polarity */
	if (isspace(line[i]) && (line[i+1] == 'k' || line[i+1] == 'a')) i++;

	if (line[i] == 'k' || line[i] == 'a' || line[i] == '?') {
		*pol = line[i];
		i++;
		if (line[i] == 'a' || line[i] == 'k' || line[i] == '?') {
			*pol1 = line[i];
			i++;
		}
	} else if (line[i] == ')') {
		;
	} else if (isspace(line[i])) {
		;
	} else {
		fprintf(stderr, "g56_phase error: ");
		fprintf(stderr, "invalid character in seconds for this phase:\n%s", line);
		return -1;
	}

	if (isspace(line[i]))
		while (isspace(line[i]))
			i++;

	if (*lparenthesis) {
		if (line[i] == ')') {
			i++;
			while (isspace(line[i]))
				i++;
		} else {
			fprintf(stderr, "g56_phase error: ");
			fprintf(stderr, "closing parenthesis not present for this phase:\n%s", line);
			return -1;
		}
	}

	/* O-C or phase name */

	if (line[i] == '[') {
		*lbrackets = TRUE;
		i++;
	} else if (line[i] == '{') {
		*lbraces = TRUE;
		i++;
	}

	if (*lbrackets || *lbraces)
		while (isspace(line[i]))
			i++;

	if (line[i] == '+' || line[i] == '\177') {
		*lres = TRUE;
		isign = 1;
		i++;
	} else if (line[i] == '-') {
		*lres = TRUE;
		isign = -1;
		i++;
	} else if (line[i] == '0') {
		*lres = TRUE;
		isign = 0;
	} else if (line[i] == 'O') { /* fixes common OCR error */
		line[i] = '0';
		*lres = TRUE;
		isign = 0;
	} else if (isalpha(line[i])) {
		*lres = FALSE;
	} else if (line[i] == '?') {
		*lres = FALSE;
	} else if (line[i] >= '1' && line[i] <= '9') {
		if (*min == 1) {
			line[min_index] = 'i';
			goto start;
		} else {
			fprintf(stderr, "g56_phase error: ");
			fprintf(stderr, "non-zero residual must have sign!! (1):\n%s", line);
			return -1;
		}
	} else {
		fprintf(stderr, "g56_phase error: ");
		fprintf(stderr, "invalid character in residual/phase field: %c\n%s", line[i], line);
		return -1;
	}

	while (isspace(line[i]))  /* advance if previous char is a sign */
		i++;

	aux[0] = '\0';
	j = 0;
	if (*lres) {	/* next field must be a number */

		/* common mistakes: i instead of 1, S instead of 5 */
		if (line[i] == 'i') line[i] = '1';
		if (line[i+1] == 'i') line[i+1] = '1';
		if (line[i] == 'S') line[i] = '5';
		if (line[i+1] == 'S') line[i+1] = '5';

		while(isdigit(line[i])) {
			aux[j++] = line[i++];
		}
		aux[j] = '\0';
		if (j == 0) {
			fprintf(stderr, "g56_phase error: ");
			fprintf(stderr, "residual must be a number for this phase:\n%s", &line[i]);
			return -1;
		}
		sscanf(aux, "%d", res);
		if (*res < 0 || *res > 99) {
			fprintf(stderr, "g56_phase error: ");
			fprintf(stderr, "residual out of range for this phase:\n%s", aux);
			return -1;
		}
		if (isign == 0) {
			if (*res != 0) {
				fprintf(stderr, "g56_phase error: ");
				fprintf(stderr, "non-zero residual must have sign!! (2):\n%s", &line[index]);
				return -1;
			}
		} else {
			*res *= isign;
		}
		if (line[i] == 'g' || line[i] == '*') {
			*pres = line[i];
			i++;
		}
	} else {	/* next field must be a valid phase */
		while(isalnum(line[i]) || line[i] == '?' || line[i] == '*' || line[i] == '\'') {
			aux[j++] = line[i++];
		}
		aux[j] = '\0';
		if (valid_phase(aux)) {
			strcpy(phase, aux);
		} else {
			fprintf(stderr, "g56_phase error: ");
			fprintf(stderr, "invalid phase: %s\n%s", aux, line);
			return -1;
		}
	}

	/* if lbrackets or lbraces, next non-blank character
	must be closing bracket/brace */

	while (isspace(line[i]))
		i++;

	if (*lbrackets) {
		if (line[i] == ']') {
			i++;
			while (isspace(line[i]))
				i++;
		} else {
			fprintf(stderr, "g56_phase error: ");
			fprintf(stderr, "closing bracket not present:\n%s", &line[index]);
			/*return -1;*/
			exit(1);
		}
	}
	if (*lbraces) {
		if (line[i] == '}') {
			i++;
			while (isspace(line[i]))
				i++;
		} else {
			fprintf(stderr, "g56_phase error: ");
			fprintf(stderr, "closing brace not present:\n%s", &line[index]);
			/*return -1;*/
			exit(1);
		}
	}

	/* Verify that minutes are reasonable */

	if (nph == 1 || nph == 3) {	/* P phase arrival */
		if (*onset == ' ' && *min >= 10 && delta < 85.) {
			if (delta < dminp[*min-10]) {
				fprintf(stderr, "WARNING: ");
				fprintf(stderr, "minutes dubious for P phase: %d\n%s", *min, line);
			}
		}
		if (*onset == 'i' && *min < 10) {
			if (delta > dminp[*min]) {
				fprintf(stderr, "WARNING: ");
				fprintf(stderr, "minutes dubious for P phase: %d\n%s", *min, line);
			}
		}
	} else if (nph == 2) {	/* S phase arrival */
		if (*onset == ' ' && *min >= 10 && delta < 45.) {
			if (delta < dmins[*min-10]) {
				fprintf(stderr, "WARNING: ");
				fprintf(stderr, "minutes dubious for S phase: %d\n%s", *min, line);
			}
		}
		if (*onset == 'i' && *min < 10) {
			if (delta > dmins[*min]) {
				fprintf(stderr, "WARNING: ");
				fprintf(stderr, "minutes dubious for S phase: %d\n%s", *min, line);
			}
		}
	/* Supp. phase arrival
	} else if (nph == 3) {
		;
	*/
	}

	/* now i should point to first non-blank character after current phase */

	if (line[i] == '\0')
		*next_phase = FALSE;
	else
		*next_phase = TRUE;

	return i;
}
