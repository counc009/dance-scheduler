#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char** argv) {
    if(argc != 2) {
        fprintf(stderr, "Usage: %s <number of dances>\n", argv[0]);
        exit(-1);
    }

    int cnt = atoi(argv[1]);
    if (cnt <= 0) {
        fprintf(stderr, "Number of dances specified interpreted as %d. Exiting.\n", cnt);
        exit(-1);
    }

    int* order = malloc(sizeof(int) * cnt);

    while(read(0, order, sizeof(int) * cnt) == sizeof(int) * cnt) {
        for (int i = 0; i < cnt; i++) {
            printf("%2d ", order[i]);
        }
        printf("\n");
    }
}
