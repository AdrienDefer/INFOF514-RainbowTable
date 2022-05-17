//
// Created by Adrien Defer on 12/04/2022.
//

#include "../Headers/main.h"

#define NTHREADS 6

int pwd_generated = 0;
int pwd_to_generate;
int chain_length;

FILE *pwd_storage_file;

pthread_mutex_t fileMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t randMutex = PTHREAD_MUTEX_INITIALIZER;

int tail_computation(char *improved_head_pwd) {
    char *pwd_hashed = malloc(sizeof(char) * 41);
    char *hash_reduced = malloc(sizeof(char) * (strlen(improved_head_pwd) + 1));
    strcpy(hash_reduced, improved_head_pwd);
    hash_reduced[strlen(hash_reduced)] = '\0';
    for (int i = 0; i < chain_length - 1; i++) {
        Sha1Digest sha1_computed = Sha1_get(hash_reduced, strlen(hash_reduced));
        Sha1Digest_toStr(&sha1_computed, pwd_hashed);
        pwd_hashed[40] = '\0';
        char *reduction_result = improved_pwd_reduction(pwd_hashed);
        hash_reduced = realloc(hash_reduced, sizeof(char) * (strlen(reduction_result) + 1));
        strcpy(hash_reduced, reduction_result);
        hash_reduced[strlen(hash_reduced)] = '\0';
        free(reduction_result);
    }
    Sha1Digest sha1_computed = Sha1_get(hash_reduced, strlen(hash_reduced));
    Sha1Digest_toStr(&sha1_computed, pwd_hashed);
    pwd_hashed[40] = '\0';
    int tail_pwd_index = hash_to_index(pwd_hashed);
    free(hash_reduced);
    free(pwd_hashed);
    return tail_pwd_index;
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

void* chain_generation() {
    while (pwd_generated < pwd_to_generate) {
        improved_pwd_couple_int improved_couple;
        pthread_mutex_lock(&randMutex);
        int random_nbr = rand();
        pthread_mutex_unlock(&randMutex);
        improved_couple.head_pwd = random_nbr;
        char *improved_pwd = first_pwd_generation(random_nbr);
        improved_couple.tail_pwd = tail_computation(improved_pwd);
        free(improved_pwd);
        pthread_mutex_lock(&fileMutex);
        fprintf(pwd_storage_file, "%s %s\n", first_pwd_generation(improved_couple.head_pwd), index_to_plain(improved_couple.tail_pwd));
        pwd_generated++;
        // if (check_if_already_there(index_to_plain(improved_couple.tail_pwd), pwd_generated) == 0) {
        //     printf("The tail : %s was not already there, adding at index :%d.\n", index_to_plain(improved_couple.tail_pwd), pwd_generated);
        //     buffer_of_generated_pwd[pwd_generated] = (char*) malloc(sizeof(char) * (strlen(index_to_plain(improved_couple.tail_pwd)) + 1));
        //     strcpy(buffer_of_generated_pwd[pwd_generated], index_to_plain(improved_couple.tail_pwd));
        //     buffer_of_generated_pwd[pwd_generated][strlen(index_to_plain(improved_couple.tail_pwd))] = '\0';
        //     pwd_generated++;
        //     fprintf(pwd_storage_file, "%s %s\n", first_pwd_generation(improved_couple.head_pwd), index_to_plain(improved_couple.tail_pwd));
        // }
        pthread_mutex_unlock(&fileMutex);
    }
    pthread_exit(NULL);
}

void thread_launching(void) {
    pthread_t thread[NTHREADS];
    for (int i = 0; i < NTHREADS; i++) {
        pthread_create(&(thread[i]), NULL, chain_generation, NULL);
    }

    for (int i = 0; i < NTHREADS; i++) {
        pthread_join(thread[i], NULL);
    }
}

void start_rainbow_table_creation(int passwd_to_generate, int chain_size) {
    srand(time(NULL));
    initialize_buffers();
    //buffer_of_generated_pwd = (char**) malloc(sizeof(char*) * passwd_to_generate);
    pwd_storage_file = fopen("Files/UnsortedTables/UnsortedRainbowTable.txt", "w");
    if (pwd_storage_file == NULL) {
        printf("Error while opening the file : Files/UnsortedTables/UnsortedRainbowTable.txt.\n");
        exit(1);
    }
    pwd_to_generate = passwd_to_generate;
    chain_length = chain_size;
    thread_launching();
    //for (int i = 0; i < passwd_to_generate; i++) {
        //free(buffer_of_generated_pwd[i]);
    //}
    //free(buffer_of_generated_pwd);
    free_all_buffers();
    fclose(pwd_storage_file);
}