#include <stdlib.h>
#include "Objective.h"

struct Objective * Objective_new(struct State * new_goalState, struct State * new_tolerances)
{
    struct Objective * pointer = (struct Objective *) malloc(sizeof(struct Objective));
    pointer->goalState = new_goalState;
    pointer->tolerances = new_tolerances;
    return pointer;
}

void Objective_delete(struct Objective * objective)
{
    State_delete(objective->goalState);
    State_delete(objective->tolerances);
    free(objective);
}

int withinWithTolerance(int value, int goal, int tolerance)
{
    return (value < goal + tolerance && value > goal - tolerance);
}

int xIsWithinToleranceOfGoal(struct Objective * objective, struct State * currentState)
{
    int currentX = currentState->pose->x;
    int goalX = objective->goalState->pose->x;

    return withinWithTolerance(currentX, goalX, X_TOLERANCE_DEFAULT);
}

int Objective_isReached(struct Objective * objective, struct State * currentState)
{
    int reached = xIsWithinToleranceOfGoal(objective, currentState);

    return reached;
}
