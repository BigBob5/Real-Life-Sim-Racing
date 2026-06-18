import sys
import pygame
import serial
import time
import numpy as np

pygame.init()
pygame.joystick.init()

if pygame.joystick.get_count() == 0:
    print("Nothing detected")
    sys.exit()

wheel = pygame.joystick.Joystick(0)
wheel.init()
print(f"connected to: {wheel.get_name()}")

ESP32_PORT = 'COM5'
BAUD_RATE = 115200

try:
    ser = serial.Serial(ESP32_PORT, BAUD_RATE, timeout=0.1)
    time.sleep(2)
    print(f"connected to {ESP32_PORT}")
except serial.SerialException as e:
    print(f"error: could not open {ESP32_PORT}. {e}")
    pygame.quit()
    sys.exit()

try:
    clock = pygame.time.Clock()
    while True:
        pygame.event.pump()

        steering = int(np.interp(wheel.get_axis(0),[-1,1],[75,160]))
        if int(np.interp(wheel.get_axis(2), [-1, 1], [255, 0])) > 1:
            gas = int(np.interp(wheel.get_axis(2), [-1, 1], [255,0]))
            gear = 2
        elif int(np.interp(wheel.get_axis(1), [-1, 1], [255, 0])) > 1:
            gas = int(np.interp(wheel.get_axis(1), [-1, 1], [255, 0]))
            gear = 1
        else:
            gas = 0
            gear = 0

        data_packet = f"{steering},{gas},{gear}\n"

        ser.write(data_packet.encode('utf-8'))
        print(f"Sent: {data_packet.strip()}")

        clock.tick(80)

except KeyboardInterrupt:
    print("\nclosing")
finally:
    if 'ser' in locals() and ser.is_open:
        ser.close()
        print("closed safely.")
    pygame.quit()
