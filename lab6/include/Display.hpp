#pragma once
#include <opencv2/opencv.hpp>
#include <string>

class Display {
public:
    Display(const std::string& windowName);
    void show(const cv::Mat& frame);
    void createTrackbar(const std::string& name, int* value, int maxVal);
    const std::string& getWindowName() const;

private:
    std::string windowName;
};
