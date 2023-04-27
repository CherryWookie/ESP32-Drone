#ifndef MBL_Quadcopter_h
#define MBL_Quadcopter_h

#include "motor.h"
#include <PID_v1.h>


class Quadcopter {
public:
    Quadcopter(int FL_pin, int FR_pin, int RL_pin, int RR_pin, double Kp, double Ki, double Kd);
    void takeoff();
    void land();
    void adjust_altitude(int velocity);
    void move(int x, int y);
    void rotate(int rad);
    void balance();
    void test(int speed);
    void calibrate(int code);
    void update_PID(double nKp, double nKi, double nKd);

    double roll = 0, pitch = 0, roll_out = 0, pitch_out = 0;
    double target_roll = 0.0;
    double target_pitch = 0.0;
    
    int FL_pwm = 0, FR_pwm = 0, RL_pwm = 0, RR_pwm = 0;
private:
    Motor FL, FR, RL, RR;
    double Kp, Ki, Kd;
    PID roll_PID;
    PID pitch_PID;
    // double weight;
    int motor_sum = 4800;
    void update_motors();
};
#endif