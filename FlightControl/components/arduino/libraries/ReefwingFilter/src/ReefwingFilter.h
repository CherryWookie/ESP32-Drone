/******************************************************************
  @file       ReefwingFilter.h
  @brief      A collection of filters used in the Reefwing Flight Controller.
  @author     David Such
  @copyright  Please see the accompanying LICENSE.txt file.

  Code:        David Such
  Version:     2.0.0
  Date:        15/12/22

  1.0.0 Original Release.           14/02/22
  1.0.1 Fixed Guassian defn.        20/02/22
  1.0.2 Fixed #define               24/02/22
  1.1.0 Added Madgwick & Mahony     04/03/22
  2.0.0 Changed Repo and Branding   15/12/22

  Credits - SMA and EMA filter code is extracted from the 
            Arduino-Filters Library by Pieter Pas
            (https://github.com/tttapa/Arduino-Filters)
          - Simple Kalman Filter is from the library by 
            Denys Sene (https://github.com/denyssene/SimpleKalmanFilter).
          - Pink Noise Algorithm (http://www.ridgerat-tech.us/pink/pinkalg.htm)
          - Quaternion conversion to Euler Angles
            (https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles)

******************************************************************/

#ifndef ReefwingFilter_h
#define ReefwingFilter_h

#include "Arduino.h"

/******************************************************************
 *
 * Quaternion Class Definition - 
 * 
 ******************************************************************/

struct EulerAngles {
  double yaw, pitch, roll;    
};

class Quaternion {
  public:
    Quaternion();
    Quaternion(double w, double x, double y, double z);
    Quaternion(double yaw, double pitch, double roll);

    EulerAngles toEulerAngels();
    void madgwickUpdate(float ax, float ay, float az, float gx, float gy, float gz, float mx, float my, float mz); 

    double q0, q1, q2, q3;      //  Euler Parameters
    EulerAngles eulerAngles;

};

/******************************************************************
 *
 * Filter Class Prototypes - 
 * 
 ******************************************************************/

template <uint8_t N, class input_t = uint16_t, class sum_t = uint32_t>
class SMA {
  public:
    //  Default constructor (initial state is initialized to all zeros).
    SMA() = default;

    //  Constructor (initial state is initialized to given value).
    SMA(input_t initialValue) : sum(N * (sum_t)initialValue) {
        fill(begin(previousInputs), end(previousInputs),
                  initialValue);
    }

    //  Update the internal state with the new input and return the new output.
    input_t operator()(input_t input) {
        sum -= previousInputs[index];
        sum += input;
        previousInputs[index] = input;
        if (++index == N)
            index = 0;
        return (sum + (N / 2)) / N;
    }
    
    static_assert(
        sum_t(0) < sum_t(-1),  // Check that `sum_t` is an unsigned type
        "Error: sum data type should be an unsigned integer, otherwise, "
        "the rounding operation in the return statement is invalid.");

  private:
    uint8_t index             = 0;
    input_t previousInputs[N] = {};
    sum_t sum                 = 0;
};

template <uint8_t K, class uint_t = uint16_t>
class EMA {
  public:
    /// Update the filter with the given input and return the filtered output.
    uint_t operator()(uint_t input) {
        state += input;
        uint_t output = (state + half) >> K;
        state -= output;
        return output;
    }

    static_assert(
        uint_t(0) < uint_t(-1),  // Check that `uint_t` is an unsigned type
        "The `uint_t` type should be an unsigned integer, otherwise, "
        "the division using bit shifts is invalid.");

    /// Fixed point representation of one half, used for rounding.
    constexpr static uint_t half = 1 << (K - 1);

  private:
    uint_t state = 0;
};

class ComplementaryFilter {
  public:
    ComplementaryFilter() = default;

    ComplementaryFilter(float bias, float A, float B) {
      alpha = bias;
      input_A = A;
      input_B = B;
    }

    float result() { return ((alpha * input_A) + (1 - alpha) * input_B); }
    void setValues(float bias, float A, float B) {
      alpha = bias;
      input_A = A;
      input_B = B;
    }

  private:
    float alpha = 0.98;
    float input_A = 0.0;
    float input_B = 0.0;
};

/*
 * SimpleKalmanFilter - a Kalman Filter implementation for single variable models.
 * Created by Denys Sene, January, 1, 2017.
 * Released under MIT License - see LICENSE file for details.
 */

class SimpleKalmanFilter {

public:
  SimpleKalmanFilter(float mea_e, float est_e, float q);
  
  float updateEstimate(float mea);
  void setMeasurementError(float mea_e);
  void setEstimateError(float est_e);
  void setProcessNoise(float q);
  float getKalmanGain();
  float getEstimateError();

private:
  float _err_measure;
  float _err_estimate;
  float _q;
  float _current_estimate = 0;
  float _last_estimate = 0;
  float _kalman_gain = 0;

};

/******************************************************************
 *
 * Filter Class Prototypes - 
 * 
 ******************************************************************/

class NoiseGenerator {
  public:
    NoiseGenerator() {
      randomSeed(analogRead(A0));
      clearPinkState();
    }

    bool oneBitLFSR();
    double gaussianWithDeviation(int sd=1);
    void clearPinkState();
    float threeStagePink();
    long randomWithRange(int min=-5, int max=5) { return (random(min, max)); }

  private:
    const uint16_t LFSR_INIT = 0xfeed;  //  Seed Value - any non-zero value
    const uint16_t LFSR_MASK = 0x8016;  //  Ref: http://users.ece.cmu.edu/~koopman/lfsr/
    
    static const uint16_t PINK_NOISE_STAGES = 3;
    float state[PINK_NOISE_STAGES];
    static const float A[PINK_NOISE_STAGES];
    static const float P[PINK_NOISE_STAGES];

};

#endif