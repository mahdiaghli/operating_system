/*
Mahdi Aghli 4001262591
Mohammad Amin Hatefi 4001262615
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/mman.h>

#define MAX_FILES 100
#define MAX_LINE_LENGTH 256

// Structure to hold file information and search pattern
typedef struct {
    char* files[MAX_FILES];
    int file_count;
    char* pattern;
    pthread_mutex_t mutex;
    sem_t sem;
} ProcessData;

// Structure to hold each thread information witch is passed to the process_file function
typedef  struct {
    int* matchedFile;
    ProcessData* data;
    int i;
    FILE* file;
} ThreadResult;

void *process_file(ThreadResult *args) {

    char line[MAX_LINE_LENGTH];
    char *charnum;
    int line_number = 0;

    while (fgets(line, sizeof(line), args->file) != NULL) {
        line_number++;

        // Check if the line contains the specified pattern
        if ((charnum = strstr(line, args->data->pattern)) != NULL) {
            // Use lock for synchronized printing
            pthread_mutex_lock(&args->data->mutex);
            sem_wait(&args->data->sem);
            printf("%s:%d:%d: %s", args->data->files[args->i], line_number, (int)(charnum - line) + 1, line);
            (*args->matchedFile)++;
            sem_post(&args->data->sem);
            pthread_mutex_unlock(&args->data->mutex);
        }
    }
    fclose(args->file);
}

// Function to process files (lines containing the specified pattern)
void process_files_with_thread(ProcessData* data,  int* checked_files, int* matched_files) {
    pthread_t process_file_thread[data->file_count];
    ThreadResult thread_results[data->file_count];

    for (int i = 0; i < data->file_count; ++i) {
        FILE* file = fopen(data->files[i], "r");
        if (file == NULL) {
            perror("Error opening file");
            continue;
        }
        pthread_mutex_lock(&data->mutex);
        sem_wait(&data->sem);
        (*checked_files)++;
        sem_post(&data->sem);
        pthread_mutex_unlock(&data->mutex);

        thread_results[i].i = i;
        thread_results[i].matchedFile = matched_files;
        thread_results[i].data = data;
        thread_results[i].file = file;

        pthread_create(&process_file_thread[i], NULL, process_file, &thread_results[i]);
    }
    for (int i = 0; i < data->file_count; ++i) {
        pthread_join(process_file_thread[i], NULL);
    }


}

// Recursive function to traverse directories
void traverse_directory(const char* path, ProcessData* data, int* checked_files, int* matched_files) {
    DIR* dir = opendir(path);
    struct dirent* entry;

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {

            // Regular file - add to the list
            char file_path[256];
            snprintf(file_path, sizeof(file_path), "%s/%s", path, entry->d_name);
            data->files[data->file_count++] = strdup(file_path);

        } else if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {

            // Directory - process recursively
            char subdir_path[256];
            snprintf(subdir_path, sizeof(subdir_path), "%s/%s", path, entry->d_name);
            pid_t child_pid = fork();

            if (child_pid == 0) {  // Child process
                traverse_directory(subdir_path, data, checked_files, matched_files);
                process_files_with_thread(data, checked_files, matched_files);
                exit(0);  // Child process exits after processing
            } else if (child_pid < 0) {
                perror("Fork failed");
            }
        }
    }

    closedir(dir);
}

int main(int argc, char* argv[]) {
    const char* root_directory = argv[1];
    const char* search_pattern = argv[2];  // Replace with your desired pattern

    ProcessData main_data;

    int* checked_files = (int*) mmap(NULL, sizeof (int) , PROT_READ | PROT_WRITE,
                         MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    int* matched_files = (int*) mmap(NULL, sizeof (int) , PROT_READ | PROT_WRITE,
                                     MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    *checked_files = 0;
    *matched_files = 0;

    main_data.file_count = 0;
    main_data.pattern = (char*)search_pattern;
    pthread_mutex_init(&main_data.mutex, NULL);  // Initialize mutex
    sem_init(&main_data.sem, 0, 1);  // Initialize semaphore

    traverse_directory(root_directory, &main_data, checked_files, matched_files);

    // Wait for all child processes to finish
    while (wait(NULL) > 0);

    // Process files in the main process
    process_files_with_thread(&main_data, checked_files, matched_files);

    // Clean up
    pthread_mutex_destroy(&main_data.mutex);  // Destroy mutex
    sem_destroy(&main_data.sem);  // Destroy semaphore

    printf("Total number of files: %d\nTotal number of Matches: %d\n", *checked_files, *matched_files);
    return 0;
}