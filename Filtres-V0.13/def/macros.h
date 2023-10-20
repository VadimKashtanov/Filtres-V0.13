#pragma once

#include <stdio.h>
#include <stdatomic.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <assert.h>
#include <omp.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
#include <time.h> 
#include <stdarg.h>
#include <inttypes.h>
#include <unistd.h>
#include <stdalign.h>
#include <pthread.h>
#include <fcntl.h>
//
#include <x86intrin.h>
 
//	===== Clarete de Code =====
#define FOR(d,i,N) for (uint i=d; i < N; i++)
#define ptr printf
#define RETRO_FOR(i,N,d) for (int i=N-1; i >= d; i--)

//	===== Eternels Arguments variadiques =====
#define OK(str, ...) printf("[\033[35;1m*\033[0m]:\033[96m%s:(%d)\033[32m: " str "\033[0m\n", __FILE__, __LINE__, ##__VA_ARGS__);
#define MSG(str, ...) printf("\033[35;1m -> \033[0m \033[96m%s:(%d)\033[35m: " str "\033[0m\n", __FILE__, __LINE__, ##__VA_ARGS__);
#define ERR(str, ...) do {printf("[\033[30;101mError\033[0m]:\033[96m%s:(%d)\033[30m: " str "\033[0m\n", __FILE__, __LINE__, ##__VA_ARGS__);raise(SIGINT);} while (0);
#define ASSERT(ass) do {if (!(ass)) ERR("Assertion %s Negative !", #ass);}while(0);
#define TODO() ERR("A ecrire/reecrire.");

//	=== Couleures ===
#define FONT_VERT(str, ...) printf("\033[42m" str "\033[0m", ##__VA_ARGS__);
#define FONT_ROUGE(str, ...) printf("\033[41m" str "\033[0m", ##__VA_ARGS__);
#define FONT_JAUNE(str, ...) printf("\033[43m" str "\033[0m", ##__VA_ARGS__);

#define ROUGE(str, ...) printf("\033[91m" str "\033[0m", ##__VA_ARGS__);
#define VERT(str, ...) printf("\033[92m" str "\033[0m", ##__VA_ARGS__);
#define JAUNE(str, ...) printf("\033[93m" str "\033[0m", ##__VA_ARGS__);

#define JAUNE_GRAS(str, ...) printf("\033[1;93m" str "\033[0m", ##__VA_ARGS__);

//  ==== Mesure du temps ====
#define INIT_CHRONO(a) time_t a##_chrono;
#define DEPART_CHRONO(a) a##_chrono = clock();
#define VALEUR_CHRONO(a) (((float)(clock()-a##_chrono))/CLOCKS_PER_SEC)
#define CHRONO(a) printf("Chrono " #a " : %f\n", VALEUR_CHRONO(a));

#define MESURER(f) do {INIT_CHRONO(mesure);DEPART_CHRONO(mesure);f;printf("Estimation temps ~= %f sec\n", VALEUR_CHRONO(mesure));} while(0);

//  ==== Barre de Progression ====

#define PBSTR "##############################"
#define PBWIDTH 30
#define INIT_BARRE(a) INIT_CHRONO(a##_barre);
#define DEPART_BARRE(a) DEPART_CHRONO(a##_barre);
#define PROGRESSION(a, p) do {uint val = (uint) ((p)* 100);uint lpad = (uint)((p) * PBWIDTH);uint rpad = PBWIDTH - lpad; uint s = (uint)roundf((float)VALEUR_CHRONO(a##_barre)*(1-(p))/(p)); uint m = (s - (s%60))/60; s %= 60; printf("\r%3d%% [%.*s%*s] t ~= %i mins et %i secs", val, lpad, PBSTR, rpad, "", m, s); fflush(stdout);} while(0);

//	===== Plumation de courbes =====

#define UNE_COURBE(nom) float * nom=malloc(sizeof(float)*2); uint nom##taille_reele=2; uint nom##_l=0;
#define EXPANDRE_LA_COURBE(nom) do { nom##taille_reele*=2; nom = realloc(nom, sizeof(float)*nom##taille_reele); } while(0);
#define ASSIGNER_VALEUR(nom, val) do {nom[nom##_l++] = val; } while(0);
#define SUIVIE_COURBE(nom, val) do {if (nom##_l < nom##taille_reele) {ASSIGNER_VALEUR(nom, val)} else {EXPANDRE_LA_COURBE(nom); ASSIGNER_VALEUR(nom, val);} } while(0);
#define PLUMER_LA_COURBE(nom) gnuplot(nom, nom##_l, " Courbe de "#nom);
#define LIBERER_LA_COURBE(nom) free(nom);

//  =================================

#define FACT(n) ((n) <= 1 ? 1 : \
                 (n) == 2 ? 2 : \
                 (n) == 3 ? 6 : \
                 (n) == 4 ? 24 : \
                 (n) == 5 ? 120 : \
                 (n) == 6 ? 720 : \
                 (n) == 7 ? 5040 : \
                 (n) == 8 ? 40320 : \
                 (n) == 9 ? 362880 : \
                 (n) == 10 ? 3628800 : \
                 (n) == 11 ? 39916800 : \
                 (n) == 12 ? 479001600 : (1 << 32))

#define MAX2(a,b) ((a) > (b) ? a : b)
#define MAX3(a,b,c) (MAX2(c, MAX2(a,b)))
#define MAX4(a,b,c,d) (MAX2(d, MAX3(a,b,c)))

#define MIN2(a,b) ((a) < (b) ? a : b)
#define MIN3(a,b,c) (MIN2(c, MIN2(a,b)))
#define MIN4(a,b,c,d) (MIN2(d, MIN3(a,b,c)))

#define ISNG() (int)(rand()%2==0 ? 1 : -1)

#define RAND(A,B) (A+rand()%(B-A+1))

//  ===================================

#define PAS_OPTIMISER() __attribute__ ((optimize(0)))

//  ===================================

#define plume_m256(vec) do {float result[8];_mm256_storeu_ps(result, vec);for (int i = 0; i < 8; i++) {printf("%f ", result[i]);}printf("\n");} while(0);
#define plume_m128(vec) do {float result[4];_mm_storeu_ps(result, vec);for (int i = 0; i < 4; i++) {printf("%f ", result[i]);};printf("\n");} while(0);