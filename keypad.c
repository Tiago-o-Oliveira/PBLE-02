#include "keypad.h"
#include "mcc_generated_files/adc1.h"

#define LEFT 1
#define DOWN 2
#define CENTER 3
#define UP 4
#define RIGHT 5

unsigned char kpRead() {
    unsigned int tst;
    tst = ADC1_GetConversion(TECLADO);
    
    //513=left
    if ((tst < 523)&&(tst > 503)) {
        return LEFT;
    }
    
    //820 = down
    else if ((tst < 831)&&(tst > 810)) {
        return DOWN;
    }
    
    //768=centro
    else if ((tst < 780)&&(tst > 750)) {
        return CENTER;
    }
    
    // 683= up
    else if ((tst < 693)&&(tst > 673)) {
        return UP;
    }
    
    // 854 right
    else if ((tst < 864)&&(tst > 844)) {
        return RIGHT;
    }
    
    else {
        return 0;
    }
}