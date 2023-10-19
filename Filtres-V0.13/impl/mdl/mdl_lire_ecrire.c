#include "mdl.h"

#define _N 4
uint params[_N] = {C, F, N, P*P};

void ecrire_mdl(const Mdl_t * mdl, const char * file) {
	FILE * fp = fopen(file, "wb");
	if (fp == 0) ERR("Fichier %s existse pas", file);
	//
	(void)fwrite(params, sizeof(uint), _N, fp);
	(void)fwrite(mdl, sizeof(Mdl_t), 1, fp);
	fclose(fp);
}

Mdl_t * lire_mdl(const char * file) {
	FILE * fp = fopen(file, "rb");
	if (fp == 0) ERR("Fichier %s existse pas", file);

	uint autres[_N];
	(void)!fread(autres, sizeof(uint), _N, fp);
	FOR(0, i, _N) assert(params[i] == autres[i]);
	Mdl_t * ret = malloc(sizeof(Mdl_t));
	(void)!fread(ret, sizeof(Mdl_t), 1, fp);
	return ret;
};