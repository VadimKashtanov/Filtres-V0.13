#include "travailleurs.h"

#define ETAT_TRAVAILLE 123
#define ETAT_ATTENTE_ORDRE 234
#define ETAT_RESULAT_FINIE 345
#define ETAT_SORTIE 456

#define ORDRE_SORTIE 123456789

uint ordres[5];

static float fichier_ordes[100];				// "tmpt/ordre{I}"
	//	: enchainnement:<uint> + cadranMin:<uint> + cadranMax:<uint> + filtreMin:<uint> + filtreMax:<uint>
static float fichier_etat[100];					// "tmpt/etat{I}"
	//	: etat:<uint>
static float mdl_resultat_travailleur[100];		// "tmpt/resultat{I}"
	//	: mdl:Mdl_t
static float score_resultat_travailleur[100];	// "tmpt/score{I}"
	//	: score:float
static float mdl_entree_travailleur[100];		// "tmpt/entree{I}"
	//	: mdl:Mdl_t

//	==========================================================
//	==========================================================
//	==========================================================

void maitre_cree_travailleurs()
{
	char commande[100];
	FOR(0, i, TRAVAILLEURS) {
		//
		snprintf(fichier_ordes, 100, "tmpt/ordre%i", i);
		ecrire_uint(fichier_ordes, 0);

		snprintf(fichier_etat, 100, "tmpt/etat%i", i);
		ecrire_uint(fichier_etat, 0);

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
	Mdl_t * mdl,
	uint enchainner,
	uint cMin, uint cMax,
	uint fMin, uint fMax)
{
	
};

void maitre_finire_travaille()
{
	FOR(0, i, TRAVAILLEURS) {
		snprintf(fichier_etat, 100, "tmpt/etat%i", i);
		if (lire_uint(fichier_etat) == ETAT_SORTIE) ERR("Maitre : Sortie de travailleur Imprevue.");
		else {
			snprintf(fichier_ordes, 100, "tmpt/ordre%i", i);
			ecrire_uint(fichier_ordes, ORDRE_SORTIE);
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

	snprintf(fichier_ordes, 100, "tmpt/ordre%i", I);
	snprintf(fichier_etat, 100, "tmpt/etat%i", I);
	snprintf(mdl_resultat_travailleur, 100, "tmpt/resultat%i", I);
	snprintf(score_resultat_travailleur, 100, "tmpt/score%i", I);
	snprintf(mdl_entree_travailleur, 100, "tmpt/entree%i", I);

	ecrire_uint(ETAT_ATTENTE_ORDRE, fichier_etat);

	while (1) {
		//	Attente d'ordre
		while (1) {
			uint _ordre = lire_uint(fichier_ordes);
			if (_ordre == ORDRE_SORTIE) {
				ERR("Travailleur %i : Temps aloue depassee. Sortie forcee.", I);
				ecrire_uint(ETAT_SORTIE, fichier_etat);
			} else if (_ordre != 0) {
				ecrire_uint(fichier_etat, ETAT_TRAVAILLE);
				ecrire_uint(fichier_ordes, 0);
			}
			if (SECONDES_EXISTANCE_MAXIMALE < VALEUR_CHRONO(a)) {
				ERR("Travailleur %i : Temps aloue depassee. Sortie forcee.", I);
				ecrire_uint(ETAT_SORTIE, fichier_etat);
			};
		};

		//	Execution Ordre
		lire_N_uint(fichier_ordes, ordres, 5);
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
				ecrire_uint(ETAT_SORTIE, fichier_etat);
			}
		};


		//	Enregistrement du resultat
		ecrire_flotant(score_resultat_travailleur, meilleur_score);
		ecrire_mdl(mdl, mdl_resultat_travailleur);
		ecrire_uint(fichier_ordes, 0);
		ecrire_uint(fichier_etat, ETAT_RESULAT_FINIE);
	}
};