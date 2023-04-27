/******************************************************************
  @file       unoTempSensor.ino
  @brief      Effect of SMA and EMA on UNO Internal Temp Sensor.
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

  This sketch is designed to be used with the Arduino IDE Serial 
  Plotter (CTRL+SHFT+L). 
  
  The Arduino UNO uses the ATMega328P MCU. It has an internal chip
  temperature sensor connected to the ADC, which you can read. It is
  not very accurate (plus or minus 10 degrees) but it serves as a
  sensor that we can filter data on.

  As MCU specific register addresses are used in this sketch, it will only
  work with Arduino boards which use the ATMega328P (i.e., the UNO and 
  original Nano).

  After switch on our ATMega328P warms up to just over 250 C fairly
  quickly. From that point there is not much to filter so we add
  some artificial noise.

  Both the SMA and EMA filters do a good job of filtering out this 
  random noise. If you can't see the SMA and EMA curves because they
  are covered by the temperature, try adding a fixed offset like plus
  and minus 5 to the SMA and EMA plots. For example,

  smaValue = sma(rawValue) + 5;
  emaValue = ema(rawValue) - 5;

  The initialisation of the Kalman Filter is as follows.

  SimpleKalmanFilter(e_mea, e_est, q);

  Where: 
  - e_mea: Measurement Uncertainty 
  - e_est: Estimation Uncertainty 
  - q: Process Noise

  The ReefwingFilter library contains various types of noise generators,
  including:

  - random white noise;
  - pink noise; and
  - gaussian noise.
  
  As the temperature reading is not inherently noisy, we can add some
  artificial noise and then try to filter it back out. We can then compare
  the filtered signal to our original temperature reading to determine
  how well our filters perform.

******************************************************************/

#include <ReefwingFilter.h>

static SMA<5, uint32_t, uint64_t> sma;
static EMA<4, uint32_t> ema;
SimpleKalmanFilter kalmanFilter(2, 2, 0.01);
NoiseGenerator noise;

long noisyValue, rawValue, smaValue, emaValue, kalmanValue;

/******************************************************************
   Serial Plot Helper Function

   Usage:  plot("Humidity",humidity_value,false); Graph1, more graphs follow.
           plot("Pressure",pressure_value,true);  Graph2, last graph.

 ******************************************************************/

void plot(String label, int value, bool last) {
  Serial.print(label); // May be an empty string.
  if (label != "") Serial.print(":");
  Serial.print(value);
  if (last == false) Serial.print(",");
  else Serial.println();
}

long getATMega328PTemp() {
  ADMUX |= B00001000;     //  Read A8 (Temp Sensor) 
  ADMUX |= B11000000;     //  (INTERNAL 1.1V Reference)
  ADCSRA |= B11000000;    //  ADEN and ADSC registers set to 1   (Start conversion)

  while (bit_is_set(ADCSRA, ADSC));         // Wait for ADC conversion to end
  
  long raw_temp = ADCL | (ADCH << 8);       // Read analogue input  
  long temperature = (raw_temp - 53)/1.22;  // f(x) = (raw - offset) / coeff
  
  return temperature;
}

void setup() {
  //  Start Serial and wait for connection
  Serial.begin(115200);
  while (!Serial);
}

void loop() {
  //  Read temperature
  rawValue = getATMega328PTemp();

  //  Select Noise Source to add to the temperature reading. For example:
  //  noisyValue = rawValue + noise.gaussianWithDeviation(2);
  //  noisyValue = rawValue + noise.threeStagePink();
  //  noisyValue = rawValue + noise.gaussianWithDeviation(2) + noise.threeStagePink() + noise.randomWithRange(-15, 15);
  noisyValue = rawValue + noise.randomWithRange(-25, 25);

  //  Apply Filters
  smaValue = sma(rawValue);
  emaValue = ema(rawValue);
  kalmanValue = kalmanFilter.updateEstimate(rawValue);

  //  Display the result on the IDE Serial Plotter
  plot("TEMP", rawValue, false);
  plot("WITH_NOISE", noisyValue, false);
  plot("SMA", smaValue, false);
  plot("EMA", emaValue, false);
  plot("KALMAN", kalmanValue, true);

  delay(120);  //  This delay shows 60 secs of data.

}