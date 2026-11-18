#include <stdio.h>
#include "config.h"

#define HELLO "original"

int main(int argc, char** argv) {
    printf("hello %s, %s!\n", HELLO, VERSION);
    return 0;
}
