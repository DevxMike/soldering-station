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

#include "pid.h"
#include "uart.h"

void init_pwm(void){
    PID_DDR |= (1 << PID_OFFSET);
}
void init_PID(volatile PID_t* pid, double Kp, double Ti, double Td, double Ts){
    pid->Kp = Kp;
    pid->Ki = (Kp / Ti) * Ts;
    pid->Kd = (Kp * Td)/(Ts + 0.1*Td);
    pid->sum = pid->error_before = 0;
}
uint8_t get_PID_pwm(volatile PID_t* pid, uint16_t desired_value, uint16_t actual_value){
    double P, I, D, temp;
    int16_t error = ((int16_t)desired_value - (int16_t)actual_value);
    
    P = pid->Kp * error;

    pid->sum += error;

    if(pid->sum > 2000){
        pid->sum = 2000;
    }
    else if(pid->sum < -2000){
        pid->sum = -2000;
    }
    I = pid->Ki * pid->sum;

    D = pid->Kd * (error - pid->error_before);
    pid->error_before = error;

    //debug logs
    /*UART_puts(" P ");
    UART_puts(int_to_str((int16_t)P));


    UART_puts(" I ");
    UART_puts(int_to_str((int16_t)I));


    UART_puts(" D ");
    UART_puts(int_to_str((int16_t)D));

    UART_puts("\n\r");*/

    temp = P + I + D;

    if(temp < 0.0){
        return 0;
    }
    else if(temp > 255.0){
        return 255;
    }
    else{
        return (uint8_t)temp;
    }
}

void reset_integrator(volatile PID_t* pid){
    pid->sum = 0;
}