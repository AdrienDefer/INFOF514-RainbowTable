//
// Created by Adrien Defer on 12/04/2022.
//

#include "../Headers/main.h"

#define NTHREADS_FOR_CRACK 5

FILE *result_file;

int chain_length;
int pwd_length;

int hash_cracked = 0;
int hash_failed = 0;

pthread_mutex_t resultFileMutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * @brief Revuild the chain to get the password associated to the hash.
 * 
 * @param head_pwd hash of the password
 * @param chain_construction_size number of steps in the chain to find a matching reduced password
 * @return char* 
 */
char *rebuild_chain(char *head_pwd, int chain_construction_size) {
    char *pwd_hashed = malloc(sizeof(char) * 41);
    char *hash_reduced = malloc(strlen(head_pwd) + 1);
    strcpy(hash_reduced, head_pwd);
    hash_reduced[strlen(hash_reduced)] = '\0';
    for (int i = 0; i < chain_construction_size; i++) {
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

/**
 * @brief Checks if the password is in the tail
 * 
 * @param hash_to_crack initial hash
 * @param reduced_pwd password found with this hash after X steps
 * @param step_counter number of steps done in the cahin
 * @return int 1 if found and 0 otherwise
 */
int check_if_found(char *hash_to_crack, char *reduced_pwd, int step_counter) {
    int binary_search_result = binary_search(0, line_in_file - 1, reduced_pwd);
    if (binary_search_result != -1) {
        char *pwd_found = rebuild_chain(buffer_of_couple[binary_search_result].head_pwd, chain_length + 1 - step_counter);
        char *hash_verification = (char*) malloc(sizeof(char) * 41);
        Sha1Digest sha1_computed = Sha1_get(pwd_found, strlen(pwd_found));
        Sha1Digest_toStr(&sha1_computed, hash_verification);
        hash_verification[40] = '\0';
        if (strcmp(hash_to_crack, hash_verification) == 0) {
            free(hash_verification);
            pthread_mutex_lock(&resultFileMutex);
            hash_cracked++;
            fprintf(result_file, "Password found for the hash : %s --> %s\n", hash_to_crack, pwd_found);
            printf("Password found for the hash : %s --> %s\n", hash_to_crack, pwd_found);
            pthread_mutex_unlock(&resultFileMutex);
            free(pwd_found);
            free(reduced_pwd);
            return 1;
        } else {
            return 0;
        }
    } else {
        return 0;
    }
}

/**
 * @brief Try to crack a hash with the table
 * 
 * @param hash_to_crack hash to crack
 */
void rainbow_cracking(char *hash_to_crack) {
    char *reduced_pwd = (char*) malloc(sizeof(char) * (pwd_length + 1));
    char *reduction_result = pwd_reduction(hash_to_crack, chain_length, pwd_length);
    strcpy(reduced_pwd, reduction_result);
    reduced_pwd[strlen(reduced_pwd)] = '\0';
    free(reduction_result);
    for (int i = chain_length; i >= 0; i--) {
        if (i == chain_length) {
            if (check_if_found(hash_to_crack, reduced_pwd, 0)) {
                return;
            }
        } else {
            int step_counter = 0;
            char *hashed_pwd = (char*) malloc(sizeof(char) * 41);
            strcpy(hashed_pwd, hash_to_crack);
            hashed_pwd[64] = '\0';
            for (int j = i; j < chain_length; j++) {
                char *reduction_result = pwd_reduction(hashed_pwd, j, pwd_length);
                strcpy(reduced_pwd, reduction_result);
                reduced_pwd[strlen(reduced_pwd)] = '\0';
                free(reduction_result);
                Sha1Digest sha1_computed = Sha1_get(reduced_pwd, strlen(reduced_pwd));
                Sha1Digest_toStr(&sha1_computed, hashed_pwd);
                hashed_pwd[40] = '\0';
                step_counter++;
            }
            char *reduction_result = pwd_reduction(hashed_pwd, chain_length, pwd_length);
            strcpy(reduced_pwd, reduction_result);
            reduced_pwd[strlen(reduced_pwd)] = '\0';
            free(reduction_result);
            step_counter++;
            if (check_if_found(hash_to_crack, reduced_pwd, step_counter)) {
                free(hashed_pwd);
                return;
            }
        }
    }
    free(reduced_pwd);
    pthread_mutex_lock(&resultFileMutex);
    hash_failed++;
    fprintf(result_file, "No password found for the hash : %s\n", hash_to_crack);
    printf("No password found for the hash : %s\n", hash_to_crack);
    pthread_mutex_unlock(&resultFileMutex);
}

/**
 * @brief Print a feedback of the cracking.
 * 
 */
void print_the_summary() {
    fprintf(result_file, "\nSummary for password of length -> %d :\n", pwd_length);
    fprintf(result_file, "     - Hash cracked -> %d\n", hash_cracked);
    fprintf(result_file, "     - Hash failed  -> %d\n", hash_failed);
    fprintf(result_file, "     - Accuracy     -> %f%c\n", ((float) hash_cracked / (float) BUFFER_HASH_SIZE) * 100, '%');
}

/**
 * @brief Start the cracking mechanisms
 * 
 * @param hash_to_crack hash that need to be cracked
 * @param chain_size size of the cain
 * @param pwd_size size of the password
 */
void start_cracking(char *hash_to_crack, int chain_size, int pwd_size) {
    buffer_of_couple_initialization(pwd_size);
    char filename[55];
    sprintf(filename, "../../Files/ResultFiles/ResultsFile_%d.txt", pwd_length);
    result_file = fopen(filename, "w");
    chain_length = chain_size - 1;
    pwd_length = pwd_size;
    rainbow_cracking(hash_to_crack);
    free_the_buffer_of_couple();
    print_the_summary();
    fclose(result_file);
}

/**
 * @brief Dispatch the treads on different hash
 * 
 * @param index thread index
 * @return void* 
 */
void* thread_dispaching(void * index) {
    for (int i = *(int*) index; i < BUFFER_HASH_SIZE; i = i + NTHREADS_FOR_CRACK) {
        rainbow_cracking(buffer_of_hash[i]);
    }
    free(index);
    return 0;
}

/**
 * @brief Crack passwords with the table
 * 
 * @param pwd_size size of the password
 * @param chain_size size of the chhain
 */
void mass_cracking(int pwd_size, int chain_size) {
    //Get the table
    line_in_file = count_line_in_rainbow_file(pwd_size);
    buffer_of_couple_initialization(pwd_size);
    char filename[60];
    //Initialize global variables necessary for cracking
    sprintf(filename, "../../Files/ResultFiles/ImpPwdResultsFile%d.txt", pwd_size);
    result_file = fopen(filename, "w");
    chain_length = chain_size - 1;
    pwd_length = pwd_size;
    //Get the hash to crack
    fill_the_buffer_of_hash(pwd_length);
    pthread_t thread[NTHREADS_FOR_CRACK];
    //Launch the different threads
    for (int i = 0; i < NTHREADS_FOR_CRACK; i++) {
        int *int_ptr = malloc(sizeof(int));
        *int_ptr = i;
        pthread_create(&thread[i], NULL, &thread_dispaching, int_ptr);
    }

    for (int i = 0; i < NTHREADS_FOR_CRACK; i++) {
        pthread_join(thread[i], NULL);
    }
    free_the_buffer_of_hash();
    free_the_buffer_of_couple();
    print_the_summary();
    fclose(result_file);
}