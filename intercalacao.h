#ifndef INTERCALACAO_H
#define INTERCALACAO_H

#include <stdio.h>
#include <stdbool.h>
#include "registro.h"
#include "analiseExperimental.h"

// Método 1: Intercalação balanceada 2f (20 fitas)
void metodo1(FILE *arqPrincipal, int n, AnaliseExperimental *analise);

// Método 2: Intercalação balanceada f+1 (20 fitas, mas 19 de entrada)
void metodo2(FILE *arqPrincipal, int n, AnaliseExperimental *analise);

#endif