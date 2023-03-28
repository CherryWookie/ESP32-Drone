#include "MBL_Quadcopter.h"
#include "motor.h"

Quadcopter::Quadcopter(int FL_pin, int FR_pin, int RL_pin, int RR_pin, double weight_in_N)
    :FL{Motor{FL_pin, 31000}}, FR{Motor{FR_pin, 31000}}, RL{Motor{RL_pin, 31000}}, RR{Motor{RR_pin, 31000}}, weight{weight_in_N} {}

void Quadcopter::liftoff() {
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

}
void Quadcopter::test(int speed) {
    FL.set_speed(speed);
    FR.set_speed(speed);
    RL.set_speed(speed);
    RR.set_speed(speed);
}
void Quadcopter::update_motors() {
    FL.set_speed(FL_rpm);
    FR.set_speed(FR_rpm);
    RL.set_speed(RL_rpm);
    RR.set_speed(RR_rpm);
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
