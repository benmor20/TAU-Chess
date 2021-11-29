from src.hardware.serial_protocol import Serial


class DigitalOutput:
    def __init__(self, serial: Serial, port: int, reversed: bool = False):
        self._serial = serial
        self.port = port
        self.reversed = reversed
        self._is_on = False
        self._serial.set_mode(port, 'OUTPUT')
        self.turn_off()

    @property
    def is_on(self) -> bool:
        return self._is_on

    def set(self, turn_on: bool):
        self._serial.set_value(self.port, turn_on ^ self.reversed)
        self._is_on = turn_on

    def turn_on(self):
        self.set(True)

    def turn_off(self):
        self.set(False)

    def toggle(self) -> bool:
        self.set(not self.is_on)
        return self.is_on
