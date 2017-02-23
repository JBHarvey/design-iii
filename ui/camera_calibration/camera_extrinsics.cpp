#include <iostream>
#include <opencv2/opencv.hpp>

enum Pattern { CHESSBOARD, CIRCLES_GRID, ASYMMETRIC_CIRCLES_GRID };
cv::Mat cameraMatrix;
float a, b, c, d;
cv::Mat rvec, tvec;

double checkCameraPose(const std::vector<cv::Point3f> &modelPts, const std::vector<cv::Point2f> &imagePts, const cv::Mat &cameraMatrix,
                       const cv::Mat &distCoeffs, const cv::Mat &rvec, const cv::Mat &tvec) {
  std::vector<cv::Point2f> projectedPts;
  cv::projectPoints(modelPts, rvec, tvec, cameraMatrix, distCoeffs, projectedPts);

  double rms = 0.0;
  for (size_t i = 0; i < projectedPts.size(); i++) {
    rms += (projectedPts[i].x-imagePts[i].x)*(projectedPts[i].x-imagePts[i].x) + (projectedPts[i].y-imagePts[i].y)*(projectedPts[i].y-imagePts[i].y);
  }

  return sqrt(rms / projectedPts.size());
}

cv::Point3f transformPoint(const cv::Point3f &pt, const cv::Mat &rvec, const cv::Mat &tvec) {
  //Compute res = (R | T) . pt
  cv::Mat rotationMatrix;
  cv::Rodrigues(rvec, rotationMatrix);

  cv::Mat transformationMatrix = (cv::Mat_<double>(4, 4) << rotationMatrix.at<double>(0,0), rotationMatrix.at<double>(0,1), rotationMatrix.at<double>(0,2), tvec.at<double>(0),
                                  rotationMatrix.at<double>(1,0), rotationMatrix.at<double>(1,1), rotationMatrix.at<double>(1,2), tvec.at<double>(1),
                                  rotationMatrix.at<double>(2,0), rotationMatrix.at<double>(2,1), rotationMatrix.at<double>(2,2), tvec.at<double>(2),
                                  0, 0, 0, 1);

  cv::Mat homogeneousPt = (cv::Mat_<double>(4, 1) << pt.x, pt.y, pt.z, 1.0);
  cv::Mat transformedPtMat = transformationMatrix * homogeneousPt;

  cv::Point3f transformedPt(transformedPtMat.at<double>(0), transformedPtMat.at<double>(1), transformedPtMat.at<double>(2));
  return transformedPt;
}

cv::Point3f transformPointInverse(const cv::Point3f &pt, const cv::Mat &rvec, const cv::Mat &tvec) {
  //Compute res = (R^t | -R^t . T) . pt
  cv::Mat rotationMatrix;
  cv::Rodrigues(rvec, rotationMatrix);
  rotationMatrix = rotationMatrix.t();

  cv::Mat translation = -rotationMatrix*tvec;

  cv::Mat transformationMatrix = (cv::Mat_<double>(4, 4) << rotationMatrix.at<double>(0,0), rotationMatrix.at<double>(0,1), rotationMatrix.at<double>(0,2), translation.at<double>(0),
                                  rotationMatrix.at<double>(1,0), rotationMatrix.at<double>(1,1), rotationMatrix.at<double>(1,2), translation.at<double>(1),
                                  rotationMatrix.at<double>(2,0), rotationMatrix.at<double>(2,1), rotationMatrix.at<double>(2,2), translation.at<double>(2),
                                  0, 0, 0, 1);

  cv::Mat homogeneousPt = (cv::Mat_<double>(4, 1) << pt.x, pt.y, pt.z, 1.0);
  cv::Mat transformedPtMat = transformationMatrix * homogeneousPt;

  cv::Point3f transformedPt(transformedPtMat.at<double>(0), transformedPtMat.at<double>(1), transformedPtMat.at<double>(2));
  return transformedPt;
}

void computePlaneEquation(const cv::Point3f &p0, const cv::Point3f &p1, const cv::Point3f &p2, float &a, float &b, float &c, float &d) {
  //Vector p0_p1
  cv::Point3f p0_p1;
  p0_p1.x = p0.x - p1.x;
  p0_p1.y = p0.y - p1.y;
  p0_p1.z = p0.z - p1.z;

  //Vector p0_p2
  cv::Point3f p0_p2;
  p0_p2.x = p0.x - p2.x;
  p0_p2.y = p0.y - p2.y;
  p0_p2.z = p0.z - p2.z;

  //Normal vector
  cv::Point3f n = p0_p1.cross(p0_p2);

  a = n.x;
  b = n.y;
  c = n.z;
  d = -(a*p0.x + b*p0.y + c*p0.z);

  float norm =  sqrt(a*a + b*b + c*c);
  a /= norm;
  b /= norm;
  c /= norm;
  d /= norm;
}

cv::Point3f compute3DOnPlaneFrom2D(const cv::Point2f &imagePt, const cv::Mat &cameraMatrix, const float a, const float b, const float c, const float d) {
  double fx = cameraMatrix.at<double>(0,0);
  double fy = cameraMatrix.at<double>(1,1);
  double cx = cameraMatrix.at<double>(0,2);
  double cy = cameraMatrix.at<double>(1,2);

  cv::Point2f normalizedImagePt;
  normalizedImagePt.x = (imagePt.x - cx) / fx;
  normalizedImagePt.y = (imagePt.y - cy) / fy;

  float s = -d / (a*normalizedImagePt.x + b*normalizedImagePt.y + c);

  cv::Point3f pt;
  pt.x = s*normalizedImagePt.x;
  pt.y = s*normalizedImagePt.y;
  pt.z = s;

  return pt;
}

void onMouse2(int evt, int x, int y, int flags, void* param) {
    if(evt == CV_EVENT_LBUTTONDOWN) {
        cv::Point3f pt = transformPointInverse(compute3DOnPlaneFrom2D(cv::Point2f((float)x,(float)y), cameraMatrix, a, b, c, d), rvec, tvec);
        std::cout << "Point 3D coords: x=" << pt.x << ", y=" << pt.y << ", z=" << pt.z << std::endl;
    }
}

void onMouse(int evt, int x, int y, int flags, void* param) {
    if(evt == CV_EVENT_LBUTTONDOWN) {
        std::vector<cv::Point>* ptPtr = (std::vector<cv::Point>*)param;
        ptPtr->push_back(cv::Point(x,y));
    }
}

int main() {
  cv::Mat img = cv::imread("camera_3015_1/2017-02-21-115224_1.jpg", cv::IMREAD_GRAYSCALE);
  cv::Mat undistorted_img;
  cv::Size boardSize(9, 6);
  std::vector<cv::Point> points;

  //Intrinsic
  cameraMatrix = (cv::Mat_<double>(3, 3) << 1.2446732706092348e+03, 0, 7.6244621237445813e+02, 0,
    1.2508092784415699e+03, 6.3033767788293358e+02, 0, 0, 1);
  cv::Mat distCoeffs = (cv::Mat_<double>(5, 1) << 4.0840672110840298e-03, -6.8475777128242335e-02,
    3.3828035938181086e-03, 2.9615979651286906e-04,
    -4.8165410611180431e-02);

  cv::Mat noDistCoeffs = (cv::Mat_<double>(5, 1) << 0, 0, 0, 0, 0); 

  undistort(img, undistorted_img, cameraMatrix, distCoeffs, getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, cv::Size(1600,1200), 1.0, cv::Size(1600,1200), 0, false ));

  std::vector<cv::Point3f> modelPts;

    modelPts.push_back(cv::Point3f(0.,0.,0.));
    modelPts.push_back(cv::Point3f(0.,660.,0.));
    modelPts.push_back(cv::Point3f(660.,660.,0.));
    modelPts.push_back(cv::Point3f(660.,0.,0.));

    cv::namedWindow("Output Window");

    cv::setMouseCallback("Output Window", onMouse, (void*)&points);

    while(1)
    {
        cv::imshow("Output Window", undistorted_img);
        if (points.size() >= 4) 
        {
            for (auto it = points.begin(); it != points.end(); ++it)
            {
                std::cout<<"X and Y coordinates are given below"<<std::endl;
                std::cout<<(*it).x<<'\t'<<(*it).y<<std::endl; 
            }
            break;
        }
        cv::waitKey();
    }

    std::vector<cv::Point2f> pointbuf;
    for (size_t i=0 ; i<points.size(); i++)
    {
        pointbuf.push_back( cv::Point2f( (float)points[i].x, (float)points[i].y  ) );
    }

  //Compute camera pose
  std::cout << pointbuf.size() <<  " " << modelPts.size() << std::endl; 
  cv::solvePnP(modelPts, pointbuf, cameraMatrix, noDistCoeffs, rvec, tvec);

  //Check camera pose
  double rms = checkCameraPose(modelPts, pointbuf, cameraMatrix, noDistCoeffs, rvec, tvec);
  std::cout << "RMS error for camera pose=" << rms << std::endl;

  //Transform model point (in object frame) to the camera frame
  cv::Point3f pt0 = transformPoint(modelPts[0], rvec, tvec);
  cv::Point3f pt1 = transformPoint(modelPts[1], rvec, tvec);
  cv::Point3f pt2 = transformPoint(modelPts[3], rvec, tvec);

  //Compute plane equation in the camera frame
  computePlaneEquation(pt0, pt1, pt2, a, b, c, d);
  std::cout << "Plane equation=" << a << " ; " << b << " ; " << c << " ; " << d << std::endl;

  //Compute 3D from 2D
  std::vector<cv::Point3f> pts3dCameraFrame, pts3dObjectFrame;
  double rms_3D = 0.0;
  for (size_t i = 0; i < pointbuf.size(); i++) {
    cv::Point3f pt = compute3DOnPlaneFrom2D(pointbuf[i], cameraMatrix, a, b, c, d);
    pts3dCameraFrame.push_back(pt);

    cv::Point3f ptObjectFrame = transformPointInverse(pt, rvec, tvec);
    pts3dObjectFrame.push_back(ptObjectFrame);

    rms_3D += (modelPts[i].x-ptObjectFrame.x)*(modelPts[i].x-ptObjectFrame.x) + (modelPts[i].y-ptObjectFrame.y)*(modelPts[i].y-ptObjectFrame.y) +
        (modelPts[i].z-ptObjectFrame.z)*(modelPts[i].z-ptObjectFrame.z);

    std::cout << "modelPts[" << i << "]=" << modelPts[i] << " ; calc=" << ptObjectFrame << std::endl;
  }

  std::cout << "RMS error for model points=" << sqrt(rms_3D / pointbuf.size()) << std::endl;

    cv::namedWindow("Output Window2");

    cv::setMouseCallback("Output Window2", onMouse2);

    cv::imshow("Output Window2", undistorted_img);

    cv::waitKey();
  return 0;
}
