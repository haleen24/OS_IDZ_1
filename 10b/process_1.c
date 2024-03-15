#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include "../lib/analysis.c"
#include <sys/msg.h>
#include <string.h>


void read_write(const char *file_name, int id) {
    if (id == -1) {
        printf("Failed: can not get queue\n");
        exit(-1);
    }
    int desc;
    if ((desc = open(file_name, O_RDONLY, 0777)) == -1) {
        printf("Failed: can't open file\n");
        exit(-1);
    }
    char buff[small_buff_size];
    ssize_t size = 0;
    msgbuf msg;
    do {
        size = read(desc, buff, small_buff_size);
        if (size == -1) {
            close(desc);
            printf("Failed: can't read\n");
            exit(-1);
        }
        strncpy(msg.mtext, buff, size);
        msg.mtype = 1;
        if (msgsnd(id, &msg, size, 0) == -1) {
            printf("Failed: can not send message\n");
            exit(-1);
        }
    } while (size != 0);
}

void write_read(const char *file_name, int id) {
    int desc = open(file_name, O_WRONLY | O_CREAT, 0777);
    if (id == -1) {
        printf("Failed: can not get queue\n");
        exit(-1);
    }
    ssize_t size;
    do {
        msgbuf msg;
        size = msgrcv(id, &msg, small_buff_size, 0, 0);
        if (size == -1) {
            printf("Failed: read from queue\n");
            exit(-1);
        }
        if (msg.mtype == 2) {
            break;
        }
        if (size == 0) {
            continue;
        }
        write(desc, msg.mtext, size);
    } while (1);
    close(desc);
}

int main(int argc, char *argv[]) {

    if (argc != 4) {
        printf("There should be only 3 names of files as params\n");
        exit(-1);
    }

    int id1 = msgget(key1, IPC_CREAT | 0666);
    int id2 = msgget(key2, IPC_CREAT | 0666);

    read_write(argv[1], id1);
    read_write(argv[2], id1);

    msgbuf msg;
    msg.mtype = 2;
    msgsnd(id1, &msg, small_buff_size, 0);

    write_read(argv[3], id2);

    if (msgctl(id1, IPC_RMID, NULL) < 0 || msgctl(id2, IPC_RMID, NULL)) {
        printf("Failed: can not delete queue\n");
    }
}