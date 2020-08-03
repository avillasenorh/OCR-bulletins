/*******************************************************************************
*******************************************************************************/

#include <stdio.h>
#include <string.h>

#define MAXLINE 128

int yrpg(char *line, int *pyear, int *ppage)
{
	int	nf;
	int	year, page;
	char	spage[MAXLINE];
	char	extra[MAXLINE] = "\0";

	/* Check that the word "PAGE" is present in the string line */

	if (strstr(line, "PAGE") == NULL) {
		fprintf(stderr, "yrpg: Not a page header line:\n%s", line);
		return(1);
	}

	nf = sscanf(line,"%d %s %d%[^\n]", &year, spage, &page, extra);

	if (nf < 3) {
		fprintf(stderr, "yrpg: Error reading page and year:\n%s", line);
		return(1);
	}

	if (year < 1957 || year > 1963) {
		fprintf(stderr, "yrpg: Error interpreting year: %d\n%s",
		year, line);
		return(1);
	}
	if (page < 0 || page > 2000) {
		fprintf(stderr, "yrpg: Error interpreting page number: %d\n%s",
		page, line);
		return(1);
	}

	*pyear = year;
	*ppage = page;

/*
	fprintf(stderr, "Number of fields: %d\n", nf);
	fprintf(stderr, "Year: %d\n", year);
	fprintf(stderr, "Page label: **%s**\n", spage);
	fprintf(stderr, "Page: %d\n", page);
	fprintf(stderr, "Extra: **%s**\n", extra);
*/

	return(0);
}
