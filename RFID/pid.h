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

#ifndef pid_h_
#define pid_h_

#include <avr/io.h>
#define PID_PORT PORTD
#define PID_DDR DDRD
#define PID_OFFSET 2

typedef struct{
    double Kp, Ki, Kd, fraction_1, fraction_2;
    int16_t error, error_before; 
    uint8_t sum;
    double derivative_before;
}PID_t;

void init_pwm(void);
void init_PID(volatile PID_t* pid, double Kp, double Ti, double Td, double Ts);
uint8_t get_PID_pwm(volatile PID_t* pid, uint16_t desired_value, uint16_t actual_value);
#endif