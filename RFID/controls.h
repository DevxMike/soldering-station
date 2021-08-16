#ifndef controls_h_
#define controls_h_

#include <avr/io.h>

#define INCREMENT_BTN_PIN PIND
#define DECREMENT_BTN_PIN PIND
#define INCREMENT_BTN_PORT PORTD
#define DECREMENT_BTN_PORT PORTD
#define INCREMENT 3
#define DECREMENT 4

void init_buttons(void);
void check_buttons(uint8_t* increment, uint8_t* decrement); 

#endif