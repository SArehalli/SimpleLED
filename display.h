#ifndef DISPLAY_H
#define DISPLAY_H

#include <thread>
#include <mutex>

/* Defines */

#define SLEEPTIME 100

/* Class */

// model of the display to be shown
class Display {
    private: 
        int state;
        int **matrix;
        int width, height;
        volatile uint32_t *GPIO_address;    
        std::thread viewLoop;

    public:
        std::mutex mutex_lock;

        Display(int, int);
        ~Display();

        int isRunning();
        int getHeight();
        int getWidth();
        int getValue(int, int);
        volatile uint32_t *getGPIO();

        void drawRectangle(int, int, int, int, int);
        void drawTriangle(int, int, int, int, int);
        void clear();

        void setValue(int, int, int);
        void stop();
};

/* Prototypes */

// Main display loop
int loop(Display*);
int viewLoop(Display *disp);

#endif
