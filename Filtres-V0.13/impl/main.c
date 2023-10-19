#include "lib_filtres.h"

PAS_OPTIMISER()
int main(int argc, char ** argv) {
	//	Initialisation
	srand(0);
	init_libfiltres();
	info_libfiltres();

	if (argc == 2) {
		//	---------- Travailleur ------------	//
		printf("Depart d'un travailleur.\n");
		travailleur(atoi(argv[1]));
	} else {
		//	Coprs du Programme
		main2_performances();
		//main2_mutations();
		//main3_ilfallait();
		main4_selection();
	}

	//	Sortie programme
	sortie_libfiltres();
};