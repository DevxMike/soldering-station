#include "controls.h"
#include "config.h"
#include "pid.h"

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
void manage_keyboard(volatile uint8_t* flags, const buttons_t* kbd, uint16_t* desired_temperature){
    static uint8_t timer, state = 0;

    switch(state){
        case 0:
        if(kbd->increment){
            timer = 150;
            state = 1;
        }
        else if(kbd->decrement){
            timer = 150;
            state = 4;
        }
        break;

        case 1:
        if(timer && !kbd->increment){
            state = 0;
        }
        else if(kbd->increment && kbd->decrement){
            state = 3;
        }
        else if(kbd->increment && !timer){
            state = 2;
        }
        break;

        case 2:
        if(*desired_temperature + 10 > TEMP_MAX){
            *desired_temperature = TEMP_MAX;
        }
        else{
            *desired_temperature += 10;
        }
        *flags |= CHANGE_CONTENT;
        *flags |= RESET_INTEGRATOR;
        timer = 150;
        state = 1;
        break;

        case 3:
        if(!kbd->increment && !kbd->decrement){
            state = 0;
        }
        break;

        case 4:
        if(timer && !kbd->decrement){
            state = 0;
        }
        else if(kbd->increment && kbd->decrement){
            state = 3;
        }
        else if(kbd->decrement && !timer){
            state = 5;
        }
        break;

        case 5:
        if(*desired_temperature - 10 < TEMP_MIN){
            *desired_temperature = TEMP_MIN;
        }
        else{
            *desired_temperature -= 10;
        }
        *flags |= CHANGE_CONTENT;
        *flags |= RESET_INTEGRATOR;
        *flags |= TURN_PID_OFF;
        timer = 150;
        state = 4;
        break;
    }
    if(timer) --timer;
}