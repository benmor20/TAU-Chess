#include <SpeedyStepper.h>

#define BUFFER_LEN 128
#define NUM_STEPPERS 3
#define MAX_DISTANCE 350
#define NUM_ANALOG 6

int ANALOG_PINS[NUM_ANALOG] = {A0, A1, A2, A3, A4, A5};

SpeedyStepper STEPPERS[NUM_STEPPERS];
const int STEPPER_PINS[NUM_STEPPERS][2] = {{2, 5}, {3, 6}, {4, 7}};

char commandBuffer[BUFFER_LEN];
int bufferPos = 0;


enum PinType {
  DIGITAL,
  ANALOG,
  STEPPER
};

struct Pin {
  PinType type;
  int num;
};

Pin getPin(int startIndex) {
  PinType type = DIGITAL;
  if (commandBuffer[startIndex] == 'A') {
    type = ANALOG;
  }
  else if (commandBuffer[startIndex] == 'S') {
    type = STEPPER;
  }
  int pin = (int)(commandBuffer[startIndex + 1] - '0');
  if (commandBuffer[startIndex] == '1') {
    pin += 10;
  }
  return { type, pin };
}

void resetSteppers() {
  Pin xMotor = getPin(1);
  Pin yMotor = getPin(3);
  Pin xLimit = getPin(5);
  Pin yLimit = getPin(7);

  STEPPERS[xMotor.num].setupRelativeMoveInMillimeters(-350);
  STEPPERS[yMotor.num].setupRelativeMoveInMillimeters(-350);

//  Serial.println("D");
//  Serial.print(xMotor.type);
//  Serial.println(xMotor.num);
//  Serial.print(yMotor.type);
//  Serial.println(yMotor.num);
//  Serial.print(xLimit.type);
//  Serial.println(xLimit.num);
//  Serial.print(yLimit.type);
//  Serial.println(yLimit.num);

  boolean foundX = false;
  boolean foundY = false;
  boolean stoppedX = false;
  boolean stoppedY = false;

  while (true) {
    if (!foundX) {
      STEPPERS[xMotor.num].processMovement();
      foundX = !digitalRead(xLimit.num);
      if (foundX) {
        STEPPERS[xMotor.num].setupStop();
      }
    } else if (!stoppedX) {
      stoppedX = STEPPERS[xMotor.num].processMovement();
    }

    if (!foundY) {
      STEPPERS[yMotor.num].processMovement();
      foundY = !digitalRead(yLimit.num);
      if (foundY) {
        STEPPERS[yMotor.num].setupStop();
      }
    } else if (!stoppedY) {
      stoppedY = STEPPERS[yMotor.num].processMovement();
    }

    if (stoppedX && stoppedY) {
      return;
    }
  }
}

void setMode(Pin pin, int mode) {
  if (pin.type == STEPPER) {
    STEPPERS[pin.num].setSpeedInMillimetersPerSecond(mode);
  }
  else {
    pinMode(pin.num, mode);
  }
}

int readValue(Pin pin) {
  int value = -1;
  if (pin.type == DIGITAL) {
    value = digitalRead(pin.num);
  } else if (pin.type == ANALOG) {
    value = analogRead(ANALOG_PINS[pin.num]);
  }
  return value;
}

int parseNum(int startPos, int base) {
  int value = 0;
  for (int pos = startPos; pos < BUFFER_LEN; pos++) {
    char c = commandBuffer[pos];
    if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z'))) {
      break;
    }
    
    value *= base;
    value += c <= '9' ? (c - '0') : (c - 'a' + 10);
  }
  return value;
}

void processCommand() {
  if (commandBuffer[0] == 'R') {
    resetSteppers();
    Serial.println("D");
    return;
  }
  Pin pin = getPin(0);

  // Set value
  if (commandBuffer[2] == ':') {
    if (commandBuffer[3] == 'H') {
      digitalWrite(pin.num, HIGH);
    }
    else if (commandBuffer[3] == 'L') {
      digitalWrite(pin.num, LOW);
    }
    else {
      int sign = commandBuffer[3] == '-' ? -1 : 1;
      int value = parseNum(sign == 1 ? 3 : 4, 16) * sign;
      STEPPERS[pin.num].moveRelativeInMillimeters(value);
      Serial.println(value);
    }
  }

  // Set mode
  else if (commandBuffer[2] == '-') {
    setMode(pin, parseNum(3, 10));
  }

  // Get value
  else if (commandBuffer[2] == '?') {
    int value = readValue(pin);
    Serial.println(value);
  }
}

void setup() {
  Serial.begin(115200);
  
  // Initialize steppers
  for (int i = 0; i < NUM_STEPPERS; i++) {
    STEPPERS[i].connectToPins(STEPPER_PINS[i][0], STEPPER_PINS[i][1]);
    STEPPERS[i].setAccelerationInStepsPerSecondPerSecond(1000);
    STEPPERS[i].setStepsPerMillimeter(133);
  }

  Serial.println("Setup done.");
}

void loop() {
  if (Serial.available()) {
    char c = Serial.read();

    if (c == '\r') {
      // End of command
      commandBuffer[bufferPos] = c;
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
