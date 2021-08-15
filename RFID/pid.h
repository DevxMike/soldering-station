#ifndef pid_h_
#define pid_h_

#include <avr/io.h>
#define PID_PORT PORTD
#define PID_DDR DDRD
#define PID_OFFSET 2

void init_pwm(void);

#endif