#pragma once

#include "mdl.h"

#define TRAVAILLEURS 4
#define SECONDES_EXISTANCE_MAXIMALE 30

//	Les maitres
void maitre_cree_travailleurs();
Mdl_t * maitre_lancer_calcule_selection(
	Mdl_t * mdl,
	uint enchainner,	//cmb chaque travailleur
						// enchainsera de mdl
	uint cMin, uint cMax,	//	cadrans a muter
	uint fMin, uint fMax	//	filtres a muter
);
void maitre_finire_travaille();


//	Les travailleurs
void travailleur(uint I);