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
    char *pwd_hashed = malloc(sizeof(char) * 41);
    char *hash_reduced = malloc(strlen(head_pwd) + 1);
    strcpy(hash_reduced, head_pwd);
    hash_reduced[strlen(hash_reduced)] = '\0';
    for (int i = 0; i < chain_length; i++) {
        Sha1Digest sha1_computed = Sha1_get(hash_reduced, strlen(hash_reduced));
        Sha1Digest_toStr(&sha1_computed, pwd_hashed);
        pwd_hashed[40] = '\0';
        char *reduction_result = pwd_reduction(pwd_hashed, i, pwd_length);
        strcpy(hash_reduced, reduction_result);
        hash_reduced[strlen(hash_reduced)] = '\0';
        free(reduction_result);
    }
    free(pwd_hashed);
    return hash_reduced;
}

int check_if_already_there(char *tail_to_check, int max_index) {
    if (max_index == 0) {
        return 0;
    } else {
        for (int i = 0; i < max_index; i++) {
            if (strcmp(buffer_of_generated_pwd[i], tail_to_check) == 0) {
                return 1;
            }
        }
        return 0;
    }
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
        if (check_if_already_there(head_tail_couple.tail_pwd, pwd_generated) == 0) {
            //printf("The tail : %s (head : %s) was not already there, adding at index : %d.\n", head_tail_couple.tail_pwd, head_tail_couple.head_pwd, pwd_generated);
            buffer_of_generated_pwd[pwd_generated] = (char*) malloc(sizeof(char) * (strlen(head_tail_couple.tail_pwd) + 1));
            strcpy(buffer_of_generated_pwd[pwd_generated], head_tail_couple.tail_pwd);
            buffer_of_generated_pwd[pwd_generated][strlen(head_tail_couple.tail_pwd)] = '\0';
            pwd_generated++;
            fprintf(pwd_storage_file, "%s %s\n", head_tail_couple.head_pwd, head_tail_couple.tail_pwd);
        }
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
    buffer_of_generated_pwd = (char**) malloc(sizeof(char*) * passwd_to_generate);
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
    for (int i = 0; i < passwd_to_generate; i++) {
        free(buffer_of_generated_pwd[i]);
    }
    free(buffer_of_generated_pwd);
    fclose(pwd_file);
}