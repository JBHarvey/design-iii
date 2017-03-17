#ifndef COMMANDSENDER_H_
#define COMMANDSENDER_H_

#include "RobotServer.h"

struct CommandSender_Callbacks {
    void (*sendTranslateCommand)(struct Command_Translate);
    void (*sendRotateCommand)(struct Command_Rotate);
    void (*sendLightRedLEDCommand)(struct Command_LightRedLED);
    void (*sendLightGreenLEDCommand)(struct Command_LightGreenLED);
    void (*sendRisePenCommand)(struct Command_RisePen);
    void (*sendLowerPenCommand)(struct Command_LowerPen);
};

struct CommandSender {
    struct Object *object;
    struct CommandSender_Callbacks command_callbacks;
};

struct CommandSender *CommandSender_new(void);
void CommandSender_delete(struct CommandSender *command_sender);

void CommandSender_changeTarget(struct CommandSender *command_sender, struct CommandSender_Callbacks new_callbacks);

void CommandSender_sendTranslateCommand(struct CommandSender *command_sender, struct Command_Translate translate_command);
void CommandSender_sendRotateCommand(struct CommandSender *command_sender, struct Command_Rotate rotate_command);
void CommandSender_sendLightRedLEDCommand(struct CommandSender *command_sender, struct Command_LightRedLED light_red_led_command);
void CommandSender_sendLightGreenLEDCommand(struct CommandSender *command_sender, struct Command_LightGreenLED light_green_led_command);
void CommandSender_sendRisePenCommand(struct CommandSender *command_sender, struct Command_RisePen rise_pen_command);
void CommandSender_sendLowerPenCommand(struct CommandSender *command_sender, struct Command_LowerPen lower_pen_command);
#endif // COMMANDSENDER_H_
