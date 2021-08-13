#include "spi.h"
#define TEMPERATURE_FLAG 0x0004

uint8_t spi_read_byte(void){
    return 0;
}
uint8_t check_bit(uint16_t word, uint16_t mask){
    return 0;
}
uint16_t spi_read_word(void){
    return 0;
}
void init_spi(void){

}
uint8_t get_temperature(uint16_t* temperature){
    uint16_t spi_read_value = spi_read_word();

    if(check_bit(spi_read_value, TEMPERATURE_FLAG)){
        *temperature = 0;
        return 0; //no termocouple
    }
    else{
        *temperature = spi_read_value;
        return 1;
    }
}