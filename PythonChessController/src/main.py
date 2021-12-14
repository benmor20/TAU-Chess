"""
Main script to control the chess board
"""
from src.hardware.devices import *
from src.hardware.drive_system import ChessDrive, Stepper
from src.hardware.serial_protocol import Serial
from chess_engine import move_from_uci
from time import sleep


if __name__ == '__main__':
    drive = ChessDrive(Serial('COM8'))
    serial = Serial('COM7')
    magnet = DigitalOutput(serial, 8)
    moves = DigitalInputMatrix(serial, 'A0')
    print('Setup')
    move_from_uci(drive, magnet, 'd1e3')
