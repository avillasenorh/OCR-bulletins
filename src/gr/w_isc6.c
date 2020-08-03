#include <math.h>
#include <stdio.h>
#include <string.h>

#define NINT(x) (int)floor((x)+0.5)

#define INT_UNDEF -12345
#define FLT_UNDEF -12345.0

#define MAXLINE 96

int w_isc6(char *s, int nrc, int year, int month, int phno, int day, int hour, int min, float sec,
  int sec_acc, int op_id, char *phase, float op_res, int isc_id, float isc_res, char pol,
  char inst, char cmp, char onset)
{
	char	aux[MAXLINE];

	sprintf(s, "%2d%2d%4d%2d%2d%2d%2d%2d%4d%2d%3d%-8s%4d%3d%4d%c%c%c%c",
	6, nrc, year, month, phno, day, hour, min, NINT(sec*100.0),
	sec_acc, op_id, phase, NINT(10.0*op_res), isc_id, NINT(10.0*isc_res), pol, inst, cmp, onset);

	/* ignore signal-to-noise, and all magnitude information:
	write only undefined precision for log(A/T), A, T */

	strcat(s, "    99      99    99");

	return 0;
}
