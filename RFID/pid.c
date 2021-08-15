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
}