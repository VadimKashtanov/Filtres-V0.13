#pragma once

#include "marchee.h"

//  f32 -> c16 -> c8 -> c4 -> c2 -> c1

#define F 32
#define P 7
#define N N_FLTR
#define C 6

//	C = 4
//	Cadrans(7x7) = 4 + 2 + 1 = F - 1
//	Filtres( 8 ) = 8         = F
typedef struct {
	ema_int lignes[ F ];
	float    poids[(F-1) * P*P];	//31 = 16+8+4+2+1
	float   filtre[( F ) *  N ];
	float  dif_flt[( F ) *  N ];	//m256 aura besoin d'un 0 en trop pour la 8-eme place
} Mdl_t;	//	f(32) -> cad(16) -> cad(8) -> cad(4) -> cad(2) -> cad(1)

//	Mem
Mdl_t * cree_mdl();
Mdl_t * copier_mdl(Mdl_t * mdl);
void preparer_mdl(Mdl_t * mdl);
void liberer_mdl(Mdl_t * mdl);

void mdl_verif(const Mdl_t * mdl);

//	Alea
void filtre_aleatoire(Mdl_t * mdl, uint i);
void cadran_aleatoire(Mdl_t * mdl, uint i);
void mutation_cadran(Mdl_t * mdl, uint c, float x, float y, float signe);
void mutation_filtre(Mdl_t * mdl, uint i);

//	Plume
void plume_mdl(const Mdl_t * mdl);

//	Ecrire
void ecrire_mdl(const Mdl_t * mdl, const char * fichier);
Mdl_t * lire_mdl(const char * fichier);

//	Prediction
float      prediction_mdl_f(Mdl_t * mdl);
float prediction_mdl_f_simd(Mdl_t * mdl);

#define _168H (168*3)
float              gain(Mdl_t * mdl, float USDT, float LEVIER);
float        prediction(Mdl_t * mdl, uint depart, uint fin);
float  gain_168H_levier(Mdl_t * mdl, float USDT, float LEVIER);