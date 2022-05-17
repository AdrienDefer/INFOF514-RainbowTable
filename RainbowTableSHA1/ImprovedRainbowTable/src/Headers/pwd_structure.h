//
// Created by Adrien Defer on 16/04/2022.
//

#ifndef INFOF514_RAINBOWTABLE_PWD_STRUCTURE_H
#define INFOF514_RAINBOWTABLE_PWD_STRUCTURE_H

#include "main.h"

typedef struct pwd_couple pwd_couple;
struct pwd_couple {
    char *head_pwd;
    char *tail_pwd;
};

typedef struct improved_pwd_couple_int improved_pwd_couple_int;
struct improved_pwd_couple_int {
    int head_pwd;
    int tail_pwd;
};

void struct_initialisation(pwd_couple* current_couple, int head_length, int tail_length) {
    current_couple->head_pwd = (char*) malloc(sizeof(char) * (head_length + 1));
    current_couple->tail_pwd = (char*) malloc(sizeof(char) * (tail_length + 1));
}

void free_struct(pwd_couple* current_couple) {
    free(current_couple->head_pwd);
    free(current_couple->tail_pwd);
}

#endif //INFOF514_RAINBOWTABLE_PWD_STRUCTURE_H
