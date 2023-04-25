#pragma once

struct Data {
  // lock for protecting parameters from concurrent read/writes
//   std::mutex mutex;

  // PID parameters
  double Kd{ 1.34 };
  double Kp{ 5.67 };
  double Ki{ 0.02 };
  double roll{ 0.0 };
  double pitch{ 0.0 };
  int FL_pulse{ 1000 };
  int FR_pulse{ 1000 };
  int RL_pulse{ 1000 };
  int RR_pulse{ 1000 };

  // add other parameters here and edit broadcast/set_parameters functions
};