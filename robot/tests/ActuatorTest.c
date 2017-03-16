#include <criterion/criterion.h>
#include <stdio.h>
#include "Actuator.h"

struct Actuator *actuator;

void setup_actuator(void)
{
    actuator = Actuator_new();
}

void teardown_actuator(void)
{
    Actuator_delete(actuator);
}

Test(Actuator, creation_destruction
     , .init = setup_actuator
     , .fini = teardown_actuator)
{
    cr_assert(actuator->has_prepared_new_command == 0);
}

Test(Actuator, given_aActuator_when_preparesNewCommand_then_hasReceivedNewCommandIsTrue
     , .init = setup_actuator
     , .fini = teardown_actuator)
{
    Actuator_preparesCommand(actuator);
    cr_assert(actuator->has_prepared_new_command);
}

Test(Actuator, given_aActuator_when_commandIsSent_then_hasReceivedNewCommandIsFalse
     , .init = setup_actuator
     , .fini = teardown_actuator)
{
    Actuator_preparesCommand(actuator);
    Actuator_sendsCommand(actuator);
    cr_assert(actuator->has_prepared_new_command == 0);
}
