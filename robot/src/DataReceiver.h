#ifndef DATARECEIVER_H_
#define DATARECEIVER_H_

#include "CommunicationStructures.h"
#include "Robot.h"

struct Mesurements {
    struct Communication_World world;
};

void DataReceiver_updateWorld(struct WorldCamera *world_camera, struct Communication_World world);
void DataReceiver_updateWheelsTranslation(struct Wheels *wheels, struct Communication_Translation translation);
void DataReceiver_updateWheelsRotation(struct Wheels *wheels, struct Communication_Rotation rotation);
struct Mesurements DataReceiver_fetchInputs(struct Mesurements (*communication_callback)(void));
// See bottom of InformationReceiver.c for why this is commented out.
//void DataReceiver_updateMesurements(struct Robot *robot);

#endif // DATARECEIVER_H_
