#include <stdlib.h>
#include "Objective.h"

struct Objective *Objective_new(struct State *new_goal_state, struct State *new_tolerances)
{
    struct Object *new_object = Object_new();
    struct Objective *pointer =  malloc(sizeof(struct Objective));

    pointer->object = new_object;
    pointer->goal_state = new_goal_state;
    pointer->tolerances = new_tolerances;

    Object_addOneReference(new_goal_state->object);
    Object_addOneReference(new_tolerances->object);

    return pointer;
}

void Objective_delete(struct Objective *objective)
{
    Object_removeOneReference(objective->object);

    if(Object_canBeDeleted(objective->object)) {
        Object_delete(objective->object);
        State_delete(objective->goal_state);
        State_delete(objective->tolerances);

        free(objective);
    }
}

static int coordinatesIsWithinToleranceOfGoal(struct Objective *objective, struct State *current_state)
{
    struct Coordinates *position = Coordinates_zero();
    Coordinates_copyValuesFrom(position, current_state->pose->coordinates);

    int goalX = objective->goal_state->pose->coordinates->x;
    int goalY = objective->goal_state->pose->coordinates->y;
    int toleranceX = objective->tolerances->pose->coordinates->x;
    int toleranceY = objective->tolerances->pose->coordinates->y;
    struct Coordinates *north_limit = Coordinates_new(goalX, goalY + (toleranceY + 1));
    struct Coordinates *south_limit = Coordinates_new(goalX, goalY - (toleranceY + 1));
    struct Coordinates *east_limit = Coordinates_new(goalX + (toleranceX + 1), goalY);
    struct Coordinates *west_limit = Coordinates_new(goalX - (toleranceX + 1), goalY);

    int is_within = (Coordinates_isToTheSouthOf(position, north_limit) &&
                     Coordinates_isToTheNorthOf(position, south_limit) &&
                     Coordinates_isToTheWestOf(position, east_limit) &&
                     Coordinates_isToTheEastOf(position, west_limit));

    Coordinates_delete(position);
    Coordinates_delete(north_limit);
    Coordinates_delete(south_limit);
    Coordinates_delete(east_limit);
    Coordinates_delete(west_limit);

    return is_within;
}

static int thetaIsWithinToleranceOfGoal(struct Objective *objective, struct State *current_state)
{
    struct Angle *current_angle = current_state->pose->angle;
    struct Angle *goal_angle = objective->goal_state->pose->angle;

    int distance = Angle_smallestAngleBetween(goal_angle, current_angle);
    int tolerance_angle = objective->tolerances->pose->angle->theta;
    return (distance <= tolerance_angle);
}

static int flagsAreTheSame(struct Objective *objective, struct State *current_state)
{
    return (Flags_haveTheSameValues(objective->goal_state->flags, current_state->flags));
}


int Objective_isReached(struct Objective *objective, struct State *current_state)
{
    int reached = coordinatesIsWithinToleranceOfGoal(objective, current_state)
                  && thetaIsWithinToleranceOfGoal(objective, current_state)
                  && flagsAreTheSame(objective, current_state);

    return reached;
}
