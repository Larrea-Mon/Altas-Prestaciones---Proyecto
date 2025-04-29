#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <ctype.h>
#include <openssl/md5.h>
#include <unistd.h>
#include <time.h>
#include <getopt.h>

//gcc -fopenmp hello_threads.c -o hello_threads.exe -Wno-deprecated-declarations

// generado con chatGPT

int main(int argc, char *argv[])
{
    int opt;
    int num_threads = 1; // default to 1 thread

    // Parse command-line arguments
    while ((opt = getopt(argc, argv, "t:")) != -1)
    {
        switch (opt)
        {
        case 't':
            num_threads = atoi(optarg);
            if (num_threads <= 0)
            {
                fprintf(stderr, "Invalid number of threads: %s\n", optarg);
                return EXIT_FAILURE;
            }
            break;
        default:
            fprintf(stderr, "Usage: %s -t num_threads\n", argv[0]);
            return EXIT_FAILURE;
        }
    }

    // Set number of threads
    omp_set_num_threads(num_threads);

// Parallel region
#pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        printf("hello world, I am thread %d\n", thread_id);
    }

    return 0;
}