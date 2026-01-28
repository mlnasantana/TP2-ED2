#include "quicksortExt.h"
#include "area.h"
#include <float.h>

// função principal do qsort externo: decide a recursão
void quicksortExterno(FILE **ArqLi, FILE **ArqEi, FILE **ArqLEs, int Esq, int Dir, AnaliseExperimental *analise) {
    int i, j;
    if ((Dir - Esq) < 1) return; // caso base: intervalo vazio ou de 1 elemento

    TipoArea area;
    criaArea(&area, TAMAREA); // aloca a memória interna (RAM) p/ os registros
    
    // particiona o arquivo em sub-arquivos menores
    particao(ArqLi, ArqEi, ArqLEs, &area, Esq, Dir, &i, &j, analise);
    liberaArea(&area); // limpa a RAM após particionar

    // recursão protegida: processa primeiro o menor intervalo p/ não estourar a pilha
    if ((i - Esq) < (Dir - j)) {
        if (Esq < i) quicksortExterno(ArqLi, ArqEi, ArqLEs, Esq, i, analise);
        if (j < Dir) quicksortExterno(ArqLi, ArqEi, ArqLEs, j, Dir, analise);
    } else {
        if (j < Dir) quicksortExterno(ArqLi, ArqEi, ArqLEs, j, Dir, analise);
        if (Esq < i) quicksortExterno(ArqLi, ArqEi, ArqLEs, Esq, i, analise);
    }
}

// le do limite superior e traz p/ a RAM
void LeSup(FILE **ArqLEs, TipoRegistro *UltLido, int *Ls, bool *OndeLer, AnaliseExperimental *analise) {
    fseek(*ArqLEs, (*Ls - 1) * sizeof(TipoRegistro), SEEK_SET); // vai p/ o registro Ls
    fread(UltLido, sizeof(TipoRegistro), 1, *ArqLEs);
    analise->numTransferenciasLeitura++; // conta o IO de leitura
    (*Ls)--;
    *OndeLer = false; // alterna a proxima leitura p/ o limite inferior
}

// le do limite inferior e traz p/ a RAM
void LeInf(FILE **ArqLi, TipoRegistro *UltLido, int *Li, bool *OndeLer, AnaliseExperimental *analise) {
    fseek(*ArqLi, (*Li - 1) * sizeof(TipoRegistro), SEEK_SET); // vai p/ o registro Li
    fread(UltLido, sizeof(TipoRegistro), 1, *ArqLi);
    analise->numTransferenciasLeitura++;
    (*Li)++;
    *OndeLer = true; // alterna a proxima leitura p/ o limite superior
}

// escreve o maior elemento no final da partição atual
void escreveMax(FILE **ArqLEs, TipoRegistro UltLido, int *Es, AnaliseExperimental *analise) {
    fseek(*ArqLEs, (*Es - 1) * sizeof(TipoRegistro), SEEK_SET);
    fwrite(&UltLido, sizeof(TipoRegistro), 1, *ArqLEs);
    analise->numTransferenciasEscrita++; // conta o IO de escrita
    (*Es)--;
}

// escreve o menor elemento no inicio da partição atual
void escreveMin(FILE **ArqEi, TipoRegistro UltLido, int *Ei, AnaliseExperimental *analise) {
    fseek(*ArqEi, (*Ei - 1) * sizeof(TipoRegistro), SEEK_SET);
    fwrite(&UltLido, sizeof(TipoRegistro), 1, *ArqEi);
    analise->numTransferenciasEscrita++;
    (*Ei)++;
}

// lógica de partição p/ ordenação externa (pivots Linf e Lsup)
void particao(FILE **ArqLi, FILE **ArqEi, FILE **ArqLEs, TipoArea *Area, int Esq, int Dir, int *i, int *j, AnaliseExperimental *analise) {
    int Li = Esq, Ls = Dir, Es = Dir, Ei = Esq;
    float Linf = -FLT_MAX, Lsup = FLT_MAX;
    bool OndeLer = true;
    TipoRegistro UltLido;

    while (Li <= Ls) { 
        if (!areaCheia(Area)) { // se a RAM tem espaço, carrega dados
            if (OndeLer) LeSup(ArqLEs, &UltLido, &Ls, &OndeLer, analise);
            else LeInf(ArqLi, &UltLido, &Li, &OndeLer, analise);
            insereOrdenado(Area, UltLido, analise); 
            continue;
        }

        // decide qual lado ler p/ manter o equilíbrio do arquivo
        if (Ls == Es) LeSup(ArqLEs, &UltLido, &Ls, &OndeLer, analise);
        else if (Li == Ei) LeInf(ArqLi, &UltLido, &Li, &OndeLer, analise);
        else if (OndeLer) LeSup(ArqLEs, &UltLido, &Ls, &OndeLer, analise);
        else LeInf(ArqLi, &UltLido, &Li, &OndeLer, analise);

        analise->numComparacoes++;
        if (UltLido.nota > Lsup) { // se maior q o pivot sup, vai p/ o fim do disco
            escreveMax(ArqLEs, UltLido, &Es, analise);
        } else {
            analise->numComparacoes++;
            if (UltLido.nota < Linf) { // se menor q o pivot inf, vai p/ o inicio
                escreveMin(ArqEi, UltLido, &Ei, analise);
            } else {
                // se está no meio, insere na RAM e remove um extremo p/ o disco
                insereOrdenado(Area, UltLido, analise);
                if (OndeLer) {
                    TipoRegistro R = retiraMaximo(Area);
                    escreveMax(ArqLEs, R, &Es, analise);
                    Lsup = R.nota;
                } else {
                    TipoRegistro R = retiraMinimo(Area);
                    escreveMin(ArqEi, R, &Ei, analise);
                    Linf = R.nota;
                }
            }
        }
    }

    // esvazia a RAM p/ as sub-partições no final do processo
    while (!areaVazia(Area)) {
        TipoRegistro R;
        if ((Ei - Esq) < (Dir - Es)) {
            R = retiraMinimo(Area);
            escreveMin(ArqEi, R, &Ei, analise);
        } else {
            R = retiraMaximo(Area);
            escreveMax(ArqLEs, R, &Es, analise);
        }
    }

    // define onde as proximas chamadas recursivas devem atuar
    *i = Ei - 1;
    *j = Es + 1;
}