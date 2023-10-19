#include "main/main2.h"

static void score(Mdl_t * mdl) {
	printf("prediction=%f gainX%i=%f gain_%iHX%i=%f gain_%iHX%i=%f gain_%iHX%i=%f\n",
		prediction_mdl_f(mdl),
		10, gain(mdl, 100, 10),
		_168H, 10, gain_168H_levier(mdl, 100, 10),
		_168H, 25, gain_168H_levier(mdl, 100, 25),
		_168H, 50, gain_168H_levier(mdl, 100, 50)
	);
};

PAS_OPTIMISER()
void main2_performances() {
	//	--- Entree ---
	Mdl_t * mdl = cree_mdl();
	mdl_verif(mdl);
	score(mdl);

	//	--- Corps ---
	float pred;

	INIT_CHRONO(chrono)

	/*	====================== */
	DEPART_CHRONO(chrono)
	pred = prediction_mdl_f(mdl);
	printf(
		"prediction_mdl_f(mdl)=%f.   Temps ~= %3.3f,  \033[93mpred(Mdl_t)/seconde=%3.2f\033[0m\n",
		pred, VALEUR_CHRONO(chrono), 1.f/(VALEUR_CHRONO(chrono))
	);

	/*	====================== */
	DEPART_CHRONO(chrono)
	pred = prediction_mdl_f_simd(mdl);
	printf(
		"prediction_mdl_f_simd(mdl)=%f.   Temps ~= %3.3f,  \033[93mpred(Mdl_t)/seconde=%3.2f\033[0m\n",
		pred, VALEUR_CHRONO(chrono), 1.f/(VALEUR_CHRONO(chrono))
	);

	//	--- Sortie ---
	liberer_mdl(mdl);
};

PAS_OPTIMISER()
void main2_mutations() {
	//	--- Entree ---
	Mdl_t * mdl = cree_mdl();
	mdl_verif(mdl);
	score(mdl);

	plume_mdl(mdl);
	mutation_filtre(mdl, 0);
	mutation_cadran(mdl, 0, 4.f, 4.f, 1);
	mutation_cadran(mdl, 1, 4.f, 4.f, 1);
	mutation_cadran(mdl, 2, 4.f, 4.f, 1);
	mutation_cadran(mdl, 3, 4.f, 4.f, 1);
	mutation_cadran(mdl, 4, 4.f, 4.f, 1);
	mutation_cadran(mdl, 5, 4.f, 4.f, 1);
	mutation_cadran(mdl, 6, 4.f, 4.f, 1);
	plume_mdl(mdl);

	//	--- Sortie ---
	liberer_mdl(mdl);
};