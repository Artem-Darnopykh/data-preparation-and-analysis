#pragma once

class KeyProcessor {
public:
    enum class Mode {
        NORMAL,
        GRAYSCALE,
        INVERT,
        BLUR,
        CANNY,
        SOBEL,
        GLITCH,
        BINARY
    };

    KeyProcessor();

    // Повертає false якщо треба вийти
    bool processKey(int key);

    Mode getMode() const;
    int getCrosshairX() const;
    int getCrosshairY() const;
    bool isDrawingEnabled() const;

private:
    Mode currentMode;
    int crosshairX;
    int crosshairY;
    bool drawingEnabled;
    static const int STEP = 10;
};
