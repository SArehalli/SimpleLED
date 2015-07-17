#include <thread>
#include <iostream>
#include <ctime>
#include "display.h"
#include "draw.h"

int main() {
    Display *disp = new Display(32, 32);

    struct timespec pauseTime = {0};
    pauseTime.tv_nsec = 10000;
    pauseTime.tv_sec = 10;

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            drawSquare(disp, i*4, j*4, 4, 4, ((j * 32)<<8)+ (i*32));
        }
    }

    nanosleep(&pauseTime, NULL);
    disp->stop();
}

