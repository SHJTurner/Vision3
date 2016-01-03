#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/legacy/legacy.hpp>

using namespace std;

class particleFilter{

public:
    particleFilter();
    particleFilter(float xMaxRange,float yMaxRange, float zMaxRange, float xMinRange = 0, float yMinRange = 0, float zMinRange = 0);
    cv::Point3f filter(float x, float y, float z);

private:
    float minRange[2];
    float maxRange[2];
    float Range[3];
    CvMat LB, UB;
    CvConDensation* condens;
};
