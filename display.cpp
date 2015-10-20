#include <cstdlib>
#include <stdint.h>
#include <time.h>
#include <iostream>
#include <thread>
#include <cmath>
#include "display.h"
#include "errors.h"

extern "C" {
    #include "gpio.h"
}

/* Display methods */

/*
 * CONSTRUCTOR
 *      Dynamically allocates matrix
 */

Display::Display(int h, int w) {
    height = h;
    width = w;
    state = 1;

    // Map the GPIO hardware address to virtual memory
    GPIO_address = GPIOSetup();

    viewLoop = std::thread(loop, this);
    
    // Make the matrix
    matrix = new int*[height];

    for (int i = 0; i < height; i++) {
        matrix[i] = new int[width];
    }

    // Clear the board
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            matrix[i][j] = 0;
        }
    }
}

/*
 * DESTRUCTOR
 *      Dynamically deallocates matrix
 */
Display::~Display() {

    // Delete the Matrix
    for (int i = 0; i < height; i++) {
        delete matrix[i];
    }
    
    delete matrix;

    // let the loop die;
    this->stop();
}

/* Getters */

int Display::isRunning() {
    return state;
}

int Display::getHeight() {
    return height;
}

int Display::getWidth() {
    return width;
}

int Display::getValue(int row, int col) {
    return matrix[row][col];
}

volatile uint32_t *Display::getGPIO() {
    return GPIO_address;
}

/* Setters */

void Display::setValue(int row, int col, int val) {
    matrix[row][col] = val;    
}

void Display::stop() {
    // Clear Screen
    this->drawRectangle(0,0,32,32,0);

    // Stop the loop
    state = 0;

    // ensure that the loop is finished 
    viewLoop.join();
}

void Display::drawRectangle(int x, int y, 
                            int width, int height, int color) {
   
    // Bounds checking
    if (x < 0 || y < 0 || width < 0 || height < 0 ||
        (x + width > this->width) || 
        (y + height > this->height)) {
        std::cerr << ERR_BOUNDS;
    }

    if (color < 0 || color > 0xFFFFFF) {
        std::cerr << ERR_COLOR;
    }

    // Get lock
    this->mutex_lock.lock();
    
    // Draw a rectangle
    for (int i = y; i < y + height; i++) {
        for (int j = x; j < x + width; j++) {
            this->setValue(i, j, color);
        }
    }

    // Free Lock
    this->mutex_lock.unlock();
}

void Display::drawTriangle(int x, int y, 
                           int width, int height, int color) {
   
    // Bounds checking
    if (x < 0 || y < 0 || 
        (x + width < 0) || 
        (x + height < 0) ||
        (x + width > this->width) || 
        (y + height > this->height)) {
        std::cerr << ERR_BOUNDS;
    }

    if (color < 0 || color > 0xFFFFFF) {
        std::cerr << ERR_COLOR;
    }

    // Get lock
    this->mutex_lock.lock();
    
    // Draw a triangle
    if (height > 0) {
        for (int i = y; i < y + height; i++) {
            double step = ((double) width)/height;
            if (width > 0) {
                for (int j = x; j < x + width - std::round((i - y) * step); j++) {
                    this->setValue(i, j, color);
                }
            }
            else {
                for (int j = x; j > x + width - std::round((i - y) * step); j--) {
                    this->setValue(i, j, color);
                }
            }
        }
    } else {
        for (int i = y; i > y + height; i--) {
            double step = ((double) width)/height;
            if (width > 0) {
                for (int j = x; j < x + width - std::round((i - y) * step); j++) {
                    this->setValue(i, j, color);
                }
            }
            else {
                for (int j = x; j > x + width - std::round((i - y) * step); j--) {
                    this->setValue(i, j, color);
                }
            }
        }
    }

    // Free Lock
    this->mutex_lock.unlock();
}

void Display::clear() {
    drawRectangle(0, 0, width, height, 0);
}

/* End Display methods */

/* FUNCTION NAME:
 *      PWM
 *
 * PARAMETERS:
 *      color - number from 0 - 255 designating color intensity
 *
 * DESCRIPTION:
 *      Allow the rendering of more than 7 colors by flickering LEDs 
 *      proportional to color intensity. E.g., Pulse Width Modulation
 */
int PWM(int color) {

    // return 1 with probability proportional to color/255
    if (color > 0) {
        return 1;
    }
    else {
        return 0;
    }
}

/* FUNCTION NAMES:
 *      writecolor{Top/Bot}
 *
 * PARAMETERS:
 *      GPIO_address - GPIO memory map. See gpio.h
 *      color        - Hexadecimal color to write out
 *                     Not actually hexadecimal.
 *
 * DESCRIPTION:
 *      writes out the given color to either the top or bottom
 *      pixel currently being written to. 
 *
 *      To be used only in the display loop. Other pins must be
 *      set properly for this to function as intended.
 */

void writeColorTop(volatile uint32_t *GPIO_address, int color) {
    setBit(GPIO_address, RED1, PWM((color & 0xFF0000)>>16));
    setBit(GPIO_address, GREEN1, PWM((color & 0x00FF00)>>8));
    setBit(GPIO_address, BLUE1, PWM((color & 0x0000FF)));
}

void writeColorBot(volatile uint32_t *GPIO_address, int color) {
    setBit(GPIO_address, RED2, PWM((color & 0xFF0000)>>16));
    setBit(GPIO_address, GREEN2, PWM((color & 0x00FF00)>>8));
    setBit(GPIO_address, BLUE2, PWM((color & 0x0000FF)));
}


/*
 * FUNCTION NAME:
 *      loop(struct display *)
 *
 * PARAMETERS:
 *      disp - model to write out to the actual board
 *
 * DESCRIPTION:
 *      the main display loop for the LED Display.
 *
 */
int loop(Display *disp) {
    
    // Get pertinent information 
    disp->mutex_lock.lock();
    int halfheight = (disp->getHeight())/2;
    int width = disp->getWidth();
    volatile uint32_t *GPIO_address = disp->getGPIO();
    disp->mutex_lock.unlock();

    struct timespec sleepVal = {0};
    sleepVal.tv_nsec = SLEEPTIME;

    // Set all pins to output
    setMode(GPIO_address, RED1, OUT);
    setMode(GPIO_address, GREEN1, OUT);
    setMode(GPIO_address, BLUE1, OUT);
    setMode(GPIO_address, RED2, OUT);
    setMode(GPIO_address, GREEN2, OUT);
    setMode(GPIO_address, BLUE2, OUT);
    setMode(GPIO_address, A_PIN, OUT);
    setMode(GPIO_address, B_PIN, OUT);
    setMode(GPIO_address, C_PIN, OUT);
    setMode(GPIO_address, D_PIN, OUT);
    setMode(GPIO_address, OE, OUT);
    setMode(GPIO_address, LATCH, OUT);
    setMode(GPIO_address, CLOCK, OUT);

    // Loop infinitely
    while(disp->isRunning()) {
        for (int row = 0; row < halfheight; row++) {
            // Tell the board which rows to write to
            setBit(GPIO_address, OE, 1);
            setBit(GPIO_address, A_PIN, row & 1);
            setBit(GPIO_address, B_PIN, row & 2);
            setBit(GPIO_address, C_PIN, row & 4);
            setBit(GPIO_address, D_PIN, row & 8);

            for (int col = 0; col < width; col++) {
                // Get the lock on the display
                disp->mutex_lock.lock();
                
                // Write out the board
                writeColorTop(GPIO_address, disp->getValue(row, col));
                writeColorBot(GPIO_address, disp->getValue(row + halfheight,
                                                           col));

                // Free the display
                disp->mutex_lock.unlock();

                // Tick to move to the next pixel in the row
                setBit(GPIO_address, CLOCK, 1);
                setBit(GPIO_address, CLOCK, 0);
            }
            // Flip the Latch to move to the next row
            setBit(GPIO_address, LATCH, 1);
            setBit(GPIO_address, LATCH, 0);
            setBit(GPIO_address, OE, 0);

            // Pause momentarily
            nanosleep(&sleepVal, NULL);
         }
    }
    return 0;
}

