#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// This function finds and prints show orders that don't have any conflicts. The printing of the show order is done just as numbers
// not names, since that has been abstracted away at this level.
// The arguments are:
//      conflicts- an array of length n where each element is a bit-string that represents whether two dances conflict
//          (if bit j or element i is a 1, then dances i and j conflict)
//      n- The number of dances in the show
//      cnt- The number in the show order to schedule
//      prv2- The bit-string of conflicts for the dance two prior to the one to be scheduled
//      prv1- The bit-string of conflicts for the dance one prior to the one to be scheduled
//      used- A bit-string representing whether a dance has been placed into the show order or not
//      order- An array of integers representing dances placed into the show order
//      assigned- An array of integers representing where dances are required to be placed (or -1 if there is no restriction)
//      reserved- A bit-string where each bit tells whether a dance has an assigned location in the show
int solve(uint64_t* conflicts, int n, int cnt, uint64_t prv2, uint64_t prv1, uint64_t used, int* order, int* assigned, uint64_t reserved) {
    if(cnt < n) {
        // If there are more dances to assign to the show order, try each dance to see if it fits
        for(int i = 0; i < n; i++) {
            uint64_t dance = 1L << i;
            
            // The dance doesn't work if it is assigned to another number in the show order
            if(assigned[cnt] != -1 && assigned[cnt] != i) continue;
            if(assigned[cnt] != i && (dance & reserved) != 0) continue;

            // Check for conflicts between either of the previous two dances, and check that this
            // dance is not already used int he order
            if(((dance & prv2) == 0) && ((dance & prv1) == 0) && ((dance & used) == 0)) {
                order[cnt] = i;
                solve(conflicts, n, cnt+1, prv1, conflicts[i], used | dance, order, assigned, reserved);
            }
        }
    } else {
        // If cnt == n, then we have reached the end of the order, so we can print it
        if (write(1, order, sizeof(int) * n) <= 0) exit(-1);
    }
    return 0;
}

int main(int argc, char** argv) {
    if(argc != 4) {
        fprintf(stderr, "Usage: %s <dance list> <dancer list> <restrictions>\n", argv[0]);
        exit(-1);
    }

    char buffer[1024];

    FILE* dances = fopen(argv[1], "r");
    FILE* dancers = fopen(argv[2], "r");
    FILE* restric = fopen(argv[3], "r");

    // Read through the dances file to determine the number of dances
    int cnt = 0;
    while(fgets(buffer, 1024, dances) != NULL) {
        cnt++;
    }

    char** names = malloc(sizeof(char*) * cnt);
    // conflicts is an array of bit-strings where each bit represents whether there exists
    // a conflict between two dances (specifically a 1 in bit j of conflicts[i] means that
    // there is a dancer in both dances i and j)
    uint64_t*  conflicts = malloc(sizeof(uint64_t) * cnt);
    
    // assigned is an array that stores whether a dance has been added to the show order (or has
    // a specific place in the order it must go). Elements in it are -1 if a dance is not placed
    // in the order, and a non-negative number when it is in the show order.
    int* assigned = malloc(sizeof(int) * cnt);
    // reserved is a bit-string where each bit represents whether a specific dance can be
    // placed freely in the show order or must be placed at a specific point in the order
    uint64_t reserved = 0;

    rewind(dances);

    for(int i = 0; i < cnt; i++) {
        // Read the name of a dance in
        char* unused = fgets(buffer, 1024, dances);
        int len = strlen(buffer);
        
        // Remove newlines from the end of the dance name
        while(buffer[len - 1] == '\n' || buffer[len - 1] == '\r') { 
            buffer[len - 1] = '\0';
            len--;
        }
        
        names[i] = malloc(sizeof(char) * (len + 1));
        strcpy(names[i], buffer);
        conflicts[i] = 0;
    }
    
    fclose(dances);

    int num = 0;
    while(fgets(buffer, 1024, restric) != NULL && num < cnt) {
        int len = strlen(buffer);
        while(buffer[len-1] == '\n' || buffer[len-1] == '\r') {
            buffer[len-1] = '\0';
            len--;
        }

        assigned[num] = -1;
        if(len > 0) {
            int found = 0;
            // Search the dance names to find the dance that was specified
            for(int i = 0; i < cnt; i++) {
                if(strcmp(names[i], buffer) == 0) {
                    found = 1;
                    assigned[num] = i;
                    reserved |= 1L << i;
                    break;
                }
            }
            if(!found) {
                fprintf(stderr, "Restriction to dance '%s' not found.\n", buffer);
                exit(-1);
            }
        }
        num++;
    }
    fclose(restric);

    while(fgets(buffer, 1024, dancers) != NULL) {
        int prevIdx, curIdx;
        int len = strlen(buffer);
        
        // Locate the first comma, which sepearates the name from the dances
        for(int i = 0; i < len; i++) {
            if(buffer[i] == ',') {
                prevIdx = i;
                break;
            }
        }

        uint64_t dances = 0;
        int inDances[16];
        int danceCnt = 0;
        // This reads in all the dances the dancer is in
        for(int i = prevIdx + 1; i < len + 1; i++) {
            if(buffer[i] == ',' || buffer[i] == '\0' || buffer[i] == '\n' || buffer[i] == '\r') {
                curIdx = i;
                if(curIdx > prevIdx + 1) {
                    danceCnt++;
                    int danceIdx = -1;

                    // Compare the dance name to the list of dance names to find the number of the
                    // specified dance
                    for(int j = 0; j < cnt; j++) {
                        int found = 1;
                        for(int idx = prevIdx + 1; idx < curIdx; idx++) {
                            if(buffer[idx] != names[j][idx - prevIdx - 1]) {
                                found = 0;
                                break;
                            }
                        }

                        if(found) {
                            danceIdx = j;
                            break;
                        }
                    }

                    if(danceIdx == -1) {
                        buffer[curIdx] = '\0';
                        fprintf(stderr, "Unknown Dance Found: '%s'\n", buffer + prevIdx + 1);
                        exit(-1);
                    }

                    inDances[danceCnt-1] = danceIdx;
                    dances |= 1L << danceIdx;
                }
                prevIdx = curIdx;
            }
        }

        // All the dances a single dancer is in conflict, so update the conflicts of the dances to
        // reflect this
        for(int i = 0; i < danceCnt; i++) {
            conflicts[inDances[i]] |= dances;
        }
    }

    fclose(dancers);

    for(int i = 0; i < cnt; i++) {
        free(names[i]);
    }
    free(names);

    #pragma omp parallel for
    for(int i = 0; i < cnt; i++) {
        int* order = malloc(sizeof(int) * cnt);
        uint64_t dance = 1L << i;
        
        if(assigned[0] != -1 && assigned[0] != i) {}
        else if(assigned[0] != i && (dance & reserved) != 0) {}
        else {
            order[0] = i;
            solve(conflicts, cnt, 1, 0L, conflicts[i], dance, order, assigned, reserved);
        }

        free(order);
    }

    free(conflicts);

    return 0;
}
