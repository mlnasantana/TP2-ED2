#ifndef ANALISEEXPERIMENTAL_H
#define ANALISEEXPERIMENTAL_H

typedef struct {
    int numTransferenciasLeitura;
    int numTransferenciasEscrita;
    int numComparacoes;
    double tempoExecucao;
} AnaliseExperimental;

// Apenas a declaração da função que está no .c
void imprimeAnalise(AnaliseExperimental *analise);

#endif