"""
Main script to control the chess board
"""
from src.hardware.devices import DigitalOutput
from src.hardware.drive_system import ChessDrive, Stepper
from src.hardware.serial_protocol import Serial
from time import sleep


if __name__ == '__main__':
    serial = Serial()
    serial.wait_for_setup()

    # stepper = Stepper(serial, 1)
    # stepper.increment_pos(50)
    # stepper.set_power(1)
    # stepper.update()

    # board = ChessDrive(serial)
    # board.move((2, -2, 0))
    # board.update()

    magnet = DigitalOutput(serial, 4)
    loops = 10
    for i in range(loops):
        print(f'Turning on ({i + 1}/{loops})')
        magnet.turn_on()
        sleep(2)

        print(f'Turning off ({i + 1}/{loops})')
        magnet.turn_off()
        sleep(2)
