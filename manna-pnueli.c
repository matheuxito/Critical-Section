#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int main(void) {
    int client[3] = {0};
    int server;
    int request = 0;
    int respond = 0;
    int soma = 0;
    int local = 0;

    srand(time(NULL));
    clock_t initial_time = clock();
    clock_t elapsed_time = 0;

    char clear_command[10];

    #ifdef _WIN32 // Windows
        strcpy(clear_command, "cls");
    #else // Linux / Mac
        strcpy(clear_command, "clear");
    #endif

    #pragma omp parallel num_threads(4) shared(request, respond, soma) private(server, local)
    {
        int id = omp_get_thread_num();
        if (id == 0) {
            // 1 Servidor
            while (1) {
                while (request == 0) {

                }
                respond = request;
                while (respond != 0) {

                }
                request = 0;
                system(clear_command);
                printf("Soma: %d\n", soma);
                elapsed_time = clock() - initial_time;
                printf("Tempo decorrido: %ds\n\n", elapsed_time / CLOCKS_PER_SEC);
                printf("Digite Ctrl + C para sair.\n");
            }
        } else {
            // Vários Clientes
            while (1) {
                while (respond != id) {
                    request = id;
                }
                // Seção crítica:
                local = soma;
                sleep(1);
                soma = local + 1;
                // Fim da seção crítica
                respond = 0;
            }
        }
    }

    return 0;
}

// gcc -o manna-pnueli -fopenmp manna-pnueli.c