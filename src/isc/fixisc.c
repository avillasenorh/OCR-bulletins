#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*******************************************************************************
Program: fisisc.c

Purpose: Reads single or multiple ISC 96-byte format file(s) and fixes:
         - next record category field (-1 for last card in file)
         - number of stations for this event
         Writes corrected file to stdout.
Usage:

% fixisc iscfile(s) > out_iscfile

Author:	Antonio Villasenor, USGS, Golden

Date:	16-JAN-2000
*******************************************************************************/
/* maximum filename length */
#ifndef FILENAME_MAX
#define FILENAME_MAX 1024
#endif

#define TRUE 1
#define FALSE 0

#define MAXFILES 100	/* maximum number of ISC 96-byte format files in argument list */
#define MAXCARDS 2000
#define MAXLINE 132

int main(int argc, char *argv[])
{
	FILE	*fpisc;

	char	*iscfile[MAXFILES];	/* input ISC files */

	int	nfiles;		/* number of input ISC files */
	char	*p;		/* auxiliary pointer to char used to assign ISC files */
	int	len;		/* length of input ISC files */

	char	line[MAXLINE];
	char	pline[MAXLINE];

	int	rc, prc;

	int	neq = 0;	/* number of events in data file */

	int	nsta = 0;	/* number of stations for current event */
	int	npha = 0;	/* number of phases   for current event */
	int	ntel = 0;
	char	nobs[5];

	int	ncard;
	char	*card[MAXCARDS];

	int	verbose = FALSE;    /* if TRUE, writes processing information to stderr */

	int	i, j, k, l;

	/* Decode flag options */

	if (argc <= 1) {
		fprintf(stderr, "usage: fixisc iscfile(s) > out_iscfile\n");
		exit(1);
	}

	nfiles = 0;
	for (i = 1; i < argc; i++) {
		/* copy all arguments that are files to array of pointers to char */

		len = strlen(argv[i]);
		if (nfiles >= MAXFILES || len > FILENAME_MAX ||
			(p = (char *)malloc(FILENAME_MAX)) == NULL) {
			fprintf(stderr, "Error in ISC file # %d:%s\n", nfiles, argv[i]);
			exit(1);
		} else {
			strcpy(p, argv[i]);
			iscfile[nfiles++] = p;
		}
	}
	/* if no input file(s), try to read from stdin */
	if (nfiles <= 0) {
			fpisc = stdin;
			nfiles = 1;
	}

	/* initialize arrays */
	for (i = 0; i < MAXCARDS; i++) {
		card[i] = (char *)malloc(MAXLINE);
		if (card[i] != NULL) {
			card[i][0] = '\0';
		} else {
			fprintf(stderr, "Cannot allocate memory for card[%d]\n", i);
			exit(1);
		}
	}

	/* begin loop over all ISC files */

	neq = 0;  /* counter for number of events processed in this run */

	for (k = 0; k < nfiles; k++) {

		if ((fpisc = fopen(iscfile[k], "r")) == NULL) {
			fprintf(stderr, "fixisc error: ");
			fprintf(stderr, "cannot open output input ISC file %s\n", iscfile[k]);
			exit(1);
		} else {
			if (verbose) fprintf(stderr, "Processing file: %s\n", iscfile[k]);
		}

		i = 0;    /* line number for input ISC files */

		while (fgets(line, MAXLINE, fpisc) != NULL) {

			sscanf(line, "%2d", &rc);

			/* valid record categories range from -1 to 9 */
			if (rc < -1 || rc > 9) { /* unknown record category */
				fprintf(stderr, "fixisc error: ");
				fprintf(stderr, "unknown record category: %d\n%s\n", rc, line);
				exit(1);
			}

			/* first line must contain record category 1 */
			if (i == 0 && rc != 1 ) {
				fprintf(stderr, "fixisc error: ");
				fprintf(stderr, "first line must contain record category 1:\n%s",line);
				exit(1);
			}

			if (rc == 1) {
				/* write previous event (if any) */
				if (neq > 0) {
					/* fix previous line */
						sprintf(card[j-1], "%2d%2d%s", prc, rc, &pline[4]);
					/* fix header line (record category 1) */
						sprintf(nobs, "%4d", npha);
						nobs[4] = '\0';
						card[0][79] = nobs[0];
						card[0][80] = nobs[1];
						card[0][81] = nobs[2];
						card[0][82] = nobs[3];
					/* write previous event to stdout */
						for (l = 0; l < j; l++)
							printf("%s", card[l]);

				}
				neq++;
				nsta = 0;
				npha = 0;
				j = 0; /* number of cards for this event */
			} else {
				if (rc == 5) {
					nsta++;
					npha++;
				} else if (rc == 6) {
					npha++;
				}
				sprintf(card[j-1], "%2d%2d%s", prc, rc, &pline[4]);
			}
			strcpy(pline, line);
			prc = rc;
			i++;
			j++;

		} /* end of while loop over all lines inside each input ISC file */
		fclose(fpisc);



	} /* end of for loop over each input ISC file */

	/* write last event (if any) */
	if (neq > 0) {
		/* fix previous line */
			sprintf(card[j-1], "%2d%2d%s", prc, -1, &pline[4]);
		/* fix header line (record category 1) */
			sprintf(nobs, "%4d", npha);
			nobs[4] = '\0';
			card[0][79] = nobs[0];
			card[0][80] = nobs[1];
			card[0][81] = nobs[2];
			card[0][82] = nobs[3];
		/* write previous event to stdout */
			for (l = 0; l < j; l++)
				printf("%s", card[l]);
	}


	for (i = 0; i < MAXCARDS; i++)
		free(card[i]);

	return 0;
}
