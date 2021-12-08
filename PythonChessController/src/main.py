"""
Main script to control the chess board
"""
from src.hardware.devices import *
from src.hardware.drive_system import ChessDrive, Stepper
from src.hardware.serial_protocol import Serial
from time import sleep


if __name__ == '__main__':
    serial = Serial()
    print('Setup')

    # stepper = Stepper(serial, 1)
    # stepper.increment_pos(50)
    # stepper.set_power(1)
    # stepper.update()

    # board = ChessDrive(serial)
    # board.move((2, -2, 0))
    # board.update()

    magnet = DigitalInputMatrix(serial, 'A0')
    while True:
        matrix = magnet.value
        for row in matrix:
            for v in row:
                print(int(v), end='')
            print()
        magnet.reset()
        sleep(1)
