# DIY Quadcopter
Michael Sell & Ben Schaser

## Table of Contents:

[`Introduction](#introduction)

[`Recources`](#resources)

[`Materials`](#materials)

[`Calculations`](#calculations)

[`Construction`](#construction)

[`Troubleshooting`](#troubleshooting)

[`PID`](#pid)


## Introduction 
#
This project is intended to provide a research experience within computer science.
It will include reading of scientific literature, exploring unsolved problems, and
making progress on difficult problems within CS.
The goal is to understand the hardware and software associated with quadcopter drones. We will build a drone that will include a frame,
motors and their controllers, a microcontroller, and a battery. This hardware
will be controlled by writing software that controls the motors, communicates
with a remote control, and provides smooth control to produce stable flight.

### Objectives
* Gain an intimate knowledge of drone hardware and commonly used software techniques
* Be able to apply this knowledge to build a drone that can be flown via
remote control
* Gain experience in writing documentation for the hardware and software

## Resources
#
* [`Bluepad32` Game Controller Library](https://retro.moe/2020/11/24/bluepad32-gamepad-support-for-esp32/)
* [ESP Proprietary Drones](https://espressif-docs.readthedocs-hosted.com/projects/espressif-esp-drone/en/latest/gettingstarted.html)
* [ESP with PWM and Servo Control](https://dronebotworkshop.com/esp32-servo/#ESP32_PWM)
* [How Brushless Motors work and how to Control them with ESC/Arduino](https://www.youtube.com/watch?v=uOQk8SJso6Q)
* [Quadcopter Physics and Control Software](https://andrew.gibiansky.com/blog/physics/quadcopter-dynamics/)
* [Drone Mechanics Review](Resources/review-drones%20(1).pdf)
* [Quadcopter Calculator](https://www.ecalc.ch/xcoptercalc.php)
* [ESC Calibration Video (w/really funny voice inflections)](https://youtu.be/t-w5Oog8Jcg)
* [ESC Calibration Library Github Link](https://github.com/lobodol/ESC-calibration)


`Motor Bench Tests:`
In order to accurately estimate how much thrust our propellers and motors are capable of producing, we took data from three different tests that used roughly the same spec of motors we had and averaged the three to produce a thrust estimate for our drone.

* [Test 1](https://www.youtube.com/watch?v=77WlZwNHjo8&t=430s)
* [Test 2](https://www.google.com/search?q=2306+2450KV+Brushless+Motor+bench+test&rlz=1C5GCEM_enUS1028US1028&biw=1280&bih=696&tbm=vid&sxsrf=AJOqlzWs4r4yYQFD0tcjWqWzBB7pllfhbQ%3A1675227781592&ei=hfLZY8TmI7ahptQPzN6msA4&ved=0ahUKEwiEvdvmxfP8AhW2kIkEHUyvCeYQ4dUDCA0&uact=5&oq=2306+2450KV+Brushless+Motor+bench+test&gs_lcp=Cg1nd3Mtd2l6LXZpZGVvEAMyBQghEKsCOgQIIxAnOgUIABCiBDoHCAAQHhCiBFDrAljhDWDKDmgAcAB4AIABbogB4AiSAQM5LjOYAQCgAQHAAQE&sclient=gws-wiz-video#fpstate=ive&vld=cid:67e3aa07,vid:T0EzXr54jb8)
* [Test 3](https://www.youtube.com/watch?v=yRARMQXxQSY)



## Materials
#
* [`ESP32-WROOM-32D`](https://www.amazon.com/HiLetgo-ESP-WROOM-32-Development-Microcontroller-Integrated/dp/B0718T232Z/ref=sr_1_3?keywords=esp32+vroom+32D&qid=1676402037&sr=8-3)
    * <a href="https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32d_esp32-wroom-32u_datasheet_en.pdf">View DataSheet</a> 

* [`295mm Drone Frame (7inch Carbon Fiber)`](https://www.amazon.com/dp/B086X2JZD6/ref=sspa_dk_detail_1?psc=1&pd_rd_i=B086X2JZD6&pd_rd_w=WCDXi&content-id=amzn1.sym.dd2c6db7-6626-466d-bf04-9570e69a7df0&pf_rd_p=dd2c6db7-6626-466d-bf04-9570e69a7df0&pf_rd_r=J0KNY7VF83TW2ERNHBJ2&pd_rd_wg=QHlVM&pd_rd_r=0c775401-ea1b-4a0c-b214-b0ee40c58d4a&s=toys-and-games&sp_csd=d2lkZ2V0TmFtZT1zcF9kZXRhaWxfdGhlbWF0aWM&spLa=ZW5jcnlwdGVkUXVhbGlmaWVyPUExMUUxVjBDT1VUMUQ2JmVuY3J5cHRlZElkPUEwNzk4MTg1MUw0UFRLT0lKMVhDMSZlbmNyeXB0ZWRBZElkPUEwNjY2MjQwMUpNTklOWE8xSFY2WSZ3aWRnZXROYW1lPXNwX2RldGFpbF90aGVtYXRpYyZhY3Rpb249Y2xpY2tSZWRpcmVjdCZkb05vdExvZ0NsaWNrPXRydWU=)

* [`Motors`](https://www.amazon.com/iFlight-2450KV-Brushless-Racing-Quadcopter/dp/B096RTCGDT/ref=sr_1_2_sspa?crid=186K7HOMICKND&keywords=fpv+drone+motors+2306&qid=1675195938&sprefix=fpv+drone+motors+2306%2Caps%2C211&sr=8-2-spons&ufe=app_do%3Aamzn1.fos.006c50ae-5d4c-4777-9bc0-4513d670b6bc&psc=1&spLa=ZW5jcnlwdGVkUXVhbGlmaWVyPUExUkZQWjgwTlJNOFpGJmVuY3J5cHRlZElkPUEwODEzNDE4MlVaRUJKS0UwV1pJTCZlbmNyeXB0ZWRBZElkPUEwMDc5NDgxMUYxQkdFR1IxRVRXNiZ3aWRnZXROYW1lPXNwX2F0ZiZhY3Rpb249Y2xpY2tSZWRpcmVjdCZkb05vdExvZ0NsaWNrPXRydWU=)
    * <https://shop.iflight-rc.com/XING-E-Pro-2306-4S-6S-FPV-Motor-pro1565>

* [`4 in 1 ESC`](https://www.amazon.com/dp/B09SNWZRDG/ref=sspa_dk_detail_0?psc=1&pd_rd_i=B09SNWZRDG&pd_rd_w=Mll1v&content-id=amzn1.sym.bff6e147-54ad-4be3-b4ea-ec19ea6167f7&pf_rd_p=bff6e147-54ad-4be3-b4ea-ec19ea6167f7&pf_rd_r=2F9ZJ0W92CTRR8411K03&pd_rd_wg=JeM0S&pd_rd_r=8f039164-7b73-4ce9-93e9-99d09ca311aa&s=electronics&sp_csd=d2lkZ2V0TmFtZT1zcF9kZXRhaWwy&smid=A2J1A69ATLC913&spLa=ZW5jcnlwdGVkUXVhbGlmaWVyPUEzVVdDVjFVVjE0UFZDJmVuY3J5cHRlZElkPUEwNTExNjE5MjU1SFlCUUUzUUhVVyZlbmNyeXB0ZWRBZElkPUEwNDEzNTA0MjRPU09XOVZGQTA1OCZ3aWRnZXROYW1lPXNwX2RldGFpbDImYWN0aW9uPWNsaWNrUmVkaXJlY3QmZG9Ob3RMb2dDbGljaz10cnVl)

* [`Battery`](https://www.amazon.com/HOOVO-Battery-5500mAh-Connector-Compatible/dp/B09FJZKPKV/ref=sr_1_8?keywords=hoovo+4s+5500ma+lipo+battery&qid=1675199066&sr=8-8&ufe=app_do%3Aamzn1.fos.006c50ae-5d4c-4777-9bc0-4513d670b6bc
)
    * [3000mAh 4S Battery](https://www.amazon.com/HRB-3000Mah-Battery-Airplane-Traxxas/dp/B06XKNM73N/ref=sr_1_1_sspa?crid=30DEGJM5CF0MQ&keywords=hoovo+4s+2200ma+lipo+battery+60c&qid=1677012077&sprefix=hoovo+4s+2200ma+lipo+battery+60c%2Caps%2C129&sr=8-1-spons&psc=1&spLa=ZW5jcnlwdGVkUXVhbGlmaWVyPUEzUE9MOUdWT1BGTEFPJmVuY3J5cHRlZElkPUEwMzQ5MzQyMTdJWldWVUhOWUVGSiZlbmNyeXB0ZWRBZElkPUEwNzQyMDUyMTVQMlNMVTIxQ0tBOCZ3aWRnZXROYW1lPXNwX2F0ZiZhY3Rpb249Y2xpY2tSZWRpcmVjdCZkb05vdExvZ0NsaWNrPXRydWU=)
     
* [`Propellers 7”`](https://www.amazon.com/12PCS-HQProp-7X4X3-Light-Range/dp/B09NV9CGG2/ref=sr_1_12?crid=8WZDZD947POE&keywords=FPV+7%22+props&qid=1675798270&s=toys-and-games&sprefix=fpv+7+props%2Ctoys-and-games%2C98&sr=1-12)

* [`Buck Converter`](https://www.amazon.com/DZS-Elec-Adjustable-Electronic-Stabilizer/dp/B06XRN7NFQ/ref=sxin_15_pa_sp_search_thematic_sspa?content-id=amzn1.sym.fe3abdfa-d248-4e07-8b0d-b8a0a47d4a6c%3Aamzn1.sym.fe3abdfa-d248-4e07-8b0d-b8a0a47d4a6c&crid=2WEEA633BVTRX&cv_ct_cx=step+down+dc+voltage+converter&keywords=step+down+dc+voltage+converter&pd_rd_i=B06XRN7NFQ&pd_rd_r=9490743f-4ec6-4445-b12f-edb43e1d4aa7&pd_rd_w=IANd8&pd_rd_wg=dkxLE&pf_rd_p=fe3abdfa-d248-4e07-8b0d-b8a0a47d4a6c&pf_rd_r=NXVM7FCRCKWPGDQKD34G&qid=1675364662&s=electronics&sprefix=step+down+dc+voltage+converter%2Celectronics%2C105&sr=1-2-a73d1c8c-2fd2-4f19-aa41-2df022bcb241-spons&psc=1&spLa=ZW5jcnlwdGVkUXVhbGlmaWVyPUFQRU41NEZFNEdRT0UmZW5jcnlwdGVkSWQ9QTA1MzY0NDcyODhLWU9BWU0ySDlMJmVuY3J5cHRlZEFkSWQ9QTA2Nzk2NzAxMkoxS1lZWUdYRzlHJndpZGdldE5hbWU9c3Bfc2VhcmNoX3RoZW1hdGljJmFjdGlvbj1jbGlja1JlZGlyZWN0JmRvTm90TG9nQ2xpY2s9dHJ1ZQ==)

* [`Battery Charger`](https://www.amazon.com/Haisito-HB6-lipo-Charger/dp/B08C592PNV/ref=sr_1_2_sspa?keywords=4s+lipo+battery+charger+eC5&qid=1675796088&sr=8-2-spons&psc=1&spLa=ZW5jcnlwdGVkUXVhbGlmaWVyPUExNE5CMTY4WFRVREZTJmVuY3J5cHRlZElkPUEwOTcyMjQwQTc2SUxKTUREVTYxJmVuY3J5cHRlZEFkSWQ9QTA4MjI3NzgyVksxMEJGUUZTTVE3JndpZGdldE5hbWU9c3BfYXRmJmFjdGlvbj1jbGlja1JlZGlyZWN0JmRvTm90TG9nQ2xpY2s9dHJ1ZQ==)

* [`Battery Adapters`](https://www.amazon.com/FLY-RC-Connector-Silicone-11-8inch/dp/B07C23S3RK/ref=sr_1_10?crid=16JVO0JYO4LT5&keywords=lipo%2BeC5%2Badapter&qid=1675796767&sprefix=lipo%2Bec5%2Badapter%2Caps%2C143&sr=8-10&th=1)

## Calculations

## Installing Libraries (Ben you can do this thanks bb)
1. Install ESP-IDF v4.4 and dependencies found [HERE](https://docs.espressif.com/projects/esp-idf/en/v4.4.4/esp32/get-started/index.html).
2. Install Espressif IDF VSCode extension.
3. In VSCode, open command palette and choose ESP-IDF: Configure ESP-IDF Extension.
4. Select 'Use Existing Setup' and wait for completion.
5. Clone this repository into a new folder using `git clone https://github.com/CherryWookie/ESP32-Drone`
6. Open FlightControl folder in VSCode.
7. Build and flash to ESP32 board using built-in IDF buttons on bottom toolbar.
9. MBL Quadcopter Telemetry Console can be opened by running `MBL_QTC` in a python shell.
10. Python library `tkinter` will need to be installed for the MBL_QTC to run properly.

## Connectivity


## ESC Calibration
To calibrate our 4 in 1 ESC, we used the following procedure:

*(Note:Steps may vary depending on ESC Model.)*

<Font Size= 4>`Steps:`</Font>

1. Power ESP ONLY while sending 2000 PWM (Max throttle)
2. Plug in the battery/power on the ESC. A series of beeps should ensue.
3. While the ESC is still plugged in, send 1000 PWM (Min throttle). Another different series of beeps should follow.
4. Unplug battery from ESC. Calibration should be complete.

NOTE: After calibration is completed successfully, the power up should result in only one sycronized 3 beep series instead of a series of different beeps. The different beep patterns can be deciphered in your specific ESC manual; however, for our purposes, we used a third party ESC to decipher our beeps and found them to be quite similar.

```C++
ESC VScode code
```

<details>
<summary>Failed Attempts</summary>
<br>
Using the [ESC-Calibration.io](https://github.com/lobodol/ESC-calibration) from lobodo we were able to calibrate the esc by finding max and min throttle and then running the test function that starts at minimum throttle and goes to max. We ended up using an Arduino UNO to perform the calibration as the library wasn't working with the ESP32 and kept giving an error of 'Timed out waiting for packet header'. We tried it in arduino IDE along with VSCode and none worked with the ESP32. We anticipate that the library wasn't friendly with the model of ESP32? Not sure

As we were testing the motors with the Arduino, we were able to calibrate them individually without a problem; however, when we started to test all four of them at once, at least one motor remained motionless. To remedy this, we switched back over to the ESP32 board and used the following code to test our motors with through VScode and all four worked simultaneously without a problem

</details>

#

## Construction
### `Frame/General`
For the frame construction, we followed the inluded instructions. The ESC was simply put on with the included screws, shock absorbers, and nuts. 

### Soldering/ESC
Soldering the wires onto the ESC was unnecessarily difficult. What ended up working the best was to hold the iron at an angle so that as much of the iron as possible was in contact with the ESC metal contacts. Adding solder to the contacts prior to placing the wire worked well also. Holding the wire down with the iron while resting it in position on top of the contacts and adding solder eventually was the best method. Remember to be aware of any stray solder connecting any of the chips on the ESC.

Keep in mind that the ESC will get very hot during this process so make sure to let the chip cool down in between solders to avoid burning the chip. 

With the motor leads coming off of the ESC, we initially tried soldering the wires to the pins on top of the ESC but quickly realized that it wasn't viable as the pins weren't holding solder very well. We ended up splicing wires to the short small Molex connector that was included with the ESC. 

### `Buck Converter`
We tested the converter first and tuned it to approximately 5.41 V output using the onboard potentiometer. 

### `ESP32`
We opted to use a prototype PCB board to run our wiring through. We soldered the ESP into the PCB and ran wiring underneath in order to connect to the wires coming out of the ESC to the back of the drone.

### `MPU 6050 Gyro Module`
For this sensor, we had slight issues with the unit overshooting the actual degree of inclination and slowly coming back down to the expected value. This is most likely due to the Kalman filter library that we used. It didn't seem to be giving us a lot of problems besides an occasional overshoot within the range of motion that we were expecting so we opted to use it for the time being.

Another thing we had to account for was our position. We oriented our MPU unit with the `x axis` facing down, the `y axis` pointing backwards, and the `z axis` pointing to the left of the drone. Because of this, we had to add some 90 degree manual offsets to the readings.

## Troubleshooting

Done let ben wire the esp
This is


## PID

For the PID integration, we initially came up with a system to average the different telemetry conditions in order to send our motos cohesive and accurate values. This involved having a motor PWM value for each individual motor for `Pitch` as well as `Roll`. From this, would have 2 PWM values for each motor. Our solution to end up with one single PWM value per motor was to average these values together. *(e.g. (M1roll + M1pitch)/2 etc...)* 

With these values, we could utilize our accelerometer/gyroscope data and send these values accordingly to our motors in hopes to self stabalize.

```C++
PID CODE
```
