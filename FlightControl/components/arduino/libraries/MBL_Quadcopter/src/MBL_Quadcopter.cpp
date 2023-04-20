#include "MBL_Quadcopter.h"
#include "motor.h"
#include <PID_v1.h>


Quadcopter::Quadcopter(int FL_pin, int FR_pin, int RL_pin, int RR_pin, double Kp, double Ki, double Kd)
    :FL{Motor{FL_pin, 31000}}, FR{Motor{FR_pin, 31000}}, RL{Motor{RL_pin, 31000}}, RR{Motor{RR_pin, 31000}}, 
    Kp{Kp}, Ki{Ki}, Kd{Kd},
    roll_PID{PID{&roll, &roll_out, &target_roll, Kp, Ki, Kd, DIRECT}},
    pitch_PID{PID{&pitch, &pitch_out, &target_pitch, Kp, Ki, Kd, DIRECT}} {
        roll_PID.SetSampleTime(4);
        pitch_PID.SetSampleTime(4);
    }

void Quadcopter::takeoff() {
    // increase motorspeed and achieve initial altitude
}
void Quadcopter::land() {
    // reduce motorspeed to 0
}
void Quadcopter::adjust_altitude(int velocity) {
    // motorspeed += velocity * factor
}
void Quadcopter::move(int x, int y) {
    // pitch and maintain upwards lift
}
void Quadcopter::rotate(int rad) {
    // if turning right - slow down FL & RR

    // if turning left - slow down FR & RL

}
void Quadcopter::balance() {
    bool r = roll_PID.Compute();
    bool p = pitch_PID.Compute();

    double FL_next_1 = FL_pwm + roll_out;
    double FL_next_2 = FL_pwm + pitch_out;
    int FL_next = static_cast<int>((FL_next_1 + FL_next_2) / 2);

    double FR_next_1 = FR_pwm + roll_out;
    double FR_next_2 = FR_pwm + pitch_out;
    int FR_next = static_cast<int>((FR_next_1 + FR_next_2) / 2);

    double RL_next_1 = RL_pwm + roll_out;
    double RL_next_2 = RL_pwm + pitch_out;
    int RL_next = static_cast<int>((RL_next_1 + RL_next_2) / 2);

    double RR_next_1 = RR_pwm + roll_out;
    double RR_next_2 = RR_pwm + pitch_out;
    int RR_next = static_cast<int>((RR_next_1 + RR_next_2) / 2);

    FL_pwm = FL_next;
    FR_pwm = FR_next;
    RL_pwm = RL_next;
    RR_pwm = RR_next;

    update_motors();
}
void Quadcopter::update_PID(double nKp, double nKi, double nKd) {
    roll_PID.SetTunings(nKp, nKi, nKd);
    roll_PID.SetTunings(nKp, nKi, nKd);
}
void Quadcopter::test(int speed) {
    FL_pwm = speed;
    FR_pwm = speed;
    RL_pwm = speed;
    RR_pwm = speed;
    update_motors();
}
void Quadcopter::update_motors() {
    FL.set_speed(FL_pwm);
    FR.set_speed(FR_pwm);
    RL.set_speed(RL_pwm);
    RR.set_speed(RR_pwm);
}
// void Quadcopter::test() {
//     FL_rpm = 2000;
//     update_motors();
// }
void Quadcopter::calibrate(int code) {
    switch (code) {
        // 0
        case 0 :
            FL.motor.writeMicroseconds(ESC_MIN);
            FR.motor.writeMicroseconds(ESC_MIN);
            RL.motor.writeMicroseconds(ESC_MIN);
            RR.motor.writeMicroseconds(ESC_MIN);
            break;

        // 1
        case 1 :
            FL.motor.writeMicroseconds(ESC_MID);
            FR.motor.writeMicroseconds(ESC_MID);
            RL.motor.writeMicroseconds(ESC_MID);
            RR.motor.writeMicroseconds(ESC_MID);
            break;

        // 2
        case 2 : 
            for (int i = ESC_MIN; i <= ESC_MAX; i += 5) {
                FL.motor.writeMicroseconds(i);
                FR.motor.writeMicroseconds(i);
                RL.motor.writeMicroseconds(i);
                RR.motor.writeMicroseconds(i);
                delay(200);
            }

            FL.motor.writeMicroseconds(ESC_MIN);
            FR.motor.writeMicroseconds(ESC_MIN);
            RL.motor.writeMicroseconds(ESC_MIN);
            RR.motor.writeMicroseconds(ESC_MIN);
            break;
    }
    
}
