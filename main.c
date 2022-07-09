#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>

#define DEBUG 1

/* Entry-point of the program, two parameters:
    -   Path of the file containing the graph
    -   Number of threads (default is 1, i.e. sequential version)
*/
int main(int argc, char *argv[])
{
    char *path;
    int nThreads;

    if (argc > 3 || argc < 2)
    {
        printf("The program accepts only two parameters: the graph path and the number of threads.\n");
        return -1;
    }

    path = argv[1];
#if DEBUG
    printf("%s\n", path);
#endif

    if (argc == 3) 
    {
        nThreads = atoi(argv[2]);

        // Avoid unwanted number of threads
        nThreads = nThreads > 0 ? nThreads : 1;  
           
#if DEBUG
        printf("%d\n", nThreads);
#endif       
    }

    return 0;
}