#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NINT(x) (int)floor((x)+0.5)

#define TRUE 1
#define FALSE 0

#define INT_UNDEF -12345

int r_iss_phase(char *s, int n, char *phase, int *min, int *sec, char *onset, char *pol,
   int *res, char *isc_phase)
{
	static  index[] = {32, 52, 72, 92}; /* start of phases */
	float	fmin;
	int	lbracket = FALSE,
		lbrace = FALSE;
	int	lg = FALSE,
		lstar = FALSE;
	int	sign = INT_UNDEF;

	/* assign default values */

	*min = *sec = *res = INT_UNDEF;
	*onset = *pol = ' ';
	phase[0] = isc_phase[0] = '\0';

	/* phases in parenthesis are duplicated phases -> ignored */
	if (s[index[n]] == '(') return FALSE;

	/* dash (minus sign) in this position indicates phase not present;
	works both for normal phases (P, S, Supp.) and L */
	if (s[index[n]+5] == '-') return FALSE;

	*onset = s[index[n]+1];

	if (n >= 0 && n <= 2) { /* P, S, and Supp. */
		if (sscanf(&s[index[n]+2], "%d %d", min, sec) != 2) {
			fprintf(stderr, "r_iss_phase error: ");
			fprintf(stderr, "cannot read min and sec for phase %d:\n%s\n", n, s);
			fprintf(stderr, "index[%d] = %d: %s\n", n, index[n], &s[index[n]+2]);
			exit(1);
		}
		/* phase polarity */
		*pol = s[index[n]+8];
		/* read residual */
		/* Phase depens on
		- n
		- brackets or braces
		- suffix in residual
		- phase in res field (phase or isc_phase)
		*/

		if (s[index[n]+13] == '[') lbracket = TRUE;
		if (s[index[n]+13] == '{') lbrace = TRUE;

		if (s[index[n]+14] == '+') sign = 1;
		if (s[index[n]+14] == '-') sign = -1;
		if (s[index[n]+14] == ' ' && s[index[n]+16] == '0') sign = 0;

		if (s[index[n]+17] == 'g') lg = TRUE;
		if (s[index[n]+17] == '*') lstar = TRUE;

		if (sign != INT_UNDEF) {
			sscanf(&s[index[n]+15], "%d", res);
			*res *= sign;
			if (n == 0) {
				if (lbracket) {
					strcpy(phase, "PKP");
					strcpy(isc_phase, "PKP");
				} else if (lg) {
					strcpy(phase, "Pg");
					strcpy(isc_phase, "Pg");
				} else if (lstar) {
					strcpy(phase, "P*");
					strcpy(isc_phase, "P*");
				} else {
					strcpy(phase, "P");
					strcpy(isc_phase, "P");
				}
			} else if (n == 1) {
				if (lbracket) {
					strcpy(phase, "SKS");
					strcpy(isc_phase, "SKS");
				} else if (lbrace) {
					strcpy(phase, "SKKS");
					strcpy(isc_phase, "SKKS");
				} else if (lg) {
					strcpy(phase, "Sg");
					strcpy(isc_phase, "Sg");
				} else if (lstar) {
					strcpy(phase, "S*");
					strcpy(isc_phase, "S*");
				} else {
					strcpy(phase, "S");
					strcpy(isc_phase, "S");
				}
			} else {
				fprintf(stderr, "r_iss_phase error: ");
				fprintf(stderr, "Supp. phase cannot have residual: %d\n%s\n", res, s);
				exit(1);
			}
		} else {
			if (n == 0) {
				strcpy(phase, "P");
				sscanf(&s[index[n]+10], "%s", isc_phase);
			} else if (n == 1) {
				strcpy(phase, "S");
				sscanf(&s[index[n]+10], "%s", isc_phase);
			} else {
				sscanf(&s[index[n]+10], "%s", phase);
				strcpy(isc_phase, phase);
			}
	
		}

	} else if (n == 3) { /* L */
		sscanf(&s[index[n]+2], "%f", &fmin);
		*min = (int)fmin;
		*sec = NINT((fmin - *min)*60.0);
		strcpy(phase, "L");
		strcpy(isc_phase, "L");
	} else { /* invalid phase number */
		;
	}

	return TRUE;
}
