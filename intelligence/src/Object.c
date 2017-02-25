#include <stdlib.h>
#include <stdio.h>
#include "Object.h"


struct Object *Object_new(void)
{
    struct Object *pointer = (struct Object *) malloc(sizeof(struct Object));
    pointer->referenceCount = 1;
    return pointer;
}

void Object_delete(struct Object *object)
{
    free(object);
}

void Object_addOneReference(struct Object *object)
{
    ++object->referenceCount;
}

void Object_removeOneReference(struct Object *object)
{
    --object->referenceCount;
}

int Object_canBeDeleted(struct Object *object)
{
    return object->referenceCount == 0;
}
