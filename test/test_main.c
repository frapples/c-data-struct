#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include "google/cmockery.h"
#include "voidp.h"


void test_voidp(void **state)
{
    assert_true(true);
}

int main(void)
{
    const UnitTest tests[] = {
            unit_test(test_voidp),
    };
    return run_tests(tests);
}


