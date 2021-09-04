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
#include "pid.h"
#include "controls.h"

volatile uint8_t cycle = 0;
volatile uint8_t main_flags;
volatile uint8_t PID_pwm = 70;
volatile PID_t regulator;

char main_string_buffer[20] = { 0 };
uint16_t temperature = 0;
uint16_t desired_temperature = TEMP_MIN;
uint16_t displayed_temperature;

int main(void){
    uint16_t display_change = 1000;  
    buttons_t keyboard;
    int16_t temp;
    
    init_buttons();
    init_PID(&regulator, 5.0, 20.0, 5.0, 0.250);
    init_display();
    init_UART(103);
    write_instruction(DISP_CTRL & BLINK_OFF & CURSOR_OFF); //turn on the display
    write_string("T_A     T_D ");
    locate_ddram(0, 1);
    write_string("HEATING ");
    init_pwm();
    init_spi();
    init_pwm_timer();
    init_cycle_timer();
    sei();

    while(1){

        if(main_flags & CMD_READY){ //this uC is to slow so there is a need for automata
            main_flags &= ~CMD_READY;
            strcpy(main_string_buffer, UART_gets());
        }
        if(main_flags & SAMPLE_READY){
            main_flags &= ~SAMPLE_READY;
        }

        check_buttons(&keyboard);
        manage_lcd(&main_flags);
        measure_temperature(&main_flags, &temperature);
        manage_keyboard(&main_flags, &keyboard, &desired_temperature);

        if(main_flags & TURN_PID_OFF){
            temp = (int16_t)desired_temperature - (int16_t)displayed_temperature;
            if(temp > -2){
                main_flags &= ~TURN_PID_OFF;
            }
        }
        
        if(main_flags & GET_PID){
            if(main_flags & RESET_INTEGRATOR){
                reset_integrator(&regulator);
                main_flags &= ~RESET_INTEGRATOR;
            }
            if(main_flags & TURN_PID_OFF){
                PID_pwm = 0;
            }
            else{
                PID_pwm = get_PID_pwm(&regulator, desired_temperature, displayed_temperature);
            }
            main_flags &= ~GET_PID;
            UART_puts(int_to_str(displayed_temperature));
            UART_puts(" ");
            UART_puts(int_to_str(desired_temperature));
            UART_puts(" ");
            UART_puts(int_to_str(PID_pwm));
            UART_puts("\n\r");
        }
        
        if(display_change) --display_change;
        else if(!(main_flags & (1 << CHANGE_CONTENT))){
            main_flags |= 1 << CHANGE_CONTENT;
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
ISR(TIMER2_COMP_vect){
    static uint8_t pwm_check;
    if(pwm_check++ < PID_pwm){
        if(!(PID_PORT & (1 << PID_OFFSET))){
            PID_PORT |= (1 << PID_OFFSET);
        } 
    }
    else{
        PID_PORT &= ~(1 << PID_OFFSET);
    }
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
        case 0: if(*flags & CHANGE_CONTENT){ *flags &= ~CHANGE_CONTENT; state = 0; } break;
        case 1: locate_ddram(4, 0); state = 2; break;
        case 2: if(displayed_temperature < 100){write_string(" ");} write_string(int_to_str(displayed_temperature)); state = 3; break;
        case 3: locate_ddram(13, 0); state = 4; break;
        case 4: write_string(int_to_str(desired_temperature)); state = 5; break;
        case 5: locate_ddram(0, 1); state = 6; break;
        case 6: if(PID_pwm > 0) write_string("HEATING"); else write_string("       "); state = 0; break; 
    }
}
void measure_temperature(volatile uint8_t* flags, uint16_t* temperature){
    static uint8_t state = 0;
    static uint16_t timer = 0;

    switch(state){
        case 0: 
        if(get_temperature(temperature)){
            *flags &= ~NO_TERMOCOUPLE_ATTACHED;
            state = 1; 
        }
        else{
            if(!(*flags & NO_TERMOCOUPLE_ATTACHED)){
                *flags |= NO_TERMOCOUPLE_ATTACHED;
            }
        }
        break;
        case 1: if(get_temperature(temperature)){
            *flags &= ~NO_TERMOCOUPLE_ATTACHED;
            *flags |= SAMPLE_READY;
            timer = 250; state = 2;
            *flags |= GET_PID;
            displayed_temperature = displayed_temperature * 0.9 + *temperature * 0.1;
        }
        else{
            if(!(*flags & NO_TERMOCOUPLE_ATTACHED)){
                *flags |= NO_TERMOCOUPLE_ATTACHED;
            }
        }
        *flags |= CHANGE_CONTENT;
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