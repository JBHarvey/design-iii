#include <criterion/criterion.h>
#include <stdio.h>
#include "Object.h"

struct Object *object;
const int ONE = 1;
const int INITIAL_COUNT = 1;
/*
void setupObject(void)
{
    object = Object_new();
}

void teardownObject(void)
{
    Object_delete(object);
}

Test(Object, creation_destruction
     , .init = setupObject
     , .fini = teardownObject)
{
    cr_assert(object->referenceCount == INITIAL_COUNT);
}

Test(Object, given_anObject_when_addOneReference_then_referenceCountGoesUpByOne
     , .init = setupObject
     , .fini = teardownObject)
{
    Object_addOneReference(object);
    cr_assert(object->referenceCount == INITIAL_COUNT + ONE);
}

Test(Object, given_anObject_when_removeOneReference_then_referenceCountGoesDownByOne
     , .init = setupObject
     , .fini = teardownObject)
{
    Object_removeOneReference(object);
    cr_assert(object->referenceCount == INITIAL_COUNT - ONE);
}

Test(Object, given_anObjectWithReferenceCountEqualToZero_when_checksIfCanBeDeleted_then_returnsTrue
     , .init = setupObject
     , .fini = teardownObject)
{
    Object_removeOneReference(object);
    int canBeDeleted = Object_canBeDeleted(object);
    cr_assert(canBeDeleted == ONE);
}
*/
