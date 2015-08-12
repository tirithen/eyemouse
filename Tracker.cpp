#include "Tracker.h"
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;


void Tracker::updateDebugWindow() {
  Size size = this->image.size();
  if (size.width > 0 && size.height > 0) {
    imshow(this->debugWindowName, this->image);
  }
}

void Tracker::say(string message) {
  string className = this->getClassName();
  cout << className + " \"" + this->name + "\": "+ message + "\n";
}

Tracker::Tracker(string name, VideoCapture* capture) {
  this->name = name;
  this->debugWindowName = name + " - Debug window";
  this->capture = capture;
  this->debug = false;
  this->frameColor = CV_RGB(0, 255, 0);
}

void Tracker::enableDebug() {
  this->debug = true;
  this->say("Enable debugging");
}

void Tracker::disableDebug() {
  this->debug = false;
  destroyWindow(this->debugWindowName);
  this->say("Disable debugging");
}

void Tracker::toggleDebug() {
  if (this->debug) {
    this->disableDebug();
  } else {
    this->enableDebug();
  }
}

bool Tracker::getDebug() {
  return this->debug;
}

void Tracker::updateImage() {
  // Grab the current image
  this->capture->read(this->image);

  // Flip the image
  //flip(this->image, this->image, 1);

  // Create a gray image
  cvtColor(this->image, this->imageGray, CV_BGR2GRAY);
}

void Tracker::update() {
  this->updateImage();

  // If in debug mode, show the debug window
  if (this->debug) {
    this->updateDebugWindow();
  }
}

string Tracker::getClassName() {
  return "Tracker";
}
