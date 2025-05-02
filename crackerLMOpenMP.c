// Compile with:
// gcc -fopenmp crackerLMOpenMP.c -o crackerLMOpenMP.exe -lcrypto -Wno-deprecated-declarations

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <openssl/md5.h>
#include <unistd.h>
#include <getopt.h>
#include <omp.h>
#include <stdbool.h>
#include <time.h>

#define SIZE 33
/*
#define ALPHA \
    "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"
#define DICT_LEN 94
*/ //Este es demasiado largo, encontrar una contraseña de 6 caracteres tarda horas
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

void int_to_pass(int num, char *str)
{
    char buffer[100];
    int index = 0;

    while (num > 0)
    {
        num--; // Adjust for 1-based indexing
        int remainder = num % DICT_LEN;
        buffer[index++] = ALPHA[remainder];
        num /= DICT_LEN;
    }
    buffer[index] = '\0';

    // Reverse buffer into str
    for (int i = 0; i < index; i++)
    {
        str[i] = buffer[index - i - 1];
    }
    str[index] = '\0';
}

int check(long candidata, char secreto[])
{
    char pass[100];
    char passMD5[100];
    int_to_pass(candidata, pass);
    hashMD5(pass, passMD5);

    // Des-comenta la linea inferior para imprimir 40 septillones de líneas
    printf("%li - comprobando contraseña %s - %s\n", candidata, pass, passMD5);

    if (strcmp(passMD5, secreto) == 0)
    {
        printf("¡Hash encontrado!\n");
        printf("Hash:     %s\n", passMD5);
        printf("Original: %s\n", secreto);
        return 1;
    }
    return 0;
}

int checkMD5(char entrada[])
{
    if (strlen(entrada) != 32)
    {
        printf("El hash debe tener exactamente 32 caracteres.\n");
        return 1;
    }
    for (int i = 0; i < 32; i++)
    {
        if (!isxdigit((unsigned char)entrada[i]))
        {
            printf("El hash debe estar en formato hexadecimal.\n");
            return 1;
        }
    }
    return 0;
}

int main(int argc, char *argv[])
{
    int opt;
    char secreto[SIZE] = "";
    int num_threads = 1;

    while ((opt = getopt(argc, argv, "h:t:")) != -1)
    {
        switch (opt)
        {
        case 'h':
            strncpy(secreto, optarg, SIZE);
            break;
        case 't':
            num_threads = atoi(optarg);
            if (num_threads <= 0 || num_threads > 255)
            {
                fprintf(stderr, "Número de hilos inválido: %s\n", optarg);
                return EXIT_FAILURE;
            }
            break;
        default:
            fprintf(stderr, "Uso: %s -t num_threads -h hash\n", argv[0]);
            return EXIT_FAILURE;
        }
    }

    if (checkMD5(secreto) != 0)
    {
        return EXIT_FAILURE;
    }

    omp_set_num_threads(num_threads);

    printf("Usando %d hilos para buscar la contraseña que genera el hash: %s\n", num_threads, secreto);
    printf("Buscando...\n");

    volatile bool encontrada = false;
    long winning_value = -1;
    int winning_thread = -1;

    struct timespec start_time, end_time;
    double elapsed;

    // Start timing
    clock_gettime(CLOCK_MONOTONIC, &start_time); // esto no da error en linux, confia

#pragma omp parallel shared(encontrada, secreto, winning_value, winning_thread)
    {
        int thread_id = omp_get_thread_num();
        long current = thread_id + 1;
        int local_result;

        while (!encontrada)
        {
            local_result = check(current, secreto);

            if (local_result == 1)
            {
#pragma omp critical
                {
                    if (!encontrada)
                    {
                        encontrada = true;
                        winning_value = current;
                        winning_thread = thread_id;
                    }
                }
                break;
            }

            current += num_threads;
            // Hardcodeado porque no me apecete escribir una funcion exponente en C a estas alturas
            if (current == 7339040224)
            {
                printf("Comprobadas todas las contraseñas de hasta 5 caracteres...");
            }
            if (current == 689869781056)
            {
                printf("Comprobadas todas las contraseñas de hasta 6 caracteres...");
            }
        }
    }

    // Stop timing
    clock_gettime(CLOCK_MONOTONIC, &end_time);

    // Calculate elapsed time in seconds
    elapsed = (end_time.tv_sec - start_time.tv_sec) +
              (end_time.tv_nsec - start_time.tv_nsec) / 1e9;

    int hours = (int)(elapsed / 3600);
    int minutes = ((int)elapsed % 3600) / 60;
    double seconds = elapsed - (hours * 3600) - (minutes * 60);

    if (encontrada)
    {
        char resultado[100];
        int_to_pass(winning_value, resultado);
        printf("Contraseña encontrada por el hilo %d: %s\n", winning_thread, resultado);
        printf("Tiempo transcurrido: %02d horas, %02d minutos, %06.3f segundos\n", hours, minutes, seconds);

    }
    else
    {
        printf("No se encontró ninguna contraseña que coincida.\n");
    }

    return 0;
}
