"""
Main script to control the chess board
"""
from src.hardware.drive_system import Stepper
from src.hardware.serial_protocol import Serial


if __name__ == '__main__':
    serial = Serial()
    serial.wait_for_setup()
    print('Setup done')
    stepper = Stepper(serial, 0)
    stepper.set_accel_limit(100)
    stepper.set_power(1)
    stepper.increment_pos(10000)
    stepper.update()
    while True:
        print(f'From serial: {serial.bridge.readline().decode()}')
