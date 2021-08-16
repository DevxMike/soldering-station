#ifndef controls_h_
#define controls_h_

#include <avr/io.h>

#define INCREMENT_BTN_PIN PIND
#define DECREMENT_BTN_PIN PIND
#define INCREMENT_BTN_PORT PORTD
#define DECREMENT_BTN_PORT PORTD
#define INCREMENT 3
#define DECREMENT 4

typedef struct{
    uint8_t increment : 1;
    uint8_t decrement : 1;
}buttons_t;


void init_buttons(void);
void check_buttons(buttons_t* keyboard); 

#endif