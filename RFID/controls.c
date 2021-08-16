#include "controls.h"

void init_buttons(void){
    INCREMENT_BTN_PORT |= (1 << INCREMENT);
    DECREMENT_BTN_PORT |= (1 << DECREMENT);
}
void check_buttons(buttons_t* keyboard){
    if(!(INCREMENT_BTN_PIN & (1 << INCREMENT))){
        keyboard->increment = 1;
    }
    else{
        keyboard->increment = 0;
    }

    if(!(DECREMENT_BTN_PIN & (1 << DECREMENT))){
        keyboard->decrement = 1;
    }
    else{
        keyboard->decrement = 0;
    }
}