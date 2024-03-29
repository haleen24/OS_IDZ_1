#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

const int buff_size = 5000;
const int small_buff_size = 128;
#define SMALL_SIZE 128

const int key1 = 2222;
const int key2 = 2220;

typedef struct {
               long mtype;       
               char mtext[SMALL_SIZE];   
} msgbuf;

char *analyze(const char *buff1, ssize_t len1, const char *buff2, ssize_t len2) {
    char *res = malloc(sizeof(char) * (buff_size + 1));

    int k = 0;
    for (int i = 0; i < len1; ++i) {
        char c = buff1[i];
        bool flag = false;
        for (int j = 0; j < k; ++j) {
            if (res[j] == c) {
                flag = true;
                break;
            }
        }
        if (!flag) {
            res[k++] = c;
        }
    }
    for (int i = 0; i < len1; ++i) {
        char c = buff1[i];
        bool flag = false;
        for (int j = 0; j < k; ++j) {
            if (res[j] == c) {
                flag = true;
                break;
            }
        }
        if (!flag) {
            res[k++] = c;
        }
    }
    for (int i = 0; i < len2; ++i) {
        char c = buff2[i];
        bool flag = false;
        for (int j = 0; j < k; ++j) {
            if (res[j] == c) {
                flag = true;
                break;
            }
        }
        if (!flag) {
            res[k++] = c;
        }
    }
    res[k] = '\0';
    return res;
}
