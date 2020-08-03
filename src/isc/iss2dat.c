#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*******************************************************************************
Program: iss2dat.c

Purpose: Reads single-page ISS text files and writes to "dat" format

Author:	Antonio Villasenor, USGS, Golden

Date:	29-MAR-1999
*******************************************************************************/

/* maximum filename length */
#ifndef FILENAME_MAX
#define FILENAME_MAX 1024
#endif

#define TRUE 1
#define FALSE 0

#define MAXLINE 81
#define MAXCARDS 1000

int trim(char *s);

int main(int argc, char *argv[])
{
	FILE	*fpiss, *fpdat;

	char	issfile[FILENAME_MAX];
	char	datfile[FILENAME_MAX];

	char	line[MAXLINE+1];
	char	*card[MAXCARDS];

	int	page, ppage;

	int	iy, ipy;
	int	im, ipm;
	int	id, ipd;

	char	ord = 'A' - 1;
	int	evno = 0;

	int	header = FALSE;	/* true while reading hypocenter header.  False otherwise */

	int	neq = 0;	/* number of events in data file */
	int	nobs = 0;	/* number of observations (stations) in current event */

	int	levent = FALSE;

	int	prompt = TRUE;
	int	verbose = FALSE;
	int	infile = FALSE;

	char	process[256];

	int	i, j, k;

	char	stnm[13];
	char	month[16];

	char *month_name[13] = {
		"illegal month",
		"JANUARY", "FEBRUARY", "MARCH", "APRIL", "MAY", "JUNE",
		"JULY", "AUGUST", "SEPTEMBER", "OCTOBER", "NOVEMBER",
		"DECEMBER"
	};

	/* Initialize "date" and "previous date" variables */
	page = ppage = 0;
	iy = ipy = 0;
	im = ipm = 0;
	id = ipd = 0;

	/* Decode flag options */

	for (i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			switch (argv[i][1]) {
				case 'N':
					prompt = FALSE;
					break;
				case 'V':
					verbose = TRUE;
					break;
				default:
					break;
			}
		} else {
			/* copy all arguments that are files to array of pointers to char */

			if ((fpiss = fopen(argv[i], "r")) == NULL) {
				fprintf(stderr, "Error in dat2isc: ");
				fprintf(stderr, "cannot open %s \n", argv[i]);
				return 1;
			} else {
				strcpy(issfile,argv[i]);
				infile = TRUE;
			}
		}
	}

	/* make sure that file names are in alphabetical order */
	/* warning if they are not */

	if (! infile) {
		/* read from standard input */
		fpiss = stdin;
	} else {
		if (verbose) {
			fprintf(stderr, "iss2dat: ");
			fprintf(stderr, "processing file %s\n", issfile);
		}
	}
	/* initialize arrays */

	for (i = 0; i < MAXCARDS; i++) {
		card[i] = (char *)malloc(MAXLINE);
		if (card[i] != NULL) {
			card[i][0] = '\0';
		} else {
			fprintf(stderr, "Cannot allocate memory for card[]\n");
			return 1;
		}
	}

	/* Open DAT file */

	if ((fpdat = fopen("output.dat", "w")) == NULL) {
		fprintf(stderr, "Error in iss2dat: ");
		fprintf(stderr, "cannot open output  file in dat format\n");
		return 1;
	}

	i = 0;
	neq = 0;
	levent = FALSE;

	while (fgets(line, MAXLINE, fpiss) != NULL) {
		trim(line);
		if (i == 0) {
			if (strstr(line, "PAGE") == (char *)NULL) {
				fprintf(stderr,
				"ERROR: first line must contain year and page number.\n");
				return 1;
			} else {
				if (sscanf(line, "%d PAGE %d", &iy, &page) != 2) {
					fprintf(stderr,
					"Error reading year and page number.\n");
					return 1;
				} else {
					iy -= 1900;
					ipy = iy;
					ppage = page;
				}
			}
		}



		if (strstr(line, "PAGE") != (char *)NULL) {
			if (i != 0) {
				if (sscanf(line, "%d PAGE %d", &iy, &page) != 2) {
					fprintf(stderr,
					"Error reading year and page number.\n");
					return 1;
				} else {
					iy -= 1900;
					if (iy != ipy) {
						fprintf(stderr,
						"ERROR: processing more than one year: %d %d\n",
						ipy, iy);
						return 1;
					} 
					if (page != ppage+1) {
						fprintf(stderr,
						"WARNING: page(s) missing between %4d and %4d\n", ppage, page);
					}
					ppage = page;
				}
			}
			i++;
		} else if (strstr(line, "EPICENTRE") != (char *)NULL) {
			sscanf(line, "%s %d", month, &id);
			if (ipd == 0) ipd = id;
			/* find month number */
			im = 0;
			for (k = 1; k < 13; k++)
				if (strcmp(month, month_name[k]) == 0) {
					im = k;
					break;
				}
			if (k == 0) {
				fprintf(stderr, "ERROR: illegal month: %s\n", month);
				return 1;
			}
			if (ipm == 0) ipm = im;
			/* change of day and/or month */
			if (im == ipm && id == ipd) {
				ord++;
			} else {
				ord = 'A';
				ipd = id;
				ipm = im;
			}
			/* Prompt if we want to process this event */
			if (prompt) {
				printf("\nHypocenter line found:\n %s\n", line);
				printf("Do you want to process this event (y/n)? ");
				scanf("%s", process);
			} else {
				strcpy(process, "y"); /* if no prompt, process all events */
			}

			if (process[0] == 'n' || process[0] == 'N') {
				levent = FALSE;
			} else {
				/* Initialize variables for this event */
				j = 1;
				levent = TRUE;
				fprintf(fpdat, "%02d%02d%02d%c%03d %s\n",
				iy, im, id, ord, j++, &line[5]);
			}
		} else if (strstr(line, "A=") != (char *)NULL) {
			if (levent)
				fprintf(fpdat, "%02d%02d%02d%c%03d %s\n",
				iy, im, id, ord, j++, &line[5]);
		} else if (strstr(line, "G=") != (char *)NULL) {
			if (levent)
				fprintf(fpdat, "%02d%02d%02d%c%03d %s\n",
				iy, im, id, ord, j++, &line[5]);
		} else if (strstr(line, "DEPTH OF FOCUS") != (char *)NULL) {
			if (levent)
				fprintf(fpdat, "%02d%02d%02d%c%03d %s\n",
				iy, im, id, ord, j++, &line[5]);
		} else if (strstr(line, "SE=") != (char *)NULL) {
			if (levent)
				fprintf(fpdat, "%02d%02d%02d%c%03d %s\n",
				iy, im, id, ord, j++, &line[5]);
		} else if (strstr(line, "O-C") != (char *)NULL) {
			;
		} else if (strstr(line, "DEG.") != (char *)NULL) {
			;
		} else if (isupper(line[0])) {
			if (levent) {
				strncpy(stnm, line, 12);
				stnm[12] = '\0';
				fprintf(fpdat, "%02d%02d%02d%c%03d %-12s %s\n",
				iy, im, id, ord, j++, stnm, &line[17]);
			}
		} else {
			for (k = 0; k < strlen(line); k++) {
				if (! isspace(line[k])) {
					fprintf(stderr, "Unknown line type:\n%s", line);
					return 1;
				}
			}
		}

		evno = ord - 'A' + 1;


	}

	fclose(fpiss);
	fclose(fpdat);

	for (i = 0; i < MAXCARDS; i++)
		free(card[i]);

	return 0;
}
