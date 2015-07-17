#ifndef DiSPLAY_LOOP
#define DiSPLAY_LOOP

#include <thread>
#include <mutex>

/* Defines */

#define SLEEPTIME 1000

/* Structs */

// model of the display to be shown
struct display {
    int **matrix;
    int width, height;
    std::mutex mutex_lock;
};

/* Prototypes */

// Main display loop
int loop(struct display*);

#endif
