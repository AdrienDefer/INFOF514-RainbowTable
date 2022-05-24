//
// Created by Adrien Defer on 12/04/2022.
//

#include "../Headers/main.h"

#define NTHREADS 6

int pwd_generated = 0;
int pwd_to_generate;
int chain_length;
int pwd_length;

FILE *pwd_storage_file;

pthread_mutex_t fileMutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * @brief Generate the chain. Get the tail.
 * 
 * @param head_pwd initial password
 * @return char* tail of the chain
 */
char* tail_computation(char* head_pwd) {
    char *pwd_hashed = malloc(sizeof(char) * 65);
    char *hash_reduced = malloc(strlen(head_pwd) + 1);
    strcpy(hash_reduced, head_pwd);
    hash_reduced[strlen(hash_reduced)] = '\0';
    //chain size
    for (int i = 0; i < chain_length; i++) {
        //Hash the pwd
        sha256_easy_hash_hex(hash_reduced, strlen(hash_reduced), pwd_hashed);
        pwd_hashed[64] = '\0';
        //reduction function
        char *reduction_result = pwd_reduction(pwd_hashed, i, pwd_length);
        strcpy(hash_reduced, reduction_result);
        hash_reduced[strlen(hash_reduced)] = '\0';
        free(reduction_result);
    }
    free(pwd_hashed);
    return hash_reduced;
}

/**
 * @brief Create a chain of the table
 * 
 * @return void* 
 */
void* start_chain_computation() {
    while (pwd_generated < pwd_to_generate) {
        pwd_couple head_tail_couple;
        //Initialize a couple head-tail
        struct_initialisation(&head_tail_couple, pwd_length);
        //Generate initial password
        char *generated_pwd = pwd_generation(pwd_length);
        strcpy(head_tail_couple.head_pwd, generated_pwd);
        head_tail_couple.head_pwd[strlen(head_tail_couple.head_pwd)] = '\0';
        free(generated_pwd);
        //Generate the chain, get the tail
        char *tail_result = tail_computation(head_tail_couple.head_pwd);
        strcpy(head_tail_couple.tail_pwd, tail_result);
        head_tail_couple.tail_pwd[strlen(head_tail_couple.tail_pwd)] = '\0';
        free(tail_result);
        pthread_mutex_lock(&fileMutex);
        pwd_generated++;
        fprintf(pwd_storage_file, "%s %s\n", head_tail_couple.head_pwd, head_tail_couple.tail_pwd);
        pthread_mutex_unlock(&fileMutex);
        free_struct(&head_tail_couple);
    }
    pthread_exit(NULL);
}

/**
 * @brief Launch several threads to generate the table.
 * 
 */
void thread_launching(void) {
    pthread_t thread[NTHREADS];
    for (int i = 0; i < NTHREADS; i++) {
        //Set the threads to make chains of the table
        pthread_create(&(thread[i]), NULL, start_chain_computation, NULL);
    }

    for (int i = 0; i < NTHREADS; i++) {
        pthread_join(thread[i], NULL);
    }
}

/**
 * @brief Create a rainbox table.
 * 
 * @param passwd_to_generate number of password to generate
 * @param passwd_length size of the passwords
 * @param chain_size size of the chain
 */
void start_rainbow_table_creation(int passwd_to_generate, int passwd_length, int chain_size) {
    char fileName[50];
    sprintf(fileName, "Files/UnsortedTables/UnsortedTable%d.txt", passwd_length);
    pwd_storage_file = fopen(fileName, "w");
    if (pwd_storage_file == NULL) {
        printf("Error while opening the file : Files/UnsortedTables/UnsortedTableX.txt");
        exit(1);
    }
    //Initialize global variables
    pwd_to_generate = passwd_to_generate;
    chain_length = chain_size;
    pwd_length = passwd_length;
    thread_launching();
    fclose(pwd_file);
}