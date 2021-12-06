from hardware.serial_protocol import Serial
from hardware.drive_system import Stepper
from time import sleep


if __name__ == '__main__':
    s1 = Serial('COM13')
    s1.wait_for_setup()
    s2 = Serial('COM14')
    s2.wait_for_setup()
    step = Stepper(s2, 0)

    s1.set_value(13, True)
    step.move_to_pos(1000)
    step.update()
    sleep(2)

    while True:
        s1.set_value(13, False)
        sleep(0.5)
        s1.set_value(13, True)
        sleep(0.5)
