#include "pid.h"

void init_pwm(void){
    PID_DDR |= (1 << PID_OFFSET);
}