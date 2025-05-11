// mpicc crackerLMMPI.c -o crackerLMMPI.exe -lcrypto -Wno-deprecated-declarations
// mpirun -np 4 ./crackerLMMPI.exe -h 827ccb0eea8a706c4c34a16891f84e7b
// el hash es de 12345
// Si al prender el programa todos los procesos piensan que son el proceso 0, desinstala todas tus versiones de MPI y haz "sudo apt install openmpi-bin libopenmpi-dev" 
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <openssl/md5.h>
#include <unistd.h>
#include <getopt.h>
#include <mpi.h>
#include <stdbool.h>
#include <time.h>

#define SIZE 33
#define DICT_LEN 66
#define ALPHA "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890!?+"

void hashMD5(const char *texto, char *md5_str)
{
    unsigned char digest[MD5_DIGEST_LENGTH];
    MD5((unsigned char *)texto, strlen(texto), digest);

    for (int i = 0; i < MD5_DIGEST_LENGTH; i++)
    {
        sprintf(&md5_str[i * 2], "%02x", digest[i]);
    }
    md5_str[MD5_DIGEST_LENGTH * 2] = '\0';
}

void int_to_pass(unsigned long long num, char *str)
{
    char buffer[100];
    unsigned long long index = 0;

    while (num > 0)
    {
        num--;
        unsigned long long remainder = num % DICT_LEN;
        buffer[index++] = ALPHA[remainder];
        num /= DICT_LEN;
    }
    buffer[index] = '\0';

    for (int i = 0; i < index; i++)
    {
        str[i] = buffer[index - i - 1];
    }
    str[index] = '\0';
}

bool check(unsigned long long candidata, char secreto[], int rank, bool verbose)
{
    char pass[100];
    char passMD5[100];
    int_to_pass(candidata, pass);
    hashMD5(pass, passMD5);

    if (verbose)
        printf("Proceso %d - %lli - comprobando contraseña %s - %s\n", rank, candidata, pass, passMD5);

     if(strcmp(passMD5, secreto) == 0)
     return true;
     else
     return false;
}

bool checkMD5(char entrada[])
{
    if (strlen(entrada) != 32)
    {
        printf("El hash debe tener exactamente 32 caracteres.\n");
        return false;
    }
    for (int i = 0; i < 32; i++)
    {
        if (!isxdigit((unsigned char)entrada[i]))
        {
            printf("El hash debe estar en formato hexadecimal.\n");
            return false;
        }
    }
    return true;
}

int main(int argc, char *argv[])
{
    
    int opt;
    char secreto[SIZE] = "";

    bool verbose = false;

    while ((opt = getopt(argc, argv, "h:v")) != -1)
{
    switch (opt)
    {
    case 'h':
        strncpy(secreto, optarg, SIZE);
        break;
    case 'v':
        verbose = true;
        break;
    default:
        fprintf(stderr, "Uso: %s -h hash [-v]\n", argv[0]);
        MPI_Finalize();
        return EXIT_FAILURE;
    }
}

    if (checkMD5(secreto) != 0)
    {
        return EXIT_FAILURE;
    }

    MPI_Init(&argc, &argv);

    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    printf("Proceso %d inicializado\n", world_rank);

    unsigned long long current = world_rank + 1; // Starts at rank+1 for proper interleaving
    unsigned long long winning_value = -1;
    int found = 0;
    int global_found = 0;

    struct timespec start_time, end_time;
    double elapsed;

    if (world_rank == 0)
    {
        clock_gettime(CLOCK_MONOTONIC, &start_time);
        printf("Usando %d procesos MPI para buscar la contraseña con hash: %s\n", world_size, secreto);
        printf("Buscando...\n");
    }

    while (!global_found)
    {
        if (check(current, secreto, world_rank,verbose))
        {
            found = 1;
            winning_value = current;
        }

        // Communicate if any process found the solution
        MPI_Allreduce(&found, &global_found, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);

        if (global_found)
            break;

        current += world_size;

    }

    // Share the winning value from the rank that found it
    MPI_Bcast(&winning_value, 1, MPI_LONG, found ? world_rank : 0, MPI_COMM_WORLD);

    if (found)
    {
        clock_gettime(CLOCK_MONOTONIC, &end_time);

        elapsed = (end_time.tv_sec - start_time.tv_sec) +
                  (end_time.tv_nsec - start_time.tv_nsec) / 1e9;

        char resultado[100];
        int_to_pass(winning_value, resultado);
        printf("Contraseña encontrada por el proceso %d: %s\n", world_rank, resultado);
        printf("Tiempo transcurrido: %.2f segundos\n", elapsed);
    }

    MPI_Finalize();
    return 0;
}