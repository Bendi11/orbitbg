#include <stdio.h>

int main(int const argc, char const *argv[]) {
    if(argc != 3) {
        printf(stderr, "Usage: %s <INPUT-PATH> <OUTPUT-PATh>\n");
        return -1;
    }

    FILE *input = fopen(argv[1], "r");
    if(input == NULL) {
        printf(stderr, "Eror: input file '%s' not found\n");
        return -1;
    }

}
