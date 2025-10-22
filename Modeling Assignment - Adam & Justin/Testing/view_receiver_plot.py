import serial
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from collections import deque
import time


PORT = '/dev/cu.wchusbserial552E0066031'
BAUD = 9600
WINDOW_SIZE = 300
REFRESH_INTERVAL_MS = 10

ser = serial.Serial(PORT, BAUD, timeout=1)
time.sleep(2)

data = deque([0]*WINDOW_SIZE, maxlen=WINDOW_SIZE)
x = list(range(WINDOW_SIZE))

fig, ax = plt.subplots()
line, = ax.plot(x, data, lw=1.5)
ax.set_ylim(-0.2, 1.2)
ax.set_xlim(0, WINDOW_SIZE)
ax.set_xlabel("Sample index")
ax.set_ylabel("IR state")
ax.set_title("IR Receiver Live Feed (Fixed Axis)")

paused = False

def on_key(event):
    global paused
    if event.key == ' ':
        paused = not paused
        print("Paused" if paused else "Resumed")

fig.canvas.mpl_connect('key_press_event', on_key)

def update(frame):
    global paused
    if paused:
        return line,
    try:
        line_raw = ser.readline().decode('utf-8').strip()
        if line_raw.isdigit():
            data.append(int(line_raw))
    except:
        pass
    line.set_ydata(data)
    return line,

ani = FuncAnimation(fig, update, interval=REFRESH_INTERVAL_MS, blit=True)
plt.show()