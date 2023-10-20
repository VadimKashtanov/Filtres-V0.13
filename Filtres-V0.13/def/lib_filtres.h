#pragma once

#include "mdl.h"

#include "main/main2.h"
#include "main/main3_experimentations.h"
#include "main/main4.h"

void   init_libfiltres();
void   info_libfiltres();
void sortie_libfiltres();

Mdl_t * parrallele_selection_mdl(Mdl_t * mdl, uint PROGS, uint T);