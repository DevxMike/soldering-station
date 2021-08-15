#ifndef pid_h_
#define pid_h_

#include <avr/io.h>
#define PID_PORT PORTD
#define PID_DDR DDRD
#define PID_OFFSET 2

typedef struct{
    double Kp, Ki, Kd, fraction_1, fraction_2;
}PID_t;

void init_pwm(void);
void init_PID(PID_t* pid, double Kp, double Ti, double Td, double Ts);

#endif