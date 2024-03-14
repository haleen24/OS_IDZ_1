#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include "../lib/analysis.c"

int main(int argc, char *argv[]) {

    int fd1[2];
    int fd2[2];
    int fd3[2];
    if (argc != 4) {
        printf("There should be only 3 names of files as params\n");
        exit(-1);
    }
    if (pipe(fd1) || pipe(fd2) || pipe(fd3)) {
        printf("Pipe failed\n");
        exit(-1);
    }

    ssize_t pid = fork();

    if (pid > 0) {
        int desc1;
        int desc2;
        close(fd1[0]);
        close(fd2[0]);
        if ((desc1 = open(argv[1], O_RDONLY, 0777)) == -1 || (desc2 = open(argv[2], O_RDONLY, 0777)) == -1) {
            printf("Failed: can't open file\n");
            exit(-1);
        }
        char buff1[buff_size];
        char buff2[buff_size];
        ssize_t size1 = read(desc1, buff1, buff_size);
        ssize_t size2 = read(desc2, buff2, buff_size);
        if (size1 == -1 || size2 == -1) {
            printf("Failed: can't read\n");
            exit(-1);
        }
        write(fd1[1], buff1, size1);
        write(fd2[1], buff2, size2);
        close(fd1[1]);
        close(fd2[1]);
        close(desc1);
        close(desc2);

    } else {
        if (close(fd1[1]) || close(fd2[1])) {
            printf("Pipe close failed\n");
            exit(-1);
        }

        close(fd3[0]);
        close(fd1[1]);
        close(fd2[1]);

        char buff1[buff_size];
        char buff2[buff_size];
        ssize_t size1 = read(fd1[0], buff1, buff_size);
        ssize_t size2 = read(fd2[0], buff2, buff_size);

        close(fd1[0]);
        close(fd2[0]);

        if (size1 == -1 || size2 == -1) {
            printf("Failed: read from pipe\n");
            exit(-1);
        }
        char *res_buff = analyze(buff1, size1, buff2, size2);
        write(fd3[1], res_buff, strlen(res_buff));
        close(fd3[1]);
        free(res_buff);
        return 0;
    }
    close(fd3[1]);
    char buff[buff_size];
    ssize_t size = read(fd3[0], buff, buff_size);
    if (size == -1) {
        printf("Failed: read from pipe\n");
        exit(-1);
    }
    int desc = open(argv[3], O_WRONLY | O_CREAT, 0777);

    if (desc == -1) {
        printf("Failed: can not open file\n");
        exit(-1);
    }

    write(desc, buff, size);
    close(desc);
    return 0;
}