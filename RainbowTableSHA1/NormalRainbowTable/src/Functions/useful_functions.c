//
// Created by Adrien Defer on 12/04/2022.
//

#include "../Headers/main.h"

FILE *pwd_file;

int line_in_file;

/**
 * @brief Genrate random password.
 * 
 * @param length size of password
 * @return char* password generated
 */
char* pwd_generation(int length) {
    static const char pwd_policy[] = "azertyuiopqsdfghjklmwxcvbnAZERTYUIOPQSDFGHJKLMWXCVBN1234567890!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~";
    char *pwd = (char*) malloc(sizeof(char) * (length + 1));
    for (int i = 0; i < length; i++) {
        pwd[i] = pwd_policy[rand() % 93];
    }
    pwd[length] = '\0';
    return pwd;
}

/**
 * @brief Reduct an hash to password
 * 
 * @param hash_to_reduce hash to reduce
 * @param step step of the chain we are in
 * @param pwd_length size of the password
 * @return char* password generated, reduced hash
 */
char* pwd_reduction(char* hash_to_reduce, int step, int pwd_length) {
    static const char pwd_policy[] = "azertyuiopqsdfghjklmwxcvbnAZERTYUIOPQSDFGHJKLMWXCVBN1234567890!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~";
    char *pwd_reduced = (char*) malloc(sizeof(char) * (pwd_length + 1));
    for (int i = 0; i < pwd_length; i++) {
        pwd_reduced[i] = pwd_policy[(((hash_to_reduce[i] ^ step) + hash_to_reduce[i + 11]) % 93)];
    }
    pwd_reduced[pwd_length] = '\0';
    return pwd_reduced;
}

/**
 * @brief Return the number of lines in the table.
 * 
 * @param pwd_length size of the password
 * @return int number of lines
 */
int count_line_in_rainbow_file(int pwd_length) {
    char fileName[60];
    sprintf(fileName, "../../Files/RainbowTables/ImpPwdRainbowTable_%d.txt", pwd_length);
    FILE *file_to_count_line;
    file_to_count_line = fopen(fileName, "r");
    int count = 0;
    char c;
    for (c = getc(file_to_count_line); c != EOF; c = getc(file_to_count_line)) {
        if (c == '\n') {
            count++;
        }
    }
    return count;
}

/**
 * @brief Fill the file of the hash to crack in.
 * 
 * @param pwd_length size of the password
 */
void fill_hash_to_crack_file(int pwd_length) {
    char fileName[60];
    //sprintf(fileName, "Files/HashFiles/HashToCrackFile%d.txt", pwd_length);
    sprintf(fileName, "Files/HashFiles/HashToCrackFileForImpPwd.txt");
    FILE *hash_file_to_crack = fopen(fileName, "w");
    for (int i = 0; i < BUFFER_HASH_SIZE; i++) {
        char *pwd_generated = pwd_generation(pwd_length);
        char *hash_generated = (char*) malloc(sizeof(char) * 41);
        Sha1Digest sha1_computed = Sha1_get(pwd_generated, strlen(pwd_generated));
        Sha1Digest_toStr(&sha1_computed, hash_generated);
        hash_generated[40] = '\0';
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

/**
 * @brief Binary search of the reduced hash in the tails of the table.
 * 
 * @param first first index
 * @param last last index
 * @param to_search reduced password to search
 * @return int -1 if not found or the right index when found
 */
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

/**
 * @brief Split a line of the table into a head-tail couple.
 * 
 * @param current_couple couple to fill
 * @param the_current_line line of the table
 * @param pwd_length size of the password
 */
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

/**
 * @brief Fill the buffer that contains the couples head-tail of the table in.
 * 
 * @param pwd_length size of the password
 */
void fill_buffer_of_couple(int pwd_length) {
    if (pwd_file == NULL) {
        printf("Error while opening the file.\n");
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

/**
 * @brief Free the buffer that contains the couples head-tail of the table.
 * 
 */
void free_the_buffer_of_couple() {
    for (int i = 0; i < line_in_file; i++) {
        free_struct(&buffer_of_couple[i]);
    }
    free(buffer_of_couple);
}

/**
 * @brief Create the buffer that contains the couples head-tail of the table.
 * 
 * @param pwd_length size of the password
 */
void buffer_of_couple_initialization(int pwd_length) {
    char fileName[70];
    sprintf(fileName, "../../Files/RainbowTables/ImpPwdRainbowTable_%d.txt", pwd_length);
    printf("%s\n", fileName);
    pwd_file = fopen(fileName, "r");
    buffer_of_couple = (pwd_couple*) malloc(sizeof(pwd_couple) * line_in_file);
    fill_buffer_of_couple(pwd_length);
}

/**
 * @brief Free the buffer that contains the hash to crack.
 * 
 */
void free_the_buffer_of_hash() {
    for (int i = 0; i < BUFFER_HASH_SIZE; i++) {
        free(buffer_of_hash[i]);
    }
    free(buffer_of_hash);
}

/**
 * @brief Fill the buffer with hash to crack from the file.
 * 
 * @param pwd_length size of the password
 */
void fill_the_buffer_of_hash(int pwd_length) {
    char fileName[50];
    //sprintf(fileName, "Files/HashFiles/HashToCrackFile%d.txt", pwd_length);
    sprintf(fileName, "Files/HashFiles/HashToCrackFileForImpPwd.txt");
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

