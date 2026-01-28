#include "intercalacao.h"
#include <stdbool.h>

// Estruturas auxiliares p/ gerenciar registros e fitas
typedef struct {
    TipoRegistro reg;
    bool marcado;
} TipoNo;

typedef struct {
    FILE *f;
    TipoRegistro reg;
    bool vazia;
    bool blocoAcabou;
} ControleFita;

// Metodo 1: Intercalação Balanceada 2f (20 fitas)
void metodo1(FILE *arqPrincipal, int n, AnaliseExperimental *analise) {
    FILE *fitas[20];
    char nomeFita[30];

    // cria as 20 fitas (10 de entrada, 10 de saida)
    for (int i = 0; i < 20; i++) {
        sprintf(nomeFita, "fita%d.bin", i + 1);
        fitas[i] = fopen(nomeFita, "w+b");
    }

    // PASSO 1: Seleção por Substituição p/ gerar blocos iniciais
    rewind(arqPrincipal);
    TipoNo memoria[10]; // RAM limitada a 10 registros
    int lidos = 0, tamMemoria = 0, fitaAtual = 0;

    // carrega a memoria inicial
    for (int i = 0; i < 10 && lidos < n; i++) {
        if (fread(&memoria[i].reg, sizeof(TipoRegistro), 1, arqPrincipal) == 1) {
            memoria[i].marcado = false;
            lidos++; tamMemoria++;
            analise->numTransferenciasLeitura++;
        }
    }

    while (tamMemoria > 0) {
        int menor = -1;
        // busca o menor registro não marcado na memória
        for (int i = 0; i < tamMemoria; i++) {
            if (!memoria[i].marcado) {
                if (menor == -1 || (analise->numComparacoes++, memoria[i].reg.nota < memoria[menor].reg.nota))
                    menor = i;
            }
        }

        if (menor == -1) { // se todos estão marcados, o bloco na fita atual acabou
            for (int i = 0; i < tamMemoria; i++) memoria[i].marcado = false;
            fitaAtual = (fitaAtual + 1) % 10;
            continue;
        }

        // escreve o menor na fita e le o proximo do arquivo principal
        fwrite(&memoria[menor].reg, sizeof(TipoRegistro), 1, fitas[fitaAtual]);
        analise->numTransferenciasEscrita++;
        TipoRegistro ultimo = memoria[menor].reg;

        if (lidos < n && fread(&memoria[menor].reg, sizeof(TipoRegistro), 1, arqPrincipal) == 1) {
            lidos++; analise->numTransferenciasLeitura++;
            analise->numComparacoes++;
            // se o novo for menor q o ultimo escrito, ele entra no proximo bloco (marcado)
            memoria[menor].marcado = (memoria[menor].reg.nota < ultimo.nota);
        } else {
            memoria[menor] = memoria[tamMemoria - 1];
            tamMemoria--;
        }
    }

    // PASSO 2: Intercalação das fitas
    int entradaIdx = 0, saidaIdx = 10;
    bool ordenado = false;

    while (!ordenado) {
        for (int i = 0; i < 10; i++) {
            rewind(fitas[entradaIdx + i]);
            fitas[saidaIdx + i] = freopen(NULL, "w+b", fitas[saidaIdx + i]); // limpa fita de saida
        }

        ControleFita entrada[10];
        for (int i = 0; i < 10; i++) {
            entrada[i].f = fitas[entradaIdx + i];
            entrada[i].vazia = (fread(&entrada[i].reg, sizeof(TipoRegistro), 1, entrada[i].f) != 1);
            if (!entrada[i].vazia) analise->numTransferenciasLeitura++;
            entrada[i].blocoAcabou = false;
        }

        int fitaSaida = 0, blocosNaRodada = 0;
        while (1) {
            bool existeDados = false;
            for (int i = 0; i < 10; i++) if (!entrada[i].vazia) existeDados = true;
            if (!existeDados) break;

            // intercala os blocos das 10 fitas de entrada
            while (1) {
                int menor = -1;
                for (int i = 0; i < 10; i++) {
                    if (!entrada[i].vazia && !entrada[i].blocoAcabou) {
                        if (menor == -1 || (analise->numComparacoes++, entrada[i].reg.nota < entrada[menor].reg.nota))
                            menor = i;
                    }
                }
                if (menor == -1) break;

                fwrite(&entrada[menor].reg, sizeof(TipoRegistro), 1, fitas[saidaIdx + fitaSaida]);
                analise->numTransferenciasEscrita++;
                TipoRegistro anterior = entrada[menor].reg;

                if (fread(&entrada[menor].reg, sizeof(TipoRegistro), 1, entrada[menor].f) == 1) {
                    analise->numTransferenciasLeitura++;
                    analise->numComparacoes++;
                    if (entrada[menor].reg.nota < anterior.nota) entrada[menor].blocoAcabou = true;
                } else {
                    entrada[menor].vazia = true;
                    entrada[menor].blocoAcabou = true;
                }
            }
            fitaSaida = (fitaSaida + 1) % 10;
            blocosNaRodada++;
            for (int i = 0; i < 10; i++) if (!entrada[i].vazia) entrada[i].blocoAcabou = false;
        }

        // se sobrou apenas 1 bloco, o arquivo está pronto
        if (blocosNaRodada <= 1) {
            ordenado = true;
            int fitaVencedora = saidaIdx; 
            rewind(fitas[fitaVencedora]);
            rewind(arqPrincipal);
            
            TipoRegistro aux;
            while (fread(&aux, sizeof(TipoRegistro), 1, fitas[fitaVencedora]) == 1) {
                fwrite(&aux, sizeof(TipoRegistro), 1, arqPrincipal);
                analise->numTransferenciasLeitura++; analise->numTransferenciasEscrita++;
            }
        } else {
            int tmp = entradaIdx; // inverte entrada e saida p/ a prox rodada
            entradaIdx = saidaIdx;
            saidaIdx = tmp;
        }
    }

    for (int i = 0; i < 20; i++) fclose(fitas[i]);
}

// Metodo 2: Intercalação Balanceada f+1 (usa 19 de entrada e 1 de saida)
void metodo2(FILE *arqPrincipal, int n, AnaliseExperimental *analise) {
    FILE *fitas[20];
    char nomeFita[30];

    for (int i = 0; i < 20; i++) {
        sprintf(nomeFita, "fita%d_m2.bin", i + 1);
        fitas[i] = fopen(nomeFita, "w+b");
    }

    // PASSO 1: Seleção por Substituição (usa 19 espaços de memória aqui)
    rewind(arqPrincipal);
    TipoNo memoria[19]; 
    int lidos = 0, tamMemoria = 0, fitaAtual = 0;

    for (int i = 0; i < 19 && lidos < n; i++) {
        if (fread(&memoria[i].reg, sizeof(TipoRegistro), 1, arqPrincipal) == 1) {
            memoria[i].marcado = false;
            lidos++; tamMemoria++;
            analise->numTransferenciasLeitura++;
        }
    }

    while (tamMemoria > 0) {
        int menor = -1;
        for (int i = 0; i < tamMemoria; i++) {
            if (!memoria[i].marcado) {
                if (menor == -1 || (analise->numComparacoes++, memoria[i].reg.nota < memoria[menor].reg.nota))
                    menor = i;
            }
        }

        if (menor == -1) { 
            for (int i = 0; i < tamMemoria; i++) memoria[i].marcado = false;
            fitaAtual = (fitaAtual + 1) % 19; 
            continue;
        }

        fwrite(&memoria[menor].reg, sizeof(TipoRegistro), 1, fitas[fitaAtual]);
        analise->numTransferenciasEscrita++;
        TipoRegistro ultimo = memoria[menor].reg;

        if (lidos < n && fread(&memoria[menor].reg, sizeof(TipoRegistro), 1, arqPrincipal) == 1) {
            lidos++; analise->numTransferenciasLeitura++;
            analise->numComparacoes++;
            memoria[menor].marcado = (memoria[menor].reg.nota < ultimo.nota);
        } else {
            memoria[menor] = memoria[tamMemoria - 1];
            tamMemoria--;
        }
    }

    // PASSO 2: Intercalação f+1
    int fitaSaidaIdx = 19; 
    bool ordenado = false;

    while (!ordenado) {
        ControleFita entrada[19];
        int fitasComDados = 0;

        for (int i = 0, k = 0; i < 20; i++) {
            if (i == fitaSaidaIdx) {
                sprintf(nomeFita, "fita%d_m2.bin", i + 1);
                fitas[i] = freopen(nomeFita, "w+b", fitas[i]);
                continue;
            }
            rewind(fitas[i]);
            entrada[k].f = fitas[i];
            entrada[k].vazia = (fread(&entrada[k].reg, sizeof(TipoRegistro), 1, entrada[k].f) != 1);
            if (!entrada[k].vazia) {
                analise->numTransferenciasLeitura++;
                fitasComDados++;
            }
            entrada[k].blocoAcabou = false;
            k++;
        }

        if (fitasComDados <= 1) { // se so sobrou uma fita com dados, acabou
            ordenado = true;
            int fitaVencedora = (fitaSaidaIdx == 0) ? 1 : 0; 
            for(int i=0; i<20; i++) {
                rewind(fitas[i]);
                TipoRegistro lixo;
                if(fread(&lixo, sizeof(TipoRegistro), 1, fitas[i]) == 1) { fitaVencedora = i; break; }
            }

            rewind(fitas[fitaVencedora]);
            rewind(arqPrincipal);
            TipoRegistro aux;
            while (fread(&aux, sizeof(TipoRegistro), 1, fitas[fitaVencedora]) == 1) {
                fwrite(&aux, sizeof(TipoRegistro), 1, arqPrincipal);
                analise->numTransferenciasLeitura++;
                analise->numTransferenciasEscrita++;
            }
            fflush(arqPrincipal);
            break;
        }

        int blocosNaRodada = 0;
        while (1) {
            bool existeDadosNoBloco = false;
            for (int i = 0; i < 19; i++) if (!entrada[i].vazia) existeDadosNoBloco = true;
            if (!existeDadosNoBloco) break;

            while (1) {
                int menor = -1;
                for (int i = 0; i < 19; i++) {
                    if (!entrada[i].vazia && !entrada[i].blocoAcabou) {
                        if (menor == -1 || (analise->numComparacoes++, entrada[i].reg.nota < entrada[menor].reg.nota))
                            menor = i;
                    }
                }
                if (menor == -1) break;

                fwrite(&entrada[menor].reg, sizeof(TipoRegistro), 1, fitas[fitaSaidaIdx]);
                analise->numTransferenciasEscrita++;
                TipoRegistro ant = entrada[menor].reg;

                if (fread(&entrada[menor].reg, sizeof(TipoRegistro), 1, entrada[menor].f) == 1) {
                    analise->numTransferenciasLeitura++;
                    analise->numComparacoes++;
                    if (entrada[menor].reg.nota < ant.nota) entrada[menor].blocoAcabou = true;
                } else {
                    entrada[menor].vazia = entrada[menor].blocoAcabou = true;
                }
            }
            
            blocosNaRodada++;
            for (int i = 0; i < 19; i++) if (!entrada[i].vazia) entrada[i].blocoAcabou = false;
        }

        if (blocosNaRodada <= 1) {
            ordenado = true;
            rewind(fitas[fitaSaidaIdx]);
            rewind(arqPrincipal);
            TipoRegistro aux;
            while (fread(&aux, sizeof(TipoRegistro), 1, fitas[fitaSaidaIdx]) == 1) {
                fwrite(&aux, sizeof(TipoRegistro), 1, arqPrincipal);
                analise->numTransferenciasLeitura++;
                analise->numTransferenciasEscrita++;
            }
            fflush(arqPrincipal);
        } else {
            // define a proxima fita de saida (uma que esteja vazia)
            for (int i = 0; i < 20; i++) {
                rewind(fitas[i]);
                TipoRegistro aux;
                if (fread(&aux, sizeof(TipoRegistro), 1, fitas[i]) != 1) {
                    fitaSaidaIdx = i;
                    break;
                }
            }
        }
    }

    for (int i = 0; i < 20; i++) fclose(fitas[i]);
}