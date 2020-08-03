#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLINE 2048
#define MAXLEN 89
#define INDENT 4

int w56_comment(FILE *fp, char *line)
{
	int	i, j;
	int	nw = 0;
	int	nline = 0;
	int	len;

	char	card[MAXLEN];
	char	s[MAXLEN];

	len = strlen(line);

	if (strlen(s) > MAXLINE) {
		fprintf(stderr, "w56_comment error: MAXLINE = %d.  Array is not long enough\n", MAXLINE);
		exit(1);
	}

	nline = 0;

	i = j = 0;
	card[0] = s[0] = '\0';
	while (i < len) {
		j = 0;		/* index for word */
		s[0] = '\0';
		/* skip blank spaces at beginning of word */
		while(isspace(line[i]))
			i++;
		/* build word */
		while(! isspace(line[i]) && i < len) s[j++] = line[i++];
		s[j] = '\0';
		if (j >= MAXLEN - INDENT) {
			fprintf(stderr, "w56_comment error: word too long in line:\n%s\n", line);
			exit(1);
		}
		if (strlen(card) + strlen(s) + 2 <= MAXLEN) {
			if (nw > 0) strcat(card, " ");
			strcat(card, s);
		} else {
			fprintf(fp, "    %s\n", card);
			nline++;
			nw = 0;
			card[0] = '\0';
			for (j = 0; j < INDENT; j++)
				card[j] = ' ';
			card[INDENT] = '\0';
			strcat(card, s);
		}
		nw++;
	}
	/* write last line */
	fprintf(fp, "    %s\n", card);
	nline++;

	return nline;
}
