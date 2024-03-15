#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include "../lib/analysis.c"

const char fifo1[] = "fifo_1";
const char fifo2[] = "fifo_2";

char *add(const char *ptr1, const char *ptr2, ssize_t len1, ssize_t len2) {
    char *res = malloc(sizeof(char) * (len1 + len2 + 1));
    for (ssize_t i = 0; i < len1 + len2; ++i) {
        if (i < len1) {
            res[i] = ptr1[i];
        } else {
            res[i] = ptr2[i - len1];
        }
    }
    return res;
}

int main() {
    mknod(fifo1, S_IFIFO | 0666, 0);
    mknod(fifo2, S_IFIFO | 0666, 0);

    int fr = open(fifo1, O_RDONLY);
    if (fr == -1) {
        printf("Failed: open fifo");
        exit(-1);
    }

    char buff[small_buff_size];
    char *result = NULL;

    ssize_t res_size = 0;
    ssize_t size = 0;
    do {
        size = read(fr, buff, small_buff_size);
        if (size == -1) {
            printf("Failed: read from fifo\n");
            exit(-1);
        }
        char *ptr = result;
        result = add(result, buff, res_size, size);
        res_size += size;
        free(ptr);

    } while (size != 0);
    close(fr);

    int fw = open(fifo2, O_WRONLY);
    char *ptr = result;
    result = analyze(result, res_size, "", 0);
    free(ptr);

    write(fw, result, strlen(result));
    close(fw);
    free(result);
    return 0;
}
