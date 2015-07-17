#include <thread>
#include <iostream>
#include <ctime>
#include "display.h"
#include "draw.h"

int main() {
    Display *disp = new Display(32, 32);

    struct timespec pauseTime = {0};
    pauseTime.tv_sec = 10;

    std::thread mainLoop = std::thread(loop, disp);

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            drawSquare(disp, i*4, j*4, 4, 4, 
                       (((i+j)% 2 == 0)? 0x00FF00 : 0x000000)  + 
                       (((i+j)% 3 == 0)? 0x0000FF : 0x000000)  + 
                       (((i+j)% 5 == 0)? 0xFF0000 : 0x000000));
        }
    }

    nanosleep(&pauseTime, NULL);
    drawSquare(disp, 0, 0, 32, 32, 0);

    mainLoop.join();
}

