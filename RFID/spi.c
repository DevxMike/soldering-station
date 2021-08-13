#include "spi.h"
#define TEMPERATURE_FLAG_BIT 3

void set_ss(void){
    PORTB |= (1 << SS);
}
void clr_ss(void){
    PORTB &= ~(1 << SS);
}

uint8_t spi_read_byte(void){
    return 0;
}
uint8_t check_bit(uint16_t word, uint8_t bit){
    if(word &  (1 << bit)){
        return 1;
    }
    else{
        return 0;
    }
}
uint16_t spi_read_word(void){
    return 0;
}
void init_spi(void){

}
uint8_t get_temperature(uint16_t* temperature){
    uint16_t spi_read_value = spi_read_word();

    if(check_bit(spi_read_value, TEMPERATURE_FLAG_BIT)){
        *temperature = 0;
        return 0; //no termocouple
    }
    else{
        *temperature = ((spi_read_value & 0x7FF8) >> 3) * 0.25;
        return 1;
    }
}