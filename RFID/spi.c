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

#include "spi.h"
#include <util/delay.h>
#include "uart.h"

#define TEMPERATURE_FLAG_BIT 3

void set_ss(void){
    PORTB |= (1 << SS);
}
void clr_ss(void){
    PORTB &= ~(1 << SS);
}

void set_sck(void){
    PORTB |= (1 << SCK);
}
void clr_sck(void){
    PORTB &= ~(1 << SCK);
}

void init_spi(void){
    DDRB |= (1 << SCK) | (1 << SS);
    PORTB |= (1 << MISO);
    set_ss();
    _delay_ms(250);
}

uint8_t spi_read_byte(void){
    uint8_t temp = 0;
    
    for(uint8_t i = 7; i != 0; --i){
        set_sck();
        _delay_us(1);
        if(PINB & (1 << MISO)){
            temp |= (1 << i);
        }
        clr_sck();
        _delay_us(1);
    }

    return temp;
}

uint16_t spi_read_word(void){
    uint16_t result = 0;

    clr_ss();
    _delay_us(1);
    result |= (uint16_t)spi_read_byte() << 8;
    result |= (uint16_t)spi_read_byte();
    set_ss();

    return result;
}

uint8_t check_bit(uint16_t word, uint8_t bit){
    if(word &  (1 << bit)){
        return 1;
    }
    else{
        return 0;
    }
}

uint8_t get_temperature(uint16_t* temperature){
    uint16_t spi_read_value = spi_read_word();

        *temperature = ((spi_read_value & 0x7FF8) >> 3) * 0.25;
        return 1;

}