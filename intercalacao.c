#include "intercalacao.h"
#include <stdbool.h>

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

void metodo1(FILE *arqPrincipal, int n, AnaliseExperimental *analise) {
    FILE *fitas[20];
    char nomeFita[30];

    // Inicialização: Cria as 20 fitas [cite: 16]
    for (int i = 0; i < 20; i++) {
        sprintf(nomeFita, "fita%d.bin", i + 1);
        fitas[i] = fopen(nomeFita, "w+b");
    }

    // ========================================================
    // PASSO 1: SELEÇÃO POR SUBSTITUIÇÃO
    // ========================================================
    rewind(arqPrincipal);
    TipoNo memoria[10]; // Limite de 10 registros [cite: 16]
    int lidos = 0, tamMemoria = 0, fitaAtual = 0;

    for (int i = 0; i < 10 && lidos < n; i++) {
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

        if (menor == -1) { // Bloco atual acabou na fita
            for (int i = 0; i < tamMemoria; i++) memoria[i].marcado = false;
            fitaAtual = (fitaAtual + 1) % 10;
            continue;
        }

        fwrite(&memoria[menor].reg, sizeof(TipoRegistro), 1, fitas[fitaAtual]);
        analise->numTransferenciasEscrita++;
        TipoRegistro ultimo = memoria[menor].reg;

        if (lidos < n && fread(&memoria[menor].reg, sizeof(TipoRegistro), 1, arqPrincipal) == 1) {
            lidos++; analise->numTransferenciasLeitura++;
            analise->numComparacoes++; // Comparação do if abaixo
            memoria[menor].marcado = (memoria[menor].reg.nota < ultimo.nota);
        } else {
            memoria[menor] = memoria[tamMemoria - 1];
            tamMemoria--;
        }
    }

    // ========================================================
    // PASSO 2: INTERCALAÇÃO BALANCEADA (2f)
    // ========================================================
    int entradaIdx = 0, saidaIdx = 10;
    bool ordenado = false;

    while (!ordenado) {
        // 1. Resetar ponteiros de entrada e LIMPAR fitas de saída [cite: 19]
        for (int i = 0; i < 10; i++) {
            rewind(fitas[entradaIdx + i]);
            // freopen zera o arquivo para a nova rodada de escrita
            fitas[saidaIdx + i] = freopen(NULL, "w+b", fitas[saidaIdx + i]);
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

            // Intercalação dos blocos atuais das fitas de entrada
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
                    analise->numComparacoes++; // Comparação do if abaixo
                    if (entrada[menor].reg.nota < anterior.nota) entrada[menor].blocoAcabou = true;
                } else {
                    entrada[menor].vazia = true;
                    entrada[menor].blocoAcabou = true;
                }
            }
            // Fim de um bloco intercalado
            fitaSaida = (fitaSaida + 1) % 10;
            blocosNaRodada++;
            for (int i = 0; i < 10; i++) if (!entrada[i].vazia) entrada[i].blocoAcabou = false;
        }

        if (blocosNaRodada <= 1) {
            ordenado = true;
            // O resultado final está na PRIMEIRA fita de saída da rodada (saidaIdx + 0)
            int fitaVencedora = saidaIdx; 
            rewind(fitas[fitaVencedora]);
            rewind(arqPrincipal);
            
            TipoRegistro aux;
            while (fread(&aux, sizeof(TipoRegistro), 1, fitas[fitaVencedora]) == 1) {
                fwrite(&aux, sizeof(TipoRegistro), 1, arqPrincipal);
                analise->numTransferenciasLeitura++; analise->numTransferenciasEscrita++;
            }
        } else {
            // Inverte entrada/saída
            int tmp = entradaIdx;
            entradaIdx = saidaIdx;
            saidaIdx = tmp;
        }
    }

    for (int i = 0; i < 20; i++) fclose(fitas[i]);
}
// Método 2: Intercalação Balanceada (f+1 fitas)
void metodo2(FILE *arqPrincipal, int n, AnaliseExperimental *analise) {
    FILE *fitas[20];
    char nomeFita[30];

    for (int i = 0; i < 20; i++) {
        sprintf(nomeFita, "fita%d_m2.bin", i + 1);
        fitas[i] = fopen(nomeFita, "w+b");
    }

    // PASSO 1: SELEÇÃO POR SUBSTITUIÇÃO
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

    // PASSO 2: INTERCALAÇÃO BALANCEADA (f+1)
    int fitaSaidaIdx = 19; 
    bool ordenado = false;

    while (!ordenado) {
        ControleFita entrada[19];
        int fitasComDados = 0;

        for (int i = 0, k = 0; i < 20; i++) {
            if (i == fitaSaidaIdx) {
                // Limpa a fita de saída para receber a nova intercalação
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

        if (fitasComDados <= 1) {
            ordenado = true;
            // Acha qual fita restou com os dados
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

        int blocosNaRodada = 0; // <--- AQUI ESTÁ O SEU CONTADOR
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
            
            blocosNaRodada++; // Incrementa cada vez que termina de intercalar um conjunto de blocos
            for (int i = 0; i < 19; i++) if (!entrada[i].vazia) entrada[i].blocoAcabou = false;
        }

        // Se na rodada toda só geramos 1 bloco na fita de saída, está ordenado
        if (blocosNaRodada <= 1) {
            ordenado = true;
            // Copia da fita de saída (que agora é a vencedora) para o principal
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
            // Define qual será a fita de saída para a próxima rodada (uma que esteja vazia)
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