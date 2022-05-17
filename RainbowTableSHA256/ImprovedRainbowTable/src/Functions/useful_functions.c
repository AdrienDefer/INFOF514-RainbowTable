//
// Created by Adrien Defer on 12/04/2022.
//

#include "../Headers/main.h"

FILE *improved_pwd_file;

// SIZE -> [0...398]
void fill_buffer_w() {
    FILE *file_w = fopen("Files/WNSFiles/WFile.txt", "r");
    char *line_in_file = (char*) malloc(sizeof(char) * 10);
    size_t line_buff_size = 0;
    int index = 0;
    for (ssize_t line_size = getline(&line_in_file, &line_buff_size, file_w); line_size >= 2; line_size = getline(&line_in_file, &line_buff_size, file_w)) {
        if (line_in_file[strlen(line_in_file) - 1] == '\n') {
            line_in_file[strlen(line_in_file) - 1] = '\0';
            buffer_w[index] = (char*) malloc(sizeof(char) * (strlen(line_in_file) + 1));
            strcpy(buffer_w[index], line_in_file);
            buffer_w[index][strlen(buffer_w[index])] = '\0';
            index++;
        }
    }
    free(line_in_file);
    fclose(file_w);
}

// SIZE -> [0...797]
void fill_transform_buffer_w() {
    FILE *file_transform_w = fopen("Files/WNSFiles/TransformWFile.txt", "r");
    char *line_in_file = (char*) malloc(sizeof(char) * 10);
    size_t line_buff_size = 0;
    int index = 0;
    for (ssize_t line_size = getline(&line_in_file, &line_buff_size, file_transform_w); line_size >= 2; line_size = getline(&line_in_file, &line_buff_size, file_transform_w)) {
        if (line_in_file[strlen(line_in_file) - 1] == '\n') {
            line_in_file[strlen(line_in_file) - 1] = '\0';
            transform_buffer_w[index] = (char*) malloc(sizeof(char) * (strlen(line_in_file) + 1));
            strcpy(transform_buffer_w[index], line_in_file);
            transform_buffer_w[index][strlen(transform_buffer_w[index])] = '\0';
            index++;
        }
    }
    free(line_in_file);
    fclose(file_transform_w);
}

// SIZE -> [0...9999]
void fill_buffer_n() {
    FILE *file_n = fopen("Files/WNSFiles/NFile.txt", "r");
    char *line_in_file = (char*) malloc(sizeof(char) * 4);
    size_t line_buff_size = 0;
    int index = 0;
    for (ssize_t line_size = getline(&line_in_file, &line_buff_size, file_n); line_size >= 2; line_size = getline(&line_in_file, &line_buff_size, file_n)) {
        if (line_in_file[strlen(line_in_file) - 1] == '\n') {
            line_in_file[strlen(line_in_file) - 1] = '\0';
            buffer_n[index] = (char*) malloc(sizeof(char) * (strlen(line_in_file) + 1));
            strcpy(buffer_n[index], line_in_file);
            buffer_n[index][strlen(buffer_n[index])] = '\0';
            index++;
        }
    }
    free(line_in_file);
    fclose(file_n);
}

// SIZE -> [0...31]
void fill_buffer_s() {
    FILE *file_s = fopen("Files/WNSFiles/SFile.txt", "r");
    for (int i = 0; i < 32; i++) {
        buffer_s[i] = fgetc(file_s);
    }
    buffer_s[32] = '\0';
    fclose(file_s);
}

void initialize_buffers() {
    buffer_w = malloc(sizeof(char*) * 399);
    transform_buffer_w = malloc(sizeof(char*) * 798);
    buffer_n = malloc(sizeof(char*) * 10000);
    buffer_s = malloc(sizeof(char) * 33);

    fill_buffer_s();
    fill_buffer_n();
    fill_buffer_w();
    fill_transform_buffer_w();
}

void free_all_buffers() {
    for (int i = 0; i < 399; i++) {free(buffer_w[i]);}
    free(buffer_w);
    for (int i = 0; i < 10000; i++) {free(buffer_n[i]);}
    free(buffer_n);
    for (int i = 0; i < 798; i++) {free(transform_buffer_w[i]);}
    free(transform_buffer_w);
    free(buffer_s);
}

char* first_pwd_generation(int random_index) {
    char *improved_pwd = (char*) malloc(sizeof(char) * (strlen(buffer_w[random_index % 399]) + 6));
    strcat(strcpy(improved_pwd, buffer_w[random_index % 399]), buffer_n[random_index % 10000]);
    strncat(improved_pwd, &buffer_s[random_index % 32], 1);
    improved_pwd[strlen(improved_pwd)] = '\0';
    return improved_pwd;
}

char* pwd_from_index(int index) {
    char *improved_pwd = (char*) malloc(sizeof(char) * (strlen(transform_buffer_w[index % 798]) + 6));
    strcat(strcpy(improved_pwd, transform_buffer_w[index % 798]), buffer_n[index % 10000]);
    strncat(improved_pwd, &buffer_s[index % 32], 1);
    improved_pwd[strlen(improved_pwd)] = '\0';
    return improved_pwd;
}

int hash_to_index(char *hash_to_reduce) {
    int hashcode = 0;
    for (int i = 0; i < strlen(hash_to_reduce); i++) {
        hashcode = 31 * hashcode + hash_to_reduce[i];
    }
    return abs(hashcode);
}

char* index_to_plain(int index) {
    int new_index = index % 255360000;
    return pwd_from_index(new_index);
}

char* improved_pwd_reduction(char *hash_to_reduce) {
    int index = hash_to_index(hash_to_reduce);
    return index_to_plain(index);
}

void split_and_fill_improved_buffer(pwd_couple *current_couple, char *the_current_line) {
    const char *separator = " ";
    char *str_token_head = strtok(the_current_line, separator);
    char *str_token_tail = strtok(NULL, separator);
    struct_initialisation(current_couple, strlen(str_token_head), strlen(str_token_tail));
    strcpy(current_couple->head_pwd, str_token_head);
    current_couple->head_pwd[strlen(current_couple->head_pwd)] = '\0';
    strcpy(current_couple->tail_pwd, str_token_tail);
    current_couple->tail_pwd[strlen(current_couple->tail_pwd)] = '\0';
}

void fill_improved_buffer() {
    if (improved_pwd_file == NULL) {
        printf("Error while opening the file.");
        exit(1);
    }
    char *line_in_file = (char*) malloc(sizeof(char) * 21);
    int i = 0;
    ssize_t line_size = 0;
    size_t line_buff_size = 0;
    for (line_size = getline(&line_in_file, &line_buff_size, improved_pwd_file); line_size >= 2; line_size = getline(&line_in_file, &line_buff_size, improved_pwd_file)) {
        line_in_file[strlen(line_in_file) - 1] ='\0';
        pwd_couple improved_pwd_couple;
        split_and_fill_improved_buffer(&improved_pwd_couple, line_in_file);
        improved_buffer_of_pwd[i] = improved_pwd_couple;
        i++;
    }
    free(line_in_file);
    fclose(improved_pwd_file);
}

void improved_buffer_of_pwd_initialization() {
    improved_pwd_file = fopen("../../Files/RainbowTables/RainbowTable.txt", "r");
    improved_buffer_of_pwd = (pwd_couple*) malloc(sizeof(pwd_couple) * IMPROVED_NBR_PASSWORD);
    fill_improved_buffer();
}

void free_the_improved_buffer() {
    for (int i = 0; i < IMPROVED_NBR_PASSWORD; i++) {
        free_struct(&improved_buffer_of_pwd[i]);
    }
    free(improved_buffer_of_pwd);
}

int binary_search(int first, int last, char *to_search) {
    if (last >= first) {
        int middle = (first + last) / 2;
        if (strcmp(improved_buffer_of_pwd[middle].tail_pwd, to_search) == 0) {
            return middle;
        }
        if (strcmp(improved_buffer_of_pwd[middle].tail_pwd, to_search) > 0) {
            return binary_search(first, middle - 1, to_search);
        }
        return binary_search(middle + 1, last, to_search);
    }
    return -1;
}

void free_the_buffer_of_hash() {
    for (int i = 0; i < BUFFER_HASH_SIZE; i++) {
        free(buffer_of_hash[i]);
    }
    free(buffer_of_hash);
}

void fill_the_buffer_of_hash() {
    buffer_of_hash = malloc(sizeof(char*) * BUFFER_HASH_SIZE);
    FILE *file = fopen("Files/HashFiles/HashToCrackFile.txt", "r");
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

void fill_hash_to_crack_file() {
    initialize_buffers();
    FILE *hash_file_to_crack = fopen("Files/HashFiles/HashToCrackFile.txt", "w");
    for (int i = 0; i < BUFFER_HASH_SIZE; i++) {
        char *pwd_generated = pwd_from_index(rand());
        char *malloc_pwd_gen = (char*) malloc(sizeof(char) * (strlen(pwd_generated) + 1));
        strcpy(malloc_pwd_gen, pwd_generated);
        malloc_pwd_gen[strlen(malloc_pwd_gen)] = '\0';
        free(pwd_generated);
        char *hash_generated = (char*) malloc(sizeof(char) * 65);
        sha256_easy_hash_hex(malloc_pwd_gen, strlen(malloc_pwd_gen), hash_generated);
        hash_generated[64] = '\0';
        if (i == (BUFFER_HASH_SIZE - 1)) {
            fprintf(hash_file_to_crack, "%s", hash_generated);
        } else {
            fprintf(hash_file_to_crack, "%s\n", hash_generated);
        }
        free(malloc_pwd_gen);
        free(hash_generated);
    }
    fclose(hash_file_to_crack);
}
