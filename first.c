
int main() {
    const char* root_directory = "/Users/amin/Documents/دانشگاه/۵.سیستم عامل/grep/testfiles";
    const char* search_pattern = "hello";  // Replace with your desired pattern

    ProcessData main_data;

    int* checked_files = (int*) mmap(NULL, sizeof (int) , PROT_READ | PROT_WRITE,
                         MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    int* matched_files = (int*) mmap(NULL, sizeof (int) , PROT_READ | PROT_WRITE,
                                     MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    *checked_files = 0;
    *matched_files = 0;
//    (*checked_files)++;


//    Result result;
//    result.checkedFiles = 0;
//    result.matchedFile = 0;

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

    printf("Total number of files: %d\nTotal number of Matches: %d", *checked_files, *matched_files);

    return 0;
}