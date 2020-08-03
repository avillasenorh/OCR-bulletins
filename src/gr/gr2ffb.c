#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***********************************************************************
* C template for programs that read from stdin and write to stdout.
* Optionally, if an argument (not starting with the "-" character)
*  is passed, it is assumed to be the input file.
* Optionally, if the -o option is used, the next argument is
*  used as the output file.
*
* Options:
*
* -o outfile     : writes output to outfile instead of stdout
*
*
* Usage: % progname [ infile ] [-o outfile ] [ ... ] [ -V ]
***********************************************************************/

#ifndef FILENAME_MAX
#define FILENAME_MAX 1024	/* maximum length of a filename */
#endif

#define MAXLINE	512
#define MAXCARDS 1000
#define CARDLEN 96
#define TRUE	1
#define FALSE	0

int substr(char *s, char *t, int n, int m);
int trim(char s[]);

int phase_code(char *phase);

int w_isc1(char *s, int nrc, int year, int month, int day, int hour, int minute,
  float second, int sec_acc, float lat, int lat_acc, float lon, int lon_acc,
  float depth, int depth_acc, int igreg, int isreg, int nobs);
int w_isc5(char *s, int nrc, int year, int month, char *stnm, int stno, char net, char src,
  char fmt, char ltflag, float az, float dist, int nph, int day, int hour, int min, float sec,
  int sec_acc, int op_id, char *phase, float op_res, int isc_id, float isc_res, char pol,
  char inst, char cmp, char onset);
int w_isc6(char *s, int nrc, int year, int month, int phno, int day, int hour, int min, float sec,
  int sec_acc, int op_id, char *phase, float op_res, int isc_id, float isc_res, char pol, 
  char inst, char cmp, char onset);
int w_isc7(char *s, int nrc, int year, int month, char *station);

struct itm {
        int     year;
        int     month;
        int     day;
        int     hour;
        int     minute;
        int     second;
        int     millisecond;
};
struct itm iaddsec1(struct itm RefTime, int dt);

int main(int argc, char *argv[])
{
	FILE	*fp,*fpout;
	char	infile[FILENAME_MAX];
	char	outfile[FILENAME_MAX];

	char	line[MAXLINE],data[MAXLINE];

	char	card1[CARDLEN];
	char    **card;
	int	ncard = 0;

	int	year, month, day, hour, minute, second;
	char	smonth[12], epic[12], slat[12], slon[12];
	char	aux[16], kres[16];
	char	*daux;
	float	lat, lon;
	int	depth;
	int	nf;
	int	igreg=0,
		isreg=0,
		nobs=0;

	char	station[21], pstation[21] = "none";
	float	dist, az;
	int	id, isc_id;
	char	phase[12];
	int	nph;
	int	p_hour, p_min, p_sec, p_res;
	int	pp_hour, pp_min, pp_sec, pp_res;
	int	s_hour, s_min, s_sec, s_res;
	int	sec_acc = 0;
	float	fres;
	int	i;

	int	pcol = 32,
		scol = 43;

	static char *month_name[13] = {
		"illegal month",
		"January", "February", "March", "April", "May", "June",
		"July", "August", "September", "October", "November", "December"
	};

	struct itm	OriginTime, PTime, STime;
	float	dt;

	int	lfound = FALSE;
	int	lpphase = FALSE;

	int	lin=FALSE;
	int	lout=FALSE;

	/* decode flag options */

	for (i=1; i < argc; i++) {
		if (argv[i][0] == '-') {
			switch (argv[i][1]) {
				case 'o': /* ouptput file */
					if (argv[i][2] != '\0') strcpy(outfile,&argv[i][2]);
					else strcpy(outfile,argv[++i]);
					lout=TRUE;
					break;
				default:
					break;
			}
		} else {
			if ((fp=fopen(argv[i],"r")) == NULL) {
				fprintf(stderr,"ERROR: cannot open input file: %s\n",argv[i]);
				exit(1);
			} else {
				strcpy(infile,argv[i]);
				lin=TRUE;
			}
		}
	}

	/* if no input file passed, read from stdin */

	if (! lin) fp=stdin;

	/* if no output file passed, write to stdout */

	if (! lout) fpout=stdout;
	else {
		if ((fpout=fopen(outfile, "w")) == NULL) {
			fprintf(stderr,"ERROR: cannot open output file: %s\n", outfile);
			exit(1);
		}
	}

	/* allocate space for phase card array */

	card=(char **)malloc(MAXCARDS*sizeof(char *));
	card[0]=(char *)malloc(MAXCARDS*CARDLEN*sizeof(char));
	for (i = 1; i < MAXCARDS; i++) card[i]=card[i-1]+CARDLEN;

	/* read hypocenter line */

	while(fgets(line,MAXLINE,fp) != NULL) {

		if (strstr(line, "Epicentre") != NULL) {
			nf = sscanf(line,"%d %s %dd. %dh. %dm. %ds. %s %s %s",
			&year, smonth, &day, &hour, &minute, &second, epic, slat, slon);

			if (nf != 9) {
				fprintf(stderr, "ERROR: invalid hypocenter line:\n%s", line);
				fprintf(stderr, "%d\n", nf);
				exit(1);
			}
			month = 0;
			for (i = 1; i < 13; i++) {
				if (strcmp(smonth, month_name[i]) == 0) {
					month = i;
					break;
				}
			}
			if (month == 0) {
				fprintf(stderr, "ERROR: invalid month:\n%s",line);
				exit(1);
			}

			OriginTime.year = year;
			OriginTime.month = month;
			OriginTime.day = day;
			OriginTime.hour = hour;
			OriginTime.minute = minute;
			OriginTime.second = second;
			OriginTime.millisecond = 0;

			if (strstr(slat, "N.") != NULL) {
				sscanf(slat, "%fN.", &lat);
			} else if (strstr(slat, "S.") != NULL) {
				sscanf(slat, "%fS.", &lat);
				lat *= -1.0;
			} else {
				fprintf(stderr, "ERROR: invalid latitude: %s\n", slat);
				exit(1);
			}
			if (strstr(slon, "E.") != NULL) {
				sscanf(slon, "%fE.", &lon);
			} else if (strstr(slon, "W.") != NULL) {
				sscanf(slon, "%fW.", &lon);
				lon *= -1.0;
			} else {
				fprintf(stderr, "ERROR: invalid longitude: %s\n", slon);
				exit(1);
			}

			daux=strstr(line, "Depth of focus");
			if (daux != NULL) {
				sscanf(daux, "%*s %*s %*s %dkm.", &depth);
			} else {
				depth=0;
			}
			/*
			printf("%s",line);
			printf("%d %d %d %d %d %d %f %f\n", year, month, day, hour, minute, second, lat, lon);
			*/
			w_isc1(card1, 0, year, month, day, hour, minute, (float)second, 0, lat, -1, lon, -1, (float)depth, 0, igreg, isreg, nobs);
			fprintf(fpout,"%s\n",card1);

			lfound = TRUE;
			break;
		}

	}

	if (! lfound) {
		fprintf(stderr, "ERROR: Epicentre line not found\n");
		exit(1);
	}

	/* read phase cards */
	i=0;
	ncard=0;
	nph=0;
	while (fgets(line,MAXLINE,fp) != NULL) {
		lpphase=FALSE;
		if (isupper(line[0]) && strstr(line, "Station") == NULL) {
			substr(station, line, 1, 18);
			trim(station);
			if (station[0] == '\0') {
				strcpy(station,pstation);
			} else {
				if (nph > 0) {
					w_isc7(card[ncard], 0, year, month, pstation);
					fprintf(fpout,"%s\n", card[ncard]);
					ncard++;
				}
				nph=0;
			}

			if (line[19] != '-') {
				substr(aux,line,20,6);
				sscanf(aux, "%f", &dist);
			} else dist=999.;

			if (line[26] != '-') {
				substr(aux,line,27,5);
				sscanf(aux, "%f", &az);
			} else az=999.;

			if (isdigit(line[pcol])) {
				lpphase=TRUE;
				nph++;
				PTime.year = year;
				PTime.month = month;
				PTime.day = day;
				substr(aux, line, pcol+1, 10);
				nf = sscanf(aux, "%d %d %d", &p_hour, &p_min, &p_sec);
				if (nf == 3) {
					PTime.hour = p_hour;
					PTime.minute = p_min;
					PTime.second = p_sec;
					PTime.millisecond = 0;
				} else if (nf == 2) {
					fprintf(stderr, "WARNING: assumed mm:ss travel time for P phase: **%s**\n%s", aux, line);
					dt = (float)(p_hour*60 + p_min);
					PTime=iaddsec1(OriginTime, dt);
				} else {
				}

				id=phase_code("P");
				isc_id=id;

				if (nph == 1)
					w_isc5(card[ncard], 0, year, month, "????", 0, ' ', ' ', ' ', ' ', az, dist,
					0, PTime.day, PTime.hour, PTime.minute, (float)PTime.second, sec_acc,
					id, "P", 999.9, isc_id, 999.9, ' ', ' ', ' ', ' ');
				else
					w_isc6(card[ncard], 0, year, month, ncard+1, PTime.day, PTime.hour, PTime.minute, (float)PTime.second,
					sec_acc, id, "P", 999.9, isc_id, 999.9, ' ', ' ', ' ', ' ');
				fprintf(fpout,"%s\n", card[ncard]);
				ncard++;
			}
			if (isdigit(line[scol]) && lpphase) {
				nph++;
				substr(aux, line, scol+1, 8);
				nf = sscanf(aux, "%d %d", &s_min, &s_sec);
				if (nf != 2) fprintf(stderr, "ERROR: invalid format for S phase: **%s**\n%s", aux, line);

				id=phase_code("S");
				isc_id=id;

				dt = (float)(s_min*60 + s_sec);
				STime=iaddsec1(PTime, dt);

				if (nph == 1)
					w_isc5(card[ncard], 0, year, month, "????", 0, ' ', ' ', ' ', ' ', az, dist,
					0, STime.day, STime.hour, STime.minute, (float)STime.second, sec_acc, id, "S", 999.9,
					isc_id, 999.9, ' ', ' ', ' ', ' ');
				else
					w_isc6(card[ncard], 0, year, month, ncard+1, STime.day, STime.hour, STime.minute, (float)STime.second,
					sec_acc, id, "S", 999.9, isc_id, 999.9, ' ', ' ', ' ', ' ');
				fprintf(fpout, "%s\n", card[ncard]);
				ncard++;
			}

			strcpy(pstation, station);

		} else {
			/*fprintf(stderr,"***%s",line);*/
			;
		}
		i++;
	}

	/* write last station name */

	if (nph > 0) {
		w_isc7(card[ncard], 0, year, month, pstation);
		fprintf(fpout, "%s\n", card[ncard]);
		ncard++;
	}

	if (lin) fclose(fp);
	if (lout) fclose(fpout);

	return 0;
}
