from hardware.serial_protocol import Serial
from hardware.drive_system import Stepper
from time import sleep


if __name__ == '__main__':
    serial = Serial()
    stepper = Stepper(serial, 1)
    stepper = Stepper(serial, 1)
    stepper.set_power(.25)
    stepper.move_to_pos(50)
    stepper.update()
