//
// Created by Adrien Defer on 16/04/2022.
//

#ifndef INFOF514_RAINBOWTABLE_BUFFER_MANAGEMENT_H
#define INFOF514_RAINBOWTABLE_BUFFER_MANAGEMENT_H

#include "main.h"

#define BUFFER_HASH_SIZE 100
#define IMPROVED_NBR_PASSWORD 809

pwd_couple *improved_buffer_of_pwd;
char **buffer_of_hash;
char **buffer_of_generated_pwd;

char **buffer_w;
char **transform_buffer_w;
char **buffer_n;
char *buffer_s;

#endif //INFOF514_RAINBOWTABLE_BUFFER_MANAGEMENT_H
