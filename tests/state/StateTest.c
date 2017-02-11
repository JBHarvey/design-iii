#include <criterion/criterion.h>
#include <stdio.h>
#include "State.h"

Test(states, first)
{
    int y = act(5);
    cr_assert(y == 6);
}
