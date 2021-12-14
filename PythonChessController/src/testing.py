from hardware.serial_protocol import Serial
from hardware.drive_system import *
from hardware.devices import *
from chess_engine import move_from_uci
from time import sleep


if __name__ == '__main__':
    # stepper = Stepper(Serial(), 1)
    # stepper.move_to_pos(50)
    # stepper.update()

    drive = ChessDrive(Serial())
    drive.move((0, 3))
    drive.update()

    # magnet = DigitalOutput(Serial(), 8)
    # magnet.turn_on()
    # input("Press enter to turn off the magnet")
    # magnet.turn_off()

    # matrix = DigitalInputMatrix(Serial(), 'A0')
    # while True:
    #     for row in matrix.value:
    #         print(''.join('1' if v else '0' for v in row))
    #     print()
    #     matrix.reset()
    #     sleep(0.5)

    move_sensor = MoveSensor(Serial(), 'A0')
    while True:
        if move_sensor.pick_up is None:
            print('No pick up')
        else:
            move = move_sensor.value
            if move is None:
                print(f'Picked up {move_sensor.pick_up}')
            else:
                print(f'Move is {move_sensor.value}')
        move_sensor.reset()
        sleep(0.2)
