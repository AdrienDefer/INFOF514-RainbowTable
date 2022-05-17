//
// Created by Adrien Defer on 16/04/2022.
//

#include "Headers/main.h"
#include "Functions/chain_construction.c"
#include "Functions/rainbow_crack.c"

int main(int argc, const char * argv[]) {
    srand(time(NULL));
    if (strtol(argv[1], NULL, 10) == 0) {
        start_rainbow_table_creation(strtol(argv[2], NULL, 10), strtol(argv[3], NULL, 10), strtol(argv[4], NULL, 10));
    } else if (strtol(argv[1], NULL, 10) == 1) {
        start_cracking((char*) argv[4], strtol(argv[2], NULL, 10), strtol(argv[3], NULL, 10));
    } else if (strtol(argv[1], NULL, 10) == 2) {
        fill_hash_to_crack_file(strtol(argv[2], NULL, 10));
    } else if (strtol(argv[1], NULL, 10) == 3) {
        mass_cracking(strtol(argv[2], NULL, 10), strtol(argv[3], NULL, 10));
    } else {
        printf("No option selected.\n");
    }
}