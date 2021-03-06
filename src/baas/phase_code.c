#include <stdio.h>
#include <string.h>

/***********************************************************************
* Function: phase_code
*
* Purpose: returns ISC phase code
*
*	op_id =
*	isc_id =
*
***********************************************************************/

int phase_code(char *phase)
{
	int	i;

	int	pc = -1;

	static char *op_id[] = { "P", 
		"PP", "PPP", "PcP", "PKP", "PKP2", "PKPPKP", "PcPPKP", "PS", "PPS", "PcS",
		"PKS", "PKKS", "PcSPKP", "PKPPKS", "PKPSKS", "PKKP", "3PKP", "PKIKP", "PKP1", "PKHKP",
		"PHASE21", "PSS", "PHASE23", "PHASE24", "PHASE25", "PHASE26", "PHASE27", "PHASE28", "PHASE29", "PHASE30",
		"PHASE31", "PHASE32", "PHASE33", "PHASE34", "S", "SS", "SSS", "ScS", "SKS", "SKKS",
		"SKKKS", "ScSPKP", "SKSSKS", "ScSP", "SKSP", "ScP", "SP", "SKP", "SKKP", "SKPPKP",
		"SSP", "PHASE52", "PHASE53", "PHASE54", "PHASE55", "PHASE56", "sPKP2", "pPcP", "pPKP", "pP",
		"pPP", "sP", "sPKP", "sS", "sSS", "sPP", "sPcP", "sScS", "pPKP2", "P*",
		"S*", "Pg", "Sg", "PN", "SN", "PgPg", "SgSg", "LR", "LQ", "L",
		"PHASE81", "PHASE82", "SPP", "PHASE84", "SPECIAL", "QM", "RM", "T", "T(MAX)", "NORTH",
		"SOUTH", "EAST", "WEST", "UP", "DOWN", "E", "I", "MAXIMUM", "FINAL", "S/SKS",
		"P/PKP", "PX", "X1", "X2", "SX", "SB1", "SB2", "", "S/(SKS)", "(S)/SKS"
	};

	if (strcmp(phase, "") == 0 || strcmp(phase, " ") == 0) return 999;

	for (i = 0; i < 110; i++) {
		if (strcmp(phase, op_id[i]) == 0) {
			pc = i;
			break;
		}
	}

	return pc;
}
