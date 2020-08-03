#include <stdio.h>
#include <string.h>

#define MAXLINE 128

int w_hypo(char *card, int month, int day, int hour, int minute, int second,
		float lat, float lon, float depth)
{
	static char *name[] = {
		"illegal month",
		"JANUARY",
		"FEBRUARY",
		"MARCH",
		"APRIL",
		"MAY",
		"JUNE",
		"JULY",
		"AUGUST",
		"SEPTEMBER",
		"OCTOBER",
		"NOVEMBER",
		"DECEMBER"
	};

	int	i;
	char	sepic[MAXLINE];

	sprintf(card, "      %s %d", name[month], day);

	for (i = strlen(card); i <= 22; i++)
		card[i] = ' ';
	card[23] = '\0';

	sprintf(sepic,
	"%2d.H %2d.M %2d.S  EPICENTRE %6.2f%7.2f DEPTH= %3.0f.KM",
	hour, minute, second, lat, lon, depth);
	strcat(card, sepic);

	return(0);
}
