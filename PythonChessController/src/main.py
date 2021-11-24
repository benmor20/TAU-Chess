"""
Main script to control the chess board
"""
from src.hardware.drive_system import ChessDrive, Stepper
from src.hardware.serial_protocol import Serial


if __name__ == '__main__':
    serial = Serial()
    serial.wait_for_setup()
    # stepper = Stepper(serial, 1)
    # stepper.increment_pos(50)
    # stepper.set_power(1)
    # stepper.update()
    board = ChessDrive(serial)
    board.move((2, -2, 0))
    board.update()
