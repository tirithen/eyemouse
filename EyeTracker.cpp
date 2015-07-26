#include "Tracker.cpp"
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

		void updateDebugWindow() {
            if (this->foundFace) {
                rectangle(this->image, this->faceFrame, this->frameColor);
            }

            if (this->foundEye) {
                rectangle(this->image, this->eyeFrame, this->frameColor);
            }

            if (this->foundPupil) {
                circle(this->image, this->pupil.position, this->pupil.radius, this->frameColor);
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
            this->foundPupil = false;

            FeatureResult result = this->findFeature(this->imageGray, &this->faceCascade, 30);
			this->tellIfFeatureHasChangedVisibility(result.found, this->foundFace, "face");
            this->foundFace = result.found;
            this->faceFrame = result.frame;
            this->faceImage = result.image;

            if (this->foundFace) {
                FeatureResult result = this->findFeature(this->faceImage, &this->eyeCascade, 20);
				this->tellIfFeatureHasChangedVisibility(result.found, this->foundEye, "eye");
                this->foundEye = result.found;
                this->eyeFrame = result.frame;
                this->eyeImage = result.image;

                if (this->foundEye) {
                    // Apply face frame offset to eye frame
                    this->eyeFrame.x += this->faceFrame.x;
                    this->eyeFrame.y += this->faceFrame.y;

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
            Mat thresholdImage;

            // Create inverted and equalized image
            equalizeHist(~this->eyeImage, thresholdImage);
            //thresholdImage = Mat(~this->eyeImage); // Without equalization

            // Make the image binary
            threshold(thresholdImage, thresholdImage, 250, 255, THRESH_BINARY);

            // Apply some blur
            blur(thresholdImage, thresholdImage, Size(6, 6));

            // Make the image binary
            threshold(thresholdImage, thresholdImage, 100, 255, THRESH_BINARY);

            // Find all contours
            vector<vector<Point> > contours;
            findContours(thresholdImage.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

            // Fill holes in each contour
            drawContours(thresholdImage, contours, -1, CV_RGB(255,255,255), -1);

            // Find the round white blob (pupil)
            for (int i = 0; i < contours.size(); i++) {
                double area = contourArea(contours[i]);    // Blob area
                Rect frame = boundingRect(contours[i]);    // Bounding box
                int radius = frame.width / 2;              // Approximate radius

                // Look for round shaped blob
                if (
                    area >= 30 &&
                    abs(1 - ((double)frame.width / (double)frame.height)) <= 0.5 &&
                    abs(1 - (area / (CV_PI * pow(radius, 2)))) <= 0.5
                ) {
                    this->pupil.position.x = frame.x + radius + this->eyeFrame.x;
                    this->pupil.position.y = frame.y + radius + this->eyeFrame.y;
                    this->pupil.radius = radius;
                    this->foundPupil = true;
                    break;
                }
            }

            imshow("tresh pupil", thresholdImage);
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
            //this->eyeCascade.load("haarcascade_eye.xml");
            //this->eyeCascade.load("haarcascade_lefteye_2splits.xml");
            this->eyeCascade.load("haarcascade_righteye_2splits.xml");
            //this->eyeCascade.load("haarcascade_eye_three_eyeglasses.xml");;
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
