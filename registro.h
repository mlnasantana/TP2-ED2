#ifndef REGISTRO_H
#define REGISTRO_H

typedef struct {
    long int numInscricao;
    float nota; //a variável base para a ordenação!
    char estado[3]; //UF com 2 caracteres + '\0'
    char cidade[51];
    char curso[31];
} TipoRegistro;

#endif