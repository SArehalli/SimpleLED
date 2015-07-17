#include "display.h"


int main() {
    Display *disp = new Display(32, 32);
    
    disp->setValue(0, 0, 0x00FF00);

    loop(disp);
}
