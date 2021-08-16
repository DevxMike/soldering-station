#include "pid.h"

void init_pwm(void){
    PID_DDR |= (1 << PID_OFFSET);
}
void init_PID(PID_t* pid, double Kp, double Ti, double Td, double Ts){
    pid->Kp = Kp;
    pid->Ki = 0.5*(Kp / Ti) * Ts;
    pid->Kd = Kp * Td;
    pid->fraction_1 = (1.0 / (Td * 0.1 + Ts));
    pid->fraction_2 = (0.1 * Td)/(0.1 * Td + Ts);
    pid->derivative_before = 0.0;
    pid->sum = pid->error = pid->error_before = 0;
}
void compute_error(PID_t* pid, uint16_t error){
    pid->error_before = pid->error;
    pid->error = error;
}
uint8_t get_PID_pwm(PID_t* pid, uint16_t desired_value, uint16_t actual_value){
    double P, I, D, temp;
    int16_t sum;

    compute_error(pid, desired_value - actual_value);
    P = pid->Kp * pid->error;   
    
    if((sum = pid->error + pid->error_before + (int16_t)pid->sum) > 255){
        pid->sum = 255;
    }
    else if((sum < 0)){
        pid->sum = 0;
    }
    else{
        pid->sum = (uint8_t)sum;
    }

    I = pid->Ki * pid->sum;
    D = pid->Kd * (pid->fraction_1 * (pid->error - pid->error_before) + pid->fraction_2*pid->derivative_before);
    pid->derivative_before = D;

    if((temp = P + I + D) > 255.0){
        return 255;
    }
    else if(temp < 0){
        return 0;
    }
    else{
        return (uint8_t)temp;
    }
}
