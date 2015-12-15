#include <thread>
#include <stdio.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#define IMG_WIDTH 960
#define IMG_HEIGHT 720

#define cap0dev 1
#define cap1dev 2

using namespace std;
using namespace cv;

struct StereoImagePair {
    Mat img0;
    Mat img1;
};

VideoCapture cap0;
VideoCapture cap1;

Mat img0;
Mat img1;

string ImgSavePath = "stereo_imgs/";

StereoImagePair stereoCap;


VideoCapture ConfigVideoCapture(int dev){
    VideoCapture Vcapture(dev);
    Vcapture.set(CV_CAP_PROP_FRAME_WIDTH,IMG_WIDTH);
    Vcapture.set(CV_CAP_PROP_FRAME_HEIGHT,IMG_HEIGHT);
    return Vcapture;
}

void threadGrab0(){
        cap0.grab();
        cap0.retrieve(img0);
        Mat Tmp = img0.t(); //Rotate Image
        flip(Tmp,img0,0);
}

void threadGrab1(){
        cap1.grab();
        cap1.retrieve(img1);
        Mat Tmp = img1.t(); //Rotate Image
        flip(Tmp,img1,1);
}

int main(int argc, char** argv ){

    //init capture devices
    cap0 = ConfigVideoCapture(cap0dev);
    cap1 = ConfigVideoCapture(cap1dev);
    namedWindow("cap0",WINDOW_NORMAL);
    namedWindow("cap1",WINDOW_NORMAL);

    vector<Mat> leftImgs;
    vector<Mat> rightImgs;

    printf("Press 's' to capture images\n");
    fflush(stdout);

    for(unsigned int i = 0; i < 15; i++){
            thread Grab0(threadGrab0);
            thread Grab1(threadGrab1);
            Grab0.join();
            Grab1.join();
            leftImgs.push_back(img0);
            rightImgs.push_back(img1);
            printf("Image set %d saved\n",leftImgs.size());
            fflush(stdout);
    }

    //save images
    for(int i = 0; i < leftImgs.size();i++){
        string filenameLeft;
        string filenameRight;
        printf("Writing image pair %d of 15... ",i+1);
        fflush(stdout);
        if(i < 9){
            filenameLeft = ImgSavePath + "left0" + to_string(i+1) +".jpg";
            filenameRight = ImgSavePath + "right0" + to_string(i+1) +".jpg";
        }
        else{
            filenameLeft = ImgSavePath + "left" + to_string(i+1) +".jpg";
            filenameRight = ImgSavePath + "right" + to_string(i+1) +".jpg";
        }
        imwrite(filenameLeft,leftImgs[i]);
        imwrite(filenameRight,rightImgs[i]);
        printf("done\n");
        fflush(stdout);
    }
}
