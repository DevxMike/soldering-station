#include "controls.h"

void init_buttons(void){
    INCREMENT_BTN_PORT |= (1 << INCREMENT);
    DECREMENT_BTN_PORT |= (1 << DECREMENT);
}
void check_buttons(uint8_t* increment, uint8_t* decrement){
    if(!(INCREMENT_BTN_PIN & (1 << INCREMENT))){
        *increment = 1;
    }
    else{
        *increment = 0;
    }

    if(!(DECREMENT_BTN_PIN & (1 << DECREMENT))){
        *decrement = 1;
    }
    else{
        *decrement = 0;
    }
}