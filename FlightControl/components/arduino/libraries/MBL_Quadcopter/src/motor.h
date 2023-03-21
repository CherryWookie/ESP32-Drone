#ifndef motor_h
#define motor_h

#include <ESP32Servo.h>

constexpr int ESC_MIN = 1000;
constexpr int ESC_MAX = 2000;
constexpr int ESC_MID = (ESC_MIN + ESC_MAX) / 2;

class Motor {
public:
    Motor(int pin_num, int max_rpm_);
    void set_speed(int rpm);
    void stop();
    int pin;
    int max_rpm;
    Servo motor;
};
#endif