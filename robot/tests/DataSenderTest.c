#include <criterion/criterion.h>
#include <stdio.h>
#include "DataSender.h"


Test(DataSender, given_when_fetchDataSenderCallbacksForRobot_then_theCorrectSturctureIsReturned)
{
    void (*sendRobotPoseEstimate)(struct Pose *) = &RobotServer_sendRobotPoseEstimate;
    void (*sendImage)(IplImage *) = &RobotServer_sendImageToStation;
    void (*sendPlannedTrajectory)(struct CoordinatesSequence *) = &RobotServer_sendPlannedTrajectoryToStation;
    void (*sendSignalReadyToStart)(void) = &RobotServer_sendSignalReadyToStart;
    void (*sendSignalReadyToDraw)(void) = &RobotServer_sendSignalReadyToDraw;
    void (*sendSignalEndOfCycle)(void) = &RobotServer_sendSignalEndOfCycle;

    struct DataSender_Callbacks callbacks = DataSender_fetchCallbacksForRobot();

    cr_assert_eq(callbacks.sendRobotPoseEstimate, sendRobotPoseEstimate);
    cr_assert_eq(callbacks.sendImage, sendImage);
    cr_assert_eq(callbacks.sendPlannedTrajectory, sendPlannedTrajectory);
    cr_assert_eq(callbacks.sendSignalReadyToStart, sendSignalReadyToStart);
    cr_assert_eq(callbacks.sendSignalReadyToDraw, sendSignalReadyToDraw);
    cr_assert_eq(callbacks.sendSignalEndOfCycle, sendSignalEndOfCycle);
}

int robot_pose_estimate_validator;
int image_validator;
int planned_trajectory_validator;
int signal_ready_to_start_validator;
int signal_ready_to_draw_validator;
int signal_end_of_cycle_validator;

const int DATA_SENDER_EXPECTED_VALIDATON_VALUE = 1;

struct DataSender *data_sender;

void setup_DataSender(void)
{
    data_sender = DataSender_new();
    robot_pose_estimate_validator = 0;
    image_validator = 0;
    planned_trajectory_validator = 0;
    signal_ready_to_start_validator = 0;
    signal_ready_to_draw_validator = 0;
    signal_end_of_cycle_validator = 0;
}

void teardown_DataSender(void)
{
    DataSender_delete(data_sender);
}

Test(DataSender, construction_destruction)
{
    struct DataSender *data_sender = DataSender_new();

    cr_assert_eq(data_sender->data_callbacks.sendRobotPoseEstimate, &RobotServer_sendRobotPoseEstimate);
    cr_assert_eq(data_sender->data_callbacks.sendImage, &RobotServer_sendImageToStation);
    cr_assert_eq(data_sender->data_callbacks.sendPlannedTrajectory, &RobotServer_sendPlannedTrajectoryToStation);
    cr_assert_eq(data_sender->data_callbacks.sendSignalReadyToStart, &RobotServer_sendSignalReadyToStart);
    cr_assert_eq(data_sender->data_callbacks.sendSignalReadyToDraw, &RobotServer_sendSignalReadyToDraw);
    cr_assert_eq(data_sender->data_callbacks.sendSignalEndOfCycle, &RobotServer_sendSignalEndOfCycle);

    DataSender_delete(data_sender);
}


void DataSenderTest_sendRobotPoseEstimate(struct Pose *pose)
{
    ++robot_pose_estimate_validator;
}
void DataSenderTest_sendImageToStation(IplImage *image)
{
    ++image_validator;
}
void DataSenderTest_sendPlannedTrajectory(struct CoordinatesSequence *sequence)
{
    ++planned_trajectory_validator;
}
void DataSenderTest_sendSignalReadyToStart(void)
{
    ++signal_ready_to_start_validator;
}
void DataSenderTest_sendSignalReadyToDraw(void)
{
    ++signal_ready_to_draw_validator;
}
void DataSenderTest_sendSignalEndOfCycle(void)
{
    ++signal_end_of_cycle_validator;
}

struct DataSender_Callbacks generateDataSenderTestTarget(void)
{
    struct DataSender_Callbacks test_callbacks = {
        .sendRobotPoseEstimate = &DataSenderTest_sendRobotPoseEstimate,
        .sendImage = &DataSenderTest_sendImageToStation,
        .sendPlannedTrajectory = &DataSenderTest_sendPlannedTrajectory,
        .sendSignalReadyToStart = &DataSenderTest_sendSignalReadyToStart,
        .sendSignalReadyToDraw = &DataSenderTest_sendSignalReadyToDraw,
        .sendSignalEndOfCycle = &DataSenderTest_sendSignalEndOfCycle
    };
    return test_callbacks;
}

Test(DataSender, given_aDataSenderCallbacks_when_changesDataSenderTarget_then_theCallsAreUpdated
     , .init = setup_DataSender
     , .fini = teardown_DataSender)
{
    struct DataSender_Callbacks test_callbacks = generateDataSenderTestTarget();

    DataSender_changeTarget(data_sender, test_callbacks);

    cr_assert_eq(data_sender->data_callbacks.sendRobotPoseEstimate, test_callbacks.sendRobotPoseEstimate);
    cr_assert_eq(data_sender->data_callbacks.sendImage, test_callbacks.sendImage);
    cr_assert_eq(data_sender->data_callbacks.sendPlannedTrajectory, test_callbacks.sendPlannedTrajectory);
    cr_assert_eq(data_sender->data_callbacks.sendSignalReadyToStart, test_callbacks.sendSignalReadyToStart);
    cr_assert_eq(data_sender->data_callbacks.sendSignalReadyToDraw, test_callbacks.sendSignalReadyToDraw);
    cr_assert_eq(data_sender->data_callbacks.sendSignalEndOfCycle, test_callbacks.sendSignalEndOfCycle);

}

Test(DataSender, given_aPose_when_askedToSendData_then_theCallbackIsCalled
     , .init = setup_DataSender
     , .fini = teardown_DataSender)
{
    struct DataSender_Callbacks test_callbacks = generateDataSenderTestTarget();
    DataSender_changeTarget(data_sender, test_callbacks);
    struct Pose *pose;

    DataSenderTest_sendRobotPoseEstimate(pose);
    cr_assert_eq(robot_pose_estimate_validator, DATA_SENDER_EXPECTED_VALIDATON_VALUE);

}

Test(DataSender, given_anImage_when_askedToSendData_then_theCallbackIsCalled
     , .init = setup_DataSender
     , .fini = teardown_DataSender)
{
    struct DataSender_Callbacks test_callbacks = generateDataSenderTestTarget();
    DataSender_changeTarget(data_sender, test_callbacks);
    IplImage *image;

    DataSenderTest_sendImageToStation(image);
    cr_assert_eq(image_validator, DATA_SENDER_EXPECTED_VALIDATON_VALUE);
}

Test(DataSender, given_aCoordinatesSequence_when_askedToSendData_then_theCallbackIsCalled
     , .init = setup_DataSender
     , .fini = teardown_DataSender)
{
    struct DataSender_Callbacks test_callbacks = generateDataSenderTestTarget();
    DataSender_changeTarget(data_sender, test_callbacks);
    struct CoordinatesSequence *sequence;

    DataSenderTest_sendPlannedTrajectory(sequence);
    cr_assert_eq(planned_trajectory_validator, DATA_SENDER_EXPECTED_VALIDATON_VALUE);
}

Test(DataSender, given_aReadyToStartCallback_when_askedToSendData_then_theCallbackIsCalled
     , .init = setup_DataSender
     , .fini = teardown_DataSender)
{
    struct DataSender_Callbacks test_callbacks = generateDataSenderTestTarget();
    DataSender_changeTarget(data_sender, test_callbacks);

    DataSenderTest_sendSignalReadyToStart();
    cr_assert_eq(signal_ready_to_start_validator, DATA_SENDER_EXPECTED_VALIDATON_VALUE);
}

Test(DataSender, given_aReadyToDrawCallback_when_askedToSendData_then_theCallbackIsCalled
     , .init = setup_DataSender
     , .fini = teardown_DataSender)
{
    struct DataSender_Callbacks test_callbacks = generateDataSenderTestTarget();
    DataSender_changeTarget(data_sender, test_callbacks);

    DataSenderTest_sendSignalReadyToDraw();
    cr_assert_eq(signal_ready_to_draw_validator, DATA_SENDER_EXPECTED_VALIDATON_VALUE);
}

Test(DataSender, given_anEndOfCycleCallback_when_askedToSendData_then_theCallbackIsCalled
     , .init = setup_DataSender
     , .fini = teardown_DataSender)
{
    struct DataSender_Callbacks test_callbacks = generateDataSenderTestTarget();
    DataSender_changeTarget(data_sender, test_callbacks);

    DataSenderTest_sendSignalEndOfCycle();
    cr_assert_eq(signal_end_of_cycle_validator, DATA_SENDER_EXPECTED_VALIDATON_VALUE);
}
