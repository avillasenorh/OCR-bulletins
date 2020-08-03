#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define INT_UNDEF -12345
#define FLT_UNDEF -12345.0

#define TRUE 1
#define FALSE 0

int get_phases(char *line, char *stnm, float *delta, float *azim,
		int *Pmin, int *Psec, int *Pres, int *Ppol,
		int *Smin, int *Ssec, int *Sres,
		int *pPmin, int *pPsec,
		int *Suppmin, int *Suppsec, char *Supp)
{
	int	i;

	char	cmin[3];
	char	csec[4];
	char	cres[5];

	/* initialize all phase data to "undefined" values */

	*delta = *azim = FLT_UNDEF;
	*Pmin = *Psec = *Pres = *Ppol = INT_UNDEF;
	*Smin = *Ssec = *Sres = INT_UNDEF;
	*pPmin = *pPsec = *Suppmin = *Suppsec = INT_UNDEF;

	Supp[0] = '\0';

	/* station name */

	strncpy(stnm, line, 12);
	stnm[12] = '\0';

	/* distance and azimuth */

	sscanf(line+12, "%f%f", delta, azim);

	
	/* P phase */

	if (strlen(line) >= 40) {
		strncpy(cmin, &line[30], 2);
		cmin[2] = '\0';
		strncpy(csec, &line[33], 3);
		csec[3] = '\0';
		strncpy(cres, &line[36], 4);
		cres[4] = '\0';

		sscanf(cmin, "%d", Pmin);
		sscanf(csec, "%2d", Psec);
		*Ppol = line[35];
		sscanf(cres, "%d", Pres);

	} else return 0;

	/* S phase */

	if (strlen(line) >= 52) {
		strncpy(cmin, &line[43], 2);
		cmin[2] = '\0';
		strncpy(csec, &line[46], 2);
		csec[2] = '\0';
		strncpy(cres, &line[48], 4);
		cres[4] = '\0';

		sscanf(cmin, "%d", Smin);
		sscanf(csec, "%d", Ssec);
		sscanf(cres, "%d", Sres);
	} else return 0;

	/* pP phase */

	if (strlen(line) >= 60) {
		strncpy(cmin, &line[55], 2);
		cmin[2] = '\0';
		strncpy(csec, &line[58], 2);
		csec[2] = '\0';

		sscanf(cmin, "%d", pPmin);
		sscanf(csec, "%d", pPsec);
	} else return 0;

	/* Supplementary phase */

	if (strlen(line) >= 68) {
		strncpy(cmin, &line[63], 2);
		cmin[2] = '\0';
		strncpy(csec, &line[66], 2);
		csec[2] = '\0';

		sscanf(cmin, "%d", Suppmin);
		sscanf(csec, "%d", Suppsec);
		sscanf(&line[68], "%s", Supp);
	} else return 0;

	return 0;
}
