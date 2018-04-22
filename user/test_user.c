
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    puts("Hello World from ring 3!");
    printf("Test printf: %i %#010x\n", 10, 0x1234);

    char test[0x41] = { 0 };
    read(3 /* dev_inc */, test, 0x40);

    printf("from my inc char dev: %s\n", test + 0x20);

    char command[64] = {0};
    size_t ix = 0;
    char c;

    while (true) {

        printf("$ ");

        while (true) {
            read(4, &c, 1);

            if (c == 0x7f) { // backspace
                ix -= 1;
                command[ix] = '\0';
                printf("\x08");
                continue;
            }

            if (c == '\n') { // newline
                break;
            }

            command[ix++] = c;
            command[ix] = '\0';
            printf("%c", c);
        }

        printf("\n");

        if (ix == 0)
            continue;

        ix = 0;

        if (strncmp(command, "echo", 4) == 0) {
            printf("%s\n", command + 5);
        } else {
            printf("Command not found\n");
        }
    }

    return 0;
}

int _start() {
    int status = main();
    exit(status);
}

