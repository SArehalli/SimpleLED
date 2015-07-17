#include "display.h"


int main() {
    struct display disp;

    disp.width = 32;
    disp.height = 32;
    
    int *(mat[32]);
    for (int i = 0; i< 32; i++) {
        mat[i] = new int[32];
    }
    
    mat[0][0] = 0x00FF00;

    disp.matrix = mat;


    loop(&disp);
}
