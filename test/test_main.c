#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdlib.h>
#include <time.h>
#include "google/cmockery.h"
#include "voidp.h"


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

int main(void)
{
    srand(time(NULL));

    const UnitTest tests[] = {
        unit_test(test_voidp),
    };
    return run_tests(tests);
}


