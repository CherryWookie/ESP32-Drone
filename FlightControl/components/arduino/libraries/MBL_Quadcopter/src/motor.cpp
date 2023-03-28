#include "motor.h"
#include <Arduino.h>

Motor::Motor(int pin_num, int max_rpm_)
    :pin{pin_num}, max_rpm{max_rpm_} {
        motor.attach(pin, ESC_MIN, ESC_MAX);
    }

void Motor::set_speed(int rpm) {
    // set linearly
    // rpm = constrain(rpm, -max_rpm, max_rpm);
    // int speed = map(rpm, -max_rpm, max_rpm, ESC_MIN, ESC_MAX);
    motor.writeMicroseconds(rpm);
}