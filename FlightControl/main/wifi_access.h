#include <WiFi.h>
#include <AsyncUDP.h>
#include <Bluepad32.h>
#include <mutex>
#include <sstream>
#include "data.h"


Data data;

// WiFiServer server(80);

AsyncUDP udp;

void set_parameters(const std::string& message) {
  // std::stringstream ss{ message };

  // double Kd, Kp, Ki;
  // if (ss >> Kd >> Kp >> Ki) {
  //   const std::lock_guard<std::mutex> lock{ data.mutex };
  //   data.Kd = Kd;
  //   data.Kp = Kp;
  //   data.Ki = Ki;
  // } else {
  //   Serial.println("Failed to parse parameters");
  // }
}

void broadcast_parameters() {
  double Kd, Kp, Ki;
  {
    // const std::lock_guard<std::mutex> lock{ data.mutex };
    Kd = data.Kd;
    Kp = data.Kp;
    Ki = data.Ki;
  }

  // construct nicely-formatted message to broadcast
  std::stringstream ss;
  // ss << "T " << roll << ' ' << pitch << ' ' << FL_pulse << ' ' << FR_pulse << ' ' << RL_pulse << ' ' << RR_pulse;
  udp.broadcast(ss.str().c_str());
}

void handle_packet(AsyncUDPPacket packet) {
  // convert uint8_t array to string
  Console.println("Receiving packet...");
  std::string message{ reinterpret_cast<char*>(packet.data()), packet.length() };

  // print what we received
  // Console.print("Recv from ");
  // Serial.print(packet.remoteIP());
  Console.print("Receiving: ");
  Console.println(message.data());

  // update drone parameters
  set_parameters(message);
}