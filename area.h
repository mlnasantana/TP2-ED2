#ifndef AREA_H
#define AREA_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "registro.h"
#include "analiseExperimental.h" // Adicionado para o tipo AnaliseExperimental

typedef struct {
    TipoRegistro *itens;
    int n; 
    int capacidade; 
} TipoArea;

void criaArea(TipoArea *, int);
void liberaArea(TipoArea *);
bool areaVazia(TipoArea *);
bool areaCheia(TipoArea *);
// Atualizado para receber o ponteiro de análise
void insereOrdenado(TipoArea *, TipoRegistro, AnaliseExperimental *);
TipoRegistro retiraMinimo(TipoArea *);
TipoRegistro retiraMaximo(TipoArea *);

#endif