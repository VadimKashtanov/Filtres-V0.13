#include "travailleurs.h"

#define ETAT_TRAVAILLE 1
#define ETAT_ATTENTE_ORDRE 2
#define ETAT_RESULAT_FINIE 3
#define ETAT_SORTIE 4

#define AUCUN_ORDRE 0
#define ORDRE_ACTION 1
#define ORDRE_SORTIE 2

uint ordres[5];

static char fichier_ordre_1char[100];			// "tmpt/ordre1char{I}"
	//	: pret:<char>
static char fichier_ordres[100];				// "tmpt/ordres{I}"
	//	: enchainnement:<uint> + cadranMin:<uint> + cadranMax:<uint> + filtreMin:<uint> + filtreMax:<uint>
static char fichier_etat[100];					// "tmpt/etat{I}"
	//	: etat:<char>
static char mdl_resultat_travailleur[100];		// "tmpt/resultat{I}"
	//	: mdl:Mdl_t
static char score_resultat_travailleur[100];	// "tmpt/score{I}"
	//	: score:float
static char mdl_entree_travailleur[100];		// "tmpt/entree{I}"
	//	: mdl:Mdl_t

//	==========================================================
//	==========================================================
//	==========================================================

void maitre_cree_travailleurs()
{
	char commande[100];
	FOR(0, i, TRAVAILLEURS) {
		snprintf(fichier_ordre_1char, 100, "tmpt/ordre1char%i", i);
		ecrire_uint(fichier_ordre_1char, AUCUN_ORDRE);

		snprintf(fichier_ordres, 100, "tmpt/ordres%i", i);
		ecrire_uint(fichier_ordres, 0);

		snprintf(fichier_etat, 100, "tmpt/etat%i", i);
		ecrire_char(fichier_etat, 0);

		snprintf(mdl_resultat_travailleur, 100, "tmpt/resultat%i", i);
		ecrire_uint(mdl_resultat_travailleur, 0);

		snprintf(score_resultat_travailleur, 100, "tmpt/score%i", i);
		ecrire_uint(score_resultat_travailleur, 0);

		snprintf(mdl_entree_travailleur, 100, "tmpt/travailleur%i", i);
		ecrire_uint(mdl_entree_travailleur, 0);

		//
		snprintf(commande, 100, "./prog %i &", i);
		if (system(commande) == 0) OK("Creation travailleur %i", i);
	}
};

Mdl_t * maitre_lancer_calcule_selection(
	Mdl_t * mdl, float score_mdl,
	uint enchainner,
	uint cMin, uint cMax,
	uint fMin, uint fMax)
{
	assert(enchainner > 0);
	//
	INIT_CHRONO(s);
	DEPART_CHRONO(s);
	//	,---------- faut faire un lock file ------------
	FOR(0, i, TRAVAILLEURS) {
		snprintf(fichier_ordres, 100, "tmpt/ordres%i", i);
		FILE * fp = fopen(fichier_ordres, "wb");
		ordres[0] = enchainner;
		ordres[1] = cMin; ordres[2] = cMax;
		ordres[3] = fMin; ordres[4] = fMax;
		fwrite(ordres, sizeof(uint), 5, fp);
		fclose(fp);

		snprintf(mdl_entree_travailleur, 100, "tmpt/travailleur%i", i);
		ecrire_mdl(mdl, mdl_entree_travailleur);

		snprintf(fichier_ordre_1char, 100, "tmpt/ordre1char%i", i);
		ecrire_char(fichier_ordre_1char, ORDRE_ACTION);
	};

	Mdl_t * sortie = copier_mdl(mdl);

	//	Model par seconde
	uint MODEL_PAR_SECOND = 30;

	struct timespec tim, tim2;
		tim.tv_sec = 0;
		tim.tv_nsec = (1e9 / MODEL_PAR_SECOND) * TRAVAILLEURS;
	nanosleep(&tim , &tim2);

	FOR(0, i, TRAVAILLEURS) {
		snprintf(fichier_etat, 100, "tmpt/etat%i", i);
		while (lire_char(fichier_etat) != ETAT_RESULAT_FINIE) {
			if (SECONDES_EXISTANCE_MAXIMALE < VALEUR_CHRONO(s))
				ERR("Maitre : Meme `maitre_lancer_calcule_selection` prend plus de temps que l'existance des travailleurs");
		}
		snprintf(score_resultat_travailleur, 100, "tmpt/score%i", i);
		float __score = lire_flotant(score_resultat_travailleur);
		if (__score > score_mdl) {
			score_mdl = __score;
			liberer_mdl(sortie);
			snprintf(mdl_resultat_travailleur, 100, "tmpt/resultat%i", i);
			sortie = lire_mdl(mdl_resultat_travailleur);

			//
			printf("Travailleur %i, trouve %f\n", i, __score);
		}
	};

	printf("Temps calcule %i travailleurs pour %i models = %f\n", TRAVAILLEURS, enchainner, VALEUR_CHRONO(s));
	return sortie;
};

void maitre_finire_travaille()
{
	FOR(0, i, TRAVAILLEURS) {
		snprintf(fichier_etat, 100, "tmpt/etat%i", i);
		if (lire_char(fichier_etat) == ETAT_SORTIE) {
			ERR("Maitre : Sortie de travailleur Imprevue.");
		} else {
			snprintf(fichier_ordre_1char, 100, "tmpt/ordre1char%i", i);
			ecrire_char(fichier_ordre_1char, ORDRE_SORTIE);
		}
	}
	OK("Tous les travailleurs sortent avec success");
};

//	==========================================================
//	==========================================================
//	==========================================================

void travailleur(uint I)
{
	INIT_CHRONO(a)
	DEPART_CHRONO(a)

	snprintf(fichier_ordres, 100, "tmpt/ordres%i", I);
	snprintf(fichier_ordre_1char, 100, "tmpt/ordre1char%i", I);
	snprintf(fichier_etat, 100, "tmpt/etat%i", I);
	snprintf(mdl_resultat_travailleur, 100, "tmpt/resultat%i", I);
	snprintf(score_resultat_travailleur, 100, "tmpt/score%i", I);
	snprintf(mdl_entree_travailleur, 100, "tmpt/entree%i", I);

	ecrire_char(ETAT_ATTENTE_ORDRE, fichier_etat);

	while (1) {
		//	Attente d'ordre
		while (1) {
			char _ordre = lire_char(fichier_ordre_1char);
			if (_ordre == ORDRE_SORTIE) {
				ERR("Travailleur %i : Temps aloue depassee. Sortie forcee.", I);
				ecrire_char(ETAT_SORTIE, fichier_etat);
			} else if (_ordre == ORDRE_ACTION) {
				ecrire_char(fichier_etat, ETAT_TRAVAILLE);
				ecrire_char(fichier_ordre_1char, AUCUN_ORDRE);
				break;
			}
			if (SECONDES_EXISTANCE_MAXIMALE < VALEUR_CHRONO(a)) {
				ERR("Travailleur %i : Temps aloue depassee. Sortie forcee.", I);
				ecrire_char(ETAT_SORTIE, fichier_etat);
			};
		};

		//	Execution Ordre
		lire_N_uint(fichier_ordres, ordres, 5);
		Mdl_t * mdl = lire_mdl(mdl_entree_travailleur);
		Mdl_t * tmp = copier_mdl(mdl);
		float meilleur_score=0;
		float score;
		FOR(0, i, ordres[0]) {
			//	Muter cadrans `tmp`
			uint a_muter = ordres[1] + rand()%(ordres[2]-ordres[1]);
			FOR(0, j, a_muter)
				mutation_cadran(tmp, rand()%(F-1), rnd()*7, rnd()*7, (rand()%2==0 ? 1:-1));
			//	Muter filtres `tmp`
			a_muter = ordres[3] + rand()%(ordres[4]-ordres[3]);
			FOR(0, j, a_muter)
				mutation_filtre(tmp, rand() % F);



			//	Ce qui prend 99% du temps
			//-------------------------------------//
			score = prediction_mdl_f_simd(tmp);    //
			//-------------------------------------//



			if (score > meilleur_score) {
				meilleur_score = score;
				liberer_mdl(mdl);
				mdl = copier_mdl(tmp);
			} else {
				liberer_mdl(tmp);
				tmp = copier_mdl(mdl);
			};

			if (SECONDES_EXISTANCE_MAXIMALE < VALEUR_CHRONO(a)) {
				ERR("Travailleur %i : Temps aloue depassee. Sortie forcee.", I);
				ecrire_char(ETAT_SORTIE, fichier_etat);
			}
		};


		//	Enregistrement du resultat
		ecrire_flotant(score_resultat_travailleur, meilleur_score);
		ecrire_mdl(mdl, mdl_resultat_travailleur);
		ecrire_char(fichier_ordre_1char, AUCUN_ORDRE);
		ecrire_char(fichier_etat, ETAT_RESULAT_FINIE);
	}
};