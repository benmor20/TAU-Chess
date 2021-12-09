from hardware.serial_protocol import Serial
from hardware.drive_system import Stepper
from time import sleep


if __name__ == '__main__':
    s1 = Serial('COM6')
    s1.wait_for_setup()
    s2 = Serial('COM11')
    s2.wait_for_setup()
    step1 = Stepper(s1, 0)
    step2 = Stepper(s2, 0)

    s2.set_value(13, True)
    step2.move_to_pos(1000)
    step2.update()
    sleep(2)

    while True:
        s2.set_value(13, False)
        sleep(0.5)
        s2.set_value(13, True)
        sleep(0.5)
