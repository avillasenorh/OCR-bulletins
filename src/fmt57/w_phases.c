#include <stdio.h>
#include <string.h>

#define INT_UNDEF -12345
#define FLT_UNDEF -12345.0

int w_phases(char card[], char stnm[], float delta, float azim,
		int Pmin, int Psec, int Pres, int Ppol,
		int Smin, int Ssec, int Sres,
		int pPmin, int pPsec,
		int Suppmin, int Suppsec, char Supp[])
{
	int	i;
	char	ptime[] =	"              ";
	char	stime[] =	"            ";
	char	pPtime[] =	"        ";
	char	supptime[] =	"                ";

	i = 0;
	sprintf(card, "%-16s %6.2f  %5.1f", stnm, delta, azim);
	if (Pmin != INT_UNDEF && Psec != INT_UNDEF && Pres != INT_UNDEF) {
		sprintf(ptime, "    %2d %2d%c%4d", Pmin, Psec, Ppol, Pres);
		i++;
	}
	if (Smin != INT_UNDEF && Ssec != INT_UNDEF && Sres != INT_UNDEF) {
		sprintf(stime, "   %2d %2d%4d", Smin, Ssec, Sres);
		i++;
	}
	if (pPmin != INT_UNDEF && pPsec != INT_UNDEF) {
		sprintf(pPtime, "   %2d %2d", pPmin, pPsec);
		i++;
	}
	if (Suppmin != INT_UNDEF && Suppsec != INT_UNDEF) {
		if (strcmp(Supp, "-12345") == 0) {
			sprintf(supptime, "   %2d %2d", Suppmin, Suppsec);
		} else {
			sprintf(supptime, "   %2d %2d %s", Suppmin, Suppsec, Supp);
		}
		i++;
	}

	strcat(card, ptime);
	strcat(card, stime);
	strcat(card, pPtime);
	strcat(card, supptime);

	return 0;
}
