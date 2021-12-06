from abc import ABC, abstractmethod
from typing import *
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


class CachedValue(ABC):
    def __init__(self, serial: Serial):
        self._serial = serial
        self._value = None

    @property
    @abstractmethod
    def _type(self) -> TypeVar:
        pass

    @property
    def value(self) -> _type:
        if self._value is None:
            self._value = self._fetch_value()
        return self._value

    def reset(self):
        self._value = None

    @abstractmethod
    def _fetch_value(self) -> _type:
        pass


class DigitalInput(CachedValue):
    def __init__(self, serial: Serial, port: int, reversed: bool = False):
        super().__init__(serial)
        self.port = port
        self.reversed = reversed
        self._serial.set_mode(port, 'INPUT')

    @property
    def _type(self) -> TypeVar:
        return TypeVar('Digital', bool)

    def _fetch_value(self) -> Any:
        return self._serial.get_value(self.port)


class DigitalInputMatrix(CachedValue):
    def __init__(self, serial: Serial, port: Union[str, int], reversed: bool = False):
        super().__init__(serial)
        self.reversed = reversed
        self.port = port
        self._serial.set_mode(self.port, 'MATRIX')

    @property
    def _type(self) -> TypeVar:
        return TypeVar('DigitalMatrix', List[List[bool]])

    def _fetch_value(self) -> _type:
        val = self._serial.get_value(self.port)
        res = []
        for row in val.split(';')[:-1]:
            res.append([(v == '1') ^ self.reversed for v in row])
        return res
