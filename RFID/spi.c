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