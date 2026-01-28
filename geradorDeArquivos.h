#ifndef GERADOR_DE_ARQUIVOS_H
#define GERADOR_DE_ARQUIVOS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "registro.h"

#define ORDENADO_CRESCENTE 1
#define ORDENADO_DECRESCENTE 2
#define DESORDENADO 3

bool gerarArquivo(int, int);
bool lerRegistroTXT(FILE *, TipoRegistro *);
void trim(char *);
bool imprimirArquivoBinario(char *);
int comparaCrescente(const void *, const void *);
int comparaDecrescente(const void *, const void *);
bool binarioParaTXT(const char *, const char *);
bool imprimirArquivoTXT(const char *);

#endif