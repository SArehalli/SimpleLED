/*
 * FILE:
 *      gpio.h
 * PURPOSE:
 *      Set up low-level interaction with the GPIO pins.
 * AUTHOR:
 *      Suhas Arehalli
 */

#include <sys/mman.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdint.h>
#include <errno.h>
#include "gpio.h"

/* FUNCTION NAME:
 *      GPIOSetup()
 *
 * DESCRIPTION:
 *      Maps the hardware addresses of the RPi's GPIO pins to virtual memory.
 *
 * RETURN VALUE:
 *      a volatile uint32_t pointer to the virtual memory address that was
 *      mapped to.
 *
 * ERROR CONDTIONS:
 *      If the memory could not be mapped for any reason (lack of priviledges,
 *      lack of memory, etc.), NULL is returned.
 *
 */
volatile uint32_t* GPIOSetup() {

    // Get a file descriptor for /dev/mem
    int mem_file;
    if ((mem_file = open("/dev/mem", O_RDWR|O_SYNC)) < 0) {
        perror("ERROR: Could not open /dev/mem:");
        return NULL;
    }


    errno = 0;
    // mmap to get the virtual address of the GPIO pins
    volatile uint32_t *GPIO_loc = (volatile uint32_t *) mmap(NULL, BLOCK_SIZE,
                                             PROT_READ | PROT_WRITE,
                                             MAP_SHARED,
                                             mem_file,
                                             PBASE_ADDRESS + GPIO_OFFSET);
    if (GPIO_loc == MAP_FAILED) {
        fprintf(stderr, "ERROR: Could not map /dev/mem");
        fprintf(stderr,"%d" ,errno);
        return NULL;
    }

    close(mem_file);
}

/*
 * FUNCTION NAME:
 *      setBit(volatile uint32_t*, int, int)
 *
 * PARAMETERS:
 *      GPIO_address - address of the GPIO memory map. See GPIOSetup()
 *      pin          - the GPIO pin to write to.
 *      bit          - the binary value to write
 *
 * DESCRIPTION:
 *      Given an address to a GPIO memory map, setBit will write the
 *      binary value given in bit to the GPIO pin given in pin.
 *
 *  RETURN:
 *      void
 *
 */

void setBit(volatile uint32_t* GPIO_address, int pin, int bit) {
    // Address to write to
    volatile uint32_t* write_address = GPIO_address;

    // If we set the bit
    if (bit > 0) {
        write_address += GPIO_SET/4; // Adjust for scaled pointer arithmetic
    }

    // If we clear the bit
    else {
        write_address += GPIO_CLR/4; // Adjust for scaled pointer arithmetic
    }

    *write_address = 1 << pin;
}

/*
 * FUNCTION NAME:
 *      setMode(volatile uint32_t, int, int)
 *
 * PARAMETERS:
 *      GPIO_address - address of the GPIO Memory Map. See GPIOSetup()
 *      pin          - GPIO pin to set
 *      mode         - mode to set the pin to. See constants in gpio.h.
 *
 * DESCRIPTION:
 *      given a GPIO memory map, setMode will set the mode of
 *      the GPIO pin given in pin to the mode given in mode.
 *
 * RETURN:
 *      void
 */

void setMode(volatile uint32_t* GPIO_address, int pin, int mode) {
    // Address to write to
    volatile uint32_t* write_address = GPIO_address;

    // Get to the right write address
    // 32 bits / 3 bits per pin = 10 pins per 4 byte address
    int num_pins_away = pin;
    while (num_pins_away > 9) {
        write_address++; // Scaled pointer arithmetic
        num_pins_away -= 10;
    }

    *write_address |= mode << (num_pins_away * 3);

}
