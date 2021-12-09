"""
Main script to control the chess board
"""
from src.hardware.devices import *
from src.hardware.drive_system import ChessDrive, Stepper
from src.hardware.serial_protocol import Serial
from time import sleep


if __name__ == '__main__':
    serial = Serial()

    board = ChessDrive(serial)
    board.move((1, 1))
    board.update()
