#define _POSIX_C_SOURCE 199309L
#define _LINUX_TIME_H
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#define BUFFER_SIZE 1
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

    off_t file_size = lseek(fd, 0, SEEK_END);
    if (file_size == -1) {
        perror("lseek");
        close(fd);
        return EXIT_FAILURE;
    }

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    char buffer[BUFFER_SIZE];
    volatile char c;
    for (int i = 0; i < NUM_ACCESSES; i++) {
        off_t offset = (rand() % file_size);
        if (lseek(fd, offset, SEEK_SET) == -1) {
            perror("lseek");
            close(fd);
            return EXIT_FAILURE;
        }
        if (read(fd, buffer, BUFFER_SIZE) == -1) {
            perror("read");
            close(fd);
            return EXIT_FAILURE;
        }
        c = buffer[0];
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    close(fd);

    double elapsed = (end.tv_sec - start.tv_sec) + 
                     (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("read test time: %.6f seconds\n", elapsed);

    return EXIT_SUCCESS;
}