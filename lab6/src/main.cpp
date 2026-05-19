#include <opencv2/opencv.hpp>
#include "CameraProvider.hpp"
#include "KeyProcessor.hpp"
#include "FrameProcessor.hpp"
#include "Display.hpp"

// Глобальні змінні для callback миші
struct MouseState {
    bool drawing = false;
    bool hasRect = false;
    cv::Point start{0, 0};
    cv::Point end{0, 0};
};

MouseState mouseState;

void onMouse(int event, int x, int y, int /*flags*/, void* /*userdata*/) {
    if (event == cv::EVENT_LBUTTONDOWN) {
        mouseState.drawing = true;
        mouseState.start = cv::Point(x, y);
        mouseState.end   = cv::Point(x, y);
    } else if (event == cv::EVENT_MOUSEMOVE && mouseState.drawing) {
        mouseState.end = cv::Point(x, y);
    } else if (event == cv::EVENT_LBUTTONUP) {
        mouseState.drawing = false;
        mouseState.end = cv::Point(x, y);
        mouseState.hasRect = true;
    }
}

int main() {
    CameraProvider camera(0);
    if (!camera.isOpened()) {
        return -1;
    }

    Display display("Lab6: OpenCV Camera");
    KeyProcessor keyProc;
    FrameProcessor frameProc;

    int brightness = 50; // слайдер 0-100, середина = без змін
    display.createTrackbar("Brightness", &brightness, 100);

    cv::setMouseCallback(display.getWindowName(), onMouse);

    while (true) {
        cv::Mat frame = camera.getFrame();
        if (frame.empty()) continue;

        cv::Mat result = frameProc.process(
            frame,
            keyProc.getMode(),
            brightness,
            keyProc.getCrosshairX(),
            keyProc.getCrosshairY(),
            mouseState.hasRect || mouseState.drawing,
            mouseState.start,
            mouseState.end
        );

        display.show(result);

        int key = cv::waitKey(1);
        if (key != -1) {
            if (!keyProc.processKey(key)) break;
        }
    }

    cv::destroyAllWindows();
    return 0;
}
