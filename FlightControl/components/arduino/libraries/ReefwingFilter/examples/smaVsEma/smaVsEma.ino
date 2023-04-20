/******************************************************************
  @file       smaVsEma.ino
  @brief      Compares effect of SMA and EMA on analogRead() Value.
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
  mark indicative of a Serial.println(). With a 120 ms delay you see
  60 seconds of data (i.e., 500 x 120 ms).

  If you want to stop autoscaling, add the following to loop():
  Serial.println("Min:0,Max:1023");

  Even with nothing connected to A0 you will see the pin floating.
  On our UNO there was a sinusoidal signal that varied between 360
  and 400 - probably around 50Hz.

******************************************************************/

#include <ReefwingFilter.h>

static SMA<5> sma;
static EMA<2> ema;

int rawValue, smaValue, emaValue;

const int INPUT_PIN = A0;

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

void setup() {
  //  Pin Configuration
  pinMode(INPUT_PIN, INPUT); //  ATMega pins default to inputs, but other MCU's may not.

  //  Start Serial and wait for connection
  Serial.begin(115200);
  while (!Serial);
}

void loop() {
  //  Read input voltage and filter
  rawValue = analogRead(INPUT_PIN);
  smaValue = sma(rawValue);
  emaValue = ema(rawValue);

  plot("RAW_DATA", rawValue, false);
  plot("SMA", smaValue, false);
  plot("EMA", emaValue, true);

  delay(120);  //  This delay shows 60 secs of data.
}