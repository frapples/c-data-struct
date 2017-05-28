#include <stdlib.h>

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
