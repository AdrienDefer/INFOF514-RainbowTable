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

char* tail_computation(char* head_pwd) {
    char *pwd_hashed = malloc(sizeof(char) * 65);
    char *hash_reduced = malloc(strlen(head_pwd) + 1);
    strcpy(hash_reduced, head_pwd);
    hash_reduced[strlen(hash_reduced)] = '\0';
    for (int i = 0; i < chain_length; i++) {
        sha256_easy_hash_hex(hash_reduced, strlen(hash_reduced), pwd_hashed);
        pwd_hashed[64] = '\0';
        char *reduction_result = pwd_reduction(pwd_hashed, i, pwd_length);
        strcpy(hash_reduced, reduction_result);
        hash_reduced[strlen(hash_reduced)] = '\0';
        free(reduction_result);
    }
    free(pwd_hashed);
    return hash_reduced;
}

void* start_chain_computation() {
    while (pwd_generated < pwd_to_generate) {
        pwd_couple head_tail_couple;
        struct_initialisation(&head_tail_couple, pwd_length);
        char *generated_pwd = pwd_generation(pwd_length);
        strcpy(head_tail_couple.head_pwd, generated_pwd);
        head_tail_couple.head_pwd[strlen(head_tail_couple.head_pwd)] = '\0';
        free(generated_pwd);
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

void thread_launching(void) {
    pthread_t thread[NTHREADS];
    for (int i = 0; i < NTHREADS; i++) {
        pthread_create(&(thread[i]), NULL, start_chain_computation, NULL);
    }

    for (int i = 0; i < NTHREADS; i++) {
        pthread_join(thread[i], NULL);
    }
}

void start_rainbow_table_creation(int passwd_to_generate, int passwd_length, int chain_size) {
    char fileName[50];
    sprintf(fileName, "Files/UnsortedTables/UnsortedTable%d.txt", passwd_length);
    pwd_storage_file = fopen(fileName, "w");
    if (pwd_storage_file == NULL) {
        printf("Error while opening the file : Files/UnsortedTables/UnsortedTableX.txt");
        exit(1);
    }
    pwd_to_generate = passwd_to_generate;
    chain_length = chain_size;
    pwd_length = passwd_length;
    thread_launching();
    fclose(pwd_file);
}