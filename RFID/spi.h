#ifndef spi_h_
#define spi_h_

#include <avr/io.h>

#define SS 2
#define MISO 4
#define SCK 5

void init_spi(void);
uint8_t get_temperature(uint16_t* temperature);

#endif