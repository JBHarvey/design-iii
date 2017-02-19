#include <criterion/criterion.h>
#include <stdio.h>
#include "State.h"
#include "Objective.h"


static struct State* state;
static struct State* valuesToValidate;
static struct Objective* objective;

void setup(void)
{
    const int SOME_X = 13256;
    const int SOME_Y = 521651;
    const int SOME_THETA = 11611;

    struct Pose* pose = Pose_new(SOME_X, SOME_Y, SOME_THETA);

    state = State_new(pose);



    const int VALIDATE_X = 1;
    const int VALIDATE_Y = 0;
    const int VALIDATE_THETA = 0;

    struct Pose* objectivePose = Pose_new(VALIDATE_X, VALIDATE_Y, VALIDATE_THETA);

    valuesToValidate = State_new(objectivePose);


    objective = Objective_new(state, valuesToValidate);
}

void teardown(void)
{
    Objective_delete(objective);
}

Test(Objective, creation_destruction, .init = setup, .fini = teardown)
{
    cr_assert(objective->objectiveValues == state
              && objective->valuesToValidate == valuesToValidate);
}

Test(Objective, given_someValuesAnd100Tolerance_when_validatesIfObjectiveIsReached_then_isReached, .init = setup,
     .fini = teardown)
{
    int reached = Objective_isReached(objective, 100);
    cr_assert(reached == 1);
}
