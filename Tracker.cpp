//#include "Tracker.hpp"
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

        void updateDebugWindow() {
            imshow(this->debugWindowName, this->image);
        }

        void say(string message) {
            string className = this->getClassName();
            cout << className + " \"" + this->name + "\": "+ message + "\n";
        }

    public:
        Tracker(string name, VideoCapture* capture) {
            this->name = name;
            this->debugWindowName = name + " - Debug window";
            this->capture = capture;
            this->debug = false;
			this->frameColor = CV_RGB(0, 255, 0);
        }

        void enableDebug() {
            this->debug = true;
            this->say("Enable debugging");
        }

        void disableDebug() {
            this->debug = false;
            destroyWindow(this->debugWindowName);
            this->say("Disable debugging");
        }

        void toggleDebug() {
            if (this->debug) {
                this->disableDebug();
            } else {
                this->enableDebug();
            }
        }

        bool getDebug() {
            return this->debug;
        }

		void updateImage() {
			// Grab the current image
            this->capture->read(this->image);

            // Create a gray image
            cvtColor(this->image, this->imageGray, CV_BGR2GRAY);
		}

        void update() {
            this->updateImage();

            // If in debug mode, show the debug window
            if (this->debug) {
                this->updateDebugWindow();
            }
        }

        virtual string getClassName() {
            return "Tracker";
        }
};
