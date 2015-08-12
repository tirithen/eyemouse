#ifndef TRACKER_H
#define TRACKER_H

#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

class Tracker {
  protected:
    string name;
    string debugWindowName;
    VideoCapture* capture;
    Mat image;
    Mat imageGray;
    bool debug;
    CvScalar frameColor;

    void updateDebugWindow();

    void say(string message); 

  public:
    Tracker(string name, VideoCapture* capture);

    void enableDebug();

    void disableDebug();

    void toggleDebug(); 

    bool getDebug();

    void updateImage(); 

    void update();

    virtual string getClassName(); 
};

#endif
