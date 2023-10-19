#include "lib_filtres.h"

void init_libfiltres() {
	printf("Chargement des prixs : ");
	MESURER(charger_les_prixs());
	//printf("Initialisation des travailleurs : ");
	//MESURER(initialiser_les_travailleurs());
}; 

void info_libfiltres() {
	printf("           prixs = %3.3f Mo\n", ((float)sizeof(float)*PRIXS) / 1e6);
	printf("             ema = %3.3f Mo\n", ((float)sizeof(float)*EMA_INTS*PRIXS) / 1e6);
	printf("      normalisee = %3.3f Mo\n", ((float)sizeof(float)*EMA_INTS*N_FLTR*PRIXS) / 1e6);
	printf("  dif_normalisee = %3.3f Mo\n", ((float)sizeof(float)*EMA_INTS*(N_FLTR-1)*PRIXS) / 1e6);
	printf("   sizeof(Mdl_t) = %3.3f Mo\n", (float)sizeof(Mdl_t)/1e6);
	printf(" >> somme totale = %3.3f Mo [1 ./prog theoriquement]\n", (float)(sizeof(float)*(
		PRIXS + EMA_INTS*PRIXS + EMA_INTS*N_FLTR*PRIXS + EMA_INTS*(N_FLTR-1)*PRIXS) + sizeof(Mdl_t))/1e6);
	printf("--------------- Debut programme ----------------\n");
};

void sortie_libfiltres() {
	//printf("\n");
}