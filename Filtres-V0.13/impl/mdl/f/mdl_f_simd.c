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

static const __m128 __m128_1 = {1.f, 1.f, 1.f, 1.f};
static const __m128 __m128_A = {  A,   A,   A,   A};
static __m128 m;
static __m128i mround;
static uint ix0, ix1;

__attribute__ ((optimize(3)))
static inline float COND7_f(float x0, float x1, const float * poid) {
	m = _mm_set_ps(0, 0, x1, x0);
	m = _mm_add_ps(m, __m128_1);
	m = _mm_div_ps(m, __m128_A);
	mround = _mm_cvttps_epi32(m);
	ix0 = (uint)_mm_extract_epi32(mround, 0);
	ix1 = (uint)_mm_extract_epi32(mround, 1);
	//
	return poid[ix0*7+ix1];
};

//	===========================================================
static inline __m256 abs_m256(__m256 input) {
    __m256 sign_mask = _mm256_set1_ps(-0.0f); // Create a vector with all bits set for the sign of float
    return _mm256_andnot_ps(sign_mask, input);
}
static const __m256 __m256_1 = {1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f};

static inline float _mm256_sum(__m256 v) {
	__m256 ymm2 = _mm256_permute2f128_ps(v, v, 1);
	v = _mm256_add_ps(v, ymm2);
	v = _mm256_hadd_ps(v, v);
	v = _mm256_hadd_ps(v, v);
	return _mm256_cvtss_f32(v);
}

static float filtre_8(
	float *     x_norm, float *     f,
	float * x_norm_dif, float * f_dif)
{
	//	S
	__m256 _x = _mm256_loadu_ps(x_norm);
	__m256 _f = _mm256_loadu_ps(f);
	__m256 _s = ((abs_m256(_mm256_sub_ps(_x, _f))));
	float s = _mm256_sum(_s)/8;// - 1;
	
	//	D
	_x = _mm256_loadu_ps(x_norm_dif);
	_f = _mm256_loadu_ps(f_dif);
	__m256 _d = _mm256_add_ps(abs_m256(_mm256_sub_ps(_x, _f)), __m256_1);
	_d = _mm256_mul_ps(_d,_d);
	float d = (_mm256_sum(_d)-1)/7 - 1;

	return 2*___exp(-s*s -d*d)-1;
};

static float __r[32];

static float __f(Mdl_t * mdl, uint t) {
	//	Les intervalles ont deja ete calculees dans marchee.c
	FOR(0, i, F) {//32) {
		__r[i] = filtre_8(
			normalisee + mdl->lignes[i].ligne*N_FLTR*PRIXS + t*N_FLTR,
			mdl->filtre + i*N_FLTR,

			dif_normalisee + mdl->lignes[i].ligne*(N_FLTR)*PRIXS + t*(N_FLTR),
			mdl->dif_flt + i*(N_FLTR));
	}
	
	FOR(0, i, 16) {
		__r[i] = COND7_f(__r[i*2+0], __r[i*2+1], mdl->poids + i*P*P);
	}

	FOR(0, i, 8)
		__r[i] = COND7_f(__r[i*2+0], __r[i*2+1], mdl->poids + 16*P*P + i*P*P);

	FOR(0, i, 4)
		__r[i] = COND7_f(__r[i*2+0], __r[i*2+1], mdl->poids + (16+8)*P*P + i*P*P);

	FOR(0, i, 2)
		__r[i] = COND7_f(__r[i*2+0], __r[i*2+1], mdl->poids + (16+8+4)*P*P + i*P*P);

	//uint s = 0;
	//FOR(0, j, (C-1)) {
	//	FOR(0, i, pow(2, (C-1)-j-1)) {
	//		__r[i] = COND7_f(__r[i*2+0], __r[i*2+1], mdl->poids + s*P*P + i*P*P);
	//	};
	//	s += pow(2, (C-1)-j-1);
	//};

	return /*__r[0];*/COND7_f(__r[0], __r[1], mdl->poids + (16+8+4+2)*P*P);
};

float prediction_mdl_f_simd(Mdl_t * mdl) {
	//ERR("Petite Erreur. Passer en -O0 pour voire que m256 load pas bien f")
	preparer_mdl(mdl);
	float p0, p1;
	float pred = 0;
	uint T = 0;
	FOR(DEPART, t, FIN) {
		p1 = prixs[t+L];
		p0 = prixs[ t ];
		if (signe(__f(mdl, t)) == signe(p1/p0-1.0)) pred += 1.0;
		t += L;
		T++;
	}
	return pred / (float)T;
};