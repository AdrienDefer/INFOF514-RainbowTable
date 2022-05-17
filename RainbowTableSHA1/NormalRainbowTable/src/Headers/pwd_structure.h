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

void struct_initialisation(pwd_couple* current_couple, int pwd_length) {
    current_couple->head_pwd = (char*) malloc(sizeof(char) * (pwd_length + 1));
    current_couple->tail_pwd = (char*) malloc(sizeof(char) * (pwd_length + 1));
}

void free_struct(pwd_couple* current_couple) {
    free(current_couple->head_pwd);
    free(current_couple->tail_pwd);
}

#endif //INFOF514_RAINBOWTABLE_PWD_STRUCTURE_H
