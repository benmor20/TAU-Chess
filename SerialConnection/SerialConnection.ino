#include <Adafruit_MotorShield.h>
#include <Stepper.h>

#define BUFFER_LEN 128
#define NUM_ANALOG 6
#define NUM_MOTORS 4
#define NUM_STEPPERS 2
#define STEPS_PER_REV 200

int ANALOG_PINS[NUM_ANALOG] = {A0, A1, A2, A3, A4, A5};
Adafruit_DCMotor* MOTORS[NUM_MOTORS];
const Adafruit_MotorShield motorShield = Adafruit_MotorShield();
const int STEPPER_PINS[NUM_STEPPERS][4] = {{4, 5, 6, 7}, {8, 9, 10, 11}}
Stepper STEPPERS[NUM_STEPPERS];

char commandBuffer[BUFFER_LEN];
int bufferPos = 0;

enum PinType {
  DIGITAL,
  ANALOG,
  MOTOR,
  STEPPER
};

void writeToPin(PinType type, int pin, int value) {
  if (type == DIGITAL) {
    digitalWrite(pin, value);
  } else if (type == ANALOG) {
    analogWrite(ANALOG_PINS[pin], value);
  } else if (type == MOTOR) {
    MOTORS[pin]->setSpeed(value);
  } else if (type == STEPPER) {
    STEPPERS[pin].step(value);
  }
}

void setMode(PinType type, int pin, int mode) {
  if (type == DIGITAL) {
    pinMode(pin, mode);
  } else if (type == ANALOG) {
    pinMode(ANALOG_PINS[pin], mode);
  } else if (type == MOTOR) {
    MOTORS[pin]->run(mode);
  } else if (type == STEPPER) {
    STEPPERS[pin].setSpeed(mode);
  }
}

int readValue(PinType type, int pin) {
  int value = -1;
  if (type == DIGITAL) {
    value = analogRead(pin);
  } else if (type == ANALOG) {
    value = analogRead(ANALOG_PINS[pin]);
  }
  return value;
}

void processCommand() {
  // Find pin
  PinType type = DIGITAL;
  if (commandBuffer[0] == 'A') {
    type = ANALOG;
  }
  else if (commandBuffer[0] == 'M') {
    type = MOTOR;
  }
  else if (commandBuffer[0] == 'S') {
    type = STEPPER;
  }
  int pin = (int)(commandBuffer[1] - '0');
  if (commandBuffer[0] == '1') {
    pin += 10;
  }

  // Set value
  if (commandBuffer[2] == ':') {
    int value = 0;
    if (commandBuffer[3] == 'H') {
      value = HIGH;
    }
    else if (commandBuffer[3] == 'L') {
      value == LOW;
    }
    else {
      byte tens = commandBuffer[3] <= '9' ? (commandBuffer[3] - '0') : (commandBuffer[3] - 'a' + 10);
      byte ones = commandBuffer[4] <= '9' ? (commandBuffer[4] - '0') : (commandBuffer[4] - 'a' + 10);
      value = 16 * tens + ones;
    }
    writeToPin(type, pin, value);
  }

  // Set mode
  if (commandBuffer[2] == '-') {
    if (commandBuffer[3] == 'I') {
      setMode(type, pin, INPUT);
    } else if (commandBuffer[3] == 'O') {
      setMode(type, pin, OUTPUT);
    } else if (commandBuffer[3] == 'S') {
      setMode(type, pin, BRAKE);
    } else if (commandBuffer[3] == 'R') {
      setMode(type, pin, RELEASE);
    } else if (commandBuffer[3] == 'F') {
      setMode(type, pin, FORWARD);
    } else if (commandBuffer[3] == 'B') {
      setMode(type, pin, BACKWARD);
    } else if (commandBuffer[3] >= '0' && commandBuffer[3] <= '9') {
      setMode(type, pin, (commandBuffer[3] - '0') * 10 + (commandBuffer[4] - '0'));
    }
  }

  // Read value
  if (commandBuffer[2] == '?') {
    int value = readValue(type, pin);
    Serial.println(value);
  }
}

void setup() {
  Serial.begin(115200);
  
  if (!motorShield.begin()) {
    Serial.println("Motor Shield not found. Check wiring.");
    while (true) {}
  }
  
  // Initialize motors
  for (int i = 0; i < NUM_MOTORS; i++) {
    MOTORS[i] = motorShield.getMotor(i + 1);
    MOTORS[i]->run(BRAKE);
  }

  // Initialize steppers
  for (int i = 0; i < NUM_STEPPERS; i++) {
    STEPPERS[i] = Stepper(STEPS_PER_REV, STEPPER_PINS[i][0], STEPPER_PINS[i][1], STEPPER_PINS[i][2], STEPPER_PINS[i][3]);
  }
  
  Serial.println("Setup done.");
}

void loop() {
  if (Serial.available()) {
    char c = Serial.read();

    if (c == '\r') {
      // End of command
      processCommand();
      bufferPos = 0;
    }
    else if (bufferPos == BUFFER_LEN - 1) {
      // Should not reach here, but in case command is too long, reset
      bufferPos = 0;
    }
    else {
      // Add current character to command buffer
      commandBuffer[bufferPos] = c;
      bufferPos++;
    }
  }
}
