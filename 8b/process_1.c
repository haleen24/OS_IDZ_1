#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include "../lib/analysis.c"

const char fifo1[] = "data_from_file_1";
const char fifo2[] = "data_from_file_2";
const char fifo3[] = "data_for_output_file";

int main(int argc, char *argv[]) {

    if (argc != 4) {
        printf("There should be only 3 names of files as params\n");
        exit(-1);
    }

    mknod(fifo1, S_IFIFO | 0666, 0);
    mknod(fifo2, S_IFIFO | 0666, 0);
    mknod(fifo3, S_IFIFO | 0666, 0);

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

    char buff[buff_size];
    int fr3 = open(fifo3, O_RDONLY);
    if (fr3 == -1) {
        printf("Failed: can not open FIFO\n");
        exit(-1);
    }
    ssize_t size = read(fr3, buff, buff_size);
    if (size == -1) {
        printf("Failed: read from fifo\n");
        exit(-1);
    }

    int desc = open(argv[3], O_WRONLY | O_CREAT, 0777);
    write(desc, buff, size);
    close(desc);
}