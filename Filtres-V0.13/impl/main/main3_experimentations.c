#include "main/main3_experimentations.h"

static void score(Mdl_t * mdl) {
	printf("prediction=%f gainX%i=%f gain_%iHX%i=%f gain_%iHX%i=%f gain_%iHX%i=%f\n",
		prediction_mdl_f(mdl),
		10, gain(mdl, 100, 10),
		_168H, 10, gain_168H_levier(mdl, 100, 10),
		_168H, 25, gain_168H_levier(mdl, 100, 25),
		_168H, 50, gain_168H_levier(mdl, 100, 50)
	);
};

#define A (2.f/7.f)

#define	BORNE0 (-1 + 0*A)
#define BORNE1 (-1 + 1*A)
#define BORNE2 (-1 + 2*A)
#define BORNE3 (-1 + 3*A)
#define BORNE4 (-1 + 4*A)
#define BORNE5 (-1 + 5*A)
#define BORNE6 (-1 + 6*A)
#define BORNE7 (-1 + 7*A)

#define BORNES(x, ix) do {	\
	if (x <= BORNE1) ix = 0;\
	else if (x <= BORNE2) ix = 1;\
	else if (x <= BORNE3) ix = 2;\
	else if (x <= BORNE4) ix = 3;\
	else if (x <= BORNE5) ix = 4;\
	else if (x <= BORNE6) ix = 5;\
	else ix = 6;\
} while(0);

//static float ilfallait[(F-1)*P*P] = {0};

static inline float COND7_f(float x0, float x1, float * poid) {
	uint ix0;
	uint ix1;
	BORNES(x0, ix0);
	BORNES(x1, ix1);
	return poid[ix0*7+ix1];
};
/*
static inline float COND7_f_ilfallait_poid(		//pour que poid[x,y] change
	float x0, float x1, float * poid,
	float err)
{
	uint ix0;
	uint ix1;
	BORNES(x0, ix0);
	BORNES(x1, ix1);
	//return poid[ix0*7+ix1];
	ilfallait[ix0*7 + ix1] += err;
};

static inline float COND7_f_ilfallait_coords(	//pour que les filtres x,y changent
	float x0, float x1, float * poid,
	float err,
	float * errx0, float * errx1)
{
	uint ix0;
	uint ix1;
	BORNES(x0, ix0);
	BORNES(x1, ix1);
	float p = poid[ix0*7+ix1];
	//
	int __x, __y;
	//
	for (int __x=-1; __x < 2; __x++) {
		for (int __y=-1; __y < 2; __y++) {
			if (ix0 + __y >= 0 && ix0 + __y < P && ix1 + __x >= 0 && ix1 + __x < P) {
				if (signe(poid[(ix0+__y)*7 + (ix1+__x)] - p) == signe(err)) {
					errx0[0] = __y;
					errx1[0] = __x;
				}
			};
		};
	};
};
*/
//	===========================================================

static float filtre_8(
	float *     x_norm, float *     f,
	float * x_norm_dif, float * f_dif)
{
		//avant : sqrtf(1 + fabs(x-f))
	float s = (fabs(x_norm[0]-f[0])) \
		+ (fabs(x_norm[1]-f[1])) \
		+ (fabs(x_norm[2]-f[2])) \
		+ (fabs(x_norm[3]-f[3])) \
		+ (fabs(x_norm[4]-f[4])) \
		+ (fabs(x_norm[5]-f[5])) \
		+ (fabs(x_norm[6]-f[6])) \
		+ (fabs(x_norm[7]-f[7]));
	s = s/8;// - 1;
	float d = powf(1.f + fabs(x_norm_dif[0]-f_dif[0]), 2) \
		+ powf(1.f + fabs(x_norm_dif[1]-f_dif[1]), 2) \
		+ powf(1.f + fabs(x_norm_dif[2]-f_dif[2]), 2) \
		+ powf(1.f + fabs(x_norm_dif[3]-f_dif[3]), 2) \
		+ powf(1.f + fabs(x_norm_dif[4]-f_dif[4]), 2) \
		+ powf(1.f + fabs(x_norm_dif[5]-f_dif[5]), 2) \
		+ powf(1.f + fabs(x_norm_dif[6]-f_dif[6]), 2);
	d = d/7-1;
	return 2*___exp(-s*s -d*d)-1;	//[0;1] -> [-1;1]
};

static float __r32[F];
static float __r16[F];
static float __r8[F];
static float __r4[F];
static float __r2[F];
static float __r1[F];
/*
static float __ilfa1[F]={0};
static float __ilfa2[F]={0};
static float __ilfa4[F]={0};
static float __ilfa8[F]={0};
static float __ilfa16[F]={0};
static float __ilfa32[F]={0};*/

static float __f(Mdl_t * mdl, uint t) {
	//	Les intervalles ont deja ete calculees dans marchee.c
	FOR(0, i, F) {//32) {
		__r32[i] = filtre_8(
			normalisee + mdl->lignes[i].ligne*N_FLTR*PRIXS + t*N_FLTR,
			mdl->filtre + i*N_FLTR,

			dif_normalisee + mdl->lignes[i].ligne*(N_FLTR)*PRIXS + t*(N_FLTR),
			mdl->dif_flt + i*(N_FLTR));
	}
	
	FOR(0, i, 16) {
		__r16[i] = COND7_f(__r32[i*2+0], __r32[i*2+1], mdl->poids + i*P*P);
	}

	FOR(0, i, 8)
		__r8[i] = COND7_f(__r16[i*2+0], __r16[i*2+1], mdl->poids + 16*P*P + i*P*P);

	FOR(0, i, 4)
		__r4[i] = COND7_f(__r8[i*2+0], __r8[i*2+1], mdl->poids + (16+8)*P*P + i*P*P);

	FOR(0, i, 2)
		__r2[i] = COND7_f(__r4[i*2+0], __r4[i*2+1], mdl->poids + (16+8+4)*P*P + i*P*P);

	FOR(0, i, 1)
		__r1[i] = COND7_f(__r2[i*1+0], __r2[i*1+1], mdl->poids + (16+8+4+2)*P*P + i*P*P);

	//	Il fallait
/*	float _signe = signe(prixs[t+L]/prixs[t]-1.0);

	if (_signe != signe(__r1[0])) {
		//	__r1
		FOR(0, i, 1) {
			__ilfa1[i] = -_signe;
			if (rand()%2==0) {
				COND7_f_ilfallait_poid(__r2[i*1+0], __r2[i*1+1], mdl->poids + (16+8+4+2)*P*P + i*P*P, __ilfa1[i]);
			} else {
				COND7_f_ilfallait_coords(__r2[i*1+0], __r2[i*1+1], mdl->poids + (16+8+4+2)*P*P + i*P*P,
					__ilfa2+i*1+0, __ilfa2+i*1+1
				);
			}
		}

		//	__r2
	};
*/
	return __r1[0];
};

PAS_OPTIMISER()
void main3_ilfallait() {
	//	--- Entree ---
	Mdl_t * mdl = cree_mdl();
	mdl_verif(mdl);
	score(mdl);

	preparer_mdl(mdl);
	float p0, p1;
	float pred = 0;
	uint T = 0;
	FOR(DEPART, t, FIN) {
		p1 = prixs[t+L];
		p0 = prixs[ t ];
		if (signe(__f(mdl, t)) == signe(p1/p0-1.0)) {
			pred += 1.0;
		};
		t += L;
		T++;
	}
	printf("%f\n", pred / (float)T);

	//	--- Sortie ---
	liberer_mdl(mdl);
};