#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

const int buff_size = 5000;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        exit(-1);
    }

    char buff[buff_size];

    read(0, buff, buff_size);

    write(1, buff, buff_size);

    return 0;


}
