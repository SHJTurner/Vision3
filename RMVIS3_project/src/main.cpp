#include <stdio.h>
#include <unistd.h> //testing
#include <thread>
#include <time.h>
#include <mutex>
#include <condition_variable>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "point3dReconstruction.hpp"
#include "imgProc.hpp"
#include "particleFilter.hpp"

using namespace cv;
using namespace std;
typedef Point3_<double> Point3d;

struct ImagePair {
    Mat LeftImage;
    Mat RightImage;
};
struct PointPair {
    Point2f LeftPoint;
    Point2f RightPoint;
};

VideoWriter outVideo;
FeatureExtration _FeatureExtration;
point3dRecontruction Reconstruction3d;
particleFilter pFilter;
VideoCapture captureLeft, captureRight;
Mat imageLeftCap;
Mat imageRightCap;
std::mutex mtxRetrive;
PointPair FeaturePoints;

void CallBackFuncL(int event, int x, int y, int flags, void* userdata){
     if  ( event == EVENT_LBUTTONDOWN )
     {
          FeaturePoints.LeftPoint = Point2f(x,y);
          printf("left x: %d y: %d\n",x,y);
     }
}

void CallBackFuncR(int event, int x, int y, int flags, void* userdata){
     if  ( event == EVENT_LBUTTONDOWN )
     {
          FeaturePoints.RightPoint = Point2f(x,y);
          printf("right x: %d y: %d\n",x,y);
     }
}

VideoCapture ConfigVideoCapture(int dev){
    VideoCapture Vcapture(dev);
    Vcapture.set(CV_CAP_PROP_FRAME_WIDTH,960);
    Vcapture.set(CV_CAP_PROP_FRAME_HEIGHT,720);
    return Vcapture;
}

void threadGrabL(){
        captureLeft.grab();
        captureLeft.retrieve(imageLeftCap);
        Mat Tmp = imageLeftCap.t(); //Rotate Image
        flip(Tmp,imageLeftCap,0);
}

void threadGrabR(){
        captureRight.grab();
        captureRight.retrieve(imageRightCap);
        Mat Tmp = imageRightCap.t(); //Rotate Image
        flip(Tmp,imageRightCap,1);
}

void threadProcess(){
    Mat RemapedL, RemapedR;
    Reconstruction3d.Remap(imageLeftCap,imageRightCap,RemapedL, RemapedR);
    Rect leftROI(133,136,695-133,815-136);
    Rect rightROI(0,149,520-0,785-149);
    Mat left = RemapedL(leftROI);
    Mat right = RemapedR(rightROI);

    //imshow("cam2",right);
    if(_FeatureExtration.process(RemapedL,RemapedR,FeaturePoints.LeftPoint,FeaturePoints.RightPoint))
    {
        printf("PointL: %d %d PointR: %d %d",(int)FeaturePoints.LeftPoint.x,(int)FeaturePoints.LeftPoint.y,
               (int)FeaturePoints.RightPoint.x,(int)FeaturePoints.RightPoint.y);
        Point3d measurement = Reconstruction3d.Calculate3dPoint(FeaturePoints.LeftPoint,FeaturePoints.RightPoint);
        Point3f result = pFilter.filter(measurement.x,measurement.y,measurement.z);
        putText(left, "X: " + to_string(result.x) +"m", Point(10,26), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0,255,0),1.2);
        putText(left, "Y: " + to_string(result.y) +"m", Point(10,41), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0,255,0),1.2);
        putText(left, "Z: " + to_string(result.z) +"m", Point(10,56), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0,255,0),1.2);
        circle(left, Point(FeaturePoints.LeftPoint.x-133,FeaturePoints.LeftPoint.y-136), 10, Scalar(0,255,0), 1);
        imshow("cam1",left);
        //outVideo << left;
    }
}

int main(int argc, char** argv ){
    //outVideo.open("outVideo.avi", 0, 5, Size(695-133,815-136), 1);
    captureLeft = ConfigVideoCapture(2);
    captureRight = ConfigVideoCapture(1);
    namedWindow("cam1",WINDOW_NORMAL);
    setMouseCallback("cam1",CallBackFuncL,NULL);
    namedWindow("cam2",WINDOW_NORMAL);
    setMouseCallback("cam2",CallBackFuncR,NULL);
    Reconstruction3d = point3dRecontruction();
    Reconstruction3d.loadCam();
    Reconstruction3d.InitRmap(Size(720,960));
    _FeatureExtration = FeatureExtration();
    pFilter = particleFilter(10,10,100,-10,-10,0);
    while(true)
    {
        thread GrabL(threadGrabL);
        thread GrabR(threadGrabR);
        GrabL.join();
        GrabR.join();
        thread process(threadProcess);
        process.join();
    }

    return 0;
}
