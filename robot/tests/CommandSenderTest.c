#include <criterion/criterion.h>
#include <stdio.h>
#include "CommandSender.h"


Test(CommandSender, given_when_fetchCommandSenderCallbacksForRobot_then_theCorrectSturctureIsReturned)
{
    void (*sendTranslateCommand)(struct Command_Translate) = &RobotServer_sendTranslateCommand;
    void (*sendSpeedsCommand)(struct Command_Speeds) = &RobotServer_sendSpeedsCommand;
    void (*sendRotateCommand)(struct Command_Rotate) = &RobotServer_sendRotateCommand;
    void (*sendLightRedLEDCommand)(void) = &RobotServer_sendLightRedLEDCommand;
    void (*sendLightGreenLEDCommand)(void) = &RobotServer_sendLightGreenLEDCommand;
    void (*sendRisePenCommand)(void) = &RobotServer_sendRisePenCommand;
    void (*sendLowerPenCommand)(void) = &RobotServer_sendLowerPenCommand;
    void (*sendFetchManchesterCodeCommand)(void) = &RobotServer_fetchManchesterCodeCommand;
    void (*sendStopSendingManchesterSignalCommand)(void) = &RobotServer_sendStopSendingManchesterSignalCommand;

    struct CommandSender_Callbacks callbacks = CommandSender_fetchCallbacksForRobot();

    cr_assert_eq(callbacks.sendTranslateCommand, sendTranslateCommand);
    cr_assert_eq(callbacks.sendSpeedsCommand, sendSpeedsCommand);
    cr_assert_eq(callbacks.sendRotateCommand, sendRotateCommand);
    cr_assert_eq(callbacks.sendLightRedLEDCommand, sendLightRedLEDCommand);
    cr_assert_eq(callbacks.sendLightGreenLEDCommand, sendLightGreenLEDCommand);
    cr_assert_eq(callbacks.sendRisePenCommand, sendRisePenCommand);
    cr_assert_eq(callbacks.sendLowerPenCommand, sendLowerPenCommand);
    cr_assert_eq(callbacks.sendFetchManchesterCodeCommand, sendFetchManchesterCodeCommand);
    cr_assert_eq(callbacks.sendStopSendingManchesterSignalCommand, sendStopSendingManchesterSignalCommand);
}

int translate_validator;
int speeds_validator;
int rotate_validator;
int light_red_led_validator;
int light_green_led_validator;
int rise_pen_validator;
int lower_pen_validator;
int manchester_code_validator;
int manchester_signal_validator;

const int EXPECTED_VALIDATOR_VALUE = 1;
const int X_VALUE = 10;
const int Y_VALUE = 15;
const int X_SPEED_VALUE = 10000;
const int Y_SPEED_VALUE = 15000;
const int THETA_VALUE = 21000;

struct CommandSender *command_sender;

void setup_CommandSender(void)
{
    command_sender = CommandSender_new();
    translate_validator = 0;
    speeds_validator = 0;
    rotate_validator = 0;
    light_red_led_validator = 0;
    light_green_led_validator = 0;
    rise_pen_validator = 0;
    lower_pen_validator = 0;
    manchester_code_validator = 0;
    manchester_signal_validator = 0;
}

void teardown_CommandSender(void)
{
    CommandSender_delete(command_sender);
}

Test(CommandSender, construction_destruction)
{
    struct CommandSender *command_sender = CommandSender_new();

    cr_assert_eq(command_sender->command_callbacks.sendTranslateCommand, &RobotServer_sendTranslateCommand);
    cr_assert_eq(command_sender->command_callbacks.sendSpeedsCommand, &RobotServer_sendSpeedsCommand);
    cr_assert_eq(command_sender->command_callbacks.sendRotateCommand, &RobotServer_sendRotateCommand);
    cr_assert_eq(command_sender->command_callbacks.sendLightRedLEDCommand, &RobotServer_sendLightRedLEDCommand);
    cr_assert_eq(command_sender->command_callbacks.sendLightGreenLEDCommand, &RobotServer_sendLightGreenLEDCommand);
    cr_assert_eq(command_sender->command_callbacks.sendRisePenCommand, &RobotServer_sendRisePenCommand);
    cr_assert_eq(command_sender->command_callbacks.sendLowerPenCommand, &RobotServer_sendLowerPenCommand);
    cr_assert_eq(command_sender->command_callbacks.sendFetchManchesterCodeCommand, &RobotServer_fetchManchesterCodeCommand);
    cr_assert_eq(command_sender->command_callbacks.sendStopSendingManchesterSignalCommand,
                 &RobotServer_sendStopSendingManchesterSignalCommand);

    CommandSender_delete(command_sender);
}

void CommandSenderTest_sendTranslateCommand(struct Command_Translate command_translate)
{
    ++translate_validator;
}
void CommandSenderTest_sendSpeedsCommand(struct Command_Speeds command_speeds)
{
    ++speeds_validator;
}
void CommandSenderTest_sendRotateCommand(struct Command_Rotate command_rotate)
{
    ++rotate_validator;
}
void CommandSenderTest_sendLightRedLEDCommand(void)
{
    ++light_red_led_validator;
}
void CommandSenderTest_sendLightGreenLEDCommand(void)
{
    ++light_green_led_validator;
}
void CommandSenderTest_sendRisePenCommand(void)
{
    ++rise_pen_validator;
}
void CommandSenderTest_sendLowerPenCommand(void)
{
    ++lower_pen_validator;
}
void CommandSenderTest_fetchManchesterCode(void)
{
    ++manchester_code_validator;
}
void CommandSenderTest_stopSendingManchesterSignal(void)
{
    ++manchester_signal_validator;
}

struct CommandSender_Callbacks generateCommandSenderTestTarget(void)
{
    struct CommandSender_Callbacks test_callbacks = {
        .sendTranslateCommand = &CommandSenderTest_sendTranslateCommand,
        .sendSpeedsCommand = &CommandSenderTest_sendSpeedsCommand,
        .sendRotateCommand = &CommandSenderTest_sendRotateCommand,
        .sendLightRedLEDCommand = &CommandSenderTest_sendLightRedLEDCommand,
        .sendLightGreenLEDCommand = &CommandSenderTest_sendLightGreenLEDCommand,
        .sendRisePenCommand = &CommandSenderTest_sendRisePenCommand,
        .sendLowerPenCommand = &CommandSenderTest_sendLowerPenCommand,
        .sendFetchManchesterCodeCommand = &CommandSenderTest_fetchManchesterCode,
        .sendStopSendingManchesterSignalCommand = &CommandSenderTest_stopSendingManchesterSignal
    };
    return test_callbacks;
}

Test(CommandSender, given_aCommandSenderCallbacks_when_changesCommandSenderTarget_then_theCallsAreUpdated
     , .init = setup_CommandSender
     , .fini = teardown_CommandSender)
{
    struct CommandSender_Callbacks test_callbacks = generateCommandSenderTestTarget();

    CommandSender_changeTarget(command_sender, test_callbacks);

    cr_assert_eq(command_sender->command_callbacks.sendTranslateCommand, test_callbacks.sendTranslateCommand);
    cr_assert_eq(command_sender->command_callbacks.sendSpeedsCommand, test_callbacks.sendSpeedsCommand);
    cr_assert_eq(command_sender->command_callbacks.sendRotateCommand, test_callbacks.sendRotateCommand);
    cr_assert_eq(command_sender->command_callbacks.sendLightRedLEDCommand, test_callbacks.sendLightRedLEDCommand);
    cr_assert_eq(command_sender->command_callbacks.sendLightGreenLEDCommand, test_callbacks.sendLightGreenLEDCommand);
    cr_assert_eq(command_sender->command_callbacks.sendRisePenCommand, test_callbacks.sendRisePenCommand);
    cr_assert_eq(command_sender->command_callbacks.sendLowerPenCommand, test_callbacks.sendLowerPenCommand);
    cr_assert_eq(command_sender->command_callbacks.sendFetchManchesterCodeCommand,
                 test_callbacks.sendFetchManchesterCodeCommand);
    cr_assert_eq(command_sender->command_callbacks.sendStopSendingManchesterSignalCommand,
                 test_callbacks.sendStopSendingManchesterSignalCommand);

}

Test(CommandSender,
     given_aTranslateCallbackWithANonZeroCoordinateTranslateCommand_when_askedToSendCommand_then_theWheelsActuatorHasPrepareCommandStatusIsSetToOne
     , .init = setup_CommandSender
     , .fini = teardown_CommandSender)
{
    struct CommandSender_Callbacks test_callbacks = generateCommandSenderTestTarget();
    CommandSender_changeTarget(command_sender, test_callbacks);
    struct Command_Translate translate_command = { .x = X_VALUE, . y = Y_VALUE};
    struct Wheels *wheels = Wheels_new();

    CommandSender_sendTranslateCommand(command_sender, translate_command, wheels);
    cr_assert_eq(wheels->translation_actuator->has_prepared_new_command, 1);

    Wheels_delete(wheels);
}

Test(CommandSender, given_aTranslateCallback_when_askedToSendCommand_then_theCommandIsStoredIntoTheWheels
     , .init = setup_CommandSender
     , .fini = teardown_CommandSender)
{
    struct CommandSender_Callbacks test_callbacks = generateCommandSenderTestTarget();
    CommandSender_changeTarget(command_sender, test_callbacks);
    struct Command_Translate translate_command = { .x = X_VALUE, .y = Y_VALUE};
    struct Wheels *wheels = Wheels_new();

    CommandSender_sendTranslateCommand(command_sender, translate_command, wheels);
    cr_assert_eq(wheels->translation_command->x, X_VALUE);
    cr_assert_eq(wheels->translation_command->y, Y_VALUE);

    Wheels_delete(wheels);
}

Test(CommandSender, given_aTranslateCallback_when_askedToSendCommand_then_theCallbackIsCalled
     , .init = setup_CommandSender
     , .fini = teardown_CommandSender)
{
    struct CommandSender_Callbacks test_callbacks = generateCommandSenderTestTarget();
    CommandSender_changeTarget(command_sender, test_callbacks);
    struct Command_Translate translate_command = { .x = 0, . y = 0};
    struct Wheels *wheels = Wheels_new();

    CommandSender_sendTranslateCommand(command_sender, translate_command, wheels);
    cr_assert_eq(translate_validator, EXPECTED_VALIDATOR_VALUE);

    Wheels_delete(wheels);
}

Test(CommandSender,
     given_aSpeedsCallback_when_askedToSendCommand_then_theWheelsActuatorHasPrepareCommandStatusIsSetToOne
     , .init = setup_CommandSender
     , .fini = teardown_CommandSender)
{
    struct CommandSender_Callbacks test_callbacks = generateCommandSenderTestTarget();
    CommandSender_changeTarget(command_sender, test_callbacks);
    struct Command_Speeds speeds_command = { .x = 0, . y = 0};
    struct Wheels *wheels = Wheels_new();

    CommandSender_sendSpeedsCommand(command_sender, speeds_command, wheels);
    cr_assert_eq(wheels->speed_actuator->has_prepared_new_command, 1);

    Wheels_delete(wheels);
}

Test(CommandSender,
     given_aSpeedsCallback_when_askedToSendCommand_then_theCommandIsStoredIntoTheWheels
     , .init = setup_CommandSender
     , .fini = teardown_CommandSender)
{
    struct CommandSender_Callbacks test_callbacks = generateCommandSenderTestTarget();
    CommandSender_changeTarget(command_sender, test_callbacks);
    struct Command_Speeds speeds_command = { .x = X_SPEED_VALUE, .y = Y_SPEED_VALUE};
    struct Wheels *wheels = Wheels_new();

    CommandSender_sendSpeedsCommand(command_sender, speeds_command, wheels);
    cr_assert_eq(wheels->speed_command->x, X_SPEED_VALUE);
    cr_assert_eq(wheels->speed_command->y, Y_SPEED_VALUE);

    Wheels_delete(wheels);
}

Test(CommandSender, given_aSpeedsCallback_when_askedToSendCommand_then_theCallbackIsCalled
     , .init = setup_CommandSender
     , .fini = teardown_CommandSender)
{
    struct CommandSender_Callbacks test_callbacks = generateCommandSenderTestTarget();
    CommandSender_changeTarget(command_sender, test_callbacks);
    struct Command_Speeds speeds_command = { .x = 0, . y = 0};
    struct Wheels *wheels = Wheels_new();

    CommandSender_sendSpeedsCommand(command_sender, speeds_command, wheels);
    cr_assert_eq(speeds_validator, EXPECTED_VALIDATOR_VALUE);

    Wheels_delete(wheels);

}

Test(CommandSender,
     given_aRotateCallbackWithANonZeroAngleRotateCommand_when_askedToSendCommand_then_theWheelsActuatorHasPrepareCommandStatusIsSetToOne
     , .init = setup_CommandSender
     , .fini = teardown_CommandSender)
{
    struct CommandSender_Callbacks test_callbacks = generateCommandSenderTestTarget();
    CommandSender_changeTarget(command_sender, test_callbacks);
    struct Command_Rotate rotate_command = { .theta = THETA_VALUE };
    struct Wheels *wheels = Wheels_new();

    CommandSender_sendRotateCommand(command_sender, rotate_command, wheels);
    cr_assert_eq(wheels->rotation_actuator->has_prepared_new_command, 1);

    Wheels_delete(wheels);
}

Test(CommandSender,
     given_aRotateCallback_when_askedToSendCommand_then_theCommandIsStoredIntoTheWheels
     , .init = setup_CommandSender
     , .fini = teardown_CommandSender)
{
    struct CommandSender_Callbacks test_callbacks = generateCommandSenderTestTarget();
    CommandSender_changeTarget(command_sender, test_callbacks);
    struct Command_Rotate rotate_command = { .theta = THETA_VALUE };
    struct Wheels *wheels = Wheels_new();

    CommandSender_sendRotateCommand(command_sender, rotate_command, wheels);
    cr_assert_eq(wheels->rotation_command->theta, THETA_VALUE);

    Wheels_delete(wheels);
}

Test(CommandSender, given_aRotateCallback_when_askedToSendCommand_then_theCallbackIsCalled
     , .init = setup_CommandSender
     , .fini = teardown_CommandSender)
{
    struct CommandSender_Callbacks test_callbacks = generateCommandSenderTestTarget();
    CommandSender_changeTarget(command_sender, test_callbacks);
    struct Command_Rotate rotate_command = { .theta = 0 };
    struct Wheels *wheels = Wheels_new();

    CommandSender_sendRotateCommand(command_sender, rotate_command, wheels);
    cr_assert_eq(rotate_validator, EXPECTED_VALIDATOR_VALUE);

    Wheels_delete(wheels);
}

Test(CommandSender, given_aLightRedLEDCallback_when_askedToSendCommand_then_theCallbackIsCalled
     , .init = setup_CommandSender
     , .fini = teardown_CommandSender)
{
    struct CommandSender_Callbacks test_callbacks = generateCommandSenderTestTarget();
    CommandSender_changeTarget(command_sender, test_callbacks);

    CommandSender_sendLightRedLEDCommand(command_sender);
    cr_assert_eq(light_red_led_validator, EXPECTED_VALIDATOR_VALUE);
}

Test(CommandSender, given_aLightGreenLEDCallback_when_askedToSendCommand_then_theCallbackIsCalled
     , .init = setup_CommandSender
     , .fini = teardown_CommandSender)
{
    struct CommandSender_Callbacks test_callbacks = generateCommandSenderTestTarget();
    CommandSender_changeTarget(command_sender, test_callbacks);

    CommandSender_sendLightGreenLEDCommand(command_sender);
    cr_assert_eq(light_green_led_validator, EXPECTED_VALIDATOR_VALUE);
}

Test(CommandSender, given_aRisePenCallback_when_askedToSendCommand_then_theCallbackIsCalled
     , .init = setup_CommandSender
     , .fini = teardown_CommandSender)
{
    struct CommandSender_Callbacks test_callbacks = generateCommandSenderTestTarget();
    CommandSender_changeTarget(command_sender, test_callbacks);

    CommandSender_sendRisePenCommand(command_sender);
    cr_assert_eq(rise_pen_validator, EXPECTED_VALIDATOR_VALUE);
}

Test(CommandSender, given_aLowerPenCallback_when_askedToSendCommand_then_theCallbackIsCalled
     , .init = setup_CommandSender
     , .fini = teardown_CommandSender)
{
    struct CommandSender_Callbacks test_callbacks = generateCommandSenderTestTarget();
    CommandSender_changeTarget(command_sender, test_callbacks);

    CommandSender_sendLowerPenCommand(command_sender);
    cr_assert_eq(lower_pen_validator, EXPECTED_VALIDATOR_VALUE);
}

Test(CommandSender, given_aFetchManchesterCodeCallback_when_askedToSendCommand_then_theCallbackIsCalled
     , .init = setup_CommandSender
     , .fini = teardown_CommandSender)
{
    struct CommandSender_Callbacks test_callbacks = generateCommandSenderTestTarget();
    CommandSender_changeTarget(command_sender, test_callbacks);

    CommandSender_sendFetchManchesterCode(command_sender);
    cr_assert_eq(manchester_code_validator, EXPECTED_VALIDATOR_VALUE);
}

Test(CommandSender, given_aStopSendingManchesterSignalCallback_when_askedToSendCommand_then_theCallbackIsCalled
     , .init = setup_CommandSender
     , .fini = teardown_CommandSender)
{
    struct CommandSender_Callbacks test_callbacks = generateCommandSenderTestTarget();
    CommandSender_changeTarget(command_sender, test_callbacks);

    CommandSender_sendStopSendingManchesterSignal(command_sender);
    cr_assert_eq(manchester_signal_validator, EXPECTED_VALIDATOR_VALUE);
}
