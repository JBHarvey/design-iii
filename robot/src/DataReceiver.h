#ifndef DATARECEIVER_H_
#define DATARECEIVER_H_

#include "CommunicationStructures.h"
#include "OnboardCamera.h"
#include "Wheels.h"
#include "WorldCamera.h"

struct Mesurements {
    struct Communication_World world;
};

struct DataReceiver_Callbacks {
    void (*updateWorld)(struct WorldCamera *, struct Communication_World);
    void (*updateWheelsTranslation)(struct Wheels *, struct Communication_Translation);
    void (*updateWheelsRotation)(struct Wheels *, struct Communication_Rotation);
    void (*updateManchesterCode)(struct ManchesterCode *, struct Communication_ManchesterCode);
    void (*updateFlagsStartCycle)(struct Flags *);
    void (*updateFlagsImageReceivedByStation)(struct Flags *);
    void (*updateFlagsPlannedTrajectoryReceivedByStation)(struct Flags *);
    void (*updateFlagsReadyToStartSignalReceivedByStation)(struct Flags *);
    void (*updateFlagsReadyToDrawSignalReceivedByStation)(struct Flags *);
    void (*updateFlagsEndOfCycleSignalReceivedByStation)(struct Flags *);
};

struct DataReceiver_Callbacks DataReceiver_fetchCallbacks(void);

void DataReceiver_updateWorld(struct WorldCamera *world_camera, struct Communication_World world);
void DataReceiver_updateWheelsTranslation(struct Wheels *wheels, struct Communication_Translation translation);
void DataReceiver_updateWheelsRotation(struct Wheels *wheels, struct Communication_Rotation rotation);
void DataReceiver_updateManchesterCode(struct ManchesterCode *manchester_code, struct Communication_ManchesterCode new_manchester_code);
void DataReceiver_updateFlagsStartCycle(struct Flags *flags);
void DataReceiver_updateFlagsImageReceivedByStation(struct Flags *flags);
void DataReceiver_updateFlagsPlannedTrajectoryReceivedByStation(struct Flags *flags);
void DataReceiver_updateFlagsReadyToStartSignalReceivedByStation(struct Flags *flags);
void DataReceiver_updateFlagsReadyToDrawSignalReceivedByStation(struct Flags *flags);
void DataReceiver_updateFlagsEndOfCycleSignalReceivedByStation(struct Flags *flags);

struct Mesurements DataReceiver_fetchInputs(struct Mesurements (*communication_callback)(void));
// See bottom of InformationReceiver.c for why this is commented out.
//void DataReceiver_updateMesurements(struct Robot *robot);

#endif // DATARECEIVER_H_
