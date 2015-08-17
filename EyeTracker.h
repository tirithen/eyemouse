#ifndef EYETRACKER_H
#define EYETRACKER_H

#include "Tracker.h"
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

struct FeatureResult {
    bool found;
    Mat image;
    Rect frame;
};

struct Pupil {
    bool found;
    Point position;
    int radius;
};

class EyeTracker : public Tracker {
  protected:
    bool foundFace;
    bool foundEye;
    bool foundPupil;
    bool eyesAreClosed;
    unsigned int closedEyeDuration;
    unsigned int rightClickDelay;
    int eyesX;
    int eyesY;
    Pupil pupil;
    Rect faceFrame;
    Rect eyeFrame;
    Mat faceImage;
    Mat eyeImage;
    CascadeClassifier faceCascade;
    CascadeClassifier eyeCascade;

    void updateDebugWindow();

    void tellIfFeatureHasChangedVisibility(bool newFound, bool oldFound, string name);

    void updateTrackingAndControlMouse();

    FeatureResult findFeature(Mat image, CascadeClassifier* cascade, int size);

    void findEyeDirection();

    void moveMousePointer();

    void rightClickWithMouse();

    void leftClickWithMouse();


  public:
    EyeTracker(string name, VideoCapture* capture, unsigned int rightClickDelay);

    string getClassName();

    void update();
};

#endif
