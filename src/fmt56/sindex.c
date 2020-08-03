/***********************************************************************
* Function: sindex
************************************************************************
* Purpose: Returns index (first occurrence) of t in s, -1 if none
***********************************************************************/
/*int sindex(char s[], char t[])*/
int sindex(char *s, char *t)
{
	int	i, j, k;

	for (i = 0; s[i] != '\0'; i++) {
		for (j=i, k=0; t[k]!='\0' && s[j]==t[k]; j++, k++)
			;
		if (t[k] == '\0')
			return i;
	}
	return -1;
}
