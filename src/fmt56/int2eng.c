#include <stdio.h>
/*******************************************************************************
Function: int2eng

Purpose: converts some international characters and symbols to standard
         English characters
*******************************************************************************/

int int2eng(char *s)
{
	int	i;
	unsigned char	c;

	i = 0;
	while(s[i] != '\0') {
		c = s[i];

		if (c == 209)		/* enye (mayuscula) */
			s[i] = 'N';
		else if (c == 241)	/* enye (minuscula) */
			s[i] = 'n';
		else if (c == 199)	/* C con cedilla (mayuscula) */
			s[i] = 'C';
		else if (c == 231)	/* c con cedilla (minuscula) */
			s[i] = 'c';
		else if (c == 177)	/* plus-minus sign */
			s[i] = '+';

		else if (c == 145 || c == 146) /* single quotations */
			s[i] = '\'';

		/* uppercase vowels with accents, etc */
		else if (c >= 192 && c <= 197)
			s[i] = 'A';
		else if (c >= 200 && c <= 203)
			s[i] = 'E';
		else if (c >= 204 && c <= 207)
			s[i] = 'I';
		else if ((c >= 210 && c <= 214) || c == 216)
			s[i] = 'O';
		else if (c >= 217 && c <= 220)
			s[i] = 'U';

		/* lowercase vowels with accents, etc */
		else if (c >= 224 && c <= 229)
			s[i] = 'a';
		else if (c >= 232 && c <= 235)
			s[i] = 'e';
		else if (c >= 236 && c <= 239)
			s[i] = 'i';
		else if ((c >= 242 && c <= 246) || c == 248)
			s[i] = 'o';
		else if (c >= 249 && c <= 252)
			s[i] = 'u';

		i++;
	}

	return 0;
}
