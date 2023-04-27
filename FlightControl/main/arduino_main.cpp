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
#include "bluetooth_access.h"
#include "data.h"

// TaskHandle_t WiFiAccess;

Adafruit_MPU6050 mpu;

// wifi settings
const char* ssid = "esp32drone";
const char* password = "";
constexpr int port = 1234;

int speed = 1000;
int prev_speed = speed;
int counter = 0;

Kalman Kx;
Kalman Ky;
Kalman Kz;

// global vars to be mutexed
// Data data;

// -------------------------

// Quadcopter drone(26, 27, 14, 12, Kp, Ki, Kd);
Quadcopter drone(19, 18, 5, 17, data.Kp, data.Ki, data.Kd);
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


// void onConnectedController(ControllerPtr ctl) {
//     bool foundEmptySlot = false;
//     for (int i = 0; i < BP32_MAX_CONTROLLERS; i++) {
//         if (myControllers[i] == nullptr) {
//             Console.printf("CALLBACK: Controller is connected, index=%d\n", i);
//             // Additionally, you can get certain gamepad properties like:
//             // Model, VID, PID, BTAddr, flags, etc.
//             ControllerProperties properties = ctl->getProperties();
//             Console.printf("Controller model: %s, VID=0x%04x, PID=0x%04x\n", ctl->getModelName(), properties.vendor_id,
//                            properties.product_id);
//             myControllers[i] = ctl;
//             foundEmptySlot = true;
//             break;
//         }
//     }
//     if (!foundEmptySlot) {
//         Console.println("CALLBACK: Controller connected, but could not found empty slot");
//     }
// }

// void onDisconnectedController(ControllerPtr ctl) {
//     bool foundController = false;
//     speed = 1000;

//     for (int i = 0; i < BP32_MAX_CONTROLLERS; i++) {
//         if (myControllers[i] == ctl) {
//             Console.printf("CALLBACK: Controller is disconnected from index=%d\n", i);
//             myControllers[i] = nullptr;
//             foundController = true;
//             break;
//         }
//     }

//     if (!foundController) {
//         Console.println("CALLBACK: Controller disconnected, but not found in myControllers");
//     }
// }

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
    // Console.printf("Firmware: %s\n", BP32.firmwareVersion());
    // drone.test(1000);
    drone.test(1000);
    // Console.begin();
    
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
    
    // Setup WiFi
    WiFi.softAPdisconnect();
    WiFi.mode(WIFI_AP);
    if (!WiFi.softAP(ssid, NULL)) {
        Console.print("Waiting for WiFi Connection ...");
        while (1) {
            Console.print(".");
            delay(1000);
        }
        Console.print("\n");
    }
    Console.println("WiFi Connected.");
    delay(500);
    IPAddress Ip(192, 168, 4, 22);
    IPAddress NMask(255, 255, 255, 0);
    if (!WiFi.softAPConfig(Ip, Ip, NMask)) {
        Console.print("Configuring SoftAP ...");
        while (1) {
            Console.print(".");
            delay(1000);
        }
        Console.print("\n");
    }
    Console.print("WiFi Configured at IP: ");
    IPAddress myIP = WiFi.softAPIP();
    Console.println(myIP.toString());    
    
    
    if (udp.listen(1234)) {
        Console.print("UDP Listening on IP: ");
        Console.print(myIP.toString());
        Console.printf(" | %d", port);
        Console.println("");
        udp.onPacket(handle_packet);
    }
    else {
        Console.println("Error: UDP not connected properly");
    }
    // Console.println("");
    // Setup the Bluepad32 callbacks
    BP32.setup(&onConnectedController, &onDisconnectedController);

    // Setup MPU6050
    // while (!mpu.begin()) {
    //     Console.println("Failed to find MPU6050 chip");
    //     delay(50);
    // }
    // Console.println("MPU6050 Found!");
    // mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
    // mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    // mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
    


    // BP32.forgetBluetoothKeys();
    

    delay(500);
}

// Arduino loop function. Runs in CPU 1
void loop() {
    // WiFiClient client = server.available();
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

    // sensors_event_t a, g, temp;
    // mpu.getEvent(&a, &g, &temp);

    // // a.acceleration.x += xAccelOffset;
    // // a.acceleration.y += yAccelOffset;
    // // a.acceleration.z += zAccelOffset;

    // double roll_init = atan2(-a.acceleration.x, a.acceleration.z) * RAD_TO_DEG;
    // double pitch_init = atan2(-a.acceleration.x, a.acceleration.y) * RAD_TO_DEG;
    // // double yaw_init = atan2(a.acceleration.z, a.acceleration.y) * RAD_TO_DEG;
    // double gyroX = g.gyro.x / 131.0;
    // double gyroY = g.gyro.y / 131.0;
    // double gyroZ = g.gyro.z / 131.0;

    // drone.roll = Ky.getAngle(roll_init - 79.02, gyroY, dt);
    // drone.pitch = Kz.getAngle(pitch_init - 90.35, gyroZ / 131.0, dt);
    // drone.yaw = Kx.getAngle(yaw_init, gyroX / 131.0, dt);

    // roll = drone.roll;
    // pitch = drone.pitch;
    // // yaw = drone.yaw;

    // drone.balance();
    
    // Console.print("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
    // Console.printf("%f, %f\n", drone.roll, drone.pitch);
    // Console.printf("%d, %d, %d, %d\n", drone.FL_pwm, drone.FR_pwm, drone.RL_pwm, drone.RR_pwm);
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

    if (!controller_connected) {
        speed = 1000;
    }
    std::stringstream ss;
    // ss << "T " << drone.roll << ' ' << drone.pitch << ' ' << drone.FL_pwm << ' ' << drone.FR_pwm << ' ' << drone.RL_pwm << ' ' << drone.RR_pwm;
    ss << "T 20.2 12.01 1200 1302 1109 1312";
    udp.broadcast(ss.str().c_str());
    // Console.println("Listening...\n");
    
    delay(1000);
}


