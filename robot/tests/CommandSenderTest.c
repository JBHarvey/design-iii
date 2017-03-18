#include <criterion/criterion.h>
#include <stdio.h>
#include "CommandSender.h"


int translate_validator;
int rotate_validator;
int light_red_led_validator;
int light_green_led_validator;
int rise_pen_validator;
int lower_pen_validator;
int manchester_code_validator;
int manchester_signal_validator;

const int EXPECTED_VALIDATOR_VALUE = 1;

struct CommandSender *command_sender;

void setup_CommandSender(void)
{
    command_sender = CommandSender_new();
    translate_validator = 0;
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

Test(CommandSender, given_aTranslateCallback_when_askedToSendCommand_then_theCallbackIsCalled
     , .init = setup_CommandSender
     , .fini = teardown_CommandSender)
{
    struct CommandSender_Callbacks test_callbacks = generateCommandSenderTestTarget();
    CommandSender_changeTarget(command_sender, test_callbacks);
    struct Command_Translate translate_command = { .x = 0, . y = 0};

    CommandSender_sendTranslateCommand(command_sender, translate_command);
    cr_assert_eq(translate_validator, EXPECTED_VALIDATOR_VALUE);

}

Test(CommandSender, given_aRotateCallback_when_askedToSendCommand_then_theCallbackIsCalled
     , .init = setup_CommandSender
     , .fini = teardown_CommandSender)
{
    struct CommandSender_Callbacks test_callbacks = generateCommandSenderTestTarget();
    CommandSender_changeTarget(command_sender, test_callbacks);
    struct Command_Rotate rotate_command = { .theta = 0 };

    CommandSender_sendRotateCommand(command_sender, rotate_command);
    cr_assert_eq(rotate_validator, EXPECTED_VALIDATOR_VALUE);
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

Test(CommandSender, given_afetchManchesterCodeCallback_when_askedToSendCommand_then_theCallbackIsCalled
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
