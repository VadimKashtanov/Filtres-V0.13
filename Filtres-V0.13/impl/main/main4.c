#include "main/main4.h"

static void score(Mdl_t * mdl) {
	printf("prediction=%f gainX%i=%f gain_%iHX%i=%f gain_%iHX%i=%f gain_%iHX%i=%f\n",
		prediction_mdl_f(mdl),
		10, gain(mdl, 100, 10),
		_168H, 10, gain_168H_levier(mdl, 100, 10),
		_168H, 25, gain_168H_levier(mdl, 100, 25),
		_168H, 50, gain_168H_levier(mdl, 100, 50)
	);
};

void main4_selection() {
	//	---- Initialisation du travail ----  //
	maitre_cree_travailleurs();

	Mdl_t * mdl = cree_mdl();
	mdl_verif(mdl);
	float mdl_score = prediction_mdl_f_simd(mdl);

	Mdl_t * sortie;

	//	Boucle
	sortie = maitre_lancer_calcule_selection(
		mdl, mdl_score,
		1,
		1, 3,
		1, 3
	);
	liberer_mdl(mdl);
	mdl = sortie;
	score(mdl);

	maitre_finire_travaille();
};