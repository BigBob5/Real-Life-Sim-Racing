import sys
import pygame

pygame.init()
pygame.joystick.init()

if pygame.joystick.get_count() == 0:
    print("Nothing detected")
    sys.exit()

wheel = pygame.joystick.Joystick(0)
wheel.init()
print(f"connected to: {wheel.get_name()}")

num_axes = wheel.get_numaxes()

try:
    while True:
        pygame.event.pump()

        output_parts = []
        for i in range(num_axes):
            val = round(wheel.get_axis(i), 2)
            output_parts.append(f"Axis {i}: {val}")

        print(" | ".join(output_parts))

        pygame.time.wait(50)

except KeyboardInterrupt:
    print("\nClosing.")
    pygame.quit()
