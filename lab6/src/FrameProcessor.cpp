#include "FrameProcessor.hpp"
#include <map>

FrameProcessor::FrameProcessor() : frameCount(0), fps(0.0) {
    lastTick = cv::getTickCount();
}

cv::Mat FrameProcessor::process(const cv::Mat& frame, KeyProcessor::Mode mode,
                                 int brightness, int crossX, int crossY,
                                 bool drawRect, cv::Point rectStart, cv::Point rectEnd) {
    frameCount++;

    // Розрахунок FPS
    int64 now = cv::getTickCount();
    double elapsed = (now - lastTick) / cv::getTickFrequency();
    if (elapsed >= 0.5) {
        fps = frameCount / elapsed;
        frameCount = 0;
        lastTick = now;
    }

    cv::Mat result;
    switch (mode) {
        case KeyProcessor::Mode::NORMAL:    result = applyNormal(frame);    break;
        case KeyProcessor::Mode::GRAYSCALE: result = applyGrayscale(frame); break;
        case KeyProcessor::Mode::INVERT:    result = applyInvert(frame);    break;
        case KeyProcessor::Mode::BLUR:      result = applyBlur(frame);      break;
        case KeyProcessor::Mode::CANNY:     result = applyCanny(frame);     break;
        case KeyProcessor::Mode::SOBEL:     result = applySobel(frame);     break;
        case KeyProcessor::Mode::GLITCH:    result = applyGlitch(frame);    break;
        case KeyProcessor::Mode::BINARY:    result = applyBinary(frame);    break;
        default:                            result = applyNormal(frame);    break;
    }

    applyBrightness(result, brightness);
    drawCrosshair(result, crossX, crossY);
    if (drawRect) drawRectangle(result, rectStart, rectEnd);
    drawHUD(result, mode, static_cast<int>(fps), frameCount);

    return result;
}

cv::Mat FrameProcessor::applyNormal(const cv::Mat& frame) {
    return frame.clone();
}

cv::Mat FrameProcessor::applyGrayscale(const cv::Mat& frame) {
    cv::Mat gray, result;
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    cv::cvtColor(gray, result, cv::COLOR_GRAY2BGR);
    return result;
}

cv::Mat FrameProcessor::applyInvert(const cv::Mat& frame) {
    cv::Mat result;
    cv::bitwise_not(frame, result);
    return result;
}

cv::Mat FrameProcessor::applyBlur(const cv::Mat& frame) {
    cv::Mat result;
    cv::GaussianBlur(frame, result, cv::Size(21, 21), 0);
    return result;
}

cv::Mat FrameProcessor::applyCanny(const cv::Mat& frame) {
    cv::Mat gray, edges, result;
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    cv::Canny(gray, edges, 50, 150);
    cv::cvtColor(edges, result, cv::COLOR_GRAY2BGR);
    return result;
}

cv::Mat FrameProcessor::applySobel(const cv::Mat& frame) {
    cv::Mat gray, gradX, gradY, absX, absY, result;
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    cv::Sobel(gray, gradX, CV_16S, 1, 0);
    cv::Sobel(gray, gradY, CV_16S, 0, 1);
    cv::convertScaleAbs(gradX, absX);
    cv::convertScaleAbs(gradY, absY);
    cv::addWeighted(absX, 0.5, absY, 0.5, 0, gray);
    cv::cvtColor(gray, result, cv::COLOR_GRAY2BGR);
    return result;
}

cv::Mat FrameProcessor::applyGlitch(const cv::Mat& frame) {
    cv::Mat result = frame.clone();
    std::vector<cv::Mat> channels(3);
    cv::split(result, channels);

    int shift = 15;
    // Зсув R каналу вправо
    cv::Mat rShifted = cv::Mat::zeros(frame.size(), CV_8UC1);
    channels[2](cv::Rect(0, 0, frame.cols - shift, frame.rows))
        .copyTo(rShifted(cv::Rect(shift, 0, frame.cols - shift, frame.rows)));
    channels[2] = rShifted;

    // Зсув B каналу вліво
    cv::Mat bShifted = cv::Mat::zeros(frame.size(), CV_8UC1);
    channels[0](cv::Rect(shift, 0, frame.cols - shift, frame.rows))
        .copyTo(bShifted(cv::Rect(0, 0, frame.cols - shift, frame.rows)));
    channels[0] = bShifted;

    cv::merge(channels, result);
    return result;
}

cv::Mat FrameProcessor::applyBinary(const cv::Mat& frame) {
    cv::Mat gray, binary, result;
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    cv::threshold(gray, binary, 127, 255, cv::THRESH_BINARY);
    cv::cvtColor(binary, result, cv::COLOR_GRAY2BGR);
    return result;
}

void FrameProcessor::drawCrosshair(cv::Mat& frame, int x, int y) {
    x = std::min(x, frame.cols - 1);
    y = std::min(y, frame.rows - 1);
    int size = 20;
    cv::Scalar color(0, 255, 0);
    cv::line(frame, cv::Point(x - size, y), cv::Point(x + size, y), color, 2);
    cv::line(frame, cv::Point(x, y - size), cv::Point(x, y + size), color, 2);
    cv::circle(frame, cv::Point(x, y), 5, color, 1);
}

void FrameProcessor::drawRectangle(cv::Mat& frame, cv::Point p1, cv::Point p2) {
    cv::rectangle(frame, p1, p2, cv::Scalar(0, 0, 255), 2);
}

void FrameProcessor::drawHUD(cv::Mat& frame, KeyProcessor::Mode mode, int fps, int fc) {
    static const std::map<KeyProcessor::Mode, std::string> modeNames = {
        {KeyProcessor::Mode::NORMAL,    "1: Normal"},
        {KeyProcessor::Mode::GRAYSCALE, "2: Grayscale"},
        {KeyProcessor::Mode::INVERT,    "3: Invert"},
        {KeyProcessor::Mode::BLUR,      "4: Gaussian Blur"},
        {KeyProcessor::Mode::CANNY,     "5: Canny"},
        {KeyProcessor::Mode::SOBEL,     "6: Sobel"},
        {KeyProcessor::Mode::GLITCH,    "7: Glitch"},
        {KeyProcessor::Mode::BINARY,    "8: Binary"},
    };

    std::string modeName = modeNames.count(mode) ? modeNames.at(mode) : "Unknown";

    // Фон для тексту
    cv::rectangle(frame, cv::Point(5, 5), cv::Point(260, 75), cv::Scalar(0, 0, 0), -1);

    cv::putText(frame, "Mode: " + modeName,
                cv::Point(10, 25), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 255, 0), 1);
    cv::putText(frame, "FPS: " + std::to_string(static_cast<int>(fps)),
                cv::Point(10, 48), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 255, 0), 1);
    cv::putText(frame, "Frame: " + std::to_string(fc),
                cv::Point(10, 68), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(180, 180, 180), 1);

    // Підказки знизу
    cv::rectangle(frame, cv::Point(0, frame.rows - 30), cv::Point(frame.cols, frame.rows),
                  cv::Scalar(0, 0, 0), -1);
    cv::putText(frame, "1-8: mode | Arrows: crosshair | D: draw rect | ESC: exit",
                cv::Point(5, frame.rows - 10), cv::FONT_HERSHEY_SIMPLEX, 0.45,
                cv::Scalar(200, 200, 200), 1);
}

void FrameProcessor::applyBrightness(cv::Mat& frame, int brightness) {
    int delta = brightness - 50; // 0-100 -> -50..+50
    frame.convertTo(frame, -1, 1, delta);
}
