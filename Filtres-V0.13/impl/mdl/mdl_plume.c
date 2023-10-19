#include "mdl.h"

void plume_mdl(const Mdl_t * mdl) {
	char nom_fichier[50];
	uint nb = 0;
	FOR(0, x, 100) {
		snprintf(nom_fichier, 50, "tmpt/tempt%i", x);
		FILE * fp = fopen(nom_fichier, "rb");
		if (fp == 0) {
			nb = x + 1;
			break;
		} else {
			fclose(fp);
		}
	}
	if (nb == 0) ERR("Plus de 100 fichier ont ete cree");
	snprintf(nom_fichier, 50, "tmpt/tempt%i", nb-1);
	ecrire_mdl(mdl, nom_fichier);

	char commande[100];
	snprintf(commande, 100, "python3 plume_mdl.py %s", nom_fichier);
	printf("plume_mdl.py sortie avec %i\n", system(commande));
};