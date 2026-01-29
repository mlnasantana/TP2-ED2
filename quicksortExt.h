#ifndef QUICKSORTEXT_H
#define QUICKSORTEXT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <float.h>

#include "registro.h"
#include "area.h"
#include "analiseExperimental.h" 

#define TAMAREA 4717050

// As assinaturas devem conter os tipos completos para evitar ambiguidades no compilador
void quicksortExterno(FILE **ArqLi, FILE **ArqEi, FILE **ArqLEs, int Esq, int Dir, AnaliseExperimental *analise);
void LeSup(FILE **ArqLEs, TipoRegistro *UltLido, int *Ls, bool *OndeLer, AnaliseExperimental *analise);
void LeInf(FILE **ArqLi, TipoRegistro *UltLido, int *Li, bool *OndeLer, AnaliseExperimental *analise);
void escreveMax(FILE **ArqLEs, TipoRegistro UltLido, int *Es, AnaliseExperimental *analise);
void escreveMin(FILE **ArqEi, TipoRegistro UltLido, int *Ei, AnaliseExperimental *analise);
void particao(FILE **ArqLi, FILE **ArqEi, FILE **ArqLEs, TipoArea *Area, int Esq, int Dir, int *i, int *j, AnaliseExperimental *analise);

#endif