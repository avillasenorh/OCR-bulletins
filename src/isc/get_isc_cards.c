#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NINT(x) (int)floor((x)+0.5)

#define INT_UNDEF -12345
#define FLT_UNDEF -12345.0

#define TRUE 1
#define FALSE 0

int r_iss_phase(char *s, int n, char *phase, int *min, int *sec, char *onset, char *pol,
   int *res, char *isc_phase);
int phase_code(char *);
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

int get_isc_cards(char *s, char *card[], int longwaves,
    char *prev_sta, int nsta, char *sta_code[], char *sta_name[],
    int year, int month, int day, int hour, int minute, int second)
{
	int	i;
	int	ncard = 0;
	int	lphase = FALSE;
	char	stnm[20];
	float	dist, az;
	int	pmin, psec, res, dt;
	int	id, isc_id;
	char	phase[20], isc_phase[20];
	char	onset, pol, cmp;
	float	fres;
	int	maxph = 3;

	char	csta[5];
	char	aux[4];
	int	lfound = FALSE;

	struct itm	OriginTime;
	struct itm	PhaseTime;

	int     sec_acc =   0;  /* precision of time: to the nearest second = 0 */

	OriginTime.year = year;
	OriginTime.month = month;
	OriginTime.day = day;
	OriginTime.hour = hour;
	OriginTime.minute = minute;
	OriginTime.second = second;
	OriginTime.millisecond = 0;

	onset = cmp = pol = ' ';

	strncpy(stnm, s, 19);
	stnm[19] = '\0';

	i = 18;
	/* find component name (if any); if more than one ignore all except first */
	while (stnm[i] == '.') {
		if (stnm[i-1] == 'Z' || stnm[i-1] == 'N' || stnm[i-1] == 'E' ||
                     stnm[i-1] == 'S' || stnm[i-1] == 'W') {
			cmp = stnm[i-1];
		} else {
			fprintf(stderr, "get_isc_phases warning: ");
			fprintf(stderr, "dubious component name ignored: %c\n%s\n", stnm[i-1], s);
		}
		i -= 2;
	}
	while (isspace(stnm[i]))
		i--;
	stnm[i+1] = '\0';

	/* find station code */

	lfound = FALSE;

	/* if station name is blank, then use previous station name */
	if (strlen(stnm) == 0 && cmp != ' ') {
		/*
		fprintf(stderr, "WARNING: station name blank: %s\n", s);
		fprintf(stderr, "WARNING: using previous station name: %s\n", prev_sta);
		*/
		strcpy(stnm, prev_sta);
	}
	for (i = 0; i < nsta; i++) {
		if (strcmp(stnm, sta_name[i]) == 0) {
			strcpy(csta, sta_code[i]);
			lfound = TRUE;
			break;
		}
	}
	if (! lfound) {
		fprintf(stderr, "get_isc_cards warning: ");
		fprintf(stderr, "station code not found for: %s\n", stnm);
		/*
		return 0;
		*/
		strcpy(csta,"????");
	}
	strcpy(prev_sta, stnm);

	if (sscanf(&s[20], "%f%f", &dist, &az) != 2) {
		fprintf(stderr, "get_isc_phases error: ");
		fprintf(stderr, "cannot read distance and/or azimuth:\n%s\n", s);
		return -1;
	}
	/* get component(s) from station name string stnm */


	/* loop over phases */
	if (longwaves)
		maxph = 4;
	else
		maxph = 3;
	ncard = 0;
	for (i = 0; i < maxph; i++) {
		lphase = FALSE;
		/* function to extract phase information */
		/*
		- input arguments: i, index[i]
		- ouptup arguments: lphase (phase present or not!!)
		*/
		lphase = r_iss_phase(s, i, phase, &pmin, &psec, &onset, &pol, &res, isc_phase);
		if (pol == ' ')
			;
		else if (pol == 'a')
			pol = 'C';
		else if (pol == 'k')
			pol = 'D';
		else if (pol == '?')
			pol = ' ';
		else {
			fprintf(stderr, "get_isc_phases warning: ");
			fprintf(stderr, "invalid polarity: %c\n%s\n", pol, s);
			pol = ' ';
		}

		if (onset != ' ' && onset != 'i' && onset != 'e') {
			fprintf(stderr, "get_isc_phases warning: ");
			fprintf(stderr, "invalid onset sharpness: %c\n%s\n", onset, s);
			onset = ' ';
		}
			
		if (res == INT_UNDEF)
			fres = 999.9;
		else
			fres = (float)res;
		/* if lphase write ISC card */
		if (lphase) {

			dt = pmin*60 + psec;
			PhaseTime = iaddsec1(OriginTime, dt);
			if (PhaseTime.year != year) {
				fprintf(stderr, "Error in year\n");
				exit(1);
			}
			if (PhaseTime.month != month) {
				fprintf(stderr, "Error in year\n");
				exit(1);
			}
			day = PhaseTime.day;
			hour = PhaseTime.hour;
			minute = PhaseTime.minute;
			second = PhaseTime.second;
			if (PhaseTime.millisecond != 0) {
				fprintf(stderr, "Error in millisecond\n");
				exit(1);
			}
			if (strcmp(phase, "?") == 0) strcpy(phase, " ");
			if (strcmp(isc_phase, "?") == 0) strcpy(isc_phase, " ");
			id = phase_code(phase);
			isc_id = phase_code(isc_phase);
			/* if i == 0 write format 5 */
			if (ncard == 0) {
				w_isc5(card[ncard], 0, year, month, csta, 0, ' ', ' ', ' ', ' ', az, dist,
				0, day, hour, minute, (float)second, sec_acc, id, phase, 999.9, isc_id, fres,
				pol, ' ', cmp, onset);
			/* else write format 6 */
			} else {
				w_isc6(card[ncard], 0, year, month, ncard+1, day, hour, minute, (float)second,
				sec_acc, id, phase, 999.9, isc_id, fres, pol, ' ', cmp, onset);
			}
			/* increment ncard */
			ncard++;
		}
	}

	/* fix number of phases in card 5 */
	if (ncard > 0) {
		sprintf(aux, "%3d", ncard);
		for (i = 0; i < 3; i++)
			card[0][30+i] = aux[i];
	}

	w_isc7(card[ncard], 0, year, month, stnm);
	ncard++;

	return	ncard;
}
