#include <criterion/criterion.h>
#include <stdio.h>
#include "Object.h"

struct Object *object;
const int ONE = 1;
const int INITIAL_COUNT = 1;

void setup_object(void)
{
    object = Object_new();
}

void teardown_object(void)
{
    Object_delete(object);
}

Test(Object, creation_destruction
     , .init = setup_object
     , .fini = teardown_object)
{
    cr_assert(object->reference_count == INITIAL_COUNT);
}

Test(Object, given_anObject_when_addOneReference_then_referenceCountGoesUpByOne
     , .init = setup_object
     , .fini = teardown_object)
{
    Object_addOneReference(object);
    cr_assert(object->reference_count == INITIAL_COUNT + ONE);
}

Test(Object, given_anObject_when_removeOneReference_then_referenceCountGoesDownByOne
     , .init = setup_object
     , .fini = teardown_object)
{
    Object_removeOneReference(object);
    cr_assert(object->reference_count == INITIAL_COUNT - ONE);
}

Test(Object, given_anObjectWithReferenceCountEqualToZero_when_checksIfCanBeDeleted_then_returnsTrue
     , .init = setup_object
     , .fini = teardown_object)
{
    Object_removeOneReference(object);
    int can_be_deleted = Object_canBeDeleted(object);
    cr_assert(can_be_deleted == ONE);
}
