#include "area.h"
#include "analiseExperimental.h"

void criaArea(TipoArea *area, int capacidade) {
    area->capacidade = capacidade;
    area->n = 0;
    area->itens = (TipoRegistro *)malloc(capacidade * sizeof(TipoRegistro));
}

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

void insereOrdenado(TipoArea *area, TipoRegistro r, AnaliseExperimental *analise) {
    int i = area->n - 1;
    // Cada iteração deste while é uma comparação de notas
    while (i >= 0) {
        analise->numComparacoes++; // Conta a comparação do loop
        if (area->itens[i].nota > r.nota) {
            area->itens[i + 1] = area->itens[i];
            i--;
        } else {
            break;
        }
    }
    area->itens[i + 1] = r;
    area->n++;
}
TipoRegistro retiraMinimo(TipoArea *area) {
    TipoRegistro r = area->itens[0];
    for (int i = 1; i < area->n; i++) {
        area->itens[i - 1] = area->itens[i];
    }
    area->n--;
    return r;
}

TipoRegistro retiraMaximo(TipoArea *area) {
    area->n--;
    return area->itens[area->n];
}
