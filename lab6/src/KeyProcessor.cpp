#include <algorithm>
#include "KeyProcessor.hpp"

KeyProcessor::KeyProcessor()
    : currentMode(Mode::NORMAL), crosshairX(320), crosshairY(240), drawingEnabled(false) {}

bool KeyProcessor::processKey(int key) {
    switch (key) {
        case 27:  // ESC — вихід
            return false;

        // Перемикання режимів
        case '1': currentMode = Mode::NORMAL;    break;
        case '2': currentMode = Mode::GRAYSCALE; break;
        case '3': currentMode = Mode::INVERT;    break;
        case '4': currentMode = Mode::BLUR;      break;
        case '5': currentMode = Mode::CANNY;     break;
        case '6': currentMode = Mode::SOBEL;     break;
        case '7': currentMode = Mode::GLITCH;    break;
        case '8': currentMode = Mode::BINARY;    break;

        // Керування хрестиком стрілками
        case 81: // ←
            crosshairX = std::max(0, crosshairX - STEP); break;
        case 83: // →
            crosshairX += STEP; break;
        case 82: // ↑
            crosshairY = std::max(0, crosshairY - STEP); break;
        case 84: // ↓
            crosshairY += STEP; break;

        // D — вмикає/вимикає малювання прямокутника
        case 'd':
        case 'D':
            drawingEnabled = !drawingEnabled; break;

        default: break;
    }
    return true;
}

KeyProcessor::Mode KeyProcessor::getMode() const { return currentMode; }
int KeyProcessor::getCrosshairX() const { return crosshairX; }
int KeyProcessor::getCrosshairY() const { return crosshairY; }
bool KeyProcessor::isDrawingEnabled() const { return drawingEnabled; }
