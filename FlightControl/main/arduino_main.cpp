#include "sdkconfig.h"
#ifndef CONFIG_BLUEPAD32_PLATFORM_ARDUINO
#error "Must only be compiled when using Bluepad32 Arduino platform"
#endif  // !CONFIG_BLUEPAD32_PLATFORM_ARDUINO

#include <Arduino.h>
#include <Bluepad32.h>
#include <MBL_Quadcopter.h>

ControllerPtr myControllers[BP32_MAX_CONTROLLERS];

Quadcopter drone(26, 27, 14, 12, 0);
//FL M4 Pin26
//FR M2 Pin27
//RL M3 Pin14
//RR M1 Pin12
int speed = 1000;

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
    // There are different ways to query whether a button is pressed.
    // By query each button individually:
    //  a(), b(), x(), y(), l1(), etc...

    if (gamepad->a()) {
        // gamepad->setPlayerLEDs(0x07);//3
        // drone.calibrate(2);
        // drone.calibrate(1);
        speed = 1000;
        // drone.test(1000);
    }

    if (gamepad->b()) {
        // gamepad->setPlayerLEDs(0x03);//2
        // drone.calibrate(1);
        // drone.calibrate(2);
    }

    if (gamepad->x()) {
        // gamepad->setPlayerLEDs(0x01); // 1
        // Duration: 255 is ~2 seconds
        // force: intensity
        // Some gamepads like DS3, DS4, DualSense, Switch, Xbox One S support
        // rumble.
        // It is possible to set it by calling:
        // gamepad->setRumble(0xc0 /* force */, 0x10 /* duration */);
        // delay(10);
        // drone.calibrate(0);
    }
    if (gamepad->y()) {
        // gamepad->setPlayerLEDs(0xf);//4
        // drone.test(2000);
        speed = 2000;
        // drone.calibrate(0);
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
        
        // drone.test(speed);
    }
    
    

    // Another way to query the buttons, is by calling buttons(), or
    // miscButtons() which return a bitmask.
    // Some gamepads also have DPAD, axis and more.
    // Console.printf(
    //     "idx=%d, dpad: 0x%02x, buttons: 0x%04x, axis L: %4d, %4d, axis R: %4d, "
    //     "%4d, brake: %4d, throttle: %4d, misc: 0x%02x, battery=%d\n",
    //     gamepad->index(),        // Controller Index
    //     gamepad->dpad(),         // DPAD
    //     gamepad->buttons(),      // bitmask of pressed buttons
    //     gamepad->axisX(),        // (-511 - 512) left X Axis
    //     gamepad->axisY(),        // (-511 - 512) left Y axis
    //     gamepad->axisRX(),       // (-511 - 512) right X axis
    //     gamepad->axisRY(),       // (-511 - 512) right Y axis
    //     gamepad->brake(),        // (0 - 1023): brake button
    //     gamepad->throttle(),     // (0 - 1023): throttle (AKA gas) button
    //     gamepad->miscButtons(),  // bitmak of pressed "misc" buttons
    //     gamepad->battery()       // 0=Unk, 1=Empty, 255=full
    // );
    // You can query the axis and other properties as well. See ArduinoController.h
    // For all the available functions.
}

// Arduino setup function. Runs in CPU 1


void setup() {
    Console.printf("Firmware: %s\n", BP32.firmwareVersion());

    // Setup the Bluepad32 callbacks
    BP32.setup(&onConnectedController, &onDisconnectedController);
    BP32.forgetBluetoothKeys();


}

// Arduino loop function. Runs in CPU 1
void loop() {
    BP32.update();

    // It is safe to always do this before using the controller API.
    // This guarantees that the controller is valid and connected.
    for (int i = 0; i < BP32_MAX_CONTROLLERS; i++) {
        ControllerPtr myController = myControllers[i];

        if (myController && myController->isConnected()) {
            if (myController->isGamepad()) {
                processGamepad(myController);
                speed = constrain(speed, 1000, 2000);
                Console.printf("PWM = %d\n", speed);
                drone.test(speed);
            }
        }
    }

    

    // The main loop must have some kind of "yield to lower priority task" event.
    // Otherwise the watchdog will get triggered.
    // If your main loop doesn't have one, just add a simple `vTaskDelay(1)`.
    // Detailed info here:
    // https://stackoverflow.com/questions/66278271/task-watchdog-got-triggered-the-tasks-did-not-reset-the-watchdog-in-time

    // vTaskDelay(1);
    delay(10);
}
