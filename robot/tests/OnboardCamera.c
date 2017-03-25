
#include <criterion/criterion.h>
#include <stdio.h>
#include "OnboardCamera.h"
struct RobotServer *robot_server;

Test(OnboardCamera, cvSeqToCoordinatesSequence)
{
    CvMemStorage *opencv_storage = cvCreateMemStorage(0);
    CvSeq *opencv_sequence = (CvSeq *)cvLoad("../utils/opencv_sequence.xml", opencv_storage, 0, 0);
    struct Coordinates *coordinates = Coordinates_new(-322, -394);

    struct CoordinatesSequence *coordinate_sequence = OnboardCamera_cvSeqToCoordinatesSequence(opencv_sequence);

    cr_assert(Coordinates_haveTheSameValues(coordinate_sequence->coordinates, coordinates));
    cr_assert_eq(CoordinatesSequence_size(coordinate_sequence), 9);

    CoordinatesSequence_delete(coordinate_sequence);
    Coordinates_delete(coordinates);
    cvReleaseMemStorage(&opencv_storage);
}
