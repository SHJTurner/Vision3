#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <thread>
using namespace std;
using namespace cv;

class FeatureExtration
{
#define ERODE_ITERATIONS        2
#define ERODE_PREP_ITERATIONS   5
#define DILATE_ITERATIONS       2
#define EROSION_SIZE            1


public:
        FeatureExtration();
        bool process(Mat& _inputArray1,Mat& _inputArray2,Point2f& _outputKeypoints1, Point2f& _outputKeypoints2);
        Rect ROIL;
        Rect ROIR;
private:
        SimpleBlobDetector ConfigBlobDetector();
        SimpleBlobDetector detector;
        void erodeMat(Mat &_src);
        std::vector<KeyPoint> Lkeypoints;
        std::vector<KeyPoint> Rkeypoints;

};


