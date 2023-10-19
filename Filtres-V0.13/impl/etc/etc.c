#include "etc.h"

const uint factoriel[13] = { 1, 1, 2, 6, 24, 120, 720, 5040, 40320, 
                                    362880, 3628800, 39916800, 479001600 };

float rnd() {
	return (float)(rand()%1000)/1000.0;	//rand()%100 pour avoire 1.0
};

inline float ___exp(register float x)  // cubic spline approximation
{
    union { float f; int i; } reinterpreter;

    reinterpreter.i = (int)(12102203.0f*x) + 127*(1 << 23);
    int m = (reinterpreter.i >> 7) & 0xFFFF;  // copy mantissa
    // empirical values for small maximum relative error (8.34e-5):
    reinterpreter.i +=
         ((((((((1277*m) >> 14) + 14825)*m) >> 14) - 79749)*m) >> 11) - 626;
    return reinterpreter.f;
}

inline float ___gauss(register float x) {return ___exp(-x*x);};
inline float ___d_gauss(register float x) {return -2*x*___gauss(x);};

inline float ___logistique(register float x) {return 1.0/(1.0+___exp(-x));};
inline float ___d_logistique(register float x) {return ___logistique(x)*(1 - ___logistique(x));};

inline float ___tanh(register float x) {return tanhf(x);};
inline float ___d_tanh(register float x) {return 1 - powf(___tanh(x), 2);};


inline float signe(register float x) {return (x>=0 ? 1:-1);};

void gnuplot(float * arr, uint len, char * titre) {
	char buff[200];
	//
	FILE * fp = fopen("gnuplot_dat.dat", "w");
	//
	for (uint i=0; i < len; i++) {
		snprintf(buff, 100, "%i ", i);
		fputs(buff, fp);
		//
		snprintf(buff, 100, "%f\n", arr[i]);
		fputs(buff, fp);
	}
	fclose(fp);
	//
	snprintf(
		buff,
		200,
		"gnuplot -p -e \"set title \'%s\'; plot 'gnuplot_dat.dat' w lp\"",
		titre);
	//
	assert(!system(buff));
	//
	assert(!system("rm gnuplot_dat.dat"));
};

uint* cpyuint(uint * arr, uint len) {
	uint * ret = malloc(sizeof(uint) * len);
	memcpy(ret, arr, sizeof(uint) * len);
	return ret;
}

float* allouer_flotants(uint nb) {
	return malloc(sizeof(float) * nb);
}

uint u_max(uint * x, uint len) {
	uint _max=x[0];
	FOR(1,i,len) {
		if (x[i] > _max)
			_max = x[i];
	};
	return _max;
}

PAS_OPTIMISER()
void normer(float * arr, uint n) {
	float max=arr[0], min=arr[0];
	FOR(1, i, n) {
		if (arr[i] > max) max = arr[i];
		if (arr[i] < min) min = arr[i];
		//printf("%f ", arr[i]);
	}
	//printf("\n");
	FOR(0, i , n) {
	//	printf("%f %f %f\n", arr[i], min, max);
		arr[i] = (arr[i]-min)/(max-min);
		assert(arr[i]>=0);
	}
}

PAS_OPTIMISER()
void prete(float * arr, uint n) {
	float s = rnd()-.5;
	float d = rnd()-.5;
	FOR(0, i, n) {
		if (i%1==0) s += rnd()-.5;
		if (i%2==0) d += rnd()-.5;
		arr[i] = s/2 + d/2;
	}
	normer(arr, n);
};

PAS_OPTIMISER()
void lisser(float * arr, uint n, float A) {
	FOR(0, i, n) {
		arr[i] = A*roundf(arr[i]/A);
	}
};

/*void __5050(float * arr, uint n) {
	assert(n == 49);
};*/

PAS_OPTIMISER()
float lire_flotant(char * fichier) {
	FILE * fp = fopen(fichier, "rb");
	float res;
	(void)!fread(&res, sizeof(float), 1, fp);
	fclose(fp);
	return res;
}

PAS_OPTIMISER()
void ecrire_flotant(float a , char * fichier) {
	FILE * fp = fopen(fichier, "wb");
	(void)!fwrite(&a, sizeof(float), 1, fp);
	fclose(fp);
};

PAS_OPTIMISER()
void ecrire_uint(uint a, char * fichier) {
	FILE * fp = fopen(fichier, "wb");
	(void)!fwrite(&a, sizeof(uint), 1, fp);
	fclose(fp);
};

PAS_OPTIMISER()
uint lire_uint(char * fichier) {
	FILE * fp = fopen(fichier, "rb");
	uint res;
	(void)!fread(&res, sizeof(uint), 1, fp);
	fclose(fp);
	return res;
};

PAS_OPTIMISER()
void lire_N_uint(char * fichier, uint * _uint, uint _N) {
	FILE * fp = fopen(fichier, "rb");
	(void)!fread(_uint, sizeof(uint), _, fp);
	fclose(fp);
};