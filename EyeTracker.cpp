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
        struct FeatureResult {
            bool found;
            Mat image;
            Rect frame;
        };

		void updateDebugWindow() {
            if (this->foundFace) {
                rectangle(this->image, this->faceFrame, this->frameColor);
            }

            if (this->foundEye) {
                Rect frame = Rect(this->eyeFrame);
                frame.x += this->faceFrame.x;
                frame.y += this->faceFrame.y;
                rectangle(this->image, frame, this->frameColor);
            }

            Tracker::updateDebugWindow();
        }

		void tellIfFeatureHasChangedVisibility(bool newFound, bool oldFound, string name) {
			if (newFound && !oldFound) {
                this->say("Found " + name);
            } else if (!newFound && oldFound) {
                this->say("Lost " + name);
            }
		}

		void updateTrackingAndControlMouse() {
            FeatureResult result = this->findFeature(this->imageGray, &this->faceCascade, 30);
			this->tellIfFeatureHasChangedVisibility(result.found, this->foundFace, "face");
            this->foundFace = result.found;
            this->faceFrame = result.frame;
            this->faceImage = result.image;

            if (this->foundFace) {
                imshow("face match", result.image);

                FeatureResult result = this->findFeature(this->faceImage, &this->eyeCascade, 20);
				this->tellIfFeatureHasChangedVisibility(result.found, this->foundEye, "eye");
                this->foundEye = result.found;
                this->eyeFrame = result.frame;
                this->eyeImage = result.image;

                if (this->foundEye) {
                    imshow("eye match", result.image);

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

        FeatureResult findFeature(Mat image, CascadeClassifier* cascade, int size) {
            vector<Rect> features;
            FeatureResult result;

            // Assume that nothing was found
            result.found = false;

            // Find all features
            cascade->detectMultiScale(image, features, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(size, size));

            // Return if no feature was found
            if (features.size() == 0) {
                return result;
            }

            // Find the larges feature
            result.frame = features[0];
            result.image = image(features[0]);
            for (int i = 1; i < features.size(); i++) {
                Rect feature = features[i];
                int featureArea = feature.area();
                int resultArea = result.frame.area();

                if (
                    featureArea > 0 &&
                    featureArea > resultArea
                    //(float)featureArea / (float)resultArea > 1.5
                ) {
                    result.frame = feature;
                    result.image = image(feature);
                }
            }

            // Verify that the found feature has a size
            Size resultImageSize = result.image.size();
            if (
                result.frame.area() > 0 &&
                resultImageSize.width > 0 &&
                resultImageSize.height > 0
            ) {
                result.found = true;
            }

            return result;
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
