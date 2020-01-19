#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv) {
    if(argc != 2) {
        fprintf(stderr, "Usage: %s <dance list>\n", argv[0]);
        exit(-1);
    }

    char buffer[1024];

    FILE* dances = fopen(argv[1], "r");

    int cnt = 0;
    while(fgets(buffer, 1024, dances) != NULL) {
        cnt++;
    }

    char** names = malloc(sizeof(char*) * cnt);
    
    rewind(dances);

    for(int i = 0; i < cnt; i++) {
        char* unused = fgets(buffer, 1024, dances);
        int len = strlen(buffer);
        while(buffer[len - 1] == '\n' || buffer[len - 1] == '\r') { 
            buffer[len - 1] = '\0';
            len--;
        }
        names[i] = malloc(sizeof(char) * (len + 1));
        strcpy(names[i], buffer);
    }
    
    fclose(dances);

    while(fgets(buffer, 1024, stdin) != NULL) {
        int num, chars;
        int offset = 0;
        while(sscanf(buffer + offset, "%d%n", &num, &chars) == 1) {
            printf("%s, ", names[num]);
            offset += chars;
        }
        printf("\n");
    }
}
