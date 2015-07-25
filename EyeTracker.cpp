#include "Tracker.cpp"
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

class EyeTracker : public Tracker {
    protected:
        bool foundFace;
        bool foundEye;
        bool eyesAreClosed;
        unsigned int closedEyeDuration;
        unsigned int rightClickDelay;
        int eyesX;
        int eyesY;
        Rect faceFrame;
        Rect eyeFrame;
        Mat faceImage;
		Mat eyeImage;
		CascadeClassifier faceCascade;
		CascadeClassifier eyeCascade;

		void updateDebugWindow() {
            if (this->foundFace) {
                rectangle(this->image, this->faceFrame, this->frameColor);
            }

            if (this->foundEye) {
                rectangle(this->image, this->eyeFrame, this->frameColor);
            }

            Tracker::updateDebugWindow();
        }

		void tellIfFeatureHasChangedVisibility(bool newFound, bool found, string name) {
			if (newFound && !found) {
                this->say("Found " + name);
            } else if (!newFound && found) {
                this->say("Lost " + name);
            }
		}

		void updateTrackingAndControlMouse() {
            bool foundFace = this->findFeature(
                &this->imageGray,
                &this->faceCascade,
                &this->faceFrame,
                &this->faceImage
            );

			this->tellIfFeatureHasChangedVisibility(foundFace, this->foundFace, "face");
            this->foundFace = foundFace;

            if (foundFace) {
                bool foundEye = this->findFeature(
                    &this->faceImage,
                    &this->eyeCascade,
                    &this->eyeFrame,
                    &this->eyeImage
                );

				this->tellIfFeatureHasChangedVisibility(foundEye, this->foundEye, "eye");
                this->foundEye = foundEye;

                if (foundEye) {
                    if (this->eyesAreClosed) {
                        if (this->closedEyeDuration >= this->rightClickDelay) {
                            this->rightClickWithMouse();
                        } else {
                            this->leftClickWithMouse();
                        }
                    } else {
                        this->findEyeDirection();
                        this->moveMousePointer();
                    }
                }
            }
        }

        bool findFeature(Mat* image, CascadeClassifier* cascade, Rect* frame, Mat* frameImage) {
            vector<Rect> features;

            // Find all features
            this->faceCascade.detectMultiScale(*image, features, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30));

            // Return if no feature was found
            if (features.size() == 0) {
                return false;
            }

            // Find the larges feature
            frame = &features[0];
            for (int i = 1; i < features.size(); i++) {
                int area = features[i].area();
                if (area > frame->area()) {
                    frame = &features[i];
                    Mat tempFrameImage = (*image)(features[i]);
                    frameImage = &tempFrameImage;
                }
            }

            return true;
        }

        void setFoundFeature(bool newFound, bool* found, string name) {
            if (newFound && !*found) {
                this->say("Found " + name);
            } else if (!newFound && *found) {
                this->say("Lost " + name);
            }

            found = &newFound;
        }

        void findEyeDirection() {

        }

        void moveMousePointer() {

        }

        void rightClickWithMouse() {

        }

        void leftClickWithMouse() {

        }

    public:
        EyeTracker(string name, VideoCapture* capture, unsigned int rightClickDelay) : Tracker(name, capture) {
            this->foundFace = false;
            this->foundEye = false;
            this->eyesAreClosed = false;
            this->closedEyeDuration = 0;
            this->rightClickDelay = rightClickDelay;

            this->faceCascade.load("haarcascade_frontalface_alt2.xml");
            this->eyeCascade.load("haarcascade_eye.xml");
        }

        string getClassName() {
            return "EyeTracker";
        }

        void update() {
			this->updateImage();
            this->updateTrackingAndControlMouse();

			// If in debug mode, show the debug window
            if (this->debug) {
                this->updateDebugWindow();
            }
        }
};
