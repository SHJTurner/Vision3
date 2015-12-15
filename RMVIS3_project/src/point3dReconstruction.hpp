#include <stdio.h>
#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>

using namespace std;
using namespace cv;

typedef Point3_<double> Point3d;

struct StereoCamera {
    Mat R;
    Mat T;
    Mat R1;
    Mat R2;
    Mat P1;
    Mat P2;
    Mat Q;

    Mat M1;
    Mat D1;
    Mat M2;
    Mat D2;
};

class point3dRecontruction{

    public:
        point3dRecontruction();
        point3dRecontruction(String pathIntrin, String pathExtren);
        void loadCam(String pathIntrin = "calibration/intrinsics.yml", String pathExtren = "calibration/extrinsics.yml");
        Point3d Calculate3dPoint(Point2f cam1Point, Point2f cam2Point);
        void Remap(Mat& _inputArray1,Mat& _inputArray2,Mat& _outputArray1,Mat& _outputArray2);
        void RemapPoints(Point& _inputCam1,Point& _inputCam2, Point& _PtCam1,Point& _outputCam2);
        void InitRmap(Size imgsize);
        Mat Cam1Rmap[2];
        Mat Cam2Rmap[2];
    private:
        StereoCamera StereoCam;
};
