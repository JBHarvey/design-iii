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

Test(sample, test)
{
    cr_expect(0, "Is this failing?");
    cr_assert(1, "Or is this?");
}

