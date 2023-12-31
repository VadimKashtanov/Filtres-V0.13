#include "mdl.h"

float          prixs[           PRIXS] = {};
float            ema[EMA_INTS * PRIXS] = {};
float     normalisee[EMA_INTS * PRIXS * N_FLTR] = {};
float dif_normalisee[EMA_INTS * PRIXS * N_FLTR] = {};

ema_int ema_ints[EMA_INTS] = {
	{0,  1,     1},
	{1,  2,     2},
	{2,  4,     4},
	{3,  6,     6},
	{4,  10,    10},
	{5,  20,    20},
	{6,  50,    50},
	{7,  100,   100},
	{8,  200,   200},
	{9,  500,   500},
    {10, 1000,  1000}
};

void charger_les_prixs() {
	//	extern float      prixs[         PRIXS         ];
	FILE * fp = fopen("prixs/prixs.bin", "rb");
	ASSERT(fp != 0);
	uint __PRIXS;
	(void)!fread(&__PRIXS, sizeof(uint), 1, fp);
	ASSERT(__PRIXS == PRIXS);
	(void)!fread(prixs, sizeof(float), PRIXS, fp);
	fclose(fp);


	//	extern float        ema[EMA_INTS][    PRIXS    ];
	float k[EMA_INTS];
	float _k[EMA_INTS];
	for (uint i=0; i < EMA_INTS; i++) {
		k[i] = 1.0/(1.0 + (float)ema_ints[i].ema);
		_k[i] = 1.0 - k[i];
		ema[i*PRIXS+0] = prixs[0];

		assert(ema_ints[i].interv <= MAX_INTERVALLE);
	}
	//
	for (uint i=1; i < PRIXS; i++) {
		for (uint j=0; j < EMA_INTS; j++) {
			ema[j*PRIXS+i] = prixs[i]*k[j] + ema[j*PRIXS + i-1]*_k[j];
		};
	};


	//	extern float normalisee[EMA_INTS][PRIXS][N_FLTR];
	float _max, _min;
	FOR(DEPART, t, FIN) {
		FOR(0, e, EMA_INTS) {
			_max = ema[e*PRIXS + t-0*ema_ints[e].interv];
			_min = ema[e*PRIXS + t-0*ema_ints[e].interv];
			FOR(1, i, N_FLTR) {
				if (_max < ema[e*PRIXS + t-i*ema_ints[e].interv])
					_max = ema[e*PRIXS + t-i*ema_ints[e].interv];
				if (_min > ema[e*PRIXS + t-i*ema_ints[e].interv])
					_min = ema[e*PRIXS + t-i*ema_ints[e].interv];
			}
			FOR(0, i, N_FLTR) {
				normalisee[e*PRIXS*N_FLTR+t*N_FLTR+i] = (ema[e*PRIXS+t-i*ema_ints[e].interv]-_min)/(_max-_min);
			}
		};
	};

	FOR(DEPART, t, FIN) {
		FOR(0, e, EMA_INTS) {
			FOR(0, i, N_FLTR-1)
				dif_normalisee[e*PRIXS*N_FLTR+t*N_FLTR+i] = normalisee[e*PRIXS*N_FLTR+t*N_FLTR+i+1]-normalisee[e*PRIXS*N_FLTR+t*N_FLTR+i];
			dif_normalisee[e*PRIXS*N_FLTR+t*N_FLTR+N_FLTR-1] = 0.f;
		}
	}
};
