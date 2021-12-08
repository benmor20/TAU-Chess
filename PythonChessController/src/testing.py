from hardware.serial_protocol import Serial
from hardware.devices import MoveSensor
from hardware.drive_system import ChessDrive, Stepper
from time import sleep


if __name__ == '__main__':
    serial = Serial()
    move_sensor = MoveSensor(serial, 'A0')
    while True:
        print(f'Pickup: {move_sensor.pick_up}, val: {move_sensor.value}, sensed: {move_sensor._matrix.value}')
        move_sensor.reset()
        sleep(0.5)

    # drive = ChessDrive(serial)
    # drive.move((-1, 1))
    # drive.update()
