#pragma once
#include <opencv2/opencv.hpp>
#include "KeyProcessor.hpp"

class FrameProcessor {
public:
    FrameProcessor();

    cv::Mat process(const cv::Mat& frame, KeyProcessor::Mode mode,
                    int brightness, int crossX, int crossY,
                    bool drawRect, cv::Point rectStart, cv::Point rectEnd);

private:
    cv::Mat applyNormal(const cv::Mat& frame);
    cv::Mat applyGrayscale(const cv::Mat& frame);
    cv::Mat applyInvert(const cv::Mat& frame);
    cv::Mat applyBlur(const cv::Mat& frame);
    cv::Mat applyCanny(const cv::Mat& frame);
    cv::Mat applySobel(const cv::Mat& frame);
    cv::Mat applyGlitch(const cv::Mat& frame);
    cv::Mat applyBinary(const cv::Mat& frame);

    void drawCrosshair(cv::Mat& frame, int x, int y);
    void drawRectangle(cv::Mat& frame, cv::Point p1, cv::Point p2);
    void drawHUD(cv::Mat& frame, KeyProcessor::Mode mode, int fps, int frameCount);
    void applyBrightness(cv::Mat& frame, int brightness);

    int frameCount;
    double fps;
    int64 lastTick;
};
