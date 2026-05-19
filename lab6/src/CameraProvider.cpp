#include "CameraProvider.hpp"
#include <iostream>

CameraProvider::CameraProvider(int cameraIndex) {
    cap.open(cameraIndex, cv::CAP_V4L2);

    if (!cap.isOpened()) {
        std::cerr << "Помилка: не вдалось відкрити камеру " << cameraIndex << std::endl;
        return;
    }

    cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M','J','P','G'));
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    cap.set(cv::CAP_PROP_FPS, 30);
    cap.set(cv::CAP_PROP_BUFFERSIZE, 1);
}

CameraProvider::~CameraProvider() {
    if (cap.isOpened()) {
        cap.release();
    }
}

bool CameraProvider::isOpened() const {
    return cap.isOpened();
}

cv::Mat CameraProvider::getFrame() {
    cv::Mat frame;
    cap >> frame;
    // Якщо кадр порожній або сірий — пробуємо ще раз
    if (frame.empty()) {
        cap >> frame;
    }
    return frame;
}