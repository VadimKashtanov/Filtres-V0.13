#include "mdl.h"

PAS_OPTIMISER()
void filtre_aleatoire(Mdl_t * mdl, uint i) {
	prete(mdl->filtre + i*N, N);
};

PAS_OPTIMISER()
void cadran_aleatoire(Mdl_t * mdl, uint i) {
	prete(mdl->poids + i*P*P, P*P);
	FOR(0, j, P*P) mdl->poids[i*P*P + j] = 2*mdl->poids[i*P*P + j]-1;
		//mdl->poids[i*P*P + j] = 2*rnd()-1;
};

//	------------------------------------------------------------------------

PAS_OPTIMISER()
void mutation_filtre(Mdl_t * mdl, uint i) {
	float f[N];
	prete(f, N);
	FOR(0, j, N) mdl->filtre[i*N + j] = mdl->filtre[i*N + j]*0.70 + 0.30*f[j];
	normer(mdl->filtre + i*N, N);
};

#define POSSIBILITEES 7
static float forces_ds[POSSIBILITEES][2] = {
	{3.0, 1.0}, {2.0, 1.0}, //d = 1.0
	{2.0, 0.7}, {1.5, 0.7},	//d = 0.7
	{2.0, 0.5},				//d = 0.5
	{3.0, 0.3}, {2.0, 0.3}	//d = 0.3
};
PAS_OPTIMISER()
void mutation_cadran(Mdl_t * mdl, uint c, float x, float y, float signe) {
	uint choix = rand() % POSSIBILITEES;
	float force = forces_ds[choix][0] * signe;
	float d = forces_ds[choix][1];
	//
	float dist, ajout;
	FOR(0, _y, P) {
		FOR(0, _x, P) {
			dist = fabs(_x-x)/d/(float)P + fabs(_y-y)/d/(float)P;
			ajout = ___exp(-dist*dist) * force;
			mdl->poids[c*P*P + _y*P + _x] += ajout;
		}
	}
	normer(mdl->poids + c*P*P, P*P);
	FOR(0, j, P*P) {
		mdl->poids[c*P*P+j] = 2*mdl->poids[c*P*P+j]-1;
	}
};