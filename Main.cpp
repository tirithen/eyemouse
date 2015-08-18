#include <stdio.h>
#include "EyeTracker.h"
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

void handleKeyInput(EyeTracker* tracker) {
    int key;
    key = waitKey(10) - 0x100000;
    switch (key) {
        case 27: // Esc
            exit(0);
        case 100: // d
            tracker->toggleDebug();
    }
}

void captureWebcam(VideoCapture* capture) {
    capture->open(0);

    if (!capture->isOpened()) {
        cout << "ERROR ACQUIRING VIDEO FEED\n";
        getchar();
        exit(1);
    }

	//capture->set(CV_CAP_PROP_FRAME_WIDTH, 500);
	//capture->set(CV_CAP_PROP_FRAME_HEIGHT, 500);
}

int main() {
    VideoCapture capture;
    captureWebcam(&capture);
    EyeTracker tracker = EyeTracker("Eye mouse", &capture, 1500);
    tracker.enableDebug();

    while (true) {
        tracker.update();
        handleKeyInput(&tracker);
    }
}
