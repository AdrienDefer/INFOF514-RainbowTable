//
// Created by Adrien Defer on 12/04/2022.
//

#include "../Headers/main.h"

FILE *pwd_file;

char* pwd_generation(int length) {
    static const char pwd_policy[] = "azertyuiopqsdfghjklmwxcvbnAZERTYUIOPQSDFGHJKLMWXCVBN1234567890";
    char *pwd = (char*) malloc(sizeof(char) * (length + 1));
    for (int i = 0; i < length; i++) {
        pwd[i] = pwd_policy[rand() % 62];
    }
    pwd[length] = '\0';
    return pwd;
}

char* pwd_reduction(char* hash_to_reduce, int step, int pwd_length) {
    static const char pwd_policy[] = "azertyuiopqsdfghjklmwxcvbnAZERTYUIOPQSDFGHJKLMWXCVBN1234567890";
    char *pwd_reduced = (char*) malloc(sizeof(char) * (pwd_length + 1));
    for (int i = 0; i < pwd_length; i++) {
        pwd_reduced[i] = pwd_policy[(((hash_to_reduce[i] ^ step) + hash_to_reduce[i + 11]) % 62)];
    }
    pwd_reduced[pwd_length] = '\0';
    return pwd_reduced;
}

void fill_hash_to_crack_file(int pwd_length) {
    char fileName[50];
    sprintf(fileName, "Files/HashFiles/HashToCrackFile%d.txt", pwd_length);
    FILE *hash_file_to_crack = fopen(fileName, "w");
    for (int i = 0; i < BUFFER_HASH_SIZE; i++) {
        char *pwd_generated = pwd_generation(pwd_length);
        char *hash_generated = (char*) malloc(sizeof(char) * 65);
        sha256_easy_hash_hex(pwd_generated, strlen(pwd_generated), hash_generated);
        hash_generated[64] = '\0';
        if (i == (BUFFER_HASH_SIZE - 1)) {
            fprintf(hash_file_to_crack, "%s", hash_generated);
        } else {
            fprintf(hash_file_to_crack, "%s\n", hash_generated);
        }
        free(pwd_generated);
        free(hash_generated);
    }
    fclose(hash_file_to_crack);
}

int binary_search(int first, int last, char *to_search) {
    if (last >= first) {
        int middle = (first + last) / 2;
        if (strcmp(buffer_of_couple[middle].tail_pwd, to_search) == 0) {
            return middle;
        }
        if (strcmp(buffer_of_couple[middle].tail_pwd, to_search) > 0) {
            return binary_search(first, middle - 1, to_search);
        }
        return binary_search(middle + 1, last, to_search);
    }
    return -1;
}

void split_and_fill_struct(pwd_couple* current_couple, char the_line_of_pwd[], int pwd_length) {
    the_line_of_pwd[strlen(the_line_of_pwd)] = '\0';
    const char *separator = " ";
    char *str_token = strtok(the_line_of_pwd, separator);
    strcpy(current_couple->head_pwd, str_token);
    current_couple->head_pwd[pwd_length] = '\0';
    str_token = strtok(NULL, separator);
    strcpy(current_couple->tail_pwd, str_token);
    current_couple->tail_pwd[pwd_length] = '\0';
}

void fill_buffer_of_couple(int pwd_length) {
    if (pwd_file == NULL) {
        printf("Error while opening the file.");
        exit(1);
    }
    char *line_in_file = (char*) malloc((2 * pwd_length) + 1);
    int i = 0;
    ssize_t line_size = 0;
    size_t line_buff_size = 0;
    line_size = getline(&line_in_file,  &line_buff_size, pwd_file);
    while (line_size >= 2) {
        pwd_couple head_tail_couple;
        struct_initialisation(&head_tail_couple, pwd_length);
        split_and_fill_struct(&head_tail_couple, line_in_file, pwd_length);
        buffer_of_couple[i] = head_tail_couple;
        i++;
        line_size = getline(&line_in_file, &line_buff_size, pwd_file);
    }
    free(line_in_file);
    fclose(pwd_file);
}

void free_the_buffer_of_couple() {
    for (int i = 0; i < NUMBER_PASSWORD; i++) {
        free_struct(&buffer_of_couple[i]);
    }
    free(buffer_of_couple);
}

void buffer_of_couple_initialization(int pwd_length) {
    char fileName[50];
    printf("where are youuuuuuu\n");
    sprintf(fileName, "../../Files/RainbowTables/RainbowTable_%d.txt", pwd_length);
    printf("where are youuuuuuu!!!!\n");
    pwd_file = fopen(fileName, "r");
    buffer_of_couple = (pwd_couple*) malloc(sizeof(pwd_couple) * NUMBER_PASSWORD);
    fill_buffer_of_couple(pwd_length);
}

void free_the_buffer_of_hash() {
    for (int i = 0; i < BUFFER_HASH_SIZE; i++) {
        free(buffer_of_hash[i]);
    }
    free(buffer_of_hash);
}

void fill_the_buffer_of_hash(int pwd_length) {
    char fileName[50];
    sprintf(fileName, "Files/HashFiles/HashToCrackFile%d.txt", pwd_length);
    buffer_of_hash = malloc(sizeof(char*) * BUFFER_HASH_SIZE);
    FILE *file = fopen(fileName, "r");
    int i = 0;
    while (i < BUFFER_HASH_SIZE) {
        buffer_of_hash[i] = (char*) malloc(sizeof(char) * 65);
        fgets(buffer_of_hash[i], 66, file);
        if (buffer_of_hash[i][strlen(buffer_of_hash[i]) - 1] == '\n') {
            buffer_of_hash[i][strlen(buffer_of_hash[i]) - 1] = 0;
        }
        buffer_of_hash[i][64] = '\0';
        i++;
    }
    fclose(file);
}

