#include "CommandSender.h"

struct CommandSender_Callbacks CommandSender_fetchCallbacksForRobot(void)
{
    struct CommandSender_Callbacks commands = {
        .sendTranslateCommand = &RobotServer_sendTranslateCommand,
        .sendSpeedsCommand = &RobotServer_sendSpeedsCommand,
        .sendRotateCommand = &RobotServer_sendRotateCommand,
        .sendLightRedLEDCommand = &RobotServer_sendLightRedLEDCommand,
        .sendLightGreenLEDCommand = &RobotServer_sendLightGreenLEDCommand,
        .sendRisePenCommand = &RobotServer_sendRisePenCommand,
        .sendLowerPenCommand = &RobotServer_sendLowerPenCommand,
        .sendFetchManchesterCodeCommand = &RobotServer_fetchManchesterCodeCommand,
        .sendStopSendingManchesterSignalCommand = &RobotServer_sendStopSendingManchesterSignalCommand
    };
    return commands;
}

struct CommandSender *CommandSender_new(void)
{
    struct Object *new_object = Object_new();
    struct CommandSender_Callbacks new_command_callbacks = CommandSender_fetchCallbacksForRobot();

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

void CommandSender_sendSpeedsCommand(struct CommandSender *command_sender,
                                     struct Command_Speeds speeds_command)
{
    (*(command_sender->command_callbacks.sendSpeedsCommand))(speeds_command);
}

void CommandSender_sendRotateCommand(struct CommandSender *command_sender, struct Command_Rotate rotate_command)
{
    (*(command_sender->command_callbacks.sendRotateCommand))(rotate_command);
}

void CommandSender_sendLightRedLEDCommand(struct CommandSender *command_sender)
{
    (*(command_sender->command_callbacks.sendLightRedLEDCommand))();
}

void CommandSender_sendLightGreenLEDCommand(struct CommandSender *command_sender)
{
    (*(command_sender->command_callbacks.sendLightGreenLEDCommand))();
}

void CommandSender_sendRisePenCommand(struct CommandSender *command_sender)
{
    (*(command_sender->command_callbacks.sendRisePenCommand))();
}

void CommandSender_sendLowerPenCommand(struct CommandSender *command_sender)
{
    (*(command_sender->command_callbacks.sendLowerPenCommand))();
}

void CommandSender_sendFetchManchesterCode(struct CommandSender *command_sender)
{
    (*(command_sender->command_callbacks.sendFetchManchesterCodeCommand))();
}

void CommandSender_sendStopSendingManchesterSignal(struct CommandSender *command_sender)
{
    (*(command_sender->command_callbacks.sendStopSendingManchesterSignalCommand))();
}
