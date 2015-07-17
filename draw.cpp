#include "display.h"

void drawSquare(Display * disp, int x, int y, 
                int width, int height, int color) {
    
    // Get lock
    disp->mutex_lock.lock();

    // Draw a square
    for (int i = y; i < y + height; i++) {
        for (int j = x; j < x + width; j++) {
            disp->setValue(i, j, color);
        }
    }

    // Free Lock
    disp->mutex_lock.unlock();
}
