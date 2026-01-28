#include <stdio.h>
#include "analiseExperimental.h"

void imprimeAnalise(AnaliseExperimental *analise){
    
    printf("========================================================\n");
    printf("     ANÁLISE EXPERIMENTAL\n");
    printf("========================================================\n");

    printf("Número de Transferências de Leitura: %d\n", analise->numTransferenciasLeitura);
    printf("Número de Transferências de Escrita: %d\n", analise->numTransferenciasEscrita);
    printf("Número de Comparações: %d\n", analise->numComparacoes);
    printf("Tempo de Execução: %.6f segundos\n", analise->tempoExecucao);

    printf("========================================================\n");
}