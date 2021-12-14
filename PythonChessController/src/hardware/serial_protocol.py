import serial
import serial.tools.list_ports as list_ports
from typing import *


ARDUINO_IDS = ((0x2341, 0x0043), (0x2341, 0x0001),
               (0x2A03, 0x0043), (0x2341, 0x0243),
               (0x0403, 0x6001), (0x1A86, 0x7523))
MODES = {'INPUT': 'I',
         'OUTPUT': 'O',
         'BRAKE': 'S',
         'RELEASE': 'R',
         'FORWARD': 'F',
         'BACKWARD': 'B',
         'MATRIX': 'M'}


def is_valid_port(port: Union[str, int]) -> bool:
    """
    Checks if the given input is able to be decoded into an Arduino port.
    If port is an int, returns True if it is between 0 and 13, inclusive
    If port is a str, returns True if it can be cast to an int in the above
    range, or if the first character is 'A' followed by a number 0-5, or if
    the first character is 'M' followed by a number 0-3, or if the first
    character is 'S' followed by a number 0-2
    These match the possible Arduino pins with a motor shield, including
    DC Motors (M) vs stepper motors (S)

    :param port: a str or int, value to check if it is a port

    :returns: True if port represents an Arduino port, or False otherwise.
    """
    if isinstance(port, str):
        if len(port) != 2:
            return False
        if port[0] == 'A':
            return port[1] in '012345'
        elif port[0] == 'M':
            return port[1] in '0123'
        elif port[0] == 'S':
            return port[1] in '012'
        try:
            int_port = int(port)
            return 0 <= int_port <= 13
        except ValueError:
            return False
    elif isinstance(port, int):
        return 0 <= port <= 13


def is_valid_value(value: Union[bool, int]) -> bool:
    """
    Checks if the given value is valid to send to the arduino.
    Returns True if value is a boolean, or if value is an int between 0 and 255 inclusive

    :param value: a bool or int, the value to check

    :returns: True if the value is valid to send to the Arduino
    """
    return isinstance(value, bool) or (isinstance(value, int) and 0 <= value < 256)


def is_valid_speed(speed: int) -> bool:
    """
    Checks if the given speed is valid to send to the arduino.
    Returns True if speed is an int between 0 and 100, inclusive

    :param speed: an int, the speed to check

    :returns: True if the speed is valid to send to the Arduino
    """
    return isinstance(speed, int) and 0 < speed < 256


def check_inputs(port: Union[str, int], value: Optional[Union[str, int]] = None,
                 mode: Optional[Union[str, int]] = None):
    """
    Checks if the given inputs are able to be sent to the Arduino, raising an error otherwise.

    :param port: the port to check
    :param value: the value to check, defaulting to None (meaning pass)
    :param mode: the mode to check, defaulting to None (meaning pass)
    """
    if not is_valid_port(port):
        raise ValueError(f'Unknown port: {port}')
    is_stepper = isinstance(port, str) and port[0] == 'S'
    if value is not None and not is_stepper and not is_valid_value(value):
        raise ValueError(f'Unacceptable value for {port}: {value}')
    if isinstance(mode, str) and mode is not None and mode not in MODES and mode not in MODES.values():
        raise ValueError(f'Unknown mode: {mode}')
    if isinstance(mode, str) and is_stepper:
        raise ValueError(f'Cannot set stepper mode to {mode}')
    if isinstance(mode, int) and not is_stepper:
        raise ValueError(f'Cannot set mode to be an int ({mode}) with a non-stepper port ({port})')
    if isinstance(mode, int) and mode <= 0:
        raise ValueError(f'Cannot set stepper speed to less than 0 ({mode})')


def format_port(port: Union[str, int]) -> str:
    """
    Properly formats the given port to send to the Arduino.
    Does nothing besides cast to string, except if port is an int less than 10,
    in which case it appends a 0 to the start.

    :param port: int or str, the port to format
    :returns: the formatted port
    """
    if isinstance(port, str):
        return port
    zero = ''
    if isinstance(port, int) and port < 10:
        zero = '0'  # Forces all ports to be two characters
    return f'{zero}{port}'


def hex_str(value: int):
    """
    :param value: a value to convert to hex
    :return: a string representation of the value in hexadecimal
    """
    neg = value < 0
    return ('-' if neg else '') + hex(value)[(3 if neg else 2):]


def format_value(value: Union[bool, int]) -> str:
    """
    Formats the given value to give to the Arduino.
    If value is an int, returns a string of the hex code.
    If value is a bool, returns 'H' if value else 'L'

    :param value: the value to format
    :return: the formatted value
    """
    if isinstance(value, bool):
        return 'H' if value else 'L'
    elif isinstance(value, int):
        return hex_str(value)


def format_mode(mode: Union[str, int]) -> str:
    """
    Formats the mode to send to the Arduino
    If mode is a string, returns a single-character representation of it, based on `MODES`
    If mode is an int, returns a hex representation of it

    :param mode: the mode to format
    :returns: a string representation of the mode to send to Arduino
    """
    if isinstance(mode, str):
        return MODES[mode] if mode in MODES else mode
    elif isinstance(mode, int):
        return hex_str(mode)


class Serial:
    def __init__(self, port=None):
        """
        :param port: the port to read/write to, or None to find one
        """
        if port is None:
            self.bridge = None
            self.connected = False
            for device in list_ports.comports():
                if (device.vid, device.pid) in ARDUINO_IDS:
                    print(f'Found {(device.vid, device.pid)} - device {device.device}')
                    try:
                        self.bridge = serial.Serial(device.device, 115200)
                        self.connected = True
                        print(f'Connected to {device.device}...')
                        break
                    except BaseException as err:
                        print(err)
                        pass
        else:
            try:
                self.bridge = serial.Serial(port, 115200)
                self.connected = True
            except BaseException as err:
                print(err)
                self.bridge = None
                self.connected = False
        self.wait_for_setup()

    def wait_for_setup(self):
        """
        Stalls until a line is given from the Arduino.
        Helpful for waiting until the Arduino has finished setup
        """
        if self.connected:
            print(self.bridge.readline().decode())

    def set_value(self, port: Union[str, int], value: Union[bool, int]):
        """
        Sets the given port to the given value.

        :param port: the port to set
        :param value: the value to set the port to
        """
        check_inputs(port, value=value)
        if self.connected:
            self._write(f'{format_port(port)}:{format_value(value)}')
            if isinstance(port, str) and port[0] == 'S':
                self._read()  # Wait for stepper to finish

    def set_mode(self, port: Union[str, int], mode: Union[str, int]):
        """
        Sets the given port to the given mode.

        :param port: the port to set
        :param mode: the mode to set the port to
        """
        check_inputs(port, mode=mode)
        if self.connected:
            self._write(f'{format_port(port)}-{format_mode(mode)}')

    def set_speed(self, port: str, speed: int):
        """
        Sets the speed of the given motor

        :param port: The port to set the speed of
        :param speed: The speed to set
        """
        if port[0] == 'M':
            self.set_value(port, speed)
        if port[0] == 'S':
            self.set_mode(port, speed)

    def get_value(self, port: Union[str, int]) -> Union[int, str]:
        """
        Gets the value from the given port

        :param port: the port to read from
        :return: the value, from 0-255, of the port
        """
        check_inputs(port)
        if self.connected:
            self._write(f'{format_port(port)}?')
            line = self._read()
            try:
                return int(line)
            except:
                return line

    def reset_steppers(self, ports: Tuple[str, str, int, int]):
        if self.connected:
            self._write(f'R{"".join(format_port(p) for p in ports)}')
            self._read()

    def _write(self, msg: str):
        """
        Writes a message to the Serial

        :param msg: the message to write to serial
        """
        self.bridge.write(f'{msg}\r'.encode())
        # print(msg)
        # if ':' in msg:
        #     print(self._read())

    def _read(self) -> str:
        """
        Waits for an output from the Arduino and returns it

        :return: the Arduino's output
        """
        line = self.bridge.readline().decode()
        # print(f'Response: {line}')
        return line
