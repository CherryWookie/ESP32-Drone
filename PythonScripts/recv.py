import socket
import tkinter as tk
from tkinter import ttk

window = tk.Tk()
window.title('MBL QTC')
window.geometry('480x240')

title_label = ttk.Label(master = window, text = 'MBL Quadcopter Telemetry Console', font = 'Menlo 24 bold')
title_label.pack()


address = ("", 1234)
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.bind(address)
while True:
    data, recv_address = s.recvfrom(1024)
    print(f"{recv_address}: {data}")
