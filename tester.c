#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv) {
    if(argc != 3) {
        fprintf(stderr, "Usage: %s <dance list> <dancer list>\n", argv[0]);
        exit(-1);
    }

    char buffer[1024];

    FILE* dances = fopen(argv[1], "r");
    FILE* dancers = fopen(argv[2], "r");

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

    int numDancers = 0;
    while(fgets(buffer, 1024, dancers) != NULL) {
        numDancers++;
    }

    uint64_t* dancerData = malloc(sizeof(uint64_t) * numDancers);
    
    rewind(dancers);

    for(int i = 0; i < numDancers; i++) {
        if(fgets(buffer, 1024, dancers) == NULL) {
            fprintf(stderr, "Error, dancer file doesn't contain enough lines\n");
            exit(-1);
        }
        int len = strlen(buffer);
        
        int prevIdx, curIdx;
        for(int j = 0; j < len; j++) {
            if(buffer[j] == ',') {
                prevIdx = j;
                break;
            }
        }

        for(int j = prevIdx + 1; j < len + 1; j++) {
            if(buffer[j] == ',' || buffer[j] == '\0' || buffer[j] == '\n' || buffer[j] == '\r') {
                curIdx = j;
                if(curIdx > prevIdx + 1) {
                    int danceIdx = -1;
                    for(int k = 0; k < cnt; k++) {
                        int found = 1;
                        for(int idx = prevIdx + 1; idx < curIdx; idx++) {
                            if(buffer[idx] != names[k][idx - prevIdx - 1]) {
                                found = 0;
                                break;
                            }
                        }

                        if(found) {
                            danceIdx = k;
                            break;
                        }
                    }

                    if(danceIdx == -1) {
                        buffer[curIdx] = '\0';
                        fprintf(stderr, "Unknown Dance Found: '%s'\n", buffer + prevIdx + 1);
                        exit(-1);
                    }

                    dancerData[i] |= 1L << danceIdx;
                }
                prevIdx = curIdx;
            }
        }
    }

    fclose(dancers);

    int* showOrder = malloc(sizeof(int) * cnt);
    
    int bestRun = -1, bestNum = -1;

    while(fgets(buffer, 1024, stdin) != NULL) {
        int num, chars;
        int offset = 0, idx = 0;
        while(sscanf(buffer + offset, "%d%n", &num, &chars) == 1) {
            showOrder[idx] = num;
            offset += chars;
            idx++;
        }

        int most = 0;
        int count = 0;
        for(int i = 0; i < numDancers; i++) {
            for(int j = 0; j < cnt; j++) {
                if((dancerData[i] & (1L << showOrder[j])) != 0) {
                    int curCnt = 1;
                    while(j + 3 * curCnt < cnt && (dancerData[i] & (1L << showOrder[j + 3 * curCnt])) != 0) {
                        curCnt++;
                    }
                    if(curCnt > most) {
                        most = curCnt;
                        count = 1;
                    } else if(curCnt == most) {
                        count++;
                    }
                }
            }
        }

        if(bestRun == -1 || most < bestRun || (most == bestRun && count < bestNum)) {
            bestRun = most;
            bestNum = count;

            printf("-> New Best (run of %d, %d occurances)\n", most, count);
            for(int i = 0; i < cnt; i++) {
                printf("%s, ", names[showOrder[i]]);
            }
            printf("\n");
        }
    }

    free(showOrder);
    free(dancerData);
    free(names);

    return 0;
}
