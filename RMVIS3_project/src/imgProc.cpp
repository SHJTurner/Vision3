#include "imgProc.hpp"
#include <iostream>
#define debug

static SimpleBlobDetector::Params params;
SimpleBlobDetector detector;


FeatureExtration::FeatureExtration(){
    this->detector = this->ConfigBlobDetector();
    ROIL = Rect(133,136,695-133,815-136);
    ROIR = Rect(0,149,520-0,785-149);
}

SimpleBlobDetector FeatureExtration::ConfigBlobDetector()
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
    params.minArea = 2.0f;
    params.maxArea = 1000.0f;
    return SimpleBlobDetector(params);
}

void FeatureExtration::erodeMat(Mat &_src) {
    Mat _element = getStructuringElement(MORPH_ELLIPSE,
            Size(2*EROSION_SIZE + 1, 2*EROSION_SIZE+1),
            Point(EROSION_SIZE, EROSION_SIZE));
    erode(_src, _src, _element, Point(-1, -1), ERODE_ITERATIONS);

}

bool FeatureExtration::process(Mat& _inputArray1,Mat& _inputArray2, Point2f& _outputKeypoints1, Point2f& _outputKeypoints2)
{
    int Hlow = 0, Slow = 0, Vlow=198;
    int Hhigh=181, Shigh=47, Vhigh=255;

    Mat left = _inputArray1(ROIL);
    Mat right = _inputArray2(ROIR);
    Mat hsv, threshold;

    cvtColor(left, hsv, COLOR_RGB2HSV);
    inRange(hsv, Scalar(Hlow, Slow, Vlow), Scalar(Hhigh, Shigh, Vhigh), threshold);
    erodeMat(threshold);
    Lkeypoints.clear();
    detector.detect( threshold, Lkeypoints);

    cvtColor(right, hsv, COLOR_RGB2HSV);
    inRange(hsv, Scalar(Hlow, Slow, Vlow), Scalar(Hhigh, Shigh, Vhigh), threshold);
    erodeMat(threshold);
    Rkeypoints.clear();
    detector.detect( threshold, Rkeypoints);
    for(KeyPoint LeftPoint :  Lkeypoints)
    {
        for(KeyPoint RightPoint :  Rkeypoints)
        {
            double diff = (LeftPoint.pt.y+136) - (RightPoint.pt.y+149);
            //printf("diff: %d\n",(int)diff);
            if(std::abs(diff) < 20 )
            {
                _outputKeypoints1 = Point2f(LeftPoint.pt.x+133,LeftPoint.pt.y+136);
                _outputKeypoints2 = Point2f(RightPoint.pt.x+0,RightPoint.pt.y+149);
                return true;
            }
        }
    }
    return false;
}

