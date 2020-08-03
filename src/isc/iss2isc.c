#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*******************************************************************************
Program: iss2isc.c

Purpose: Reads single- or multiple-page ISS text files(s) and writes directly to
         ISC 96-byte format

Options:

-C: write comment cards
-L: write long waves
-N: write events without phase data in tabular form
-P: prompt before writting an event
-V: verbose

Usage:

% iss2isc [-C] [-L] [-N] [-P] [-V] issfile(s)

Author:	Antonio Villasenor, USGS, Golden

Date:	1-AUG-1999
*******************************************************************************/
/* maximum filename length */
#ifndef FILENAME_MAX
#define FILENAME_MAX 1024
#endif

#define TRUE 1
#define FALSE 0

#define INT_UNDEF -12345
#define FLT_UNDEF -12345.0

#define MAXFILES 100	/* maximum number of ISS text files in argument list */
#define MAXSTA 1500
#define MAXLINE 132
#define MAXCARDS 4

int trim(char *s);
int ltype(char *s, int plt);
int r_iss_epic(char *s, int *mo, int *dy, int *hr, int *mn, int *sc,
  float *y, float *x, float *z, int *y_acc, int *x_acc, int *z_acc, char *dt);
/*void lookup_(float *ALAT, char *DRCLAT, float *ALNG, char *DRCLNG, int *IGREG, int *ISREG);*/
int w_heq(char *s, int year, int month, int day, int hour, int minute, float second,
  float lat, float lon, float depth, float mag, float imag, int ntel, float iscdepth,
  int igreg, int ndep);
int w_isc1(char *s, int nrc, int year, int month, int day, int hour, int minute,
  float second, int sec_acc, float lat, int lat_acc, float lon, int lon_acc,
  float depth, int depth_acc, int igreg, int isreg, int nobs);

int get_isc_cards(char *s, char *card[], int longwaves, char *prev_sta,
  int nsta, char *sta_code[], char *sta_name[],
  int year, int month, int day, int hour, int minute, int second);

int main(int argc, char *argv[])
{
	FILE	*fpiss, *fpisc, *fpheq;

	char	*issfile[MAXFILES];	/* input ISS files */
	char	iscfile[] = "DAT";
	char	heqfile[] = "HEQ";

	int	nfiles;		/* number of input ISS files */
	char	*p;		/* auxiliary pointer to char used to assing ISS files */
	int	len;		/* length of input ISS files */

	char	line[MAXLINE];

	FILE	*fpsta;
	char    *sta_name[MAXSTA];
	char    *sta_code[MAXSTA];
	int     nsta = 0;
	char	prev_sta[20];

	int	lt, plt;
	int	page, ppage;

	int	year, pyear;

	int	evno = 0;

	int	header = FALSE;	/* true while reading hypocenter header.  False otherwise */

	int	neq = 0;	/* number of events in data file */
	int	nobs = 0;	/* number of observations (stations) in current event */

	int	month, day, hour, minute, second;
	float	lat, lon, depth;
	int	sec_acc, lat_acc, lon_acc, depth_acc;
	char	data[MAXLINE];
	float	mag = 0.0,
		imag = 0.0,
		iscdepth = FLT_UNDEF;
	int	ntel = 0,
		igreg = 0,
		isreg = 0,
		ndep = 0;
	int	nrc = 0;
	char	heq_card[MAXLINE];
	char	card1[MAXLINE];

	int	ncard;
	char	*card[MAXCARDS];

	char	process[256];

	int	levent = FALSE;
	int	tabular = FALSE;    /* phase card in tabular form follows */

	int	comment = FALSE;    /* if TRUE, writes comment cards (if present) */
	int	longwaves = FALSE;  /* if TRUE, writes phases in L column (surface waves) */
	int	nodata = FALSE;     /* if TRUE, writes events without phase data */
	int	prompt = FALSE;     /* if TRUE, prompts user whether to write or not current event */
	int	verbose = FALSE;    /* if TRUE, writes processing information to stderr */

	int	i, j, k;

	char	stnm[13];

	/* Initialize "date" and "previous date" variables */
	page = ppage = 0;
	year = pyear = 0;

	/* Decode flag options */

	nfiles = 0;
	for (i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			switch (argv[i][1]) {
				case 'C':
					comment = TRUE;
					break;
				case 'L':
					longwaves = TRUE;
					break;
				case 'N':
					nodata = TRUE;
					break;
				case 'P':
					prompt = TRUE;
					break;
				case 'V':
					verbose = TRUE;
					break;
				default:
					break;
			}
		} else {
			/* copy all arguments that are files to array of pointers to char */

			len = strlen(argv[i]);
			if (nfiles >= MAXFILES || len > FILENAME_MAX ||
				(p = (char *)malloc(FILENAME_MAX)) == NULL) {
				fprintf(stderr, "Error in ISS file # %d:%s\n", nfiles, argv[i]);
				exit(1);
			} else {
				strcpy(p, argv[i]);
				issfile[nfiles++] = p;
			}
		}
	}
	/* if no input file(s), try to read from stdin */
	if (nfiles <= 0) {
		if (prompt) { /* no files passed incompatible with prompt option */
			fprintf(stderr, "iss2isc error: ");
			fprintf(stderr, "-P option incompatible with no input files\n");
			exit(1);
		} else { /* read from standard input */
			fpiss = stdin;
		}
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
	for (i = 0; i < MAXSTA; i++) {
		sta_name[i] = (char *)malloc(21);
		sta_code[i] = (char *)malloc(5);
		sta_name[i][0] = '\0';
		sta_code[i][0] = '\0';
	}

	/* open and read station file */

	/*if ((fpsta = fopen("historical.codes", "r")) == NULL) {*/
	if ((fpsta = fopen("/Users/antonio/Projects/ISS/stations/historical.codes", "r")) == NULL) {
 		fprintf(stderr, "Error in iss2isc: ");
		fprintf(stderr, "cannot open output station file: %s\n",
		"historical.codes");
		exit(1);
	}
	nsta = 0;
	while (fgets(line, MAXLINE, fpsta) != NULL) {
		if (nsta >= MAXSTA) {
			fprintf(stderr, "iss2isc error: ");
			fprintf(stderr, "station file too large for current value of MAXSTA\n");
			exit(1);
		}
		sscanf(line,"%s %[^\n]\n", sta_code[nsta], sta_name[nsta]);
		trim(sta_name[nsta]);
		nsta++;
	}

	/* open DAT file */
	if ((fpisc = fopen(iscfile, "w")) == NULL) {
		fprintf(stderr, "iss2isc error: ");
		fprintf(stderr, "cannot open output ISC file\n");
		exit(1);
	}
	/* open HEQ file */
	if ((fpheq = fopen(heqfile, "w")) == NULL) {
		fprintf(stderr, "iss2isc error: ");
		fprintf(stderr, "cannot open output HEQ file\n");
		exit(1);
	}

	/* begin loop over all ISS files */
	neq = 0;            /* counter for number of events processed in this run */
	plt = -1;           /* previous line type */
	pyear = ppage = -1;   /* previous year and previous page */
	levent = FALSE;
	for (k = 0; k < nfiles; k++) {
		if ((fpiss = fopen(issfile[k], "r")) == NULL) {
			fprintf(stderr, "Error in iss2isc: ");
			fprintf(stderr, "cannot open output input ISS file %s\n", issfile[k]);
			exit(1);
		} else {
			if (verbose) fprintf(stderr, "Processing file: %s\n", issfile[k]);
		}

		/* if file has standard name = ?yy_pppp.dat where yy = year-1900 and pppp = page number
		verify consistency between file name and first line in file */

		i = 0; /* line number for input ISS files */

		while (fgets(line, MAXLINE, fpiss) != NULL) {
			lt = ltype(line, plt);
			/*fprintf(stderr, "%2d %2d\n", lt, plt);*/
			trim(line);
			i++;
			/* first line must contain year and page number */
			if (i == 0 && lt != 0) {
				fprintf(stderr, "iss2isc error: ");
				fprintf(stderr, "first line must contain year and page number:\n%s",line);
				exit(1);
			}

			/* process each line type */
			if (lt == 0) {
				if (sscanf(line, "%d %d", &year, &page) != 2) {
					fprintf(stderr, "iss2isc error: ");
					fprintf(stderr, "cannot read year and page number:\n%s",line);
					exit(1);
				}
				if (pyear != -1) {
					if (year != pyear) {
						fprintf(stderr, "iss2isc error: ");
						fprintf(stderr, "processing different years: %d %d\n", pyear, year);
						exit(1);
					}
					pyear = year;
				} 
				if (ppage != -1) {
					if (page != ppage+1) {
						fprintf(stderr, "iss2isc warning: ");
						fprintf(stderr, "pages missing: %4d-%4d\n", page, ppage);
					}
					ppage = page;
				}
			} else if (lt == 1) { /* epicenter */

				/* Prompt if we want to process this event */
				tabular = FALSE;
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
					levent = TRUE;
				}

				if (levent) { /* read epicenter information from line */

					r_iss_epic(line, &month, &day, &hour, &minute, &second,
					&lat, &lon, &depth, &lat_acc, &lon_acc, &depth_acc, data);
					if (second == INT_UNDEF) {
						second = 0;
						sec_acc = 2;
					} else sec_acc = 0;
					if (depth == FLT_UNDEF) {
						depth = 35.0;
						iscdepth = 0.0;
					} else iscdepth = depth;
					depth_acc = 0;
					/*lookup_(&lat, " ", &lon, " ", &igreg, &isreg);*/
					w_heq(heq_card, year, month, day, hour, minute, (float)second,
					lat, lon, depth, mag, imag, ntel, iscdepth, igreg, ndep);
					fprintf(fpheq, "%s\n", heq_card);

					w_isc1(card1, nrc, year, month, day, hour, minute,
					(float)second, sec_acc, lat, lat_acc, lon, lon_acc,
					iscdepth, depth_acc, igreg, isreg, nobs);

					fprintf(fpisc, "%s\n", card1);
				}

			} else if (lt == 2) { /* depth of focus */
				;
			} else if (lt == 3) { /* ignore first  constant line (if present) */
				;
			} else if (lt == 4) { /* ignore second constant line (if present) */
				;
			} else if (lt == 5) { /* ignore standard error  line (if present) */
				;
			} else if (lt == 6) { /* First table header */
				tabular = TRUE;
			} else if (lt == 7) { /* Second table header */
				tabular = TRUE;
			} else if (lt == 8) { /* Phase card (tabular or not) or comment */
				if (levent && tabular) {
					ncard = get_isc_cards(line, card, longwaves,
					prev_sta, nsta, sta_code, sta_name,
					year, month, day, hour, minute, second);
					if (ncard > 0)
						for (i = 0; i < ncard; i++) {
							fprintf(fpisc, "%s\n", card[i]);
						}
				} else {
					;
				}
			} else if (lt == 9) { /* Additional readings */
				tabular = FALSE;
			} else if (lt == 10) { /* Continued on next page */
				;
			} else if (lt == 11) { /* Undetermined shock or Readings also at */
				;
			} else if (lt == 12) { /* For notes see next page */
				tabular = FALSE;
			} else if (lt == 13) { /* blank line: no action */
				;
			} else { /* unknown phase */
				fprintf(stderr, "iss2isc error: ");
				fprintf(stderr, "unknown line type: %d\n%s\n", lt, line);
				exit(1);
			}
		
		plt = lt;
		} /* end of while loop over all lines inside each input ISS file */
		fclose(fpiss);
	} /* end of for loop over each input ISS file */

	fclose(fpisc);
	fclose(fpheq);

	for (i = 0; i < MAXCARDS; i++)
		free(card[i]);

	return 0;
}
