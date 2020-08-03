#include <stdio.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

int valid_phase(char *phase)
{
	int	i;
	static char *name[] = {
		"?",
		"P", "Pg", "P*", "Pn", "PP", "PPP", "PKP", "PKKP", "PKKKP", "PcP",
		"pP", "sP", "sS", "pPP", "pPKP", "PKP2", "pPKP2", "P'", "PKPPKP", "P'P'",
		"S", "Sg", "S*", "Sn", "SS", "SSS", "SKS", "SKKS", "SKKKS", "ScS",
		"PS", "PPS", "pPKKP", "PKS", "pSKS", "SKP", "sSKS", "pPS", "sPKP", "sPKS",
		"L", "Q", "R", "ScP", "PcS", "SSP", "SKSP", "sPS", "SKKP", "PSKS",
		"pPcP", "pS", "SP", "SPP", "pP'", "sP'", "SPS", "PSS", "PcSP'", "pPcS",
		"sPP", "SKKS2", "pP2'", "SKPP'", "PKKS", "pPKS", "P2'", "PcPP'", "sSS", "ScSP'",
		"P'PKS", "pPPP", "sScS", "PcPPKP", "PgPg", "P'P'P'", "pP'P'", "SKS2", "PSPS", "pSKKS",
		"PPPP", "ScSP", "ScSPKP", "SSSS", "pSP", "PcSScP", "sSSS", "sSP", "PPKP", "PcSSKP",
		"SgSg", "sSKKS"
	};
	for (i = 0; i < 93; i++) {
		if (strcmp(phase, name[i]) == 0)
			return TRUE;
	}

	return FALSE;
}
