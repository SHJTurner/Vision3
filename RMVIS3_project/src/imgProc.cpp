#include "imgProc.hpp"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/features2d/features2d.hpp>

//#define recordThreshold

static SimpleBlobDetector::Params params;
SimpleBlobDetector detector;

#ifdef recordThreshold
    static VideoWriter videoOut;
#endif


FeatureExtration::FeatureExtration(){
    this->detector = this->ConfigBlobDetector();
#ifdef recordThreshold
    videoOut = VideoWriter("outputVideo.avi", 0, 10, Size(720,960), 1);
    if(!videoOut.isOpened())
    {
        printf("Failed to open output video!\n");
    }
#endif
}

FeatureExtration::~FeatureExtration(){
#ifdef recordThreshold
    videoOut.release();
#endif
}

Ptr<SimpleBlobDetector> FeatureExtration::ConfigBlobDetector()
{
    SimpleBlobDetector::Params params;
    // Change thresholds
    params.minDistBetweenBlobs = 50.0f;
    params.minThreshold = 0;
    params.maxThreshold = 255;
    params.filterByInertia = false;
    params.filterByConvexity = false;
    params.filterByColor = false;
    params.filterByCircularity = false;
    params.filterByArea = false;
    params.minArea = 4.0f;
    params.maxArea = 1000.0f;
    return SimpleBlobDetector::create(params);
}

void FeatureExtration::erodeMat(Mat &_src) {
    Mat _element = getStructuringElement(MORPH_ELLIPSE,
            Size(2*EROSION_SIZE + 1, 2*EROSION_SIZE+1),
            Point(EROSION_SIZE, EROSION_SIZE));
    dilate(_src, _src, _element, Point(-1, -1), DILATE_ITERATIONS);
    erode(_src, _src, _element, Point(-1, -1), ERODE_ITERATIONS);
}

void FeatureExtration::erodeMat(UMat &_src) {
    Mat _element = getStructuringElement(MORPH_ELLIPSE,
            Size(2*EROSION_SIZE + 1, 2*EROSION_SIZE+1),
            Point(EROSION_SIZE, EROSION_SIZE));
    dilate(_src, _src, _element, Point(-1, -1), DILATE_ITERATIONS);
    erode(_src, _src, _element, Point(-1, -1), ERODE_ITERATIONS);

}

bool FeatureExtration::process(Mat& _inputArray1,Mat& _inputArray2, Point2f& _outputKeypoints1, Point2f& _outputKeypoints2)
{
    int Hlow = 0, Slow = 0, Vlow=198;
    int Hhigh=181, Shigh=47, Vhigh=255;

    Mat left = _inputArray1;//.getUMat( ACCESS_READ );
    Mat right = _inputArray2;//.getUMat( ACCESS_READ );
    Mat hsv, gray, mask, Lmasked, Rmasked, red_hue_range;


    cvtColor( left, gray, CV_BGR2GRAY ); //gray scale
    cv::threshold( gray, mask, 100, 255,1); //threshold
    left.copyTo(Lmasked,mask); //create masked img
    cvtColor(Lmasked, hsv, COLOR_RGB2HSV); //convert to HSV
    cv::inRange(hsv, cv::Scalar(110, 200, 10), cv::Scalar(140, 255, 255), red_hue_range); //HSV color segmentation
    erodeMat(red_hue_range); //erode and dilate
    Lkeypoints.clear();
    detector->detect( red_hue_range, Lkeypoints); // find blob
#ifdef recordThreshold
    videoOut.write(Lmasked);
    imshow("Recording...",Lmasked);
    waitKey(1);
#endif

    cvtColor( right, gray, CV_BGR2GRAY ); //gray scale
    cv::threshold( gray, mask, 100, 255,1); //threshold
    right.copyTo(Rmasked,mask); //create masked img
    cvtColor(Rmasked, hsv, COLOR_RGB2HSV); //convert to HSV
    cv::inRange(hsv, cv::Scalar(110, 200, 10), cv::Scalar(140, 255, 255), red_hue_range); //HSV color segmentation
    erodeMat(red_hue_range); //erode and dilate
    Rkeypoints.clear();
    detector->detect( red_hue_range, Rkeypoints); // find blob

    for(KeyPoint LeftPoint :  Lkeypoints)
    {
        for(KeyPoint RightPoint :  Rkeypoints)
        {
            double diff = (LeftPoint.pt.y) - (RightPoint.pt.y);
            if(std::abs(diff) < 10 )
            {
                _outputKeypoints1 = Point2f(LeftPoint.pt.x,LeftPoint.pt.y);
                _outputKeypoints2 = Point2f(RightPoint.pt.x,RightPoint.pt.y);
                return true;
            }
        }
    }
    return false;
}

