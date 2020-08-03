#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INT_UNDEF -12345
#define FLT_UNDEF -12345.0

#define TRUE 1
#define FALSE 0

#define MAX_ARGS 8

int get_phases(char line[], int index, int debug, int supp_all,
		int *Pmin, int *Psec, int *Pres, int *Ppol,
		int *Smin, int *Ssec, int *Sres, int *pPmin, int *pPsec,
		int *Suppmin, int *Suppsec, char Supp[])
{
	int	i, j;

	int	iblank, itab;

	int	nphases = 0;

	int	nf;

	int	k, k1, st;
	int	supp_id;

	char	*arg_list[MAX_ARGS];
	char	aux[12];

	*Pmin = *Psec = *Pres = *Ppol = INT_UNDEF;
	*Smin = *Ssec = *Sres = INT_UNDEF;
	*pPmin = *pPsec = *Suppmin = *Suppsec = INT_UNDEF;
	strcpy(Supp, "-12345");

	if (line[index] == '\n') {
		fprintf(stderr, "get_phases warning: ");
		fprintf(stderr, "No phase data for this card\n");
		return nphases;	
	}


	/* P */

	i = index;
	if (line[i] != '\t') {
		j = 0;
		while (! isspace(line[i])) {
			aux[j++] = line[i++];
		}
		aux[j] = '\0';
		sscanf(aux, "%d", Pmin);
		if (*Pmin < 0 || *Pmin > 20) {
			fprintf(stderr, "Warning: Invalid P minute: %s\n", line);
			/*return -1;*/
		}

		/* skip all blank spaces before P seconds */
		while (isspace(line[i]))
			i++;

		/* P seconds [and polarity] */
		j = 0;
		while (! isspace(line[i])) {
			aux[j++] = line[i++];
		}
		aux[j] = '\0';
		if (aux[j-1] == 'K') {
			*Ppol = 'K';
			sscanf(aux, "%dK", Psec);
		} else if (aux[j-1] == 'A') {
			*Ppol = 'A';
			sscanf(aux, "%dA", Psec);
		} else if (isdigit(aux[j-1])) {
			*Ppol = ' ';
			sscanf(aux, "%d", Psec);
		} else {
			fprintf(stderr, "P sec error: %s\n", line);
			return -1;
		}
		if (*Psec < 0 || *Psec >= 60) {
			fprintf(stderr, "Invalid P second: %s\n", line);
			return -1;
		}
		/* skip all blank spaces before P residual */
		while (isspace(line[i]))
			i++;

		j = 0;
		while (line[i] != '\t' && line[i] != '\n' && line[i] != '\r') {
			if (isdigit(line[i]) || line[i] == '-')
				aux[j++] = line[i++];
			else {
				fprintf(stderr, "P residual error: %s\n", line);
				return -1;
			}
		}
		aux[j] = '\0';
		if (strlen(aux) > 4) {
			fprintf(stderr, "P residual error (too long): %s\n", aux);
			return -1;
		}
		if (aux[0] == '-' && strlen(aux) == 1) {
			fprintf(stderr, "Error in negative P residual: %s\n", aux);
			return -1;
		}
		sscanf(aux, "%d", Pres);

		nphases++;

	} else { /* no P */
		fprintf(stderr, "No P for this card\n%s\n", line);
	}

	/* at the end of P block:
	i = 0 if first character is a tab (we assume this means no P)
	i = position of first character after P residual.  This position is
	a tab or a new line (if not subroutine produces a "P residual error")*/

	/* check in we are at the end of the line */

	if (line[i] == '\n' || line[i] == '\r') return nphases;

	/* S */

	/* count how many spaces and tabs before first digit */

	iblank = itab = 0;
	while (isspace(line[i])) {
		if (line[i] == '\t') itab++;
		iblank++;
		i++;
	}
	/* check here again for end of line? */
	if (itab != iblank) {
		fprintf(stderr, "Verify separation between P and next phase\n");
		return -1;
	}
	for (k = 0; k < MAX_ARGS; k++) {
		arg_list[k] = (char *)malloc(8);
		arg_list[k][0] = '\0';
	}

	/* read extra lines in aux variable; error if aux is something */
	k = sscanf(&line[i], "%s%s%s%s%s%s%s%s", 
	arg_list[0], arg_list[1], arg_list[2], arg_list[3], arg_list[4], 
	arg_list[5], arg_list[6], arg_list[7]);

/*
	for (i = 0; i < k; i++)
		fprintf(stderr, "**%s**\n", arg_list[i]);
*/

	/* see if last argument is a phase id */

	if (arg_list[k-1][0] == 'P' || arg_list[k-1][0] == 'S' ||
	    arg_list[k-1][0] == '*' || arg_list[k-1][0] == 'L' ||
	    arg_list[k-1][0] == 'Q') {
		supp_id = TRUE;
	} else if (isdigit(arg_list[k-1][0]) || arg_list[k-1][0] == '-') {
		supp_id = FALSE;
	} else {
		fprintf(stderr, "Warning: unusual phase id: %s\n", arg_list[k-1]);
		return -1;
	}

	/* check for strange characters in the other arguments NOT DONE YET!!!*/

	if (supp_id) k1 = k - 1;
	else k1 = k;

	for (i = 0; i < k1; i++) {
		for (j = 0; j < strlen(arg_list[i]); j++) {
			if (!isdigit(arg_list[i][j]) && arg_list[i][j] != '-') {
				fprintf(stderr, "Invalid character in numeric field: %s\n", arg_list[i]);
				fprintf(stderr, "%s\n", line);
				return -1;
			}
		}
	}


	if (k == 0) {
		;
	} else if (k == 1) {
		fprintf(stderr, "Impossible combination: k=1\n");
		fprintf(stderr, "%s\n", line);
		return -1;
	} else if (k == 2) {
		if (supp_id) {
			fprintf(stderr, "Impossible combination: k=2 && supp_id\n");
			fprintf(stderr, "%s\n", line);
			return -1;
		} else {
			if (! debug) {
				if (supp_all) {
					supp_id = TRUE;
				} else {
				fprintf(stderr, "get_phases: ambiguous phase identification\n");
				fprintf(stderr, "%s", line);
				fprintf(stderr, "0=pP, 1=Supp: ");
				scanf("%d", &supp_id);
				}
				if (supp_id) {
					sscanf(arg_list[0], "%d", Suppmin);
					sscanf(arg_list[1], "%d", Suppsec);
				} else {
					sscanf(arg_list[0], "%d", pPmin);
					sscanf(arg_list[1], "%d", pPsec);
				}
			}
			nphases++;
		}
	} else if (k == 3) {
		if (supp_id) {
			sscanf(arg_list[0], "%d", Suppmin);
			sscanf(arg_list[1], "%d", Suppsec);
			sscanf(arg_list[2], "%s", Supp);
		} else {
			sscanf(arg_list[0], "%d", Smin);
			sscanf(arg_list[1], "%d", Ssec);
			sscanf(arg_list[2], "%d", Sres);
		}
		nphases++;
	} else if (k == 4) {
		if (supp_id) {
			fprintf(stderr, "Impossible combination: k=4 && supp_id\n");
			fprintf(stderr, "%s\n", line);
			return -1;
		} else {
			sscanf(arg_list[0], "%d", pPmin);
			sscanf(arg_list[1], "%d", pPsec);
			sscanf(arg_list[2], "%d", Suppmin);
			sscanf(arg_list[3], "%d", Suppsec);
			nphases += 2;
		}
	} else if (k == 5) {
		if (supp_id) {
			sscanf(arg_list[0], "%d", pPmin);
			sscanf(arg_list[1], "%d", pPsec);
			sscanf(arg_list[2], "%d", Suppmin);
			sscanf(arg_list[3], "%d", Suppsec);
			sscanf(arg_list[4], "%s", Supp);
		} else {
			sscanf(arg_list[0], "%d", Smin);
			sscanf(arg_list[1], "%d", Ssec);
			sscanf(arg_list[2], "%d", Sres);
			if (! debug) {
				if (supp_all) {
					supp_id = TRUE;
				} else {
				fprintf(stderr, "get_phases: ambiguous phase identification\n");
				fprintf(stderr, "%s", line);
				fprintf(stderr, "0=pP, 1=Supp: ");
				scanf("%d", &supp_id);
				}
				if (supp_id) {
					sscanf(arg_list[3], "%d", Suppmin);
					sscanf(arg_list[4], "%d", Suppsec);
				} else {
					sscanf(arg_list[3], "%d", pPmin);
					sscanf(arg_list[4], "%d", pPsec);
				}
			}
		}
		nphases += 2;
	} else if (k == 6) {
		if (supp_id) {
			sscanf(arg_list[0], "%d", Smin);
			sscanf(arg_list[1], "%d", Ssec);
			sscanf(arg_list[2], "%d", Sres);
			sscanf(arg_list[3], "%d", Suppmin);
			sscanf(arg_list[4], "%d", Suppsec);
			sscanf(arg_list[5], "%s", Supp);
			nphases += 2;
		} else {
			fprintf(stderr, "Impossible combination: k=6 && !supp_id\n");
			fprintf(stderr, "%s\n", line);
			return -1;
		}
	} else if (k == 7) {
		if (supp_id) {
			fprintf(stderr, "Impossible combination: k=7 && supp_id\n");
			fprintf(stderr, "%s\n", line);
			return -1;
		} else {
			sscanf(arg_list[0], "%d", Smin);
			sscanf(arg_list[1], "%d", Ssec);
			sscanf(arg_list[2], "%d", Sres);
			sscanf(arg_list[3], "%d", pPmin);
			sscanf(arg_list[4], "%d", pPsec);
			sscanf(arg_list[5], "%d", Suppmin);
			sscanf(arg_list[6], "%d", Suppsec);
			nphases += 3;
		}
	} else if (k == 8) {
		if (supp_id) {
			sscanf(arg_list[0], "%d", Smin);
			sscanf(arg_list[1], "%d", Ssec);
			sscanf(arg_list[2], "%d", Sres);
			sscanf(arg_list[3], "%d", pPmin);
			sscanf(arg_list[4], "%d", pPsec);
			sscanf(arg_list[5], "%d", Suppmin);
			sscanf(arg_list[6], "%d", Suppsec);
			sscanf(arg_list[7], "%s", Supp);
			nphases += 3;
		} else {
			fprintf(stderr, "Impossible combination: k=8 && !supp_id\n");
			fprintf(stderr, "%s\n", line);
			return -1;
		}
	} else {
		fprintf(stderr, "Impossible combination: k=4 && supp_id\n");
		fprintf(stderr, "%s\n", line);
		return -1;
	}

	/* verify that all sec and min values make sense */

	if (*Smin != -12345) {
		if (*Smin < 0 || *Smin > 27)
			fprintf(stderr, "Warning: S minute probably wrong:\n%s\n", line);
		if (*Ssec < 0 || *Ssec > 59) {
			fprintf(stderr, "Error: S second wrong:\n%s\n", line);
			return -1;
		}
	}
	if (*pPmin != -12345) {
		if (*pPmin < 0 || *pPmin > 21)
			fprintf(stderr, "Warning: pP minute probably wrong:\n%s\n", line);
		if (*pPsec < 0 || *pPsec > 59) {
			fprintf(stderr, "Error: pP second wrong:\n%s\n", line);
			return -1;
		}
	}
	if (*Suppmin != -12345) {
		if (*Suppmin < 0  || *Suppmin >= 100) {
			fprintf(stderr, "Error: Supp minute wrong:\n%s\n", line);
			return -1;
		}
		if (*Suppsec < 0 || *Suppsec > 59) {
			fprintf(stderr, "Error: Supp second wrong:\n%s\n", line);
			return -1;
		}
	}
	for (i = 0; i < MAX_ARGS; i++) {
		free((char *)arg_list[i]);
	}

	return nphases;
}
