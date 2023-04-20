## Reefwing Filter
 This library contains a collection of filter classes and noise generators used in the Reefwing Magpie Drone Flight Controller.

 ### Simple Moving Average (SMA)

 A moving average is a calculation used to analyze data points by creating a series of averages of different subsets of the full data set. It is the most common filter used in Digital Signal Processing (DSP), mostly because it is the easiest to understand and calculate. A moving average is a type of finite impulse response or low pass filter in the frequency domain. It is used to smooth out short-term fluctuations and highlight longer-term trends or cycles.

The more samples you use to compute a new average, the lower the cut-off frequency of your low-pass filter, and the smoother your data will be. The objective is to smooth out the noise without losing quick changes that the drone needs to respond to. 

Similar filters to the moving average filter are the Gaussian, Blackman, and multiple pass moving average. These have better performance in the frequency domain, but are computationally more expensive, and less suited to low power MCU’s.

The moving average filter is the optimal choice for reducing random noise while retaining a sharp step response. It works very well with time domain encoded signals. The moving average is the worst filter for frequency domain encoded signals, with little ability to separate one band of frequencies from another. In other words, the moving average is an exceptionally good smoothing filter (the action in the time domain), but an exceptionally bad low-pass filter (the action in the frequency domain).

 This filter implementation returns the average of the N most recent input values. It has been extracted from the [Arduino-Filters](https://github.com/tttapa/Arduino-Filters) library by Pieter Pas.

 To speed up calculation, rather than adding the N input values each time, we keep track of the previous sum in the accumulator and only have to add the new input and delete the oldest one. The new sum is then divided by N. Previous inputs are kept in a circular buffer.

 A class template allows a variety of variable types to be used with the filter. The input parameters are:
 - ```N```: The number of samples to average.
 - ```input_t```: The type of the input (and output) of the filter. The default is ```uint16_t```.
 - ```sum_t```: The type to use for the accumulator, must be large enough to fit N times the maximum input value. The default is ```uint32_t```.

 If you don't set the input/output and accumulator types then the defaults are used. Declaring a new SMA filter can be done in a number of ways. For example:

 ```c++
 static SMA<25> sma;   //  Simple Moving Average filter of length 25, initialized with a value of 0 and default types used.
 static SMA<10> average = {512};   //  Simple Moving Average filter of length 10, initialized with a value of 512 and default types used.
 static SMA<10, uint32_t, uint64_t> sma;   // for very large inputs (larger than 65,535)
 static SMA<10, int16_t, int32_t> sma;     // for inputs that can go negative
 static SMA<10, float, float> sma;         // for decimal (floating point) values
 static SMA<10, uint16_t, uint16_t> sma;   // for small input values and short averages
```

Some important things to note:
- Unsigned integer types are slightly faster than signed integer types
- Integer types are significantly faster than floating point types. Since most sensors and ADCs return an integer, it's often a good idea to filter the raw integer measurement, before converting it to a floating point number (e.g. voltage or temperature).
- The accumulator has to be large enough to fit `N` times the maximum input value. If your maximum input value is 1023 (e.g. from analogRead), and if the accumulator type is `uint16_t`, as in the last example above, the maximum length `N` of the SMA will be ⌊(2¹⁶ - 1) / 1023⌋ = 64. Alternatively, `analogRead` returns a 10-bit number, so the maximum length is 2¹⁶⁻¹⁰ = 2⁶ = 64, which is the same result as before.

 ### Exponential Moving Average (EMA)

An exponential moving average (EMA) is a type of moving average that places a greater weight and significance on the most recent data points. The exponential moving average is also referred to as the exponentially weighted moving average.

The EMA filter applies weighting factors which decrease exponentially for each prior input. Because recent data has a higher weighting than past data, the EMA responds more quickly and tracks recent changes better than a simple moving average.
An exponential moving average filter is often more appropriate than a simple moving average filter. The SMA uses much more memory and is significantly slower than the EMA. The exponential impulse response of the EMA may be better as well.

The EMA filter applies weighting factors which decrease exponentially for each prior input. The formula for the EMA looks like:

```
EMA(n) = EMA(n-1) + 𝛂 × [input(n) – EMA(n-1)]
```

Where:
- 𝛂 = smoothing factor/time constant between 0 and 1.
- n = the period.

You can use a simple moving average (SMA) when calculating the first `EMA(n)`, since there will not be a previous data point `EMA(n-1)`.
The value used for the smoothing factor alpha is often calculated using:

```
α =  2/(N+1)
```

Where `N` is the number of data values in the period. The problem with this approach is that multiplication with floating point numbers is computationally expensive. Note that `α` is between 0 and 1. We can improve things by choosing a value for `α = 1 / 2^k`. In this case, multiplication with a float can be replaced with a division with a number which is a power of 2, and division by a power of two can be replaced by a very fast right bitshift. Our EMA formula then becomes:

```
EMA(n)  = EMA(n-1) + 𝛂 × [input(n) – EMA(n-1)]
        = EMA(n-1) + (1 / 2^k) × [input(n) – EMA(n-1)]
        = EMA(n-1) + [input(n) – EMA(n-1)] >> k
```

This EMA filter implementation and examples have been extracted from the [Arduino-Filters](https://github.com/tttapa/Arduino-Filters) library by Pieter Pas.

A class template allows a variety of variable types to be used with the EMA filter. The input parameters are:
- `K`: The amount of bits to shift by - replaces `𝛂`. This determines the location of the pole in the EMA transfer function, and therefore the cut-off frequency.  The higher the number, the greater the filtering. The pole location is `1 - 2^(-K)`.
- `input_t`: The integer type to use for the input and output of the filter. Can be signed or unsigned.
- `state_t`: The unsigned integer type to use for the internal state of the filter. A fixed-point representation with `K` fractional bits is used, so this type should be at least `M + K` bits wide, where `M` is the maximum number of bits of the input.

Some examples of different combinations of the class template parameters:

1. Filtering the result of `analogRead`: analogRead returns an integer between 0 and 1023, which can be represented using 10 bits, so `M = 10`. If `input_t` and `output_t` are both `uint16_t`, the maximum shift factor `K` is `16 - M = 6`. If `state_t` is increased to `uint32_t`, the maximum shift factor `K` is `32 - M = 22`.
2. Filtering a signed integer between -32768 and 32767: this can be represented using a 16-bit signed integer, so `input_t` is `int16_t`, and `M = 16`. (2¹⁵ = 32768)Let's say the shift factor `K` is 1, then the minimum width of `state_t` should be `M + K = 17` bits, so `uint32_t` would be a sensible choice.

Declaring a new EMA filter can be done in a number of ways. For example:

 ```c++
static EMA<2> ema;  //  K = 2, use default input_t & state_t types (uint16_t)
static EMA<4, uint32_t> ema;    //  K = 4, input_t & state_t type is uint32_t
 ```

### Complementary Filter (CF)

Mahony *et al*, developed the complementary filter which has been shown to be an efficient and effective solution to gyroscopic drift in an IMU. 

A complementary filter is a sensor fusion technique which combines data from a high-pass and low-pass filter. These filters are designed to remove high and low frequency noise. Compared to the Kalman Filter, this is a computationally inexpensive calculation and is suited to low power processors. Don't be scared by the fancy name though, a complementary filter is basically a weighted average of two numbers.

In its simplest form, the complementary filter calculation is:

```
CF = 𝛂 × Input_A + 𝛃 × Input_B
```

Where:
- 𝛂 + 𝛃 = 1 and are the two weighting factors; and
- Input_A & Input_B are two measurements of the same thing from different sensors.

An example of how you would use this filter would be deriving roll or pitch angles from an IMU. Gyroscope data is fast to respond but drifts over time. Angle data calculated from the accelerometer is sensitive to vibration but accurate in the long term. The combined results of these two sensors can provide a better result than either alone. We can use a complementary filter to combine them: 

```
Angle 𝜽(t + ∆t) = 0.5 × (𝜽(t) + ω × ∆t) + 0.5 × (tan-1(ay/az))
```

The `𝛂 = 𝛃 = 0.5` in the above formula is the weighting or bias applied to the data. The two weighting factors must add up to one. The gyroscope provides an angular rate (ω), so we integrate it to work out the angle. We can use the two accelerometer vectors (ay and az) to calculate the same angle using trigonometry.

In most examples that you will see, 𝛂 = 0.98 for the gyro angle input and 𝛃 = 0.02 for the accelerometer contribution. The higher the 𝛂, the quicker the response but the higher the drift. A lower 𝛂 will reduce drift but the accelerometer is susceptible to vibration.

It is not obvious but the `𝛂` term in the complementary filter formula is providing the low and high pass filter function. The complementary filter time constant (𝜏) defines the crossover point between trusting the gyroscope data and trusting the accelerometer data.

This can be demonstrated by looking at the [discrete-time implementation of an RC low-pass filter](https://medium.com/r/?url=https%3A%2F%2Fen.wikipedia.org%2Fwiki%2FLow-pass_filter%23Passive_digital_realization). This turns out to be an exponentially weighted moving average:

```
yi = axi + (1 - 𝛂)yi-1
```

Where:

```
𝛂 = ∆t / (RC + ∆t)
```

The filter coefficient, `𝛂` is also known as the smoothing factor and by definition, `0 ≤ 𝛂 ≤ 1`. The expression for `𝛂` yields the equivalent time constant in terms of the sampling period `∆t` and smoothing factor `𝛂`. The filter time constant, `𝜏 = RC`, and we can combine this this with the definition for `𝛂` above to get:

```
𝜏 = RC = ∆t(1 - 𝛂) / 𝛂
```

The filter cutoff frequency is defined as:

```
fc = 1 / 2𝜋𝜏 = 𝛂 / ((1 - 𝛂)2𝜋∆t)
```

For a low pass filter, if `𝛂 = 0.5`, then the time constant is equal to the sampling period. If `𝛂 ≪ 0.5`, then `𝜏` is significantly larger than the sampling interval, and `∆t ≈ 𝛂𝜏`.

**To determine what filter coefficient to use in our application we use the sampling period and the time constant.**

The sampling frequency is how often the variable is measured. This may not be the same as the IMU gyro/acc sampling rate which can be over 450 Hz in the LSM9DS1. It depends on how quickly your `loop()` is executing.

You select a time constant based on what you are trying to filter out. If your gyroscope is drifting at 2 degrees per second, you want your time constant to be less than one second so that you are no more than a couple of degrees off the actual angle at anytime. However, the lower the time constant, the more horizontal acceleration noise will be allowed to pass through. As with any engineering tradeoff, the best way to determine the optimum cutoff frequency for your two filters is by testing it.

**An important point is that the formula for 𝛂 is different for LPF and HPF. For High Pass filters:**

```
𝛂 = 𝜏 / (𝜏 + ∆t)
```

We want to pass the gyro rate through a high pass filter. If we want a time constant, `𝜏 = 0.75` seconds, and have a sampling frequency of 150 Hz (i.e., sampling the gyro rate every 6.7 ms) we require the filter coefficient to be:

```
𝛂 = 0.75 / (0.75 + 0.0067) = 0.99
```

A large `α` implies that the output will decay very slowly but will also be strongly influenced by even small changes in input. The output of this filter will decay towards zero for an unchanging input.

### Simple Kalman Filter (SKF)

A basic implementation of the [Kalman Filter](https://github.com/denyssene/SimpleKalmanFilter) for single variable models. It can be used with a stream of single values like barometric sensors, temperature sensors, gyroscope, and accelerometers.

The Kalman filter is a set of mathematical equations that provides an efficient computational (recursive) means to estimate the state of a process, in a way that minimizes the mean of the squared error. The filter is very powerful because:
- it supports estimations of past, present, and even future states, and 
- it can do so even when the precise nature of the modeled system is unknown.

#### Basic Usage

- `e_mea`: Measurement Uncertainty - How much do we expect to our measurement vary
- `e_est`: Estimation Uncertainty - Can be initilized with the same value as e_mea since the kalman filter will adjust its value.
- `q`: Process Variance - usually a small number between 0.001 and 1 - how fast your measurement moves. Recommended 0.01. Should be tuned for the application.

```c++
 SimpleKalmanFilter kf = SimpleKalmanFilter(e_mea, e_est, q);

 while (1) {
  float x = analogRead(A0);
  float estimated_x = kf.updateEstimate(x);
  
  // ...
 } 
```

### Noise Generator

When testing filters, it is often handy to be able to add noise to sensor readings. We can then apply our different filters and see which performs best with different types of noise.

The noise generators available are:

- `randomWithRange(int min, int max)`: This generator will generate a random `long` between `min` and `max`. `min` and `max` can be positive or negative.
- `gaussianWithDeviation(int sd)`: Gaussian noise, is statistical noise having a probability density function (PDF) equal to that of the normal distribution, which is also known as the Gaussian distribution. In other words, the values that the noise can take on are Gaussian-distributed. This function generates an Additive White Gaussian Noise (AWGN) sample. The default generated sample set will have zero mean and a standard deviation of 1. Initialise with a different number of standard deviations (`sd`) if required.
- `threeStagePink()`: In Pink Noise the power spectral density (power per frequency interval) is inversely proportional to the frequency of the signal. In other words, each octave interval (halving or doubling in frequency) carries an equal amount of noise energy. The Pink Noise code is based on the Trammell algorithm (http://www.ridgerat-tech.us/pink/pinkalg.htm).
- `oneBitLFSR()`: Our final generator returns a pseudo random 0 or 1. A linear-feedback shift register (LFSR) is a shift register whose input bit is a linear function of its previous state. They can produce a sequence of bits that appears random and has a very long cycle (2^16 in our case). The algorithm is derived from http://users.ece.cmu.edu/~koopman/lfsr/.

### Examples

#### 1. Simple Moving Average - Analogue Read

This sketch is designed to be used with the Arduino IDE Serial 
Plotter (`CTRL+SHFT+L`). The vertical (y-axis) on the plotter
auto scales and the x-axis is a fixed 500 points with each tick
mark indicative of a Serial.println(). With a 20 ms delay you see
10 seconds of data (i.e., 500 x 20 ms).

If you want to stop autoscaling, add the following to loop():

```c++
Serial.println("Min:0,Max:1023");
```

Even with nothing connected to `A0` you will see the pin floating.
On our UNO there was a sinusoidal signal that varied between 360
and 400 - probably around 50Hz.

Try experimenting with the SMA window variable `N`. If you make this
number too big excessive smoothing and lag will be introduced.

#### 2. Comparing the SMA and EMA filter

This example is similar to the `simpleSMA.ino` sketch but adds an `EMA` filter as well. It is also is designed to be used with the Arduino IDE Serial 
Plotter (`CTRL+SHFT+L`).

You will note that the `EMA` is quicker to respond than the `SMA` and will be better for catching quick sensor changes but also be more susceptible to noise and vibration. The `SMA` has a noticable lag which increases as you increase the window (`N`). Too large an `N` on the `SMA` will obscure the original signal and needs to be suited to the application. 

For the `EMA`, the variable `K` determines the cut-off frequency of the low pass filter (LPF).  The higher the value of `K`, the lower the LPF 3 dB point and the greater the filtering.

#### 3. Arduino UNO Temperature Sensor

This sketch is designed to be used with the Arduino IDE Serial 
Plotter (`CTRL+SHFT+L`). 
  
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

Both the `SMA` and `EMA` filters do a good job of filtering out this 
random noise. If you can't see the `SMA` and `EMA` curves because they
are covered by the temperature, try adding a fixed offset like plus
and minus 5 to the SMA and EMA plots. For example,

```c++
smaValue = sma(rawValue) + 5;
emaValue = ema(rawValue) - 5;
```

The initialisation of the Kalman Filter is as follows.

```c++
SimpleKalmanFilter(e_mea, e_est, q);
```

Where: 
  - `e_mea`: Measurement Uncertainty 
  - `e_est`: Estimation Uncertainty 
  - `q`: Process Noise

The ReefwingFilter library contains various types of noise generators,
including:

  - random white noise;
  - pink noise; and
  - gaussian noise.
  
As the temperature reading is not inherently noisy, we can add some
artificial noise and then try to filter it back out. We can then compare
the filtered signal to our original temperature reading to determine
how well our filters perform.