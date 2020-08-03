#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*******************************************************************************
Program: dat2sta.c

Purpose: Reads text files and extracts station information

Author:	Antonio Villasenor, ICTJA-CSIC, Barcelona

Date:	16-FEB-2006
*******************************************************************************/

/* maximum filename length */
#ifndef FILENAME_MAX
#define FILENAME_MAX 1024
#endif

#define MAXLINE	96
#define MAXSTA 1000
#define TRUE	1
#define FALSE	0

int trim(char s[]);

int main(int argc, char *argv[])
{
	FILE	*fpdat, *fpisc, *fpheq, *fpsta;

	char	datfile[FILENAME_MAX];
	char	iscfile[FILENAME_MAX];

	char	line[MAXLINE+1];

	int	icard = 0;

	char	*sta_name[MAXSTA];
	char	*sta_code[MAXSTA];
	int	ntim[MAXSTA];

	int	nsta = 0;
	int	ist;
	int	iind;

	char	stnm[32];
	float	delta, azim;

	int	yr, mo, dy;
	int	year;
	char	ord;
	int	lineno, evno;

	char	evname[8];

	char	csta[5];

	int	header = FALSE;	/* true while reading hypocenter header.  False otherwise */

	int	ntot = 0;	/* total number of phase cards */
	int	neq = 0;	/* number of events in data file */
	int	nobs = 0;	/* number of observations (stations) in current event */

	int	verbose = FALSE;
	int	infile = FALSE;

	int	i;

	/* Decode flag options */

	for (i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			switch (argv[i][1]) {
				case 'V':
					verbose = TRUE;
					break;
				default:
					break;
			}
		} else {
			if ((fpdat = fopen(argv[i], "r")) == NULL) {
				fprintf(stderr, "ERROR: ");
				fprintf(stderr, "cannot open %s \n", argv[i]);
				exit(1);
			} else {
				strcpy(datfile,argv[i]);
				infile = TRUE;
			}
		}
	}

	if (! infile) {
		/* read from standard input */
		fpdat = stdin;
	} else {
		if (verbose) {
			fprintf(stderr, "dat2isc: ");
			fprintf(stderr, "processing file %s\n", datfile);
		}
	}

	/* initialize arrays */

	for (i = 0; i < MAXSTA; i++) {
		sta_name[i] = (char *)malloc(14);
		sta_code[i] = (char *)malloc(5);
		sta_name[i][0] = '\0';
		sta_code[i][0] = '\0';
		ntim[i] = 0;
	}


	/* Open and read station file */

	if ((fpsta = fopen("/data/PROJECTS/Historical/stations/1960.codes", "r")) == NULL) {
		fprintf(stderr, "ERROR: ");
		fprintf(stderr, "cannot open output station file: %s\n",
		"/data/PROJECTS/Historical/stations/1960.codes");
		exit(1);
	}
	i = 0;
	while (fgets(line, MAXLINE, fpsta) != NULL) {
		if (line[0] != '!') {
			strncpy(sta_name[i], &line[5], 12);
			sta_name[i][12] = '\0';
			strncpy(sta_code[i], &line[0], 4);
			sta_code[i][4] = '\0';
			trim(sta_name[i]);
			trim(sta_code[i]);
			i++;
		}
	}
	nsta = i;
	if (verbose)
	fprintf(stderr, "Number of station names in 1960.codes: %d\n", nsta);
	fclose(fpsta);

	neq = 0;
	while (fgets(line, MAXLINE, fpdat) != NULL) {

		/* read year from line header */
		sscanf(line, "%2d%2d%2d%c%3d",
		&yr, &mo, &dy, &ord, &lineno);

		year = yr + 1900;

		evno = ord - 'A' + 1;

		if (lineno == 1) {
			neq++;
			strncpy(evname, line, 7);
			evname[7] = '\0';
		}
		if (isupper(line[11])) {

			if (line[27] != '.') {
				fprintf(stderr, "ERROR: wrong phase card:\n%s",line);
				exit(1);
			}
			ntot++;

			strncpy(stnm,&line[11],12);
			stnm[12] = '\0';
			trim(stnm);
	
			/* find station code for stnm */

			iind=-1;
			for (ist = 0; ist < nsta; ist++) {
				if (strcmp(stnm, sta_name[ist]) == 0) {
					iind=ist;
					ntim[ist]++;	
					break;
				}
			}
			if (iind < 0)
			fprintf(stderr, "ERROR: station code not found for: %s\n", stnm);
		}
	}

	for (i = 0; i < nsta; i++) {
		printf("%-6s %8d\n", sta_code[i], ntim[i]);
	}
	if (verbose)
	fprintf(stderr, "Number of phase cards read in: %d\n", ntot);

	fclose(fpdat);

	return 0;
}
