#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <omp.h>

// INTEGRANTES DO GRUPO
// GABRIEL ALMEIDA RODRIGUES PEREIRA 143229
// MATHEUS DE SOUZA MEIRE 140317
// BRUNO PIRES 139892

// Vamos desenvolver o jogo da vida de Conway
// Toda célula possui 8 outras vizinhas (horizontal, vertical e diagonal)

// Cada célula está em um dos estados VIVA(1) ou MORTA(0)
// Uma geracao é um conjunto de estados das células do tabuleiro
// As sociedades evoluem com a atualização do tabuleiro

// *** Regras ***
// 1. Celulas vivas com menos de 2(dois) vizinhas morrem por abandono
// 2. Cada célula viva com 2(dois) ou 3(três) deve permanecer viva para a próxima geração
// 3. Cada célula viva com 4(quatro) ou mais vizinhos morre por superpopulação
// 4. Cada célula morta com exatamente 3(três) vizinhos deve se tornar viva
 
// O que vamos fazer será considerando a versão Rainbow Game of Life
// Possuindo a diferença de que: as novas celulas que tornam-se vivas deverão ter como valor
// a média aritimética dos valores na vizinhança imediata. Portanto, as células vivas
// tem valor maior que zero

// Vamos programar um tabuleiro infinito, ou seja, a fronteira esquerda liga-se com a direita
// e a fronteira superior liga-se com a inferior

#define VIVA 1.0
#define MORTA 0.0
#define GERACOES 2000
#define TAMANHO 2048

// testar com 1 ,2, 4 e 8 threads
#define NUM_THREADS 1

int quantidadeCelulasGlobalCritical = 0;
int quantidadeCelulasGlobalReduction = 0;


void inicializaGrid(float **grid, float **new_grid){

#pragma omp parallel for shared(grid, new_grid)
        for (int i = 0; i < TAMANHO; i++){
            grid[i] = (float *)malloc(TAMANHO * sizeof(float));
            new_grid[i] = (float *)malloc(TAMANHO * sizeof(float));
            for (int j = 0; j < TAMANHO; j++){
                grid[i][j] = MORTA;
                new_grid[i][j] = MORTA;
            }
        }
}

void estadoInicial(float **grid){

    // com a figura de um Glider
    int lin = 1, col   = 1;
    grid[lin  ][col+1]  = VIVA;
    grid[lin+1][col+2] = VIVA;
    grid[lin+2][col ]  = VIVA;
    grid[lin+2][col+1] = VIVA;
    grid[lin+2][col+2] = VIVA;
    
    // Figura de um R-pentomino
    lin = 10; col = 30;
    grid[lin  ][col+1] = VIVA;
    grid[lin  ][col+2] = VIVA;
    grid[lin+1][col  ] = VIVA;
    grid[lin+1][col+1] = VIVA;
    grid[lin+2][col+1] = VIVA;
}


// funcao que retorna a quantidade de vizinhos vivos em cada celula na posicao i,j
int getNeighbors(float ** grid, int i, int j){
    int neighbors = 0;

    if (grid[ ((i - 1) + TAMANHO) % TAMANHO][j  ] == VIVA) neighbors++;
    if (grid[ ((i - 1) + TAMANHO) % TAMANHO][ ((j - 1) + TAMANHO) % TAMANHO] == VIVA) neighbors++;
    if (grid[ ((i - 1) + TAMANHO) % TAMANHO][ ((j + 1) + TAMANHO) % TAMANHO] == VIVA) neighbors++;
    
    if (grid[ ((i + 1) + TAMANHO) % TAMANHO][j  ] == VIVA) neighbors++;
    if (grid[ ((i + 1) + TAMANHO) % TAMANHO][ ((j - 1) + TAMANHO) % TAMANHO] == VIVA) neighbors++;
    if (grid[ ((i + 1) + TAMANHO) % TAMANHO][ ((j + 1) + TAMANHO) % TAMANHO] == VIVA) neighbors++;

    if (grid[i  ][ ((j - 1) + TAMANHO) % TAMANHO] == VIVA) neighbors++;
    if (grid[i  ][ ((j + 1) + TAMANHO) % TAMANHO] == VIVA) neighbors++;

    return neighbors;
}

int quantidadeCelulasVivasGlobalReduction(float **grid){
    #pragma omp parallel for shared(grid) reduction(+:quantidadeCelulasGlobalReduction)
    for (int i = 0; i < TAMANHO; i++){
        for (int j = 0; j < TAMANHO; j++){
            if (grid[i][j] == VIVA){
                // #pragma omp critical
                quantidadeCelulasGlobalReduction++;
            }
        }
    }
    return quantidadeCelulasGlobalReduction;
}


int quantidadeCelulasVivasGlobalCritical(float **grid){
    #pragma omp parallel for shared(grid) //reduction(+:quantidadeCelulasGlobal)
    for (int i = 0; i < TAMANHO; i++){
        for (int j = 0; j < TAMANHO; j++){
            if (grid[i][j] == VIVA){
                #pragma omp critical
                quantidadeCelulasGlobalCritical++;
            }
        }
    }
    return quantidadeCelulasGlobalCritical;
}

int quantidadeCelulasVivas(float **grid){
    int celulasVivas = 0;
    #pragma omp parallel for shared(grid) reduction(+:celulasVivas)
    for (int i = 0; i < TAMANHO; i++){
        for (int j = 0; j < TAMANHO; j++){
            if (grid[i][j] == VIVA){
                celulasVivas++;
            }
        }
    }
    return celulasVivas;
}

void atualizaGrid(float **grid, float **new_grid){
    float media_aritmetica = 0.0;
    int i = 0;
#pragma omp parallel for shared(grid, new_grid) private(i, media_aritmetica)
    for (i = 0; i < TAMANHO; i++){
        for (int j = 0; j < TAMANHO; j++){
            int neighbors = getNeighbors(grid, i, j);
            // *** Regras ***
            // 1. Celulas vivas com menos de 2(dois) vizinhas morrem por abandono
            // 2. Cada célula viva com 4(quatro) ou mais vizinhos morre por superpopulação
            if (grid[i][j] == VIVA){
                if (neighbors < 2 || neighbors > 3){
                    new_grid[i][j] = MORTA;
                }
                else{ // 3. Cada célula viva com 2(dois) ou 3(três) deve permanecer viva para a próxima geração
                    new_grid[i][j] = VIVA;
                }
            }
            else{// 4. Cada célula morta com exatamente 3(três) vizinhos deve se tornar viva
                if (neighbors == 3){
                    media_aritmetica = (float)neighbors/8.0;
                    new_grid[i][j] = VIVA;
                }
            }
        }
    }  
}

void salvarMatriz(float **grid, FILE *arquivo) {
    for (int i = 0; i < TAMANHO; i++) {
        for (int j = 0; j < TAMANHO; j++) {
            fprintf(arquivo, "%d ", (int)grid[i][j]);
        }
        fprintf(arquivo, "\n");
    }
    fprintf(arquivo, "\n");
}

int main(){
    
    omp_set_num_threads(NUM_THREADS);
    
    float **grid = (float **)malloc(TAMANHO * sizeof(float *));
    float **new_grid = (float **)malloc(TAMANHO * sizeof(float *));
    inicializaGrid(grid, new_grid);
    estadoInicial(grid);
    //int sobreviventesGeracoes[GERACOES];
    //FILE *arquivo = fopen("estados.txt", "w");
    double starT = omp_get_wtime();
    for(int i = 0; i < GERACOES; i++){ 
        atualizaGrid(grid, new_grid);
        
        //salvarMatriz(grid, arquivo);
        //sobreviventesGeracoes[i] = quantidadeCelulasVivas(new_grid);
        // printf("Quantidade de celulas vivas Geracao %d: %d\n",i, sobreviventesGeracoes[i]);
        
        #pragma omp parallel for shared(grid,  new_grid)
        for(int j = 0; j < TAMANHO; j++){
            for(int k = 0; k < TAMANHO; k++){
                grid[j][k] = new_grid[j][k];
            }
        }
    }
    double stoP = omp_get_wtime();
    //printf("Tempo de execução trecho de computação das gerações: %f\n", stoP - starT);
    //fclose(arquivo);
    // printf("Quantidade de celulas vivas Geração %d: %d\n", GERACOES, quantidadeCelulasVivas(new_grid));
    double starT = omp_get_wtime();
    printf("Quantidade de celulas vivas Geração %d: %d\n", GERACOES, quantidadeCelulasVivasGlobalCritical(new_grid));
    double stoP = omp_get_wtime();
    printf("Tempo de execução trecho de computação das gerações com CRITICAL: %f\n", stoP - starT);

    double starT = omp_get_wtime();
    printf("Quantidade de celulas vivas Geração %d: %d\n", GERACOES, quantidadeCelulasVivasGlobalReduction(new_grid));
    double stoP = omp_get_wtime();
    printf("Tempo de execução trecho de computação das gerações com REDUCTION: %f\n", stoP - starT);


    for (int i = 0; i < TAMANHO; i++){
        free(grid[i]);
        free(new_grid[i]);
    }
    free(grid);
    free(new_grid);
    return 0;
}