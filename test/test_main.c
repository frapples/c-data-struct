#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "google/cmockery.h"
#include "voidp.h"
#include "ccstr.h"

void test_linked_list(void **state);
void test_array_list(void **state);

void test_voidp(void **state)
{
    int count = 100;
    while (count-- > 0) {
        long n = rand();
        assert_true(n == (voidp2long(long2voidp(n))));

        bool b = n % 2;
        assert_true(b == (voidp2bool(bool2voidp(b))));
    }
}

void test_ccstr(void **state)
{
    int count = 100;
    while (count-- > 0) {
        const char* cstr = "this is a string";
        ccstr_t str = ccstr_create(cstr);
        assert_int_equal(strlen(cstr), ccstr_len(str));
        assert_string_equal(cstr, str);
    }


    count = 100;
    while (count-- > 0) {
        const char* real = "this is a string";
        ccstr_t str = ccstr_concat("this ", "is ", "a ", "string", NULL);
        assert_int_equal(strlen(real), ccstr_len(str));
        assert_string_equal(real, str);
    }

}


int main(void)
{
    srand(time(NULL));

    const UnitTest tests[] = {
        unit_test(test_voidp),
        unit_test(test_ccstr),
        unit_test(test_linked_list),
        unit_test(test_array_list)
    };
    return run_tests(tests);
}


