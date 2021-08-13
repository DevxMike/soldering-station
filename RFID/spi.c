#include "spi.h"
#define TEMPERATURE_FLAG_BIT 2

void set_ss(void){
    PORTB |= (1 << SS);
}
void clr_ss(void){
    PORTB &= ~(1 << SS);
}

void init_spi(void){
    DDRB |= (1 << SCK) | (1 << MISO);
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << CPHA) | (1 << SPR0) | (1 << SPIE);
    set_ss();
}

uint8_t spi_read_byte(void){
    SPDR = 0x00;
    while(!(SPSR & (1 << SPIF)));
    return SPDR;
}

uint16_t spi_read_word(void){
    uint16_t result = 0;

    clr_ss();
    result |= spi_read_byte() << 8;
    result |= spi_read_byte();
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

    if(check_bit(spi_read_value, TEMPERATURE_FLAG_BIT)){
        *temperature = 0;
        return 0; //no termocouple attached
    }
    else{
        *temperature = ((spi_read_value & 0x7FF8) >> 3) * 0.25;
        return 1;
    }
}