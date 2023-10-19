#include "mdl.h"

Mdl_t * cree_mdl() {
	Mdl_t * mdl = (Mdl_t*)malloc(sizeof(Mdl_t));
	FOR(0, i, F) filtre_aleatoire(mdl, i);
	FOR(0, i, F-1) cadran_aleatoire(mdl, i);
	FOR(0, i, F) mdl->lignes[i] = ema_ints[0/*rand() % EMA_INTS*/];
	preparer_mdl(mdl);
	return mdl;
};

void preparer_mdl(Mdl_t * mdl) {
	FOR(0, i, F) {
		FOR(0, j, N-1)
			mdl->dif_flt[i*N+j] = mdl->filtre[i*N+j+1]-mdl->filtre[i*N+j];
		mdl->dif_flt[i*N+N-1] = 0.f;
	}
};

void liberer_mdl(Mdl_t * mdl) {
	free(mdl);
};

void initialiser_les_travailleurs() {
	//pthread_t * threads = (pthread_t*)malloc((FIN-DEPART)*sizeof(pthread_t));
};

void mdl_verif(const Mdl_t * mdl) {
	uint erreur = 0;
	uint incorrect = 0;
	FOR(0, i, (    F    ) *   N  ) {
		if (!(mdl->filtre[i] >= 0 && mdl->filtre[i] <= 1)) {
			if (incorrect == 0) printf("\033[91mErreurs Filtre (pas dans [0:1]) :\033[0m\n");
			incorrect = 1;

			printf("\033[91m%i| %f  (filtre #%i)\033[0m\n", i, mdl->filtre[i], (i-i%N)/N);
		}
	}
	if (incorrect == 1) erreur = 1;
	incorrect = 0;
	FOR(0, i, (F-1) *  P*P) {
		if (!(mdl->poids[i] >= -1 && mdl->poids[i] <= 1)) {
			if (incorrect == 0) printf("\033[91mErreurs Cadran (pas dans [-1:1]) :\033[0m\n");
			incorrect = 1;

			printf("\033[91m%i| %f  (cadran #%i)\033[0m\n", i, mdl->poids[i], (i-i%(P*P))/(P*P));
		}
	}
	if (incorrect == 1) erreur = 1;
	//
	if (erreur) ERR("Le model est invalide.");
};