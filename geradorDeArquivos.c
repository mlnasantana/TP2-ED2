#include "geradorDeArquivos.h"

bool gerarArquivo(int situacao, int quantReg){

    TipoRegistro reg; //Registro auxiliar para a leitura no arquivo texto e escrita no arquivo binário
    
    //Gerar nome do arquivo com base na situação e quantidade de registros
    char nomeArq[100];
    sprintf(nomeArq, "dados_%d_%d.dat", situacao, quantReg);

    //Abre o arquivo de texto PROVAO.txt para leitura
    FILE *txt = fopen("PROVAO.TXT", "r");
    
    //Cria o arquivo binário com a quantidade de registros desejada
    FILE *bin = fopen(nomeArq, "wb");

    //Verifica se os arquivos foram abertos corretamente
    if(txt == NULL || bin == NULL){
       if (txt) fclose(txt);
       if (bin) fclose(bin);
       return false;
    }

    if (situacao == DESORDENADO){
        //Se for desordenado, ler e escrever diretamente no arquivo binário
        for (int i = 0; i < quantReg; i++){
            if (!lerRegistroTXT(txt, &reg)) {
                fclose(txt);
                fclose(bin);
                return false;
            }
            fwrite(&reg, sizeof(TipoRegistro), 1, bin);
      
        }
    } else{
        //Se não for desordenado, ler todos os registros para um vetor para posterior ordenação
        TipoRegistro *vet = malloc(quantReg * sizeof(TipoRegistro));
        if (!vet) return false;

        for (int i = 0; i < quantReg; i++){
            if (!lerRegistroTXT(txt, &vet[i])) {
                free(vet);
                return false;
            }
        }

        //Chama qsort para ordenar o vetor conforme a situação
        if (situacao == ORDENADO_CRESCENTE){
            qsort(vet, quantReg, sizeof(TipoRegistro), comparaCrescente);
        } else if (situacao == ORDENADO_DECRESCENTE){
            qsort(vet, quantReg, sizeof(TipoRegistro), comparaDecrescente);
        }
        
        //Escreve o vetor ordenado no arquivo binário
        fwrite(vet, sizeof(TipoRegistro), quantReg, bin);
        
        free(vet);
    }

    fclose(txt);
    fclose(bin);

    return true;
}

bool lerRegistroTXT(FILE *txt, TipoRegistro *reg) {
    if (fscanf(txt, "%8ld %f %2s %50c %30c\n", &reg->numInscricao, &reg->nota, reg->estado, reg->cidade, reg->curso) != 5) return false;
    
    //String tem que ser terminada com '\0'
    reg->estado[2] = '\0';
    reg->cidade[50] = '\0';
    reg->curso[30] = '\0';

    //Remover espaços em branco à direita
    trim(reg->cidade);
    trim(reg->curso);

    return true;

}

//Função auxiliar para remover espaços em branco à direita de uma string
void trim(char *str) {
    for (int i = strlen(str) - 1; i >= 0 && str[i] == ' '; i--)
        str[i] = '\0';
}

//Imprimir o arquivo binário caso o -P seja passado como argumento
bool imprimirArquivoBinario(char *nomeArq) {
    FILE *bin = fopen(nomeArq, "rb");
    TipoRegistro reg; //Auxiliar para leitura

    if (bin == NULL) {
        return false;
    }

    printf("========================================================\n");
    printf("*****IMPRIMINDO ARQUIVO GERADO ANTES DA ORDENAÇÃO: %s*****\n\n", nomeArq);
    while (fread(&reg, sizeof(TipoRegistro), 1, bin) == 1) {
        printf("%8ld | %.2f | %s | %s | %s\n", reg.numInscricao, reg.nota, reg.estado, reg.cidade, reg.curso);
    }
    printf("========================================================\n");


    fclose(bin);
    return true;
}

//Funções de comparação ordenação crescente
int comparaCrescente(const void *a, const void *b) {
    float n1 = ((TipoRegistro *)a)->nota;
    float n2 = ((TipoRegistro *)b)->nota;
    return (n1 > n2) - (n1 < n2);
}

//Função de comparação para ordenação decrescente
int comparaDecrescente(const void *a, const void *b) {
    float n1 = ((TipoRegistro *)a)->nota;
    float n2 = ((TipoRegistro *)b)->nota;
    return (n2 > n1) - (n2 < n1);
}

//Converter arquivo binário para texto para conferir resultado
bool binarioParaTXT(const char *nomeBin, const char *nomeTxt) {
    FILE *bin = fopen(nomeBin, "rb");
    FILE *txt = fopen(nomeTxt, "w");

    printf("Entrou\n");

    //Verifica se os arquivos foram abertos corretamente, igual ao gerarArquivo
    if (bin == NULL || txt == NULL) {
        if (bin) fclose(bin);
        if (txt) fclose(txt);
        return false;
    }

    TipoRegistro reg; //Auxiliar para leitura

    //Lê do binário e escreve no texto
    while (fread(&reg, sizeof(TipoRegistro), 1, bin) == 1) {
        fprintf(txt, "%08ld %05.1f %-2s %-50s %-30s\n", reg.numInscricao,reg.nota, reg.estado, reg.cidade, reg.curso);
    }

    fclose(bin);
    fclose(txt);

    printf("Converteu\n");
    return true;
}

//Fiz essa função só pra diferenciar da outra que imprime o binário já que tem um cabeçalho diferente
bool imprimirArquivoTXT(const char *nomeTxt) {
    FILE *txt = fopen(nomeTxt, "r");

    if (txt == NULL) {
        return false;
    }

    char linha[256];

    printf("========================================================\n");
    printf("*****IMPRIMINDO ARQUIVO GERADO DEPOIS DA ORDENAÇÃO: %s*****\n\n", nomeTxt);

    while (fgets(linha, sizeof(linha), txt) != NULL) {
        printf("%s", linha);
    }

    printf("========================================================\n");

    fclose(txt);
    return true;
}
