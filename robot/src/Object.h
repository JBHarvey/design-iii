#ifndef OBJECT_H_
#define OBJECT_H_

// This include is all of the robot's constants
#include "Defines.h"

/**
 * This module is to be added as
 *    THE VERY FIRST element of
 *    EACH module that is meant
 *    to be an "Object" proper.
 *
 * Usage: 
 *    - A module's object is to be created in its constructor
 *      (the [ModuleName]_new(...) method).
 *    - In an object's construction, the method
 *      "Object_addOneReference(...)" is to be called for the object
 *      of each module that owns an object that is received in
 *      the constructor's parameters.
 *    - At the beginning of a module's destructor (the 
 *      [ModuleName]_delete(...) method), the method
 *      "Object_removeOneReference(...)" is to be called for the
 *      object whose destructor is called.
 *
 * THE ONLY EXCEPTION to this rule is the module "Object" itself.
 * Adding it to itlelf would create an infinitely recursive structure.
 * You don't want to create an infinitely recursive structure.
 */
struct Object {
    int reference_count;
};

struct Object *Object_new(void);
void Object_delete(struct Object *object);

void Object_addOneReference(struct Object *object);
void Object_removeOneReference(struct Object *object);

int Object_canBeDeleted(struct Object *object);

#endif // OBJECT_H_
