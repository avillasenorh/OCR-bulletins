#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*******************************************************************************
Program: fmt57.c

Purpose: Reads text files resulting from optical character recognition
  of ISS bulletins for the period 1957-1963 (output format of card-controlled
  typewriter.
  This program identifies the different fields and values, corrects common
  errors, and reformats the text so it can be converted to ISC 96-byte format

Author:	Antonio Villasenor, USGS, Golden

Date:	23-MAR-1999
*******************************************************************************/
/* maximum filename length */
#ifndef FILENAME_MAX
#define FILENAME_MAX 1024
#endif

#define FLT_UNDEF -12345.0
#define MAXLINE	128
#define MAXSTA 2000
#define TRUE	1
#define FALSE	0

int trim(char *);
int trimh(char *);
int cmdash(char *);
int yrpg(char *line, int *year, int *page);
int linetype(char *line, int plt);
int get_hypo(char *line, int *month, int *day, int *hour, int *minute,
             int *second, float *lat, float *lon, float *depth);
float get_depth(char *line);
int get_abc(char *line, float *a, float *b, float *c, float *d, float *e);
float get_se(char *line);

int get_stnm(char *line, char *stnm);
int get_distaz(char *line, float *delta, float *azim);
int get_phases(char *line, int index, int debug, int supp_all,
		int *Pmin, int *Psec, int *Pres, int *Ppol,
		int *Smin, int *Ssec, int *Sres, int *pPmin, int *pPsec,
		int *Suppmin, int *Suppsec, char Supp[]);
int w_hypo(char *card, int month, int day, int hour, int minute, int second,
		float lat, float lon, float depth);
int w_phases(char card[], char stnm[], float delta, float azim,
		int Pmin, int Psec, int Pres, int Ppol,
		int Smin, int Ssec, int Sres, 
		int pPmin, int pPsec,
		int Suppmin, int Suppsec, char Supp[]);

int main(int argc, char *argv[])
{
	FILE	*fptxt;
	FILE	*fpiss;
	FILE    *fpsta;
	char	txtfile[FILENAME_MAX];
	char	issfile[FILENAME_MAX];

	char	line[MAXLINE];
	char	card[MAXLINE];

	int	year, page;	/* page and year read from file header */
	int	yr, pg;		/* page and year read from file name */
	int	month, day, hour, minute, second;
	float	lat, lon, depth;
	float	rdepth;		/* depth of focus (fraction of Earth radius) */
	float	a, b, c, d, e;
	float	g, h, k, ht;
	float	se;		/* standard error (seconds) (RMS?) */

	char	stnm[32];
	float	delta, azim;
	int	Pmin, Psec, Pres, Ppol;
	int	Smin, Ssec, Sres;
	int	pPmin, pPsec;
	int	Suppmin, Suppsec;
	char	Supp[16];

	float	pdelta;

	char	*sta[MAXSTA];
	char	*p;
	int	nsta, len;
	int     ista, lfound = FALSE;

	int	nph;	/* number of phases read by get_phases (nph < 0 if error) */

	int	nf;	/* return code from some get_* functions */

	int	index;
	int	pindex;

	int	verbose = FALSE;
	int	debug = FALSE;
	int	supp_all = FALSE;
	int	infile = FALSE;

	int	npline = 0;	/* counter for lines in each paragraph */
	int	pl1;		/* number of lines in paragraph 1 */

	int	lt = 0;

	int	i, j;

	/* Decode flag options */

	for (i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			switch (argv[i][1]) {
				case 'P':
					sscanf(&argv[i][2], "%d", &pl1);
					if (pl1 > 5 || pl1 < 0) {
						fprintf(stderr, "Error in argument -P\n");
						return 1;
					}
					npline = 5 - pl1;
					break;
				case 'D':
					debug = TRUE;
					break;
				case 'S':
					supp_all = TRUE;
					break;
				case 'V':
					verbose = TRUE;
					break;
				default:
					break;
			}
		} else {
			/* This will have to be changed in order to
			allow multiple input text files in the command line
			(e.g. % fmt57 *.txt) */

			/* check existence of all arguments that are not
			options.  If file exists, copy to *char array.
			If some file does not exist, error, and
			read remaining files */

			if ((fptxt = fopen(argv[i], "r")) == NULL) {
				fprintf(stderr,
				"fmt57: cannot open %s \n", argv[i]);
				return(1);
			} else {
				strcpy(txtfile,argv[i]);
				infile = TRUE;
			}
		}
	}

	if (! infile) {
		fprintf(stderr,"fmt57: no input file name provided\n");
		return(1);
	} else {
		if (verbose)
			fprintf(stderr,"fmt57: processing file %s\n", txtfile);
	}

	/* read station list file */
	if ((fpsta = fopen("/Users/antonio/Projects/ISS/stations/1960.names", "r")) == NULL) {
		fprintf(stderr, "fmt56: cannot open station list file%s\n",
		"/Users/antonio/Projects/ISS/stations/1960.names");
		exit(1);
	}
	nsta = 0;
	while (fgets(line, MAXLINE, fpsta) != NULL) {
		len = strlen(line);
		if (nsta >= MAXSTA || (p = (char *)malloc(len*sizeof(char))) == NULL) {
			fprintf(stderr, "Error in station list line number %d\n%s\n", nsta, line);
			exit(1);
		} else {
			if (line[len-1] == '\n') line[len-1] = '\0';    /* eliminates newline character */
			strcpy(p, line);
			trim(p);
			sta[nsta++] = p;
		}
	}
	fclose(fpsta);

	/*for (i = 0; i < nsta; i++) fprintf(stderr, "%5d **%s**\n", i, sta[i]);*/

	/* read year and page number from file name */

	sscanf(&txtfile[1], "%2d", &yr);
	sscanf(&txtfile[4], "%4d", &pg);
        yr += 1900;

	strncpy(issfile, txtfile, 8);
	issfile[8] = '\0';
	strcat(issfile, ".dat");

	/*fprintf(stderr, "%d %d\n", yr, pg);*/

	if ((fpiss = fopen(issfile, "w")) == NULL) {
		fprintf(stderr, "fmt57: cannot open output.iss\n");
		return(1);
	}

	i = 0;
	while (fgets(line, MAXLINE, fptxt) != NULL) {

		/* trim all blank characters from beginning of line */

		trimh(line);

		/* substitute m-dash for "-" (minus) sign */

                cmdash(line);

		if (strlen(line) == 1 && line[0] == '\n') {
			;
		} else {
			/* first line if file must have year and page info */
			if (i == 0) {
				yrpg(line, &year, &page);
				if (year != yr || page != pg) {
					fprintf(stderr,
					"Inconsistent file name and header:\n");
					fprintf(stderr, "%s\n%s",
					txtfile, line);
					return(1);
				}
				/*fprintf(stderr, "%d %d\n", year, page);*/
			}
			i++;
			/*printf("%d %s", strlen(line), line);*/

			/* call to function to determine type of line:
			* Possible code values:
			-1 = Unknown (possibly an error)
			0 = Header line (year and page number)
			1 = Origin, epicentre and depth
			2 = Depth of focus (only for "abnormal" events)
			3 = First constant line (A=, B=, C=, ...)
			4 = Second constant line (G=, H=, K=, HT=)
			5 = Standard error line
			6 = First table header (DELTA AZ. P 0-C ...)
			7 = Second table header (DEG. DEG. M S S ...)
			8 = Station phase list
			*/

			lt = linetype(line, lt);

			/*fprintf(stderr, "%4d %s", lt, line);*/

			/* Use switch and case to process each type */
			switch (lt) {
			case -1:
				fprintf(stderr, "unknown line type: %s\n", line);
				return 1;
				break;
			case 0:
				fprintf(fpiss, "%4d", year);
				for (i = 0; i < 67; i++)
					fprintf(fpiss, " ");
				fprintf(fpiss, "PAGE %d\n\n", page);
				pdelta = 0.0;
				break;
			case 1:
				nf = get_hypo(line, &month, &day, &hour, &minute, &second,
					&lat, &lon, &depth);
				if (nf != 0) return 1;
				w_hypo(card, month, day, hour, minute, second,
				lat, lon, depth);
				fprintf(fpiss, "\n%s\n\n", card);
				npline = 0;
				pdelta = 0;
				break;
			case 2:
				rdepth = get_depth(line);
				if (rdepth != FLT_UNDEF) {
					fprintf(fpiss, "       DEPTH OF FOCUS=%6.3fR\n\n", rdepth);
				} else {
					;
				}
				break;
			case 3:
				if (get_abc(line, &a, &b, &c, &d, &e) == 0) {
					fprintf(fpiss,
					"            A=%8.5f B=%8.5f C=%8.5f    D=%7.4f E=%7.4f\n",
					a, b, c, d, e);
				} else {
					;
				}
				break;
			case 4:
				if (get_ghk(line, &g, &h, &k, &ht) == 0) {
					fprintf(fpiss,
					"            G=%7.4f H=%7.4f K=%7.4f   HT=%5.1f\n\n",
					g, h, k, ht);
				} else {
					;
				}

				break;
			case 5:
				se = get_se(line);
				fprintf(fpiss, "      SE=%6.2f\n\n", se);
				break;
			case 6:
				fprintf(fpiss, "                  ");
				fprintf(fpiss,
				"DELTA    AZ.       P   O-C      S  O-C    *PP       SUPP.\n");
				break;
			case 7:
				fprintf(fpiss, "                   ");
				fprintf(fpiss,
				"DEG.   DEG.     M  S   S     M  S  S     M  S    M  S\n\n");
				break;
			case 8:
				pindex = 0;
				index = get_stnm(line, stnm);
				if (index <= 0) {
					fprintf(stderr, "Error in station name.  Exiting.\n");
					return 1;
				} else if (index > 13) {
					fprintf(stderr, "Station name too long: %s\nExiting.\n", stnm);
					return 1;
				}

				lfound = FALSE;
				for (ista = 0; ista < nsta; ista++) {
					if (strcmp(sta[ista], stnm) == 0) {
						lfound = TRUE;
						break;
					}
				}
				if (! lfound) {
					fprintf(stderr,
					"WARNING: station name not in station list: **%s**\n", stnm);
					/*exit(1);*/
				}

				pindex += index;

				/* fix errors in rest of line */

				for (j = pindex; j < strlen(line); j++) {
					if (line[j] == 'O' || line[j] == 'o') line[j] = '0';
					if (line[j] == 'I' || line[j] == 'l') line[j] = '1';
				}

				index = get_distaz(&line[pindex], &delta, &azim);
				if (index < 0 || delta == FLT_UNDEF || azim == FLT_UNDEF) {
					fprintf(stderr,
					"Error reading distance and/or azimuth for line:\n%s\n",
					line);
					return 1;
				}

				if (delta < pdelta) {
					fprintf(stderr,
					"WARNING: distance smaller than in previous phase: %6.2f %6.2f\n%s\n",
					pdelta, delta, line);
				}
				pdelta = delta;
				pindex += index;
				nph = get_phases(line, pindex, debug, supp_all,
				&Pmin, &Psec, &Pres, &Ppol,
				&Smin, &Ssec, &Sres, &pPmin,
				&pPsec, &Suppmin, &Suppsec, Supp);
				if (nph < 0) {
					fprintf(stderr, "Error reading phases for:\n%s\n", line);
					return 1;
				} else {
					if (nph == 0) fprintf(stderr, "No phase data: %s\n", line);
					if (! debug) w_phases(card, stnm, delta, azim,
						Pmin, Psec, Pres, Ppol,
						Smin, Ssec, Sres, pPmin, pPsec,
						Suppmin, Suppsec, Supp);
						fprintf(fpiss, "%s\n", card);
					npline++;
					if (npline == 5) {
						if (! debug) fprintf(fpiss, "\n");
						npline = 0;
					}
				}
				break;
			default:
				break;
			}
		}
	}

	fclose(fptxt);

	return 0;
}
