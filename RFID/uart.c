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