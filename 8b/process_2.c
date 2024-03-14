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

int main() {

    char buff1[buff_size];
    char buff2[buff_size];

    mknod(fifo1, S_IFIFO | 0666, 0);
    mknod(fifo2, S_IFIFO | 0666, 0);
    mknod(fifo3, S_IFIFO | 0666, 0);
    int fr1 = open(fifo1, O_RDONLY);
    int fr2 = open(fifo2, O_RDONLY);
    if (fr1 == -1 || fr2 == -1) {
        printf("Failed: open fifo");
        exit(-1);
    }
    ssize_t size1 = read(fr1, buff1, buff_size);
    ssize_t size2 = read(fr2, buff2, buff_size);
    if (size1 == -1 || size2 == -1) {
        printf("Failed: read from fifo\n");
        exit(-1);
    }
    int fw3 = open(fifo3, O_WRONLY);
    char *res_buff = analyze(buff1, size1, buff2, size2);
    write(fw3, res_buff, strlen(res_buff));
    close(fw3);
    free(res_buff);
    close(fr1);
    close(fr2);
    return 0;

}