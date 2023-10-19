#include "mdl.h"

#define A (2.f/7.f)

#define	BORNE0 (-1 + 0*A)
#define BORNE1 (-1 + 1*A)
#define BORNE2 (-1 + 2*A)
#define BORNE3 (-1 + 3*A)
#define BORNE4 (-1 + 4*A)
#define BORNE5 (-1 + 5*A)
#define BORNE6 (-1 + 6*A)
#define BORNE7 (-1 + 7*A)

//	* Il peut y avoire 31168 poids si je veux
//	* Il y a 7 zones, mais autant de poids que je veux
//	N=8 Poids : [-1, -.65, -.3, -.1, +.1, +.3, +.65, +1]

#define BORNES(x, ix) do {	\
	if (x <= BORNE1) ix = 0;\
	else if (x <= BORNE2) ix = 1;\
	else if (x <= BORNE3) ix = 2;\
	else if (x <= BORNE4) ix = 3;\
	else if (x <= BORNE5) ix = 4;\
	else if (x <= BORNE6) ix = 5;\
	else ix = 6;\
} while(0);

static inline float COND7_f(float x0, float x1, float * poid) {
	uint ix0;
	uint ix1;
	BORNES(x0, ix0);
	BORNES(x1, ix1);
	return poid[ix0*7+ix1];
};

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

static float __r[F];//32];

static float __f(Mdl_t * mdl, uint t) {
	//	Les intervalles ont deja ete calculees dans marchee.c
	FOR(0, i, F) {//32) {
		__r[i] = filtre_8(
			normalisee + mdl->lignes[i].ligne*N_FLTR*PRIXS + t*N_FLTR,
			mdl->filtre + i*N_FLTR,

			dif_normalisee + mdl->lignes[i].ligne*(N_FLTR)*PRIXS + t*(N_FLTR),
			mdl->dif_flt + i*(N_FLTR));
	}
	
	//uint s = 0;
	//FOR(0, j, (C-1)) {
	//	FOR(0, i, pow(2, (C-1)-j-1)) {
	//		__r[i] = COND7_f(__r[i*2+0], __r[i*2+1], mdl->poids + s[j]*P*P + i*P*P);
	//	};
		//s += pow(2, (C-1)-j-1);
	//};

	//return __r[0];
	FOR(0, i, 16) {
		__r[i] = COND7_f(__r[i*2+0], __r[i*2+1], mdl->poids + i*P*P);
	}

	FOR(0, i, 8)
		__r[i] = COND7_f(__r[i*2+0], __r[i*2+1], mdl->poids + 16*P*P + i*P*P);

	FOR(0, i, 4)
		__r[i] = COND7_f(__r[i*2+0], __r[i*2+1], mdl->poids + (16+8)*P*P + i*P*P);

	FOR(0, i, 2)
		__r[i] = COND7_f(__r[i*2+0], __r[i*2+1], mdl->poids + (16+8+4)*P*P + i*P*P);

	return COND7_f(__r[0], __r[1], mdl->poids + (16+8+4+2)*P*P);
	//return __r[0];
};

float prediction_mdl_f(Mdl_t * mdl) {
	return prediction(mdl, DEPART, FIN);
};

float prediction(Mdl_t * mdl, uint depart, uint fin) {
	preparer_mdl(mdl);
	float p0, p1;
	float pred = 0;
	uint T = 0;
	FOR(depart, t, fin) {
		p1 = prixs[t+L];
		p0 = prixs[ t ];
		if (signe(__f(mdl, t)) == signe(p1/p0-1.0)) pred += 1.0;
		t += L;
		T++;
	}
	return pred / (float)T;
};

//	=============================================================

float gain(Mdl_t * mdl, float USDT, float LEVIER) {
	preparer_mdl(mdl);
	float p0, p1;
	float u = USDT;
	FOR(DEPART, t, FIN) {
		p1 = prixs[t+L];
		p0 = prixs[ t ];
		u += u * __f(mdl, t) * LEVIER * (p1/p0-1.0);
		t += L;
		if (u < 0) return -USDT;
	}
	return u - USDT;
};

float gain_168H_levier(Mdl_t * mdl, float USDT, float LEVIER) {
	preparer_mdl(mdl);
	float p0, p1;
	float u = USDT;
	FOR(FIN-_168H, t, FIN) {
		p1 = prixs[t+L];
		p0 = prixs[ t ];
		u += u * __f(mdl, t) * LEVIER * (p1/p0-1.0);
		t += L;
		if (u < 0) return -USDT;
	}
	return u - USDT;
};