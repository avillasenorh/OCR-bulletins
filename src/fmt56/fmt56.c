#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*******************************************************************************
Program: fmt56.c

Purpose: Reads a text file resulting from optical character recognition
  of ISS bulletins for the period 1917-1956
  This program identifies the different fields and values, corrects common
  errors, and reformats the text so its output can be later converted to
  ISC 96-byte format

Usage:

% fmt56 [-D] [-V] txtfile

-D: debug (not implemented; purpose not clear yet)
-V: verbose

Author:	Antonio Villasenor, USGS, Golden

Date:	15-JUL-1999
*******************************************************************************/
/* maximum filename length */
#ifndef FILENAME_MAX
#define FILENAME_MAX 1024
#endif

#define ABS(x) ( (x > 0) ? x : -x)

#define INT_UNDEF -12345
#define FLT_UNDEF -12345.0
#define MAXLINE	2048
#define MAXSTA 2000
#define TRUE	1
#define FALSE	0

int trim(char *);
int trimh(char *);
int mdash(char *);
int int2eng(char *);

int g56_ltype(char *line, int plt);

int g56_epic(char *line, int *month, int *day, int *hour, int *minute,
	    int *second, float *lat, float *lon, int *lat_acc, int *lon_acc, float *rdepth, char *data);
int w56_epic(char *card, int month, int day, int hour, int minute, int second,
	     float lat, float lon, int lat_acc, int lon_acc, float rdepth, char *data);

int g56_stnm(char *line, char *stnm, int *ncmp, int cmp[]);
int g56_dist(char *line, float *delta);
int g56_az(char *line, float *az);
int fix_errors(char *line);
int g56_phase(char *line, int index, int debug, float delta, int nph, int *lphase, int *next_phase,
               char *onset, int *min, int *sec, char *pol, char *pol1,
               int *lres, int *res, char *pres, char *phase,
               int *lparenthesis, int *lbrackets, int *lbraces);
int g56_l(char *line, int index, int debug, int *lphase, int *next_phase,
                char *onset, float *lmin, int *lparenthesis);

int w56_comment(FILE *, char *);

int main(int argc, char *argv[])
{
	FILE	*fptxt;
	FILE	*fpsta;
	FILE	*fpiss;
	char	txtfile[FILENAME_MAX];
	char	issfile[FILENAME_MAX];

	char	line[MAXLINE];
	char	card[MAXLINE];
	char	data[MAXLINE];

	char	*p;
	char	*sta[MAXSTA];
	int	nsta;
	int	len;
	int	ista, lfound = FALSE;

	int	year, page;	/* page and year read from file header */
	int	yr, pg;		/* page and year read from file name */
	int	month, day, hour, minute, second;
	float	lat, lon, depth;
	float	rdepth;		/* depth of focus (fraction of Earth radius) */
	int	lat_acc, lon_acc;
	float	a, b, c, d, e;
	float	g, h, k, ht;
	float	se;		/* standard error (seconds) (RMS?) */

	char	stnm[20];
	int	ncmp;		/* number of components */
	int	cmp[3];		/* component codes 1=Z, 2=N, 3=E*/
	int	icmp, ipos;
	float	delta, azim;
	int	min, sec, res;
	int	lpha, lnext, lres, lpar, lbk, lbc;
	char	onset, pol, pol1, pres;
	char	phnm[20];
	float	lmin;

	float	p_delta = 0.0;	/* previous value of delta */

	int	nph;	/* number of phases read by g56_phase */

	int	nf;	/* return code from some get_* functions */

	int	index;
	int	pindex;

	int	verbose = FALSE;
	int	debug = FALSE;
	int	infile = FALSE;

	int	ldepth = FALSE; /* true if depth info included in epicenter line */
	int	ltabular = FALSE;

	int	npline = 0;	/* counter for lines in each paragraph */

	int	lt = 0;
	int	plt = -1;

	int	i;

	/* Decode flag options */

	for (i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			switch (argv[i][1]) {
				case 'D':
					debug = TRUE;
					break;
				case 'V':
					verbose = TRUE;
					break;
				default:
					break;
			}
		} else {
			if ((fptxt = fopen(argv[i], "r")) == NULL) {
				fprintf(stderr,
				"fmt56: cannot open %s \n", argv[i]);
				return 1;
			} else {
				strcpy(txtfile,argv[i]);
				infile = TRUE;
			}
		}
	}

	if (! infile) {
		fprintf(stderr,"fmt56: no input file name provided\n");
		return 1;
	} else {
		if (verbose)
			fprintf(stderr,"fmt56: processing file %s\n", txtfile);
	}

	/* read year and page number from file name */
	/* can be done more general: ...yr_page.txt
	with year Y2K compliant or not! */

	sscanf(&txtfile[1], "%2d", &yr);
	sscanf(&txtfile[4], "%4d", &pg);

        yr += 1900;

	/*
	strncpy(issfile, txtfile, 8);
	strcat(issfile, ".dat");
	*/
	char dum[16];
	strncpy(dum, txtfile, 8);
	trim(dum);
	sprintf(issfile, "%-8s.dat",dum);

	/*fprintf(stderr, "%d %d\n", yr, pg);*/

	/* read station list file */
	/*if ((fpsta = fopen("historical.names", "r")) == NULL) {*/
	if ((fpsta = fopen("/Users/antonio/Projects/ISS/stations/historical.names", "r")) == NULL) {
		fprintf(stderr, "fmt56: cannot open station list file %s\n", 
		"historical.names");
		exit(1);
	}
	nsta = 0;
	while (fgets(line, MAXLINE, fpsta) != NULL) {
		len = strlen(line);
		if (nsta >= MAXSTA || (p = (char *)malloc(len*sizeof(char))) == NULL) {
			fprintf(stderr, "Error in station list line number %d\n%s\n", nsta, line);
			exit(1);
		} else {
			if (line[len-1] == '\n') line[len-1] = '\0';	/* eliminates newline character */
			strcpy(p, line);
			trim(p);
			sta[nsta++] = p;
		}
	}
	fclose(fpsta);

	/* open output file */
	if ((fpiss = fopen(issfile, "w")) == NULL) {
		fprintf(stderr, "fmt56: cannot open output file %s\n", issfile);
		exit(1);
	}

	i = 0;
	while (fgets(line, MAXLINE, fptxt) != NULL) {

		/* trim all blank characters from beginning of line */

		trimh(line);

		/* substitute m-dash for "-" (minus) sign */

                mdash(line);
                int2eng(line);

		/* ignore empty lines */

		/*printf("%d %s", strlen(line), line);*/
		if (strlen(line) == 1 && line[0] == '\n') {
			;
		} else {
			/* first line if file must have year and page info */
			if (i == 0) {
				sscanf(line, "%d %d",&year, &page);
				if (year != yr || page != pg) {
					fprintf(stderr,
					"Inconsistent file name and header:\n");
					fprintf(stderr, "%s\n%s",
					txtfile, line);
					return 1;
				}
				lt = 0;
			} else {
				plt = lt;
				lt = g56_ltype(line, plt);
				if (lt == 3 || lt == 4 || lt == 6 || lt == 7) {
					ltabular = TRUE;
				} else if (lt == 1 || lt == 9 || lt == 10 || lt == 13) {
					ltabular = FALSE;
				}
			}

			/* Use switch and case to process each type */
			switch (lt) {
			case -1:
				fprintf(stderr, "unknown line type: %s\n", line);
				return 1;
				break;
			case 0:	/* first line: year and page number */
				fprintf(fpiss, "%4d", year);
				for (i = 0; i < 41; i++)
					fprintf(fpiss, " ");
				fprintf(fpiss, "%d\n\n", page);
				npline = 0;
				break;
			case 1: /* hypocenter line */
				for (i = 0; i < MAXLINE; i++)
					data[i] = '\0';
				nf = g56_epic(line, &month, &day, &hour, &minute,
				&second, &lat, &lon, &lat_acc, &lon_acc, &rdepth, data);
				if (nf != 0) return 1;
				nf = w56_epic(card, month, day, hour, minute, second,
				lat, lon, lat_acc, lon_acc, rdepth, data);
				fprintf(fpiss, "\n\n%s", card);
				npline = 0;
				p_delta = 0.0;
				break;
			case 11: /* hypocenter line but not in tabular form */
				trim(line);
				fprintf(fpiss, "\n\n");
				w56_comment(fpiss, line);
				break;
			case 2: /* Depth of focus (or height of focus) */
				break;
			case 3: /* first set of hypocenter variables: A, B, C, delta, h */
				break;
			case 4: /* second set of hypocenter variables: D, E, G, H, K */
				break;
			case 5: /* root mean square error (seconds) */
				break;
			case 6: /* first table header */
				fprintf(fpiss, "\n\n");
				fprintf(fpiss, "                    ");
				fprintf(fpiss,
"   D   Az.       P.       O-C.       S.       O-C.         Supp.             L. \n");
				ltabular = TRUE;
				break;
			case 7: /* second table header */
				fprintf(fpiss, "                    ");
				fprintf(fpiss,
"   o    o      m.  s.      s.      m.  s.      s.      m.  s.                m. \n");
				ltabular = TRUE;
				break;
			case 8:
				if (! ltabular) { /* line not part of table */
					trim(line);
					w56_comment(fpiss, line);
				} else { /* station phase data in tabular form */
				/* first get station name and component name(s) */
				pindex = 0;
				index = g56_stnm(line, stnm, &ncmp, cmp);
				if (index <= 0) {
					break;
					/*
					fprintf(stderr, "Error in station name.  Exiting.\n");
					return 1;
					*/
				} else if (index > 20) {
					fprintf(stderr, "Station name too long: %s %d\n", stnm, index);
					break;
					/*
					fprintf(stderr, "Station name too long: %s\nExiting.\n", stnm);
					return 1;
					*/
				}
				lfound = FALSE;
				for (ista = 0; ista < nsta; ista++) {
					if (strcmp(sta[ista], stnm) == 0) {
						lfound = TRUE;
						break;
					}
				}
				if (! lfound) {
					if (strlen(stnm) == 0 && ncmp > 0) {
						;
					} else fprintf(stderr, "WARNING: station name not in station list: %s\n", stnm);
				}
				/* get epicentral distance */
				pindex += index;
				fix_errors(&line[pindex]);
				index = g56_dist(&line[pindex], &delta);
				if (index < 0 || delta == FLT_UNDEF) {
					fprintf(stderr,
					"Error reading distance for line:\n%s", line);
					return 1;
				}
				if (delta < p_delta) {
					fprintf(stderr,
					"ERROR: distance smaller than previous phase:\n%s", line);
					return 1;
				}
				p_delta = delta;
				/* get azimuth */
				pindex += index;
				index = g56_az(&line[pindex], &azim);
				if (index < 0 || delta == FLT_UNDEF) {
					fprintf(stderr,
					"Error reading azimuth for line:\n%s", line);
					return 1;
				}
				/*fprintf(stderr, "%-20s%5.1f  %3.0f\n", stnm, delta, azim);*/
				/*fprintf(fpiss, "%-20s%5.1f  %3.0f", stnm, delta, azim);*/
				sprintf(card, "%-20s%5.1f  %3.0f", stnm, delta, azim);
				ipos = 18;
				for (icmp = 0; icmp < ncmp; icmp++) {
					card[ipos] = '.';
					if (cmp[icmp] == 1)
						card[ipos-1] = 'Z';
					else if (cmp[icmp] == 2)
						card[ipos-1] = 'N';
					else if (cmp[icmp] == 3)
						card[ipos-1] = 'E';
					else if (cmp[icmp] == 13)
						card[ipos-1] = 'W';
					ipos -= 2;
				}

				/* new paragraph */
				if (npline >= 5) {
					if (strlen(stnm) != 0) {
						fprintf(fpiss, "\n");
						npline = 0;
					}
				}
				npline++;

				fprintf(fpiss, "%s", card);
				/* get phases */

				nph = 1;
				pindex += index;

				while(nph <= 3) {

				index = g56_phase(line, pindex, debug, delta, nph, &lpha, &lnext,
				&onset, &min, &sec, &pol, &pol1, &lres, &res, &pres, phnm, &lpar, &lbk, &lbc);

				if (index < 0) {
					fprintf(stderr, "Error, index = %d\n%s", index, line);
					exit(1);
				}

				if ((lbk || lbc) && pres != ' ') {
					fprintf(stderr, "brackets/braces and pres!!!!!\n");
					exit(1);
				}

				if (lpha) {
					if (lpar)
						fprintf(fpiss, "  (");
					else
						fprintf(fpiss, "   ");
					if (onset == 'e' || onset == 'i' || onset == ' ')
						fprintf(fpiss, "%c", onset);
					else {
						fprintf(stderr, "Invalid onset: %c\n", onset);
						fprintf(stderr, "%s", line);
						fprintf(fpiss, " ");
					}
					if (min != INT_UNDEF)
						fprintf(fpiss, " %2d", min);
					if (sec != INT_UNDEF)
						fprintf(fpiss, " %2d", sec);
					if (lpar) {
						if (pol1 != ' ') {
							fprintf(stderr, "WARNING: ");
							fprintf(stderr, "? and ) for this line:\n%s", line);
						}
						fprintf(fpiss, "%c)", pol);
					} else {
						fprintf(fpiss, "%c%c", pol, pol1);
					}
					if (lres) {
						if (lbk)
							fprintf(fpiss, "   [");
						else if (lbc)
							fprintf(fpiss, "   {");
						else
							fprintf(fpiss, "    ");
						if (res > 0)
							fprintf(fpiss, "+");
						else if (res < 0)
							fprintf(fpiss, "-");
						else
							fprintf(fpiss, " ");
						if (lbk)
							fprintf(fpiss, "%2d]", ABS(res));
						else if (lbc)
							fprintf(fpiss, "%2d}", ABS(res));
						else
							fprintf(fpiss, "%2d%c", ABS(res), pres);
					} else { /* temporary fix */
						if (strlen(phnm) == 1) {
							fprintf(fpiss, "     %1s  ", phnm);
						} else if (strlen(phnm) == 2) {
							fprintf(fpiss, "    %2s  ", phnm);
						} else if (strlen(phnm) == 3) {
							fprintf(fpiss, "    %3s ", phnm);
						} else if (strlen(phnm) == 4) {
							fprintf(fpiss, "   %4s ", phnm);
						} else if (strlen(phnm) == 5) {
							fprintf(fpiss, "  %5s ", phnm);
						} else if (strlen(phnm) == 6) {
							fprintf(fpiss, " %6s ", phnm);
						} else {
							fprintf(fpiss, "        ");
						}
					}
				} else {
					fprintf(fpiss, "      --         -- ");
				}

				pindex = index;
				nph++;
				}		/* end of while loop over phases */
				if (lnext) {
					pindex = index;
					index = g56_l(line, pindex, debug, &lpha, &lnext,
					&onset, &lmin, &lpar);
					if (lnext) {
						fprintf(stderr,
						"Error: end of line expected:\n%s", line);
						exit(1);
					}
					if (lpha) {
						if (lpar) 
							fprintf(fpiss, "  (");
						else
							fprintf(fpiss, "   ");
						if (onset == 'e' || onset == 'i' || onset == ' ')
							fprintf(fpiss, "%c", onset);
						else {
							fprintf(stderr, "Invalid onset: %c\n", onset);
							fprintf(stderr, "%s", line);
							fprintf(fpiss, " ");
						}
						if (lmin != FLT_UNDEF)
							fprintf(fpiss, "%5.1f", lmin);
						if (lpar) 
							fprintf(fpiss, ")\n");
						else
							fprintf(fpiss, " \n");
					} else {
						fprintf(fpiss, "       -- \n");
					}
				} else {
					fprintf(fpiss, "       -- \n");
				}
				/*
				npline++;
				if (npline == 5) {
					fprintf(fpiss, "\n");
					npline = 0;
				}
				*/
				} /* this is the end of station in tabular form */
				break;
			case 9: /* Additional readings label */
				if (sindex(line, "and notes") != -1) {
					fprintf(fpiss, "\nAdditional readings and notes:--\n");
				} else if (sindex(line, "and note:") != -1) {
					fprintf(fpiss, "\nAdditional readings and note:--\n");
				} else {
					fprintf(fpiss, "\nAdditional readings:--\n");
					if (strlen(line) > 24) {
						fprintf(stderr, "fmt56 error: ");
						fprintf(stderr, "extra characters after additional readings.\n");
						exit(1);
					}
				}
				break;
			case 10: /* Continued on next page */
				fprintf(fpiss, "\n");
				fprintf(fpiss, "                                 ");
				fprintf(fpiss, "Continued on next page.\n");
				break;
			case 12: /* For Notes see next page */
				fprintf(fpiss, "\n");
				fprintf(fpiss, "                                 ");
				fprintf(fpiss, "For Notes see next page.\n");
				ltabular = FALSE;
				break;
			case 13: /* Long waves ... */
				trim(line);
				fprintf(fpiss, "\n\n");
				w56_comment(fpiss, line);
				break;
			default:
				break;
			}
			i++;
		}
	}

	fclose(fptxt);
	fclose(fpiss);

	return 0;
}
