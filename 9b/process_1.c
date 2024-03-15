#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "../lib/analysis.c"

const char fifo_out[] = "fifo_1";
const char fifo_in[] = "fifo_2";

void read_write(const char *file_name, int fw) {
    if (fw == -1) {
        printf("Failed: can not open FIFO\n");
        exit(-1);
    }
    int desc;
    if ((desc = open(file_name, O_RDONLY, 0777)) == -1) {
        printf("Failed: can't open file\n");
        exit(-1);
    }
    char buff[small_buff_size];
    ssize_t size = 0;
    do {
        size = read(desc, buff, small_buff_size);
        if (size == -1) {
            close(desc);
            printf("Failed: can't read\n");
            exit(-1);
        }
        write(fw, buff, size);

    } while (size != 0);
}

void write_read(const char *file_name, const char *fifo_name) {
    char buff[small_buff_size];
    int desc = open(file_name, O_WRONLY | O_CREAT, 0777);
    int fr = open(fifo_name, O_RDONLY);
    if (fr == -1) {
        printf("Failed: can not open FIFO\n");
        exit(-1);
    }
    ssize_t size = 0;
    do {
        size = read(fr, buff, small_buff_size);
        if (size == -1) {
            close(fr);
            printf("Failed: read from fifo\n");
            exit(-1);
        }
        if (!size) {
            break;
        }
        write(desc, buff, size);
    } while (1);
    close(fr);
    close(desc);
}

int main(int argc, char *argv[]) {

    if (argc != 4) {
        printf("There should be only 3 names of files as params\n");
        exit(-1);
    }

    mknod(fifo_in, S_IFIFO | 0666, 0);
    mknod(fifo_out, S_IFIFO | 0666, 0);

    int fw = open(fifo_out, O_WRONLY);
    read_write(argv[1], fw);
    read_write(argv[2], fw);
    close(fw);

    write_read(argv[3], fifo_in);
}
