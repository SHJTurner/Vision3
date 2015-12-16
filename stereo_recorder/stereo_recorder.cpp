#include <thread>
#include <stdio.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#define IMG_WIDTH 960
#define IMG_HEIGHT 720

#define cap0dev 0
#define cap1dev 1

using namespace std;
using namespace cv;

VideoCapture cap0;
VideoCapture cap1;

Mat img0;
Mat img1;

vector<Mat> leftImgs;
vector<Mat> rightImgs;
VideoWriter outputVideocap0;
VideoWriter outputVideocap1;

VideoCapture ConfigVideoCapture(int dev){
    VideoCapture Vcapture(dev);
    //Vcapture.open(dev);
    Vcapture.set(CV_CAP_PROP_FRAME_WIDTH,IMG_WIDTH);
    Vcapture.set(CV_CAP_PROP_FRAME_HEIGHT,IMG_HEIGHT);
    return Vcapture;
}

void threadGrab0(){
        cap0.grab();
        cap0.retrieve(img0);
        Mat Tmp = img0.t(); //Rotate Image
        flip(Tmp,img0,0);
        //leftImgs.push_back(img0);
        outputVideocap0.write(img0);
}

void threadGrab1(){
        cap1.grab();
        cap1.retrieve(img1);
        Mat Tmp = img1.t(); //Rotate Image
        flip(Tmp,img1,1);
        //rightImgs.push_back(img1);
        outputVideocap1.write(img1);
}




int main(int argc, char** argv ){


    //init capture devices
    cap0 = ConfigVideoCapture(cap0dev);
    cap1 = ConfigVideoCapture(cap1dev);
    namedWindow("cap0",WINDOW_NORMAL);
    namedWindow("cap1",WINDOW_NORMAL);

    outputVideocap0.open("RecoredVideo/Cam0.avi",CV_FOURCC('M', 'J', 'P', 'G'),20,Size(720,960),true);
    outputVideocap1.open("RecoredVideo/Cam1.avi",CV_FOURCC('M', 'J', 'P', 'G'),20,Size(720,960),true);
    if (!outputVideocap0.isOpened() || !outputVideocap1.isOpened())
    {
            printf("Output video could not be opened\n");
            return 0;
    }


    if (!cap0.isOpened() || !cap1.isOpened()){
            printf("Output video could not be opened\n");
            return 0;
    }

    //record video
    printf("Starting to record video... \n(Press space key to stop)\n");
    fflush(stdout);
    for(;;){
            thread Grab0(threadGrab0);
            thread Grab1(threadGrab1);
            Grab0.join();
            Grab1.join();

            char c = (char)waitKey(1);
            if( c == 'c')
                break;
    }





   /* for(Mat img : leftImgs)
    {
        outputVideocap0.write(img);
    }
    if (!outputVideocap1.isOpened())
    {
            printf("Output video could not be opened\n");
            return 0;
    }
    for(Mat img : rightImgs)
    {
        outputVideocap1.write(img);
    }*/
    outputVideocap0.release();
    outputVideocap1.release();

    printf(" done\n");
    fflush(stdout);
    return 0;
}
