#include <criterion/criterion.h>
#include <stdio.h>
#include "State.h"
#include "Objective.h"


static struct State* state;
static struct State* tolerances;
static struct Objective* objective;

static const int X_TOLERANCE = 100;             // 10 cm
static const int X_TOLERANCE_MIN = 0;           // 0 mm
static const int X_TOLERANCE_MAX = 50000;       // 5 m

static const int Y_TOLERANCE = 100;             // 10 cm
static const int Y_TOLERANCE_MIN = 0;           // 0 mm
static const int Y_TOLERANCE_MAX = 50000;       // 5 m

static const int THETA_TOLERANCE = 3142;        // Pi/10 rad
static const int THETA_TOLERANCE_MIN = 0;       // 0 rad
static const int THETA_TOLERANCE_MAX = 31416;   // Pi rad

void setup(void)
{
    const int SOME_X = 1356;
    const int SOME_Y = 5251;
    const int SOME_THETA = 11611;

    struct Pose* pose = Pose_new(SOME_X, SOME_Y, SOME_THETA);
    state = State_new(pose);

    struct Pose* poseTolerance = Pose_new(X_TOLERANCE, Y_TOLERANCE, THETA_TOLERANCE);
    tolerances = State_new(poseTolerance);

    objective = Objective_new(state, tolerances);
}

void teardown(void)
{
    Objective_delete(objective);
}

Test(Objective, creation_destruction, .init = setup, .fini = teardown)
{
    cr_assert(objective->objectiveValues == state
              && objective->tolerances == tolerances);
}

Test(Objective, given_absoluteTolerances_when_validatesIfObjectiveIsReached_then_isReached,
     .init = setup, .fini = teardown)
{
    struct State* absoluteTolerances = createAbsoluteTolerance();
    struct Objective* reachableObjective = Objective_new(state, absoluteTolerances);

    int reached = Objective_isReached(reachableObjective);

    cr_assert(reached == 1);
    Objective_delete(reachableObjective);
}

struct State* createAbsoluteTolerance(void)
{
    struct Pose* absolutePoseTolerances = Pose_new(X_TOLERANCE_MAX, Y_TOLERANCE_MAX, THETA_TOLERANCE_MAX);
    struct State* absoluteTolerances = State_new(absolutePoseTolerances);
    return absoluteTolerances;
}
