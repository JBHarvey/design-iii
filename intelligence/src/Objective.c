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

int coordinatesIsWithinToleranceOfGoal(struct Objective *objective, struct State *currentState)
{
    struct Coordinates *position = Coordinates_zero();
    Coordinates_copyValuesFrom(position, currentState->pose->coordinates);

    int goalX = objective->goalState->pose->coordinates->x;
    int goalY = objective->goalState->pose->coordinates->y;
    int toleranceX = objective->tolerances->pose->coordinates->x;
    int toleranceY = objective->tolerances->pose->coordinates->y;
    struct Coordinates *northLimit = Coordinates_new(goalX, goalY + (toleranceY + 1));
    struct Coordinates *southLimit = Coordinates_new(goalX, goalY - (toleranceY + 1));
    struct Coordinates *eastLimit = Coordinates_new(goalX + (toleranceX + 1), goalY);
    struct Coordinates *westLimit = Coordinates_new(goalX - (toleranceX + 1), goalY);

    int isWithin = (Coordinates_isToTheSouthOf(position, northLimit) &&
                    Coordinates_isToTheNorthOf(position, southLimit) &&
                    Coordinates_isToTheWestOf(position, eastLimit) &&
                    Coordinates_isToTheEastOf(position, westLimit));

    Coordinates_delete(position);
    Coordinates_delete(northLimit);
    Coordinates_delete(southLimit);
    Coordinates_delete(eastLimit);
    Coordinates_delete(westLimit);

    return isWithin;
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
    int reached = coordinatesIsWithinToleranceOfGoal(objective, currentState)
                  && thetaIsWithinToleranceOfGoal(objective, currentState)
                  && flagsAreTheSame(objective, currentState);

    return reached;
}
