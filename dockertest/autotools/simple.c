#include <criterion/criterion.h>
#include <stdio.h>

TestSuite(suite);

Test(first_suite, test)
{
    cr_assert(1);
}

Test(second_suite, test)
{
    cr_assert(1);
}

