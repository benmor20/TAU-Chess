from hardware.serial_protocol import Serial
from hardware.drive_system import Stepper, ChessDrive
from time import sleep


if __name__ == '__main__':
    serial = Serial()
    drive = ChessDrive(serial)
    drive.move((-1, 1))
    drive.update()
