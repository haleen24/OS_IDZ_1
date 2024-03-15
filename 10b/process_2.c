#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../lib/analysis.c"
#include <sys/msg.h>


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

    int id1 = msgget(key1, IPC_CREAT | 0666);
    int id2 = msgget(key2, IPC_CREAT | 0666);

    if (id1 == -1 || id2 == -1) {
        printf("Failed: can not open queue");
        exit(-1);
    }

    //char buff[small_buff_size];
    char *result = NULL;

    ssize_t res_size = 0;
    ssize_t size = 0;
    msgbuf msg;
    do {
        size = msgrcv(id1, &msg, small_buff_size, 0, 0);
        if (size == -1) {
            printf("Failed: read from queue\n");
            exit(-1);
        }
        if (msg.mtype == 2) {
            break;
        }
        char *ptr = result;
        result = add(result, msg.mtext, res_size, size);
        res_size += size;
        free(ptr);

    } while (1);

    char *ptr = result;
    result = analyze(result, res_size, "", 0);
    free(ptr);

    unsigned long i = 0;
    unsigned long max_len = strlen(result);
    while (i < max_len) {
        unsigned long actual_size = (max_len - i >= small_buff_size ? small_buff_size : max_len - i);
        strncpy(msg.mtext, result + i * sizeof(char), actual_size);
        msg.mtype = 1;
        i += small_buff_size;
        msgsnd(id2, &msg, actual_size, 0);
    }
    msg.mtype = 2;
    strncpy(msg.mtext, "", 0);
    msgsnd(id2, &msg, small_buff_size, 0);
    free(result);
    return 0;
}