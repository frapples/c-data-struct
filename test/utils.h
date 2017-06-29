
int* rand_array(int size);
int rand_int(int start, int end);

char** rand_str_array(int size);
bool str_is_in(char** strs, int size, const char* str);
bool int_is_in(int* arr, int size, int find);

int* range(int start, int end, int step);

int int_cmp(const void* a, const void* b);

static inline int max_int(int a, int b)
{
    return a > b ? a : b;
}

