#include <criterion/criterion.h>
#include <stdio.h>
#include "ManchesterCode.h"

struct ManchesterCode *manchester_code;

const int MANCHESTER_CODE_PAINTING_NUMBER = 0;
const int MANCHESTER_CODE_SCALE_FACTOR = TIMES_TWO;
const enum CardinalDirection MANCHESTER_CODE_ORIENTATION = EAST;

void setup_manchester_code(void)
{
    manchester_code = ManchesterCode_new();
}

void teardown_manchester_code(void)
{
    ManchesterCode_delete(manchester_code);
}

Test(ManchesterCode, creation_destruction
     , .init = setup_manchester_code
     , .fini = teardown_manchester_code)
{
    cr_assert_eq(manchester_code->painting_number, MANCHESTER_CODE_PAINTING_NUMBER);
    cr_assert_eq(manchester_code->scale_factor, MANCHESTER_CODE_SCALE_FACTOR);
    cr_assert_eq(manchester_code->orientation, MANCHESTER_CODE_ORIENTATION);
}

Test(ManchesterCode,
     given_aNewPaintingNumberScaleFactorAndOrientation_when_updatesManchesterCodeInformation_then_theCorrespondingValuesAreUpdated
     , .init = setup_manchester_code
     , .fini = teardown_manchester_code)
{
    int new_painting_number = 7;
    int new_scale_factor = TIMES_FOUR;
    enum CardinalDirection new_orientation = WEST;

    ManchesterCode_updateCodeValues(manchester_code, new_painting_number, new_scale_factor, new_orientation);

    cr_assert_eq(manchester_code->painting_number, new_painting_number);
    cr_assert_eq(manchester_code->scale_factor, new_scale_factor);
    cr_assert_eq(manchester_code->orientation, new_orientation);
}
