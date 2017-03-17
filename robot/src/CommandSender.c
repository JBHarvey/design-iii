#include "CommandSender.h"

static struct CommandSender_Callbacks getCallbacksTargetingRobot(void)
{
    struct CommandSender_Callbacks commands = {
        .sendTranslateCommand = &RobotServer_sendTranslateCommand,
        .sendRotateCommand = &RobotServer_sendRotateCommand,
        .sendLightRedLEDCommand = &RobotServer_sendLightRedLEDCommand,
        .sendLightGreenLEDCommand = &RobotServer_sendLightGreenLEDCommand,
        .sendRisePenCommand = &RobotServer_sendRisePenCommand,
        .sendLowerPenCommand = &RobotServer_sendLowerPenCommand
    };
    return commands;
}

struct CommandSender *CommandSender_new(void)
{
    struct Object *new_object = Object_new();
    struct CommandSender_Callbacks new_command_callbacks = getCallbacksTargetingRobot();

    struct CommandSender *pointer = malloc(sizeof(struct CommandSender));

    pointer->object = new_object;
    pointer->command_callbacks = new_command_callbacks;

    return pointer;
}

void CommandSender_delete(struct CommandSender *command_sender)
{
    Object_removeOneReference(command_sender->object);

    if(Object_canBeDeleted(command_sender->object)) {
        Object_delete(command_sender->object);

        free(command_sender);
    }
}

void CommandSender_changeTarget(struct CommandSender *command_sender, struct CommandSender_Callbacks new_callbacks)
{
    command_sender->command_callbacks = new_callbacks;
}

void CommandSender_sendTranslateCommand(struct CommandSender *command_sender,
                                        struct Command_Translate translate_command)
{
    (*(command_sender->command_callbacks.sendTranslateCommand))(translate_command);
}

void CommandSender_sendRotateCommand(struct CommandSender *command_sender, struct Command_Rotate rotate_command)
{
    (*(command_sender->command_callbacks.sendRotateCommand))(rotate_command);
}

void CommandSender_sendLightRedLEDCommand(struct CommandSender *command_sender,
        struct Command_LightRedLED light_red_led_command)
{
    (*(command_sender->command_callbacks.sendLightRedLEDCommand))(light_red_led_command);
}

void CommandSender_sendLightGreenLEDCommand(struct CommandSender *command_sender,
        struct Command_LightGreenLED light_green_led_command)
{
    (*(command_sender->command_callbacks.sendLightGreenLEDCommand))(light_green_led_command);
}

void CommandSender_sendRisePenCommand(struct CommandSender *command_sender, struct Command_RisePen rise_pen_command)
{
    (*(command_sender->command_callbacks.sendRisePenCommand))(rise_pen_command);
}

void CommandSender_sendLowerPenCommand(struct CommandSender *command_sender, struct Command_LowerPen lower_pen_command)
{
    (*(command_sender->command_callbacks.sendLowerPenCommand))(lower_pen_command);
}
