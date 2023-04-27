import tkinter as tk
from tkinter import ttk
import socket
import threading
from threading import Lock
from PIL import Image, ImageTk

running = True
running_lock = Lock()

address = ("", 1234)

window = tk.Tk()

Kp = 4.7
Ki = 0.92
Kd = 0.05

telemetry_lock = Lock()
roll_t = 0.0
pitch_t = 0.0
FL_pulse_t = 1000
FR_pulse_t = 1000
RL_pulse_t = 1000
RR_pulse_t = 1000


roll = tk.DoubleVar()
roll.set(0.0)

pitch = tk.DoubleVar()
pitch.set(0.0)

FL_pulse = tk.IntVar()
FL_pulse.set(1000)
FR_pulse = tk.IntVar()
FR_pulse.set(1000)
RL_pulse = tk.IntVar()
RL_pulse.set(1000)
RR_pulse = tk.IntVar()
RR_pulse.set(1000)

def update_gui():
    telemetry_lock.acquire()
    roll.set(roll_t)
    pitch.set(pitch_t)
    FL_pulse.set(FL_pulse_t)
    FR_pulse.set(FR_pulse_t)
    RL_pulse.set(RL_pulse_t)
    RR_pulse.set(RR_pulse_t)
    telemetry_lock.release()


def task_receive_telemetry():
    # address2 = ("", 1234)
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.bind(address)
    run = running
    while run:
        running_lock.acquire()
        run = running
        running_lock.release()

        # data, recv_address = s.recvfrom(1024)
        recv_address = '(0.0.0.0)'
        data = 'T 1.002 3.232 1200 1320 1209 1108'
        if (data[0] == 'T') :
            # print(f"{recv_address}: {data}")
            data_list = data.split()

            telemetry_lock.acquire()
            global roll_t
            global pitch_t
            global FL_pulse_t
            global FR_pulse_t
            global RL_pulse_t
            global RR_pulse_t
            roll_t = data_list[1]
            pitch_t = data_list[2]
            FL_pulse_t = int(data_list[3])
            FR_pulse_t = int(data_list[4])
            RL_pulse_t = int(data_list[5])
            RR_pulse_t = int(data_list[6])
            telemetry_lock.release()
        
        # with telemetry_lock:
            # roll = roll + 0.01
            # pitch = pitch + 0.01


def update_constants():
    Kp = Kp_double.get()
    Ki = Ki_double.get()
    Kd = Kd_double.get()
    # address = ("", 1234)
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    message = f"P {Kp} {Ki} {Kd}"
    s.sendto(message.encode('utf-8'), address)

def send_cmd():
    cmd = cmd_str.get()
    # address3 = ("", 1234)
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    message = f"C {cmd}"
    s.sendto(message.encode('utf-8'), address)


t1 = threading.Thread(target = task_receive_telemetry)
t1.start()


window.title('MBL Quadcopter Telemetry Console')
window.geometry('600x440+240+240')
window_frame = ttk.Frame(master = window).grid(row = 0, column = 0, padx = 40)


empty_y = ttk.Frame(master = window_frame).grid(row = 0, column = 0, pady = 20)
empty_x = ttk.Frame(master = window_frame).grid(row = 1, column = 2, padx = 20)

PID_label = ttk.Label(master = window_frame, text = 'PID Constants', font = 'Lato 18 bold').grid(row = 1, column = 0, columnspan = 2, pady = 20)


Kp_label = ttk.Label(master = window_frame, text = 'Kp', font = 'Lato 18').grid(row = 2, column = 0, padx = 6, pady = 5, sticky = 'e')
Kp_double = tk.DoubleVar()
Kp_double.set(Kp)
Kp_entry = ttk.Entry(master = window_frame, textvariable = Kp_double).grid(row = 2, column = 1)

Ki_label = ttk.Label(master = window_frame, text = 'Ki', font = 'Lato 18').grid(row = 3, column = 0, padx = 6, pady = 5, sticky = 'e')
Ki_double = tk.DoubleVar()
Ki_double.set(Ki)
Ki_entry = ttk.Entry(master = window_frame, textvariable = Ki_double).grid(row = 3, column = 1)

Kd_label = ttk.Label(master = window_frame, text = 'Kd', font = 'Lato 18').grid(row = 4, column = 0, padx = 6, pady = 5, sticky = 'e')
Kd_double = tk.DoubleVar()
Kd_double.set(Kd)
Kd_entry = ttk.Entry(master = window_frame, textvariable = Kd_double).grid(row = 4, column = 1)

update_btn = ttk.Button(master = window_frame, text = 'Update', command = update_constants).grid(row = 5, column = 0, columnspan = 2, pady = 5)

cmd_label = ttk.Label(master = window_frame, text = 'Cmd', font = 'Lato 18').grid(row = 7, column = 0, padx = 6, pady = 5, sticky = 'e')
cmd_str = tk.StringVar()
cmd_entry = ttk.Entry(master = window_frame, textvariable = cmd_str).grid(row = 7, column = 1)
send_btn = ttk.Button(master = window_frame, text = 'Send', command = send_cmd).grid(row = 8, column = 0, columnspan = 2)

data_label = ttk.Label(master = window_frame, text = 'Current Telemetry', font = 'Lato 18 bold').grid(row = 1, column = 3, columnspan = 4)

roll_label = ttk.Label(master = window_frame, text = 'Roll:', font = 'Lato 18').grid(row = 2, column = 4, padx = 6, sticky = 'e')
roll_value = ttk.Label(master = window_frame, text = '- - - - - - - -', font = 'Lato 18', textvariable = roll).grid(row = 2, column = 5)

pitch_label = ttk.Label(master = window_frame, text = 'Pitch:', font = 'Lato 18').grid(row = 3, column = 4, padx = 6, sticky = 'e')
pitch_value = ttk.Label(master = window_frame, text = '- - - - - - - -', font = 'Lato 18', textvariable = pitch).grid(row = 3, column = 5)

drone_img = Image.open("drone.png")
drone_img_r = drone_img.resize((160, 160))
img = ImageTk.PhotoImage(drone_img_r)
drone_label = ttk.Label(master = window_frame, image = img).grid(row = 6, column = 4, columnspan = 2, rowspan = 6)

FL_label = ttk.Label(master = window_frame, text = '1000', font = 'Lato 12', textvariable = FL_pulse).grid(row = 6, column = 3)
FR_label = ttk.Label(master = window_frame, text = '1000', font = 'Lato 12', textvariable = FR_pulse).grid(row = 6, column = 6)
RL_label = ttk.Label(master = window_frame, text = '1000', font = 'Lato 12', textvariable = RL_pulse).grid(row = 10, column = 3)
RR_label = ttk.Label(master = window_frame, text = '1000', font = 'Lato 12', textvariable = RR_pulse).grid(row = 10, column = 6)

window.after(0, update_gui)
window.mainloop()
running_lock.acquire()
running = False
running_lock.release()
t1.join()


