#ifndef COMMANDSENDER_H_
#define COMMANDSENDER_H_

#include "RobotServer.h"

struct CommandSender_Callbacks {
    void (*sendTranslateCommand)(struct Command_Translate);
    void (*sendSpeedsCommand)(struct Command_Speeds);
    void (*sendRotateCommand)(struct Command_Rotate);
    void (*sendLightRedLEDCommand)(void);
    void (*sendLightGreenLEDCommand)(void);
    void (*sendRisePenCommand)(void);
    void (*sendLowerPenCommand)(void);
    void (*sendFetchManchesterCodeCommand)(void);
    void (*sendStopSendingManchesterSignalCommand)(void);
};

struct CommandSender {
    struct Object *object;
    struct CommandSender_Callbacks command_callbacks;
};

struct CommandSender *CommandSender_new(void);
void CommandSender_delete(struct CommandSender *command_sender);

struct CommandSender_Callbacks CommandSender_fetchCallbacksForRobot(void);

void CommandSender_changeTarget(struct CommandSender *command_sender, struct CommandSender_Callbacks new_callbacks);

void CommandSender_sendTranslateCommand(struct CommandSender *command_sender, struct Command_Translate translate_command, 
                                        struct Wheels *wheels);
void CommandSender_sendSpeedsCommand(struct CommandSender *command_sender, struct Command_Speeds speeds_command,
                                     struct Wheels *wheels);
void CommandSender_sendRotateCommand(struct CommandSender *command_sender, struct Command_Rotate rotate_command,
                                     struct Wheels *wheels);
void CommandSender_sendLightRedLEDCommand(struct CommandSender *command_sender);
void CommandSender_sendLightGreenLEDCommand(struct CommandSender *command_sender);
void CommandSender_sendRisePenCommand(struct CommandSender *command_sender);
void CommandSender_sendLowerPenCommand(struct CommandSender *command_sender);
void CommandSender_sendFetchManchesterCode(struct CommandSender *command_sender);
void CommandSender_sendStopSendingManchesterSignal(struct CommandSender *command_sender);

#endif // COMMANDSENDER_H_
