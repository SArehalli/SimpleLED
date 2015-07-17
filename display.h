#ifndef DiSPLAY_LOOP
#define DiSPLAY_LOOP

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
    public:
        std::mutex mutex_lock;

        Display(int, int);
        ~Display();

        int isRunning();
        int getHeight();
        int getWidth();
        int getValue(int, int);
        volatile uint32_t *getGPIO();

        void setValue(int, int, int);
        void stop();
};

/* Prototypes */

// Main display loop
int loop(Display*);

#endif
