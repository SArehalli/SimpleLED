#ifndef GPIO_SETUP
#define GPIO_SETUP

/* Defines */

// Hardware Addressing Stuff
#define PBASE_ADDRESS 0x3F000000
#define GPIO_OFFSET 0x00200000
#define BLOCK_SIZE 4 * 1024
#define GPIO_SET 0x1C
#define GPIO_CLR 0x28

// Pins
#define A_PIN 7
#define B_PIN 8
#define C_PIN 9
#define D_PIN 10

#define LATCH 4
#define CLOCK 3
#define OE 2

#define RED1 17
#define GREEN1 18
#define BLUE1 22
#define RED2 23
#define GREEN2 24
#define BLUE2 25

// pin modes

#define IN 0
#define OUT 1

/* Prototypes */

// Set up GPIO memory map
volatile uint32_t *GPIOSetup();

// Set a pin's value/mode
void setBit(volatile uint32_t*, int, int);
void setMode(volatile uint32_t*, int, int);


#endif
