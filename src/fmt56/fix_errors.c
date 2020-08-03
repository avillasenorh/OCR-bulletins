#include <stdio.h>
#include <string.h>

/*******************************************************************************
Function: fix_errors.c

Purpose: fix common OCR errors for pre-1957 ISS txt files

1) Character substitution:
j -> i
u -> i
O -> 0
h -> 1
l -> 1
E -> [

2) String substitution:
ii	->	change second 'i' for 1
iii	->	i11
PKP,	->	PKP2
PeP	->	PcP
SeS	->	ScS
-f-	->	+
-f	->	+
H-	->	+
*******************************************************************************/

/*int sindex(char s[], char t[]);*/
int sindex(char *s, char *t);

#define TRUE 1
#define FALSE 0

int fix_errors(char *s)
{
	int	i;
	int substitution;

	for (i = 0; i < strlen(s); i++) {
		if (s[i] == 'j') {
			s[i] = 'i';
		} else if (s[i] == 'u') {
			s[i] = 'i';
		} else if (s[i] == 'o') {
			s[i] = 'e';
		} else if (s[i] == 'O') {
			s[i] = '0';
		} else if (s[i] == 'l') {
			s[i] = '1';
		} else if (s[i] == 'h') {
			s[i] = '1';
		} else if (s[i] == 'b') {
			s[i] = '1';
		} else if (s[i] == 'E') {
			s[i] = '[';
		}

	}

	/*  these substitutions not always work well
	if ((i = sindex(s, "iii")) != -1) {
			s[i+1] = s[i+2] = '1';
	} else if ((i = sindex(s, "ii")) != -1) {
			s[i] = s[i+1] = '1';
	} else if ((i = sindex(s, "iS")) != -1) {
			s[i] = '1';
			s[i+1] = '5';
	*/

	/* this should work even if strings occur more than one time */

	substitution = TRUE;

	while (substitution) { /* run until no substitution is done */

		substitution = FALSE;
		if ((i = sindex(s, "PKP,")) != -1) {
			s[i+3] = '2';
			substitution = TRUE;
		} else if ((i = sindex(s, "Pc-P")) != -1) {
			s[i+2] = 'P';
			s[i+3] = ' ';
			substitution = TRUE;
		} else if ((i = sindex(s, "PeP")) != -1) {
			s[i+1] = 'c';
			substitution = TRUE;
		} else if ((i = sindex(s, "PeS")) != -1) {
			s[i+1] = 'c';
			substitution = TRUE;
		} else if ((i = sindex(s, "Sc-S")) != -1) {
			s[i+2] = 'S';
			s[i+3] = ' ';
			substitution = TRUE;
		} else if ((i = sindex(s, "SeS")) != -1) {
			s[i+1] = 'c';
			substitution = TRUE;
		} else if ((i = sindex(s, "SeP")) != -1) {
			s[i+1] = 'c';
			substitution = TRUE;
		} else if ((i = sindex(s, "-f-")) != -1) {
			s[i] = ' ';
			s[i+1] = '+';
			s[i+2] = ' ';
			substitution = TRUE;
		} else if ((i = sindex(s, "-f")) != -1) {
			s[i] = ' ';
			s[i+1] = '+';
			substitution = TRUE;
		} else if ((i = sindex(s, "H-")) != -1) {
			s[i] = ' ';
			s[i+1] = '+';
			substitution = TRUE;
		}
	}
	return 0;
}
