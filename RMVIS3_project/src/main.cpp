#include <stdio.h>
#include <unistd.h> //testing
#include <thread>
#include <time.h>
#include <mutex>
#include <fstream>
#include <condition_variable>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "point3dReconstruction.hpp"
#include "imgProc.hpp"
//#include "particleFilter.hpp"

//#define StereoCamConnected
#define Record
//#define imShow
using namespace cv;
using namespace std;
typedef Point3_<double> Point3d;


Mat tempdummy;

struct ImagePair {
    Mat LeftImage;
    Mat RightImage;
};
struct PointPair {
    Point2f LeftPoint;
    Point2f RightPoint;
};
#ifdef Record
VideoWriter outVideo;
#endif
vector<Point3d> results;

FeatureExtration _FeatureExtration;
point3dRecontruction Reconstruction3d;
//particleFilter pFilter;
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
//    Rect leftROI(133,136,695-133,815-136);
//    Rect rightROI(0,149,520-0,785-149);
//    Mat left = RemapedL(leftROI);
//    Mat right = RemapedR(rightROI);
    //imshow("cam2",right);
    if(_FeatureExtration.process(RemapedL,RemapedR,FeaturePoints.LeftPoint,FeaturePoints.RightPoint))
    {
        //printf("PointL: %d %d PointR: %d %d",(int)FeaturePoints.LeftPoint.x,(int)FeaturePoints.LeftPoint.y,
          //     (int)FeaturePoints.RightPoiFileStoragent.x,(int)FeaturePoints.RightPoint.y);
        Point3d result = Reconstruction3d.Calculate3dPoint(FeaturePoints.LeftPoint,FeaturePoints.RightPoint);
        //Point3f result = pFilter.filter(measurement.x,measurement.y,measurement.z);
#ifdef Record
        circle(RemapedL, Point(FeaturePoints.LeftPoint.x,FeaturePoints.LeftPoint.y), 5, Scalar(0,255,0), 2);
        circle(RemapedL, Point(FeaturePoints.LeftPoint.x,FeaturePoints.LeftPoint.y), result.z*25, Scalar(255,255,0), 2);
        Mat Tmp = RemapedL.t(); //Rotate Image
        flip(Tmp,RemapedL,0);
        Rect ROI = Rect(100,0,770,620);
        Mat ROIRemapedL = RemapedL(ROI);
        putText(ROIRemapedL, "Y: " + to_string(result.x) +"m", Point(10,56), FONT_HERSHEY_SIMPLEX, 1, Scalar(0,255,0),1.2);
        putText(ROIRemapedL, "X: " + to_string(result.y) +"m", Point(10,21), FONT_HERSHEY_SIMPLEX, 1, Scalar(0,255,0),1.2);
        putText(ROIRemapedL, "Z: " + to_string(result.z) +"m", Point(10,86), FONT_HERSHEY_SIMPLEX, 1, Scalar(0,255,0),1.2);
        namedWindow("Tracking (left)",WINDOW_NORMAL);
        //imshow("Tracking (left)",ROIRemapedL);
        results.push_back(Point3d(result.y,result.x,result.z));
        waitKey(1);

        Mat frame(Size(770+568,620),CV_8UC3,Scalar(0,0,0));
        ROIRemapedL.copyTo(frame(Rect(0,0,ROIRemapedL.cols,ROIRemapedL.rows)));
        tempdummy.copyTo(frame(Rect(ROIRemapedL.cols,100,tempdummy.cols,tempdummy.rows)));
        outVideo.write(frame);
        imshow("Tracking (left)",frame);
        waitKey(1);
        outVideo.write(frame);
#endif
#ifdef imShow

        circle(RemapedL, Point(FeaturePoints.LeftPoint.x,FeaturePoints.LeftPoint.y), 10, Scalar(0,255,0), 3);
        Mat Tmp = RemapedL.t(); //Rotate Image
        flip(Tmp,RemapedL,0);
        Rect ROI = Rect(100,0,770,620);
        Mat ROIRemapedL = RemapedL(ROI);
        putText(ROIRemapedL, "Y: " + to_string(result.x) +"m", Point(10,56), FONT_HERSHEY_SIMPLEX, 1, Scalar(0,255,0),1.2);
        putText(ROIRemapedL, "X: " + to_string(result.y) +"m", Point(10,21), FONT_HERSHEY_SIMPLEX, 1, Scalar(0,255,0),1.2);
        putText(ROIRemapedL, "Z: " + to_string(result.z) +"m", Point(10,86), FONT_HERSHEY_SIMPLEX, 1, Scalar(0,255,0),1.2);

        imshow("Tracking (left)",ROIRemapedL);
        waitKey(1);
#endif

    }
    else
    {
        Mat Tmp = RemapedL.t(); //Rotate Image
        flip(Tmp,RemapedL,0);
        Rect ROI = Rect(100,0,770,620);
        Mat ROIRemapedL = RemapedL(ROI);
        putText(ROIRemapedL, "Y: Marker lost!!", Point(10,56), FONT_HERSHEY_SIMPLEX, 1, Scalar(0,255,0),1.2);
        putText(ROIRemapedL, "X: Marker lost!!", Point(10,21), FONT_HERSHEY_SIMPLEX, 1, Scalar(0,255,0),1.2);
        putText(ROIRemapedL, "Z: Marker lost!!", Point(10,86), FONT_HERSHEY_SIMPLEX, 1, Scalar(0,255,0),1.2);
        namedWindow("Tracking (left)",WINDOW_NORMAL);
        //imshow("Tracking (left)",ROIRemapedL);
        if(!results.empty())
            results.push_back(Point3d(0.0,0.0,0.0));

        Mat frame(Size(770+568,620),CV_8UC3,Scalar(0,0,0));
        ROIRemapedL.copyTo(frame(Rect(0,0,ROIRemapedL.cols,ROIRemapedL.rows)));
        tempdummy.copyTo(frame(Rect(ROIRemapedL.cols,100,tempdummy.cols,tempdummy.rows)));
        outVideo.write(frame);
        imshow("Tracking (left)",frame);
        waitKey(1);
    }
}

int main(int argc, char** argv ){

VideoCapture tracking("sample4/tracking.avi");

#ifdef Record
    outVideo.open("outVideo_sample4.avi", 0, 10, Size(770+568,620), 1);
    if(!outVideo.isOpened())
    {
        printf("Failed to open output video!\n");
        return 0;
    }
#endif

#ifdef StereoCamConnected
    captureLeft = ConfigVideoCapture(2);
    captureRight = ConfigVideoCapture(1);
    //namedWindow("cam1",WINDOW_NORMAL);
    //setMouseCallback("cam1",CallBackFuncL,NULL);
    //namedWindow("cam2",WINDOW_NORMAL);
    //setMouseCallback("cam2",CallBackFuncR,NULL);
#else
    VideoCapture left("sample4/Cam0.avi");
    VideoCapture right("sample4/Cam1.avi");


    if(!left.isOpened() || !right.isOpened())
    {
        printf("Failed to open video!\n");
        return 0;
    }
#endif

    Reconstruction3d = point3dRecontruction();
    Reconstruction3d.loadCam();
    Reconstruction3d.InitRmap(Size(720,960));
    _FeatureExtration = FeatureExtration();
    //pFilter = particleFilter(10,10,100,-10,-10,0);
    namedWindow("Tracking (left)",WINDOW_NORMAL);
    for(;;)
    {
        #ifdef StereoCamConnected //capture imgs from stereo cam
            thread GrabL(threadGrabL);
            thread GrabR(threadGrabR);
            GrabL.join();
            GrabR.join();
        #else //read video
            if(!left.read(imageLeftCap)) //break when video ends
                break;
           if(! tracking.read(tempdummy))
               break;
            right.read(imageRightCap);
        #endif
        thread process(threadProcess);
        process.join();
    }
#ifdef Record
    ofstream outfile;
    outfile.open("sample4.txt");
    for (Point3d point : results)
    {
        cout << point.x << " " << point.y << " " << point.z << endl;
        outfile << point.x << " " << point.y << " " << point.z << endl;
    }
    outfile.close();
    outVideo.release();
#endif
    return 0;
}
