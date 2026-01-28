#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#include "geradorDeArquivos.h"
#include "quicksortExt.h"
#include "intercalacao.h"
#include "analiseExperimental.h" // 1. Certifique-se de que este include está aqui

int main(int argc, char *argv[]){

    if (argc < 4 || argc > 5) {
        printf("Uso: %s <método> <quantidade> <situação> [-P]\n", argv[0]);
        return 1;
    }

    int metodo = atoi(argv[1]); 
    int quantReg = atoi(argv[2]);
    int situacao = atoi(argv[3]);
    bool P = false; 

    if(argc == 5 && ((strcmp(argv[4], "-P") == 0) || (strcmp(argv[4], "-p") == 0))) P = true;

    if (!gerarArquivo(situacao, quantReg)) {
        printf("Erro ao gerar o arquivo binário.\n");
        return 1;
    }
    
    char nomeArqBin[100], nomeArqTxt[100];
    sprintf(nomeArqBin, "dados_%d_%d.dat", situacao, quantReg);
    sprintf(nomeArqTxt, "dados_%d_%d.txt", situacao, quantReg);
    
    if (P) imprimirArquivoBinario(nomeArqBin);

    FILE *arqBin = fopen(nomeArqBin, "r+b");
    if (arqBin == NULL) {
        printf("Erro ao abrir arquivo para ordenação\n");
        return 1;
    }

    // 2. DECLARAÇÃO FORA E ANTES DO SWITCH (Isso define o identificador)
    AnaliseExperimental analise;
    analise.numTransferenciasLeitura = 0;
    analise.numTransferenciasEscrita = 0;
    analise.numComparacoes = 0;
    analise.tempoExecucao = 0;

    clock_t inicio, fim;

    // 3. O SWITCH APENAS USA O QUE JÁ FOI DEFINIDO
    switch (metodo){
        case 1:
            printf("Iniciando Método 1...\n");
            inicio = clock();
            metodo1(arqBin, quantReg, &analise); 
            fim = clock();
            break;
        case 2:
            printf("Iniciando Método 2...\n");
            inicio = clock();
            metodo2(arqBin, quantReg, &analise);
            fim = clock();
            break;
        case 3:
            printf("Iniciando Quicksort Externo...\n");
            inicio = clock();
            // Note que passamos &analise como último argumento
            quicksortExterno(&arqBin, &arqBin, &arqBin, 1, quantReg, &analise);
            fim = clock();
            break;
        default:
            printf("Método inválido.\n");
            fclose(arqBin);
            return 1;
    }

    analise.tempoExecucao = (double)(fim - inicio) / CLOCKS_PER_SEC;
    
    // Imprime o relatório final
    imprimeAnalise(&analise);

    if (!binarioParaTXT(nomeArqBin, nomeArqTxt)) printf("Erro ao converter para TXT.\n");
    if (P) imprimirArquivoTXT(nomeArqTxt);

    fclose(arqBin);
    return 0;
}