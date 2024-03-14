#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include "../lib/analysis.c"

int main(int argc, char *argv[]) {

    char fifo1[] = "data_from_file_1";
    char fifo2[] = "data_from_file_2";
    char fifo3[] = "data_for_output_file";

    if (argc != 4) {
        printf("There should be only 3 names of files as params\n");
        exit(-1);
    }
    unlink(fifo1);
    unlink(fifo2);
    unlink(fifo3);
    if (mkfifo(fifo1, 0777) * mkfifo(fifo2, 0777) * mkfifo(fifo3, 0777)) {
        printf("Fifo failed\n");
        exit(-1);
    }

    ssize_t pid = fork();

    if (pid > 0) {
        int f1w = open(fifo1, O_WRONLY);
        int f2w = open(fifo2, O_WRONLY);
        if (f1w == -1 || f2w == -1) {
            printf("Failed: can not open FIFO\n");
            exit(-1);
        }
        int desc1;
        int desc2;
        if ((desc1 = open(argv[1], O_RDONLY, 0777)) == -1 || (desc2 = open(argv[2], O_RDONLY, 0777)) == -1) {
            printf("Failed: can't open file\n");
            exit(-1);
        }
        char buff1[buff_size];
        char buff2[buff_size];
        ssize_t size1 = read(desc1, buff1, buff_size);
        ssize_t size2 = read(desc2, buff2, buff_size);
        //printf("%s\n%s", buff1, buff2);
        if (size1 == -1 || size2 == -1) {
            printf("Failed: can't read\n");
            exit(-1);
        }
        write(f1w, buff1, size1);
        write(f2w, buff2, size2);
        close(f1w);
        close(f2w);
        close(desc1);
        close(desc2);
        return 0;
    }

    if (fork() > 0) {
        char buff1[buff_size];
        char buff2[buff_size];
        int fr1 = open(fifo1, O_RDONLY);
        int fr2 = open(fifo2, O_RDONLY);
        ssize_t size1 = read(fr1, buff1, buff_size);
        ssize_t size2 = read(fr2, buff2, buff_size);
        //printf("%s\n%s\n", buff1, buff2);
        if (size1 == -1 || size2 == -1) {
            printf("Failed: read from pipe\n");
            exit(-1);
        }
        int fw3 = open(fifo3, O_WRONLY);
        char *res_buff = analyze(buff1, size1, buff2, size2);
        //printf("%s", res_buff);
        write(fw3, res_buff, strlen(res_buff));
        close(fw3);
        free(res_buff);
        close(fr1);
        close(fr2);
        return 0;
    } else {
        char buff[buff_size];
        int fr3 = open(fifo3, O_RDONLY);
        if (fr3 == -1) {
            printf("Failed: can not open FIFO\n");
            exit(-1);
        }
        ssize_t size = read(fr3, buff, buff_size);
        printf("%zd", size);
        if (size == -1) {
            printf("Failed: read from pipe\n");
            exit(-1);
        }

        int desc = open(argv[3], O_WRONLY | O_CREAT, 0777);
        write(desc, buff, size);
        close(desc);
    }
    exit(0);

}