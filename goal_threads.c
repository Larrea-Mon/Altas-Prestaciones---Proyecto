#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  // for getopt
#include <omp.h>     // for OpenMP
#include <stdbool.h> // for bool type
#include <bits/getopt_core.h>

// Generado por chatgpt

// gcc -fopenmp goal_threads.c -o goal_threads.exe -Wno-deprecated-declarations

int main(int argc, char *argv[])
{
    int opt;
    int num_threads = 1;
    long goal = 10;              // default goal
    volatile bool found = false; // shared flag among threads
    int winning_thread = -1;
    long winning_value = -1;

    // Parse command-line arguments
    while ((opt = getopt(argc, argv, "t:g:")) != -1)
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
        case 'g':
            goal = atoi(optarg);
            if (goal <= 0)
            {
                fprintf(stderr, "Invalid goal: %s\n", optarg);
                return EXIT_FAILURE;
            }
            break;
        default:
            fprintf(stderr, "Usage: %s -t num_threads -g goal\n", argv[0]);
            return EXIT_FAILURE;
        }
    }

    omp_set_num_threads(num_threads);

// Parallel region
#pragma omp parallel shared(found, winning_thread, winning_value)
    {
        int thread_id = omp_get_thread_num();
        int current = thread_id + 1;

        while (!found)
        {
            // Check if this number exactly matches the goal
            if (current == goal)
            {
// Try to claim the win
#pragma omp critical
                {
                    if (!found)
                    {
                        found = true;
                        winning_thread = thread_id;
                        winning_value = current;
                    }
                }
                break; // Done
            }
            // If the number exceeds the goal, stop trying (no need to continue)
            if (current > goal)
            {
                break;
            }
            current += num_threads; // advance to next number for this thread
        }
    }

    if (winning_thread != -1)
    {
        printf("Goal was %ld, found exactly by thread %d (value = %ld)\n", goal, winning_thread, winning_value);
    }
    else
    {
        printf("Goal %ld was not found by any thread.\n", goal);
    }

    return 0;
}