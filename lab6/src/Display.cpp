#include "Display.hpp"

Display::Display(const std::string& windowName) : windowName(windowName) {
    cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE);
}

void Display::show(const cv::Mat& frame) {
    cv::imshow(windowName, frame);
}

void Display::createTrackbar(const std::string& name, int* value, int maxVal) {
    cv::createTrackbar(name, windowName, value, maxVal);
}

const std::string& Display::getWindowName() const {
    return windowName;
}
