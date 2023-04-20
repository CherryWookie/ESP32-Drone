#include "sdkconfig.h"
#ifndef CONFIG_BLUEPAD32_PLATFORM_ARDUINO
#error "Must only be compiled when using Bluepad32 Arduino platform"
#endif  // !CONFIG_BLUEPAD32_PLATFORM_ARDUINO


#include <Arduino.h>
#include <Bluepad32.h>
#include <MBL_Quadcopter.h>
#include <Adafruit_MPU6050.h>
#include <Kalman.h>
#include <PID_v1.h>
#include <WiFi.h>
#include <AsyncUDP.h>
#include <mutex>
#include <sstream>
#include "wifi_access.h"

// TaskHandle_t WiFiAccess;
ControllerPtr myControllers[BP32_MAX_CONTROLLERS];
Adafruit_MPU6050 mpu;

// wifi settings
const char* ssid = "esp32drone";
const char* password = "123456";
constexpr int port = 1234;

int speed = 1000;
int prev_speed = speed;

Kalman Kx;
Kalman Ky;
Kalman Kz;

// global vars to be mutexed
double Kp = 50.0;
double Ki = 1.2;
double Kd = 0.05;

double roll = 0;
double pitch = 0;
double yaw = 0;
// -------------------------

Quadcopter drone(26, 27, 14, 12, Kp, Ki, Kd);
// FL M4 Pin26
// FR M2 Pin27
// RL M3 Pin14
// RR M1 Pin12

unsigned long timer = micros();
double dt = 0;

// MPU Offsets
const double xAccelOffset = -9.8;
const double yAccelOffset = 0;
const double zAccelOffset = 9.8;


void onConnectedController(ControllerPtr ctl) {
    bool foundEmptySlot = false;
    for (int i = 0; i < BP32_MAX_CONTROLLERS; i++) {
        if (myControllers[i] == nullptr) {
            Console.printf("CALLBACK: Controller is connected, index=%d\n", i);
            // Additionally, you can get certain gamepad properties like:
            // Model, VID, PID, BTAddr, flags, etc.
            ControllerProperties properties = ctl->getProperties();
            Console.printf("Controller model: %s, VID=0x%04x, PID=0x%04x\n", ctl->getModelName(), properties.vendor_id,
                           properties.product_id);
            myControllers[i] = ctl;
            foundEmptySlot = true;
            break;
        }
    }
    if (!foundEmptySlot) {
        Console.println("CALLBACK: Controller connected, but could not found empty slot");
    }
}

void onDisconnectedController(ControllerPtr ctl) {
    bool foundController = false;
    speed = 1000;

    for (int i = 0; i < BP32_MAX_CONTROLLERS; i++) {
        if (myControllers[i] == ctl) {
            Console.printf("CALLBACK: Controller is disconnected from index=%d\n", i);
            myControllers[i] = nullptr;
            foundController = true;
            break;
        }
    }

    if (!foundController) {
        Console.println("CALLBACK: Controller disconnected, but not found in myControllers");
    }
}

void processGamepad(ControllerPtr gamepad) {
    if (gamepad->a()) {
        speed = 1000;
    }

    if (gamepad->b()) {

    }

    if (gamepad->x()) {
        
    }
    if (gamepad->y()) {
        
        speed = 2000;
    }
    int y = gamepad->axisY();
    y = -y;
    if (abs(y) > 100) {
        if (y > 0) {
            speed += 1;
        }
        else if (y < 0) {
            speed -= 1;
        }
        
    }
}

// void WiFiAccessLoop(void * pvParameters) {
//   for(;;) {
//     // do wifi stuff
    
//   }
// }

// Arduino setup function. Runs in CPU 1
void setup() {
    Console.printf("Firmware: %s\n", BP32.firmwareVersion());
    // drone.test(1000);
    drone.test(1000);
    
    Kx.setAngle(0);
    Ky.setAngle(0);
    Kz.setAngle(0);
   
    // xTaskCreatePinnedToCore(
    //          WiFiAccessLoop, /* Task function. */
    //          "WiFiAccess",   /* name of task. */
    //          10000,     /* Stack size of task */
    //          NULL,      /* parameter of the task */
    //          1,         /* priority of the task */
    //          &WiFiAccess,    /* Task handle to keep track of created task */
    //          0); 
    
    // Setup the Bluepad32 callbacks
    BP32.setup(&onConnectedController, &onDisconnectedController);
    while (!mpu.begin()) {
        Console.println("Failed to find MPU6050 chip");
        delay(50);
    }
    Console.println("MPU6050 Found!");
    mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
    
    // BP32.forgetBluetoothKeys();
    // WiFi.mode(WIFI_STA);
    // WiFi.begin(ssid, password);
    // if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    //     Console.println("WiFi Failed");
    // while (1) {
    //     delay(1000);
    // }
    // }
    // if (udp.listen(port)) {
    //     Console.print("UDP Listening on IP: ");
    //     Console.println(WiFi.localIP().toString());
    //     Console.print(":");
    //     Console.printf("%n", port);
    //     udp.onPacket(handle_packet);
    // }

    delay(500);
}

// Arduino loop function. Runs in CPU 1
void loop() {
    BP32.update();

    dt = (double)(micros() - timer) / 1000000; // Calculate delta time
    timer = micros();

    // It is safe to always do this before using the controller API.
    // This guarantees that the controller is valid and connected.
    for (int i = 0; i < BP32_MAX_CONTROLLERS; i++) {
        ControllerPtr myController = myControllers[i];

        if (myController && myController->isConnected()) {
            if (myController->isGamepad()) {
                processGamepad(myController);
                speed = constrain(speed, 1000, 2000);
                if (speed != prev_speed) {
                    // drone.test(speed);
                    // Console.printf("PWM = %d\n", speed);
                }
            }
        }
    }
    prev_speed = speed;

    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    // a.acceleration.x += xAccelOffset;
    // a.acceleration.y += yAccelOffset;
    // a.acceleration.z += zAccelOffset;

    double roll_init = atan2(-a.acceleration.x, a.acceleration.z) * RAD_TO_DEG;
    double pitch_init = atan2(-a.acceleration.x, a.acceleration.y) * RAD_TO_DEG;
    // double yaw_init = atan2(a.acceleration.z, a.acceleration.y) * RAD_TO_DEG;
    double gyroX = g.gyro.x / 131.0;
    double gyroY = g.gyro.y / 131.0;
    double gyroZ = g.gyro.z / 131.0;

    drone.roll = Ky.getAngle(roll_init - 79.02, gyroY, dt);
    drone.pitch = Kz.getAngle(pitch_init - 90.35, gyroZ / 131.0, dt);
    // drone.yaw = Kx.getAngle(yaw_init, gyroX / 131.0, dt);

    roll = drone.roll;
    pitch = drone.pitch;
    // yaw = drone.yaw;

    drone.balance();
    
    Console.print("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
    Console.printf("%f, %f\n", drone.roll, drone.pitch);
    Console.printf("%d, %d, %d, %d\n", drone.FL_pwm, drone.FR_pwm, drone.RL_pwm, drone.RR_pwm);
    // Console.printf("dt: %f\nx: %f\ny: %f\nz: %f\n", dt, yaw, roll, pitch);
    // Console.printf("roll: %f\n", drone.roll);

    // Console.printf("gyroZrate: %f\n", g.gyro.z);
    // Console.printf("\n\nAcceleration\n-----------\nx: %f\ny: %f\nz: %f\nGyroscope\n-----------\nx: %f\ny: %f\nz: %f\n", a.acceleration.x, a.acceleration.y, a.acceleration.z, g.gyro.x * RAD_TO_DEG, g.gyro.y * RAD_TO_DEG, g.gyro.z * RAD_TO_DEG);
        

    // The main loop must have some kind of "yield to lower priority task" event.
    // Otherwise the watchdog will get triggered.
    // If your main loop doesn't have one, just add a simple `vTaskDelay(1)`.
    // Detailed info here:
    // https://stackoverflow.com/questions/66278271/task-watchdog-got-triggered-the-tasks-did-not-reset-the-watchdog-in-time

    // vTaskDelay(1);
    // delay(200);
    // broadcast_parameters();
    delay(100);
}
