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
#include <string.h>
#include "timer.h"
#include "lcd.h"
#include "uart.h"
#include "config.h"
#include "spi.h"

volatile uint8_t cycle = 0;
volatile uint8_t main_flags;
char main_string_buffer[20] = { 0 };

int main(void){
    uint16_t debug_led = 1000;  
    uint16_t temperature = 0;
  
    DDRB |= (1 << DEBUG_DIODE);
    
    init_display();
    init_UART(103);
    

    write_instruction(DISP_CTRL & BLINK_OFF & CURSOR_OFF); //turn on the display
    write_string("Hello");
    locate_ddram(0, 1);
    write_string("RZiT");
    init_spi();
    init_cycle_timer();
    sei();

    while(1){
        
        if(main_flags & CMD_READY){ //this uC is to slow so there is a need for automata
            main_flags &= ~CMD_READY;
            main_flags |= CHANGE_CONTENT;
            strcpy(main_string_buffer, UART_gets());
        }

        manage_lcd(&main_flags);
        measure_temperature(&main_flags, &temperature);

        

        if(debug_led) --debug_led;
        else{
            PORTB = (PORTB & (1 << DEBUG_DIODE))? PORTB & ~(1 << DEBUG_DIODE) : PORTB | (1 << DEBUG_DIODE);
            UART_puts(int_to_str(temperature));
            UART_puts("\n\r");
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
ISR(USART_RXC_vect){
    static char c;
    if(c == TERMINATING_CHAR){
        c = '\0';
        main_flags |= CMD_READY;
    }
    UART_pushc(c);
}
void manage_lcd(volatile uint8_t* flags){
    static uint8_t state = 0;
    switch(state){
        case 0: if(*flags & CHANGE_CONTENT){ *flags &= ~CHANGE_CONTENT; state = 1; } break;
        case 1: write_instruction(CLEAR_DISP); state = 2; break;
        case 2: locate_ddram(0, 0); state = 3; break;
        case 3: write_string("recv: "); state = 4; break;
        case 4: write_string(main_string_buffer); state = 5; break;
        case 5: locate_ddram(0, 1); state = 6; break;
        case 6: write_string("explanation"); state = 0; break; 
    }
}
void measure_temperature(volatile uint8_t* flags, uint16_t* temperature){
    static uint8_t state = 0, timer = 0;
    uint16_t temporary = 0;

    switch(state){
        case 0: 
        if(get_temperature(temperature)){
            *flags &= ~(1 << NO_TERMOCOUPLE_ATTACHED);
            state = 1; 
        }
        else{
            if(!(*flags & NO_TERMOCOUPLE_ATTACHED)){
                *flags |= NO_TERMOCOUPLE_ATTACHED;
            }
        }
        break;
        case 1: if(get_temperature(temperature)){
            *flags &= ~(1 << NO_TERMOCOUPLE_ATTACHED);
            //*temperature = *temperature * 0.9 + 0.1 * temporary;
            timer = 3; state = 2;
        }
        else{
            if(!(*flags & NO_TERMOCOUPLE_ATTACHED)){
                *flags |= NO_TERMOCOUPLE_ATTACHED;
            }
        }
        break;
        case 2:
        if(!timer && (*flags & NO_TERMOCOUPLE_ATTACHED)){
            state = 0;
        }
        else if(!timer){
            state = 1;
        }
        break;
    }
    if(timer) --timer;
}