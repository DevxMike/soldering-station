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

#include "uart.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct{
    char string[20];
    uint8_t len;
    const uint8_t max_len;
}uart_buffer_t;

uart_buffer_t buffer = { { 0 }, 0, 20 };
char string[20] = { 0 };

void init_UART(uint8_t ubrr){ //9600 baud, config: 8, n, 1
    UBRRH = (uint8_t)(ubrr>>8);
    UBRRL = (uint8_t)ubrr;
    UCSRB = (1<<RXEN)|(1<<TXEN) | (1<<RXCIE);
    UCSRC = (1<<URSEL)|(3<<UCSZ0);
}

void UART_putc(char c){
    while (!( UCSRA & (1<<UDRE))){
        continue;
    }
    UDR = c;
}

void UART_puts(const char* string){
    while(*string){
        UART_putc(*string++);
    }
}

uint8_t is_buffer_full(const uart_buffer_t* b){
    return b->len == b->max_len;
}

uint8_t is_buffer_empty(const uart_buffer_t* b){
    return b->len == 0;
}

void UART_pushc(char c){
    if(is_buffer_full(&buffer)){
        memset(buffer.string, 0, buffer.max_len - 1);
        buffer.len = 0;
    }
    buffer.string[buffer.len++] = c;
}

const char* UART_gets(void){
    if(!is_buffer_empty(&buffer)){
        strcpy(string, buffer.string);
        UART_flush();
        return string;
    }
    else{
        return "";
    }
}

void UART_flush(void){
    memset(buffer.string, 0, buffer.max_len - 1);
    buffer.len = 0;
}

const char* int_to_str(int16_t value){
    char buffer[20];
    return itoa(value, buffer, 10);
}
void UART_puts_binary(uint16_t word){
    for(uint8_t i = 15; i != 0; --i){
        if(word & (1 << i)){
            UART_putc('0' + 1); continue;
        }
        UART_putc('0');
    }
}