#include <criterion/criterion.h>
#include <stdio.h>
#include "CommandSender.h"


int translate_validator;
int rotate_validator;
int light_red_led_validator;
int light_green_led_validator;
int rise_pen_validator;
int lower_pen_validator;

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
}

void teardown_CommandSender(void)
{
    CommandSender_delete(command_sender);
}

/*
 *
Move :
uint_8 *data;
data[0] Commande (move --> 0)
data[1] taille des data envoyés (move --> 8)
data[2..5] float x     Unit = 1 m
data[6..9] float y     Unit = 1 m
struct __attributes__((__packed__)) {
    float x;
    float y;
}
*/
Test(CommandSender, construction_destruction)
{
    struct CommandSender *command_sender = CommandSender_new();

    cr_assert_eq(command_sender->command_callbacks.sendTranslateCommand, &RobotServer_sendTranslateCommand);
    cr_assert_eq(command_sender->command_callbacks.sendRotateCommand, &RobotServer_sendRotateCommand);
    cr_assert_eq(command_sender->command_callbacks.sendLightRedLEDCommand, &RobotServer_sendLightRedLEDCommand);
    cr_assert_eq(command_sender->command_callbacks.sendLightGreenLEDCommand, &RobotServer_sendLightGreenLEDCommand);
    cr_assert_eq(command_sender->command_callbacks.sendRisePenCommand, &RobotServer_sendRisePenCommand);
    cr_assert_eq(command_sender->command_callbacks.sendLowerPenCommand, &RobotServer_sendLowerPenCommand);

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
void CommandSenderTest_sendLightRedLEDCommand(struct Command_LightRedLED command_light_red_led)
{
    ++light_red_led_validator;
}
void CommandSenderTest_sendLightGreenLEDCommand(struct Command_LightGreenLED command_light_green_led)
{
    ++light_green_led_validator;
}
void CommandSenderTest_sendRisePenCommand(struct Command_RisePen command_rise_pen)
{
    ++rise_pen_validator;
}
void CommandSenderTest_sendLowerPenCommand(struct Command_LowerPen command_lower_pen)
{
    ++lower_pen_validator;
}

struct CommandSender_Callbacks generateCommandSenderTestTarget(void)
{
    struct CommandSender_Callbacks test_callbacks = {
        .sendTranslateCommand = &CommandSenderTest_sendTranslateCommand,
        .sendRotateCommand = &CommandSenderTest_sendRotateCommand,
        .sendLightRedLEDCommand = &CommandSenderTest_sendLightRedLEDCommand,
        .sendLightGreenLEDCommand = &CommandSenderTest_sendLightGreenLEDCommand,
        .sendRisePenCommand = &CommandSenderTest_sendRisePenCommand,
        .sendLowerPenCommand = &CommandSenderTest_sendLowerPenCommand
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

}

Test(CommandSender, given_aTranlateCallback_when_askedToSendCommand_then_theCallbackIsCalled
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
    struct Command_LightRedLED light_red_led_command = {};

    CommandSender_sendLightRedLEDCommand(command_sender, light_red_led_command);
    cr_assert_eq(light_red_led_validator, EXPECTED_VALIDATOR_VALUE);
}

Test(CommandSender, given_aLightGreenLEDCallback_when_askedToSendCommand_then_theCallbackIsCalled
     , .init = setup_CommandSender
     , .fini = teardown_CommandSender)
{
    struct CommandSender_Callbacks test_callbacks = generateCommandSenderTestTarget();
    CommandSender_changeTarget(command_sender, test_callbacks);
    struct Command_LightGreenLED light_green_led_command = {};

    CommandSender_sendLightGreenLEDCommand(command_sender, light_green_led_command);
    cr_assert_eq(light_green_led_validator, EXPECTED_VALIDATOR_VALUE);
}

Test(CommandSender, given_aRisePenCallback_when_askedToSendCommand_then_theCallbackIsCalled
     , .init = setup_CommandSender
     , .fini = teardown_CommandSender)
{
    struct CommandSender_Callbacks test_callbacks = generateCommandSenderTestTarget();
    CommandSender_changeTarget(command_sender, test_callbacks);
    struct Command_RisePen rise_pen_command = {};

    CommandSender_sendRisePenCommand(command_sender, rise_pen_command);
    cr_assert_eq(rise_pen_validator, EXPECTED_VALIDATOR_VALUE);

}

Test(CommandSender, given_aLowerPenCallback_when_askedToSendCommand_then_theCallbackIsCalled
     , .init = setup_CommandSender
     , .fini = teardown_CommandSender)
{
    struct CommandSender_Callbacks test_callbacks = generateCommandSenderTestTarget();
    CommandSender_changeTarget(command_sender, test_callbacks);
    struct Command_LowerPen lower_pen_command = {};

    CommandSender_sendLowerPenCommand(command_sender, lower_pen_command);
    cr_assert_eq(lower_pen_validator, EXPECTED_VALIDATOR_VALUE);

}
