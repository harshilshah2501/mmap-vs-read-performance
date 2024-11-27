#define _POSIX_C_SOURCE 199309L
#define _LINUX_TIME_H
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#define NUM_ACCESSES 10000000

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("open");
        return EXIT_FAILURE;
    }

    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        perror("fstat");
        close(fd);
        return EXIT_FAILURE;
    }

    char *file_in_memory = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (file_in_memory == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return EXIT_FAILURE;
    }

    close(fd);

    // Touch all memory pages to force page faults
    for (size_t i = 0; i < sb.st_size; i += 4096) {
        volatile char c = file_in_memory[i];
    }

    // Random access with precise timing
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    volatile char c;
    for (int i = 0; i < NUM_ACCESSES; i++) {
        size_t offset = (rand() % sb.st_size);
        c = file_in_memory[offset];
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    munmap(file_in_memory, sb.st_size);

    double elapsed = (end.tv_sec - start.tv_sec) + 
                     (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("mmap test time: %.6f seconds\n", elapsed);

    return EXIT_SUCCESS;
}