// compile: gcc -fopenmp crackerLM-openMP.c -o crackerLM-openMP.exe -lcrypto -Wno-deprecated-declarations

#define SIZE 33
#define DICT_LEN 66
#define ALPHA "abcdefghijklmnopqrstuvwxyzABCDEFGHIJOKLMNOPQRSTUVWXYZ1234567890!?+"
// #define CHUNK 10000
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <openssl/md5.h>
#include <unistd.h>
#include <time.h>
#include <getopt.h>
#include <omp.h>

int main(int argc, char *argv[])
{
    // Variables para procesar los parámetros con getopt
    int index;
    int opt;
    // Variables para la gestión de crackeo:
    unsigned char secreto[SIZE] = "";
    long candidata;
    unsigned char hash[SIZE];
    // Variable para la gestión de threads
    int num_threads = 1;
    /*
        if (argc != 5)
        {
            printf("Error, uso: %s -h <hash> -t <threads>\n", argv[0]);
            return 1;
        }
    */

    while ((opt = getopt(argc, argv, "h:t:")) != -1)
        switch (opt)
        {
        case 'h':
            strncpy(secreto, optarg, SIZE);
            //printf("Debug: %s - secreto\n",secreto);
            break;
        case 't':
            num_threads = atoi(optarg);
            if (num_threads <= 0)
            {
                fprintf(stderr, "Invalid number of threads: %s\n", optarg);
                return EXIT_FAILURE;
            }
            if (num_threads > 255){
                fprintf(stderr, "Por motivos de seguridad, el máximo de hilos es 255: %s\n", optarg);
                return EXIT_FAILURE;
            }
            //printf("Debug: %i - threads\n",num_threads);
            break;
        default:
            fprintf(stderr, "Usage: %s -t num_threads -h hash\n", argv[0]);
            return EXIT_FAILURE;
        }

    omp_set_num_threads(num_threads);

    printf(" Has introducido %d hilos y el secreto es %s\n", num_threads, secreto);


}