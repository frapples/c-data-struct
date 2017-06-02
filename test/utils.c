#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "utils.h"

int* rand_array(int size)
{
    int* a = malloc(sizeof(int) * size);

    for (int i = 0; i < size; i++) {
        a[i] = rand();
    }
    return a;

}

int rand_int(int start, int end)
{
    return rand() % (end - start) + start;
}

char** rand_str_array(int size)
{
    char** strs = malloc(sizeof(char *) * size);

    int i = 0;
    while (i < size) {

        int len = rand_int(5, 20);
        strs[i] = malloc(sizeof(char) * (len + 1));
        for (char* s = strs[i]; s - strs[i] < len; s++) {
            *s = rand_int('a', 'z' + 1);
        }
        strs[i][len] = '\0';

        if (!str_is_in(strs, i, strs[i])) {
            i++;
        }
    }
    return strs;
}

bool str_is_in(char** strs, int size, const char* str)
{
    for (int i = 0; i < size; i++) {
        if (strcmp(strs[i], str) == 0) {
            return true;
        }
    }
    return false;
}
