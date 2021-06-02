/*Copyright (c) 2021 Michał Bazan

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

#include "main.h"
#include <avr/interrupt.h>
#include "timer.h"
#include "lcd.h"

volatile uint8_t cycle = 0;

int main(void){
    uint16_t debug_led = 1000;
    DDRB |= (1 << DEBUG_DIODE);
    init_display();
    write_instruction(DISP_CTRL & BLINK_OFF & CURSOR_OFF); //turn on the display
    write_string("Hello");
    locate_ddram(0, 1);
    write_string("RZiT");
    init_cycle_timer();
    sei();

    while(1){
        


        if(debug_led) --debug_led;
        else{
            PORTB = (PORTB & (1 << DEBUG_DIODE))? PORTB & ~(1 << DEBUG_DIODE) : PORTB | (1 << DEBUG_DIODE);
            debug_led = 1000;
        }
        while(!cycle){
            continue;
        }
        cycle = 0;
    }
}
ISR(TIMER1_COMPA_vect){
    cycle = 1;
}