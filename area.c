#include "area.h"
#include "analiseExperimental.h"

// aloca o espaço da RAM p/ a ordenação
void criaArea(TipoArea *area, int capacidade) {
    area->capacidade = capacidade;
    area->n = 0;
    area->itens = (TipoRegistro *)malloc(capacidade * sizeof(TipoRegistro));
}

// libera a memória alocada
void liberaArea(TipoArea *area) {
    free(area->itens);
    area->itens = NULL;
    area->n = 0;
    area->capacidade = 0;
}

bool areaVazia(TipoArea *area) {
    return (area->n == 0);
}

bool areaCheia(TipoArea *area) {
    return (area->n == area->capacidade);
}

// insere ordenando os registros na RAM (estilo Insertion Sort)
void insereOrdenado(TipoArea *area, TipoRegistro r, AnaliseExperimental *analise) {
    int i = area->n - 1;
    while (i >= 0) {
        analise->numComparacoes++; // conta cada comparação de nota
        if (area->itens[i].nota > r.nota) {
            area->itens[i + 1] = area->itens[i]; // shifta p/ direita
            i--;
        } else {
            break;
        }
    }
    area->itens[i + 1] = r;
    area->n++;
}

// remove o menor elemento (posição 0) e shifta o resto
TipoRegistro retiraMinimo(TipoArea *area) {
    TipoRegistro r = area->itens[0];
    for (int i = 1; i < area->n; i++) {
        area->itens[i - 1] = area->itens[i];
    }
    area->n--;
    return r;
}

// remove o maior elemento (última posição ocupada)
TipoRegistro retiraMaximo(TipoArea *area) {
    area->n--;
    return area->itens[area->n];
}