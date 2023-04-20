/******************************************************************
  @file       simpleSMA.ino
  @brief      Averages the voltage found on pin A0
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
  Plotter (CTRL+SHFT+L). The vertical (y-axis) on the plotter
  auto scales and the x-axis is a fixed 500 points with each tick
  mark indicative of a Serial.println(). With a 20 ms delay you see
  10 seconds of data (i.e., 500 x 20 ms).

  If you want to stop autoscaling, add the following to loop():
  Serial.println("Min:0,Max:1023");

  Even with nothing connected to A0 you will see the pin floating.
  On our UNO there was a sinusoidal signal that varied between 360
  and 400 - probably around 50Hz.

  Try experimenting with the SMA window variable N. If you make this
  number too big excessive smoothing and lag will be introduced.

******************************************************************/

#include <ReefwingFilter.h>

static SMA<5> sma;

const int INPUT_PIN = A0;

void setup() {
  //  Pin Configuration
  pinMode(INPUT_PIN, INPUT); //  ATMega pins default to inputs, but other MCU's may not.

  //  Start Serial and wait for connection
  Serial.begin(115200);
  while (!Serial);

  //  Serial Plotter - variable names
  Serial.println("RAW_DATA:,SMA:");
}

void loop() {
  //  Read input voltage and filter
  int rawValue = analogRead(INPUT_PIN);
  int smaValue = sma(rawValue);

  Serial.print(rawValue);
  Serial.print(" ");       //  Required for plotting multiple variables. "," and "\t" also works.
  Serial.println(smaValue);

  //  On the UNO ADC conversion takes up to 0.26 ms. This delay shows 10 secs of data.
  delay(20);  
}