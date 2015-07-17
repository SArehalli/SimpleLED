#include <cstdlib>
#include <stdint.h>
#include <ctime>
#include "display.h"
#include <iostream>

extern "C" {
    #include "gpio.h"
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
    setBit(GPIO_address, RED1, (color & 0xFF0000)? 1: 0);
    setBit(GPIO_address, GREEN1, (color & 0x00FF00)? 1: 0);
    setBit(GPIO_address, BLUE1, (color & 0x0000FF)? 1: 0);
}

void writeColorBot(volatile uint32_t *GPIO_address, int color) {
    setBit(GPIO_address, RED2, (color & 0xFF0000)? 1: 0);
    setBit(GPIO_address, GREEN2, (color & 0x00FF00)? 1: 0);
    setBit(GPIO_address, BLUE2, (color & 0x0000FF)? 1: 0);
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
int loop(struct display *disp) {
    
    // Map the GPIO hardware address to virtual memory
    volatile uint32_t* GPIO_address = GPIOSetup();

    // Ensure the memory map succeeded
    if (GPIO_address == NULL) {
        return -1;
    }

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

    // Get the half-height: the distance between simultaneously updating pixels
    disp->mutex_lock.lock();
    int halfheight = (disp->height)/2;
    int width = disp->width;
    std::cerr << width << "\n";
    for (int i=0; i < width; i++) {
        for (int j=0; j < width; j++) {
            std::cerr << disp->matrix[i][j] << " ";
        }
        std::cerr << std::endl;
    }
    
    disp->mutex_lock.unlock();


    // Loop infinitely
    while(1) {
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
                writeColorTop(GPIO_address, disp->matrix[row][col]);
                writeColorBot(GPIO_address, disp->matrix[row + halfheight][col]);
               

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
}
