#include <stdlib.h>
#include "Objective.h"

struct Objective *Objective_new(struct State *new_goalState, struct State *new_tolerances)
{
    struct Object *new_object = Object_new();
    struct Objective *pointer = (struct Objective *) malloc(sizeof(struct Objective));

    pointer->object = new_object;
    pointer->goalState = new_goalState;
    pointer->tolerances = new_tolerances;

    Object_addOneReference(new_goalState->object);
    Object_addOneReference(new_tolerances->object);

    return pointer;
}

void Objective_delete(struct Objective *objective)
{
    Object_removeOneReference(objective->object);

    if(Object_canBeDeleted(objective->object)) {
        Object_delete(objective->object);
        State_delete(objective->goalState);
        State_delete(objective->tolerances);

        free(objective);
    }
}

int withinWithTolerance(int value, int goal, int tolerance)
{
    return (value < goal + tolerance && value > goal - tolerance);
}

int xIsWithinToleranceOfGoal(struct Objective *objective, struct State *currentState)
{
    int currentX = currentState->pose->coordinates->x;
    int goalX = objective->goalState->pose->coordinates->x;

    return withinWithTolerance(currentX, goalX, X_TOLERANCE_DEFAULT);
}

int yIsWithinToleranceOfGoal(struct Objective *objective, struct State *currentState)
{
    int currentY = currentState->pose->coordinates->y;
    int goalY = objective->goalState->pose->coordinates->y;

    return withinWithTolerance(currentY, goalY, Y_TOLERANCE_DEFAULT);
}

int thetaIsWithinToleranceOfGoal(struct Objective *objective, struct State *currentState)
{
    struct Angle *currentAngle = currentState->pose->angle;
    struct Angle *goalAngle = objective->goalState->pose->angle;

    int distance = Angle_smallestAngleBetween(goalAngle, currentAngle);
    return (distance <= THETA_TOLERANCE_DEFAULT);
}

int flagsAreTheSame(struct Objective *objective, struct State *currentState)
{
    return (Flags_haveTheSameValues(objective->goalState->flags, currentState->flags));
}


int Objective_isReached(struct Objective *objective, struct State *currentState)
{
    int reached = xIsWithinToleranceOfGoal(objective, currentState)
                  && yIsWithinToleranceOfGoal(objective, currentState)
                  && thetaIsWithinToleranceOfGoal(objective, currentState)
                  && flagsAreTheSame(objective, currentState);

    return reached;
}
