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