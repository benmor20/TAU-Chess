from abc import ABC, abstractmethod
from enum import Enum
from serial_protocol import Serial
from typing import *


class StopMode(Enum):
    BRAKE = 2,
    RELEASE = 3,


class RunMode(Enum):
    FORWARD = 0, False
    BACKWARD = 1, False
    BRAKE = 2, True
    RELEASE = 3, True

    def __init__(self, _: int, stopped: bool):
        self.stopped = stopped

    @property
    def code(self) -> str:
        """
        :return: the code to pass to serial.
        """
        return str(self).split('.')[-1]

    def reverse(self) -> 'RunMode':
        """
        Returns the reverse of this Run Mode, or this Run Mode if it
        cannot be reversed. Forwards -> Backwards and vice versa.

        :return: the current Run Mode, reversed
        """
        if self == RunMode.FORWARD:
            return RunMode.BACKWARD
        elif self == RunMode.BACKWARD:
            return RunMode.FORWARD
        else:
            return self


def stop_to_run(mode: StopMode) -> RunMode:
    """
    Converts a StopMode to the corresponding RunMode

    :param mode: the StopMode to convert
    :return: the corresponding RunMode
    """
    if mode == StopMode.BRAKE:
        return RunMode.BRAKE
    elif mode == StopMode.RELEASE:
        return RunMode.RELEASE


class Motor:
    def __init__(self, port: int, serial: Serial, reversed: bool = False, max_output: int = 255):
        """
        :param port: the port this motor is connected to
        :param serial: the serial bridge to interface with the Arduino
        :param reversed: whether to flip the direction of this motor
        :param max_output: the maximum value to send to the motor (less than 256)
        """
        self._port = f'M{port}'
        self._serial = serial
        self._current_power = 0
        self._target_power = 0
        self._accel_lim = 0.1
        self._stop_mode = StopMode.BRAKE
        self._run_mode = RunMode.BRAKE
        self._reversed = reversed
        self._max_output = max_output

    @property
    def port(self) -> int:
        """
        :return: the port this motor is connected to
        """
        return int(self._port[-1])

    @property
    def current_power(self) -> float:
        """
        :return: the current power this motor is running at
        """
        return self._current_power

    @property
    def target_power(self) -> float:
        """
        :return: the power this motor is running at or moving towards
        """
        return self._target_power

    @property
    def max_speed(self) -> int:
        """
        :return: the maximum value this motor will send to the Arduino
        """
        return self._max_output

    @property
    def accel_lim(self) -> float:
        """
        :return: the maximum power difference between two loops
        """
        return self._accel_lim

    @property
    def reversed(self) -> bool:
        """
        :return: whether this motor is reversed
        """
        return self._reversed

    @property
    def mode(self) -> RunMode:
        """
        :return: the current mode this motor is running
        """
        return self._run_mode

    @property
    def stop_mode(self) -> StopMode:
        """
        :return: the mode this motor switches to when it stops
        """
        return self._stop_mode

    @property
    def accelerating(self) -> bool:
        """
        :return: whether this motor has not yet reached its target power
        """
        return self.current_power != self.target_power

    @property
    def stopped(self) -> bool:
        """
        :return: whether this motor is currently not in motion
        """
        return self.mode.stopped

    def set_power(self, power: float):
        """
        Sets the motor to the given power

        :param power: the power to set, between -1 and 1
        """
        self._target_power = power

    def set_accel_limit(self, accel_limit: float):
        """
        Sets the acceleration limit, how much this motor can change its power each loop

        :param accel_limit: the accel limit to set
        """
        self._accel_lim = accel_limit

    def set_stop_mode(self, stop_mode: StopMode):
        """
        Sets the mode this motor will switch to when it stops

        :param stop_mode: the target stop mode
        """
        self._stop_mode = stop_mode

    def _set_run_mode(self, mode: RunMode):
        """
        Sets this motor to the given RunMode

        :param mode: the RunMode to set this motor to
        :return:
        """
        if self.reversed:
            mode = mode.reverse()
        self._serial.set_mode(self._port, mode.code)
        self._run_mode = mode

    def stop(self):
        """
        Stops the motor, respecting the accel limit
        """
        self.set_power(0)

    def hard_stop(self):
        """
        Stops this motor, ignoring the accel limit
        """
        self.stop()
        self._current_power = 0
        self._serial.set_value(self._port, 0)
        self._set_run_mode(stop_to_run(self.stop_mode))

    def update(self):
        """
        Updates the motor, incrementing the current power towards the target
        while respecting the accel limit. Also deals with transitions between
        run modes. To be called once each loop per motor.
        """
        if not self.accelerating:
            if self.target_power == 0:
                self._set_run_mode(stop_to_run(self.stop_mode))
            return
        if self.current_power < self.target_power:
            new_pow = min(self.current_power + self.accel_lim, self.target_power)
        else:
            new_pow = max(self.current_power - self.accel_lim, self.target_power)
        if new_pow * self.current_power < 0 or self.stopped:
            self._set_run_mode(RunMode.FORWARD if new_pow > 0 else RunMode.BACKWARD)
        self._serial.set_value(self._port, int(abs(new_pow) * self._max_output))
        self._current_power = new_pow


class DriveSystem:
    def __init__(self, motors: Dict[str, Motor]):
        """
        :param motors: a dict mapping motor names to motors
        """
        self._motor_powers = {m: 0 for m in motors}
        self._motors = motors
        self._last_direction = (0, 0, 0)
        self.stop()

    @property
    def motor_speeds(self) -> Dict[str, int]:
        """
        :return: a dict mapping motor names to the current target speed
            each motor is trying to reach, on a -255 to 255 scale
        """
        res = {}
        for name, power in self._motor_powers.items():
            res[name] = power * self._motors[name].max_speed
        return res

    @property
    def accel_limit(self) -> float:
        """
        :return: the current acceleration limit of the motors
        """
        for motor in self._motors.values():
            return motor.accel_lim

    @property
    def stop_mode(self) -> StopMode:
        """
        :return: the current stop mode of the motors
        """
        for motor in self._motors.values():
            return motor.stop_mode

    @property
    def direction(self) -> Tuple[int, int, int]:
        """
        :return: the last direction this robot was set to
        """
        return self._last_direction

    def set_accel_limit(self, accel_limit: float):
        """
        Sets the acceleration limit of each motor in this drive system

        :param accel_limit: the accel limit to set
        """
        for motor in self._motors.values():
            motor.set_accel_limit(accel_limit)

    def set_stop_mode(self, stop_mode: StopMode):
        """
        Sets the stop mode of each motor in this drive system

        :param stop_mode: the stop mode to set
        """
        for motor in self._motors.values():
            motor.set_stop_mode(stop_mode)

    def move(self, direction: Tuple[float, float, float]):
        """
        Moves the robot in the given direction

        :param direction: the direction to move the robot. A three-element
            tuple. The first element is the horizontal speed from -1 (left)
            to 1 (right). The second is vertical speed from -1 (backwards)
            to 1 (forwards). The last is the angular speed from -1 (left) to
            1 (right).
        """
        x, y, s = direction
        if abs(x) > 1 or abs(y) > 1 or abs(s) > 1:
            raise ValueError(f'No direction can have a magnitude more than 1: {direction}')
        self._calculate_powers(direction)
        self._constrain_powers()
        self._commit_powers()
        self._last_direction = direction

    def _constrain_powers(self):
        """
        Resets motor powers so they are all between -1 and 1
        """
        for motor, power in self._motor_powers.items():
            if abs(power) > 1:
                self._motor_powers[motor] /= abs(power)

    def _commit_powers(self):
        """
        Sends the current powers to the motors
        """
        for motor, power in self._motor_powers.items():
            self._motors[motor].set_power(power)

    @abstractmethod
    def _calculate_powers(self, direction: Tuple[float, float, float]):
        """
        Given a certain direction, calculates the powers for each motor,
        saving them in `_motor_powers`

        :param direction: the target direction to move (see `move`)
        """
        pass

    def stop(self):
        """
        Stops the robot, respecting accel limit
        """
        self.move((0, 0, 0))

    def hard_stop(self):
        """
        Stops the robot instantly, regardless of accel limit
        """
        for motor in self._motors.values():
            motor.hard_stop()

    def update(self):
        """
        Updates each motor. To be called once per loop.
        """
        for motor in self._motors.values():
            motor.update()
