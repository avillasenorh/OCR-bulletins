#include <stdio.h>
#include <string.h>

/***********************************************************************
* Program: rmdash.c
* Purpose: removes m-dash characters (ASCII code 227) from text files
*
* Author:	Antonio Villasenor, USGS
* Date:		15-JUL-1999
* History:	This version uses the ASCII code of the m-dash character
*		via the MDASH symbol
***********************************************************************/

#define MAXLINE 2048
#define TRUE	1
#define FALSE	0

#define MDASH	'\227'

main(int argc, char *argv[])
{
	FILE	*fp;

	char	line[MAXLINE];
	char	infile[512];

	int	i;

	int	lfile = FALSE;
	int	verbose = FALSE;

	/* Decode flag options */

	for (i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			switch (argv[i][1]) {
				case 'V': /* verbose */
					verbose = TRUE;
					break;
				default:
					break;
			}
		} else {
			if ((fp = fopen(argv[i], "r")) == NULL) {
				fprintf(stderr,
				"cannot open %s \n", argv[i]);
				return 1;
			} else {
				strcpy(infile,argv[i]);
				lfile = TRUE;
			}
		}
	}

	/* if no file passed, will read from standard input */
	if (! lfile) fp = stdin;

	if (lfile && verbose) {
		fprintf(stderr,"Working on file %s\n",infile);
	}

	while (fgets(line, MAXLINE, fp) != NULL) {
		for (i = 0; i < strlen(line); i++) {
			if (line[i] == MDASH)
				line[i] = '-';
		}
		fprintf(stdout, "%s", line);
	}

	if (lfile) fclose(fp);

	return 0;
}
