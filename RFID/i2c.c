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

#include "i2c.h"

void I2C_start(void){
    TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
    while (!(TWCR & (1<<TWINT))) { continue; }
}

void I2C_send_byte(uint8_t byte){
    TWDR = byte;
    TWCR = (1<<TWINT) | (1<<TWEN);
    while (!(TWCR & (1<<TWINT))) { continue; }
}

uint8_t I2C_read_byte(uint8_t ack_bit){
    TWCR = (1 << TWINT) | (1 << TWEN) | (ack_bit << TWEA);
    while(!(TWCR & (1 << TWINT))) { continue; }
    return TWDR;
}

void I2C_stop(void){
    TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
}

void I2C_send_buffer(uint8_t slave_adr, const uint8_t* buffer, uint8_t buffer_len){
    I2C_start();
    I2C_send_byte(slave_adr);
    for(const uint8_t* buff_iter = buffer; buff_iter < buffer + buffer_len; ++buff_iter){
        I2C_send_byte(*buff_iter);
    }
    I2C_stop();
}

void I2C_read_buffer(uint8_t slave_adr, uint8_t read_adr, uint8_t* buffer, uint8_t buffer_len){
    I2C_start();
    I2C_send_byte(slave_adr);
    I2C_send_byte(read_adr);
    I2C_start();    
    I2C_send_byte(slave_adr + 1);
    while(buffer_len--){
        *buffer++ = I2C_read_byte(buffer_len != 0? 1 : 0);
    }
    I2C_stop();
}