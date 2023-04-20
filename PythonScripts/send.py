import socket

Kd = 0
Kp = 0
Ki = 0

address = ("", 1234)
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
message = f"{Kd} {Kp} {Ki}"
s.sendto(message.encode('utf-8'), address)

