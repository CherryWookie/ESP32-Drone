#include <WiFi.h>
#include <AsyncUDP.h>
#include <mutex>
#include <sstream>

struct Data {
  // lock for protecting parameters from concurrent read/writes
  std::mutex mutex;

  // PID parameters
  double Kd{ 1.34 };
  double Kp{ 5.67 };
  double Ki{ 0.02 };

  // add other parameters here and edit broadcast/set_parameters functions
};

Data data;

AsyncUDP udp;

void set_parameters(const std::string& message) {
  std::stringstream ss{ message };

  double Kd, Kp, Ki;
  if (ss >> Kd >> Kp >> Ki) {
    const std::lock_guard<std::mutex> lock{ data.mutex };
    data.Kd = Kd;
    data.Kp = Kp;
    data.Ki = Ki;
  } else {
    Serial.println("Failed to parse parameters");
  }
}

void broadcast_parameters() {
  double Kd, Kp, Ki;
  {
    const std::lock_guard<std::mutex> lock{ data.mutex };
    Kd = data.Kd;
    Kp = data.Kp;
    Ki = data.Ki;
  }

  // construct nicely-formatted message to broadcast
  std::stringstream ss;
//   ss << "T " << roll << ' ' << pitch << ' ' << FL_pulse << ' ' << FR_pulse << ' ' << RL_pulse << ' ' << RR_pulse;
  udp.broadcast(ss.str().c_str());
}

void handle_packet(AsyncUDPPacket packet) {
  // convert uint8_t array to string
  std::string message{ reinterpret_cast<char*>(packet.data()), packet.length() };

  // print what we received
  Serial.print("Recv from ");
  Serial.print(packet.remoteIP());
  Serial.print(": ");
  Serial.println(message.data());

  // update drone parameters
  set_parameters(message);
}