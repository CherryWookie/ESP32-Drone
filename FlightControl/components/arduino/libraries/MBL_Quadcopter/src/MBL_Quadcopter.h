#ifndef MBL_Quadcopter_h
#define MBL_Quadcopter_h

#include "motor.h"

class Quadcopter {
public:
    Quadcopter(int FL_pin, int FR_pin, int RL_pin, int RR_pin, double weight_in_N);
    void liftoff();
    void land();
    void adjust_altitude(int velocity);
    void move(int x, int y);
    void rotate(int rad);
    void balance();
    void test(int speed);
    void calibrate(int code);

    int FL_rpm = 0, FR_rpm = 0, RL_rpm = 0, RR_rpm = 0;
    // double rpm_to_lift(int rpm);
    // int lift_to_rpm(double lift);

private:
    Motor FL, FR, RL, RR;
    double weight;
    void update_motors();
};
#endif