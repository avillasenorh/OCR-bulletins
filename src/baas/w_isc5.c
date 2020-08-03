#include <math.h>
#include <stdio.h>
#include <string.h>

#define NINT(x) (int)floor((x)+0.5)

#define INT_UNDEF -12345
#define FLT_UNDEF -12345.0

#define MAXLINE 96

int w_isc5(char *s, int nrc, int year, int month, char *stnm, int stno, char net, char src,
  char fmt, char ltflag, float az, float dist, int nph, int day, int hour, int min, float sec,
  int sec_acc, int op_id, char *phase, float op_res, int isc_id, float isc_res, char pol,
  char inst, char cmp, char onset)
{
	char	aux[MAXLINE];

	/* All these fields ignored:

	station number (stno, %4d)
	network code (net, %c)
	source code (src, %c)
	format received code (fmt, %c)
	local/teleseismic flag (ltflag, %c)
	*/

	sprintf(s, "%2d%2d%4d%2d%-4s        %3d%5d%3d%2d%2d%2d%4d%2d%3d%-8s%4d%3d%4d%c%c%c%c",
	5, nrc, year, month, stnm, NINT(az), NINT(dist*100.0), nph, day, hour, min, NINT(sec*100.0),
	sec_acc, op_id, phase, NINT(10.0*op_res), isc_id, NINT(10.0*isc_res), pol, inst, cmp, onset);

	/* ignore signal-to-noise, and all magnitude information:
	write only undefined precision for log(A/T), A, T */

	strcat(s, "    99      99    99");

	return 0;
}
