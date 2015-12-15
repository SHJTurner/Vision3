#include "point3dReconstruction.hpp"


point3dRecontruction::point3dRecontruction() {}

point3dRecontruction::point3dRecontruction(String pathIntrin, String pathExtren)
{
    loadCam(pathIntrin,pathExtren);
}

void point3dRecontruction::loadCam(String pathIntrin, String pathExtren) {
    FileStorage fsIntrin(pathIntrin, FileStorage::READ);
    fsIntrin["M1"] >> StereoCam.M1;
    fsIntrin["D1"] >> StereoCam.D1;
    fsIntrin["M2"] >> StereoCam.M2;
    fsIntrin["D2"] >> StereoCam.D2;
    fsIntrin.release();
    FileStorage fsExtrin(pathExtren, FileStorage::READ);
    fsExtrin["R"] >> StereoCam.R;
    fsExtrin["T"] >> StereoCam.T;
    fsExtrin["R1"] >> StereoCam.R1;
    fsExtrin["R2"] >> StereoCam.R2;
    fsExtrin["P1"] >> StereoCam.P1;
    fsExtrin["P2"] >> StereoCam.P2;
    fsExtrin["Q"] >> StereoCam.Q;
    fsExtrin.release();
}
void point3dRecontruction::InitRmap(Size imgsize)
{
    initUndistortRectifyMap(StereoCam.M1, StereoCam.D1,StereoCam.R1,StereoCam.P1
                            ,imgsize, CV_16SC2, Cam1Rmap[0], Cam1Rmap[1]);
    initUndistortRectifyMap(StereoCam.M2, StereoCam.D2,StereoCam.R2,StereoCam.P2
                            ,imgsize, CV_16SC2, Cam2Rmap[0], Cam2Rmap[1]);
}

void point3dRecontruction::Remap(Mat& _inputCam1,Mat& _inputCam2,Mat& _outputCam1,Mat& _outputCam2)
{
    remap(_inputCam1, _outputCam1, Cam1Rmap[0], Cam1Rmap[1], CV_INTER_LINEAR);
    remap(_inputCam2, _outputCam2, Cam2Rmap[0], Cam2Rmap[1], CV_INTER_LINEAR);
}

void point3dRecontruction::RemapPoints(Point& _inputCam1,Point& _inputCam2, Point& _outputCam1,Point& _outputCam2)
{
    /* Valid areal of rectified image
      cam 1:
      Topleft x: 89 y: 90
      Buttomright: x: 431 y: 513
      roi = 89,90,342
      cam 2:
      Topleft x: 3 y: 110
      Buttomright: x: 315 y: 499*/
    for(int k = 0; k<2; k++)
    {
        //cam 1
        int x = Cam1Rmap[0].at<Vec2s>(_inputCam1)[k]; //x
        int y = Cam1Rmap[1].at<Vec2s>(_inputCam1)[k]; //y
        if(133 < x && x< 695 && 136 < y && y < 815)
        {
            _outputCam1.x = x;
            _outputCam1.y = y;
        }
        cout << "cam1["<< k <<"] x: " << x << "y: " << y << endl;

        //cam 2
        x = Cam2Rmap[0].at<Vec2s>(_inputCam2)[k];//x
        y = Cam2Rmap[1].at<Vec2s>(_inputCam2)[k];//y
        if(0 < x && x < 520 && 149 < y && y < 785)
        {
            _outputCam2.x = x;
            _outputCam2.y = y;
        }

        cout << "cam2["<< k <<"] x: " << x << "y: " << y << endl;
    }

}

Point3d point3dRecontruction::Calculate3dPoint(Point2f cam1Point, Point2f cam2Point)
{
    Mat cam1pnts(1, 1, CV_64FC2);
    Mat cam2pnts(1, 1, CV_64FC2);
    Mat pnts3d(1, 1, CV_64FC4);

    cam1pnts.at<Vec2d>(0)[0] = cam1Point.x;
    cam1pnts.at<Vec2d>(0)[1] = cam1Point.y;
    cam2pnts.at<Vec2d>(0)[0] = cam2Point.x;
    cam2pnts.at<Vec2d>(0)[1] = cam2Point.y;

    cv::triangulatePoints(StereoCam.P1, StereoCam.P2, cam1pnts, cam2pnts, pnts3d); //triangulate 3d point

    pnts3d = pnts3d / pnts3d.at<double>(3, 0); //Normalize 3d Point
    std::cout << "Normalized 3D point: " << pnts3d << std::endl;
    return Point3d (pnts3d.at<double>(0, 0),pnts3d.at<double>(1, 0),pnts3d.at<double>(2, 0));
}
