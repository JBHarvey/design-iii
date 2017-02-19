#include <stdlib.h>
#include "Objective.h"


struct Objective * Objective_new(struct State * new_objectiveValues, struct State * new_valuesToValidate)
{
    struct Objective * pointer = (struct Objective *) malloc(sizeof(struct Objective));
    pointer->objectiveValues = new_objectiveValues;
    pointer->valuesToValidate = new_valuesToValidate;
    return pointer;
}

void Objective_delete(struct Objective * objective)
{
    State_delete(objective->objectiveValues);
    State_delete(objective->valuesToValidate);
    free(objective);
}

int Objective_isReached(struct Objective * objective, int tolerance)
{
    return 1;
}
