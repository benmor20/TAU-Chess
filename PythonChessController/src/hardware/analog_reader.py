from serial_protocol import Serial
import time


serial = Serial()
serial.wait_for_setup()
serial.set_mode('A0', 'INPUT')

while True:
    print(serial.get_value('A0'))
    time.sleep(0.05)
