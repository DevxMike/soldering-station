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

void init_pwm(void){
    PID_DDR |= (1 << PID_OFFSET);
}
void init_PID(volatile PID_t* pid, double Kp, double Ti, double Td, double Ts){
    pid->Kp = Kp;
    pid->Ki = 0.5*(Kp / Ti) * Ts;
    pid->Kd = Kp * Td;
    pid->fraction_1 = (1.0 / (Td * 0.1 + Ts));
    pid->fraction_2 = (0.1 * Td)/(0.1 * Td + Ts);
    pid->derivative_before = 0.0;
    pid->sum = pid->error = pid->error_before = 0;
}
void compute_error(volatile PID_t* pid, int16_t error){
    pid->error_before = pid->error;
    pid->error = error;
}
uint8_t get_PID_pwm(volatile PID_t* pid, uint16_t desired_value, uint16_t actual_value){
    double P, I, D, temp;
    int16_t sum;
    int16_t error = ((int16_t)desired_value - (int16_t)actual_value);

    if(error <= 0){
        return 0;
    }

    compute_error(pid, error);
    P = pid->Kp * pid->error;   
    
    if((sum = pid->error + pid->error_before + (int16_t)pid->sum) > 204){
        pid->sum = 204;
    }
    else if((sum < 0)){
        pid->sum = 0;
    }
    else{
        pid->sum = sum;
    }

    I = pid->Ki * pid->sum;
    D = pid->Kd * (pid->fraction_1 * (pid->error - pid->error_before) + pid->fraction_2*pid->derivative_before);
    pid->derivative_before = D;

    if((temp = P + I + D) > 204.0){
        return 204;
    }
    else if(temp < 0){
        return 0;
    }
    else{
        return (uint8_t)temp;
    }
}
