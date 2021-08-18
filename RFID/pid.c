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
    pid->Ki = 0.5*(Kp / Ti) * Ts;
    pid->Kd = (Kp * Td) / Ts;
    pid->sum = pid->error_before = 0;
}
uint8_t get_PID_pwm(volatile PID_t* pid, uint16_t desired_value, uint16_t actual_value){
    double P, I, D, temp;
    int16_t error = ((int16_t)desired_value - (int16_t)actual_value);
    int16_t sum;
    
    if(error > 100){
        P = 100.0;
    }
    else if(error < -100){
        P = -100.0;
    }
    else{
        P = pid->Kp * error;
    }

    sum = pid->sum + error;
    if(sum > 175){
        I = 175.0;
        pid->sum = 175;
    }
    else if(sum < -175){
        I = -175.0;
        pid->sum = -175.0;
    }
    else{
        I = pid->Ki * sum;
        pid->sum = sum;
    }

    D = pid->Kd * (pid->error_before - error);
    pid->error_before = error;

    temp = P + I + D;


    UART_puts("\n\r");

    if(temp < 0.0){
        return 0;
    }
    else if(temp > 220.0){
        return 220;
    }
    else{
        return (uint8_t)temp;
    }
}
