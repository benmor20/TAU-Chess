#include <Adafruit_MotorShield.h>

#define BUFFER_LEN 128
#define NUM_ANALOG 6
#define NUM_MOTORS 4
#define NUM_MATPINS 6

#define MATRIX_THRESHOLD 850

int MATPINS[NUM_MATPINS] = {2, 12, 13, 9, 10, 11};
boolean MATRIX[14 + NUM_ANALOG];
int ANALOG_PINS[NUM_ANALOG] = {A0, A1, A2, A3, A4, A5};
Adafruit_DCMotor* MOTORS[NUM_MOTORS];
const Adafruit_MotorShield motorShield = Adafruit_MotorShield();

char commandBuffer[BUFFER_LEN];
int bufferPos = 0;

enum PinType {
  DIGITAL,
  ANALOG,
  MOTOR
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
  else if (commandBuffer[startIndex] == 'M') {
    type = MOTOR;
  }
  int pin = (int)(commandBuffer[startIndex + 1] - '0');
  if (commandBuffer[startIndex] == '1') {
    pin += 10;
  }
  return { type, pin };
}

void writeToPin(Pin pin, int value) {
  if (pin.type == DIGITAL) {
    digitalWrite(pin.num, value);
  } else if (pin.type == ANALOG) {
    analogWrite(ANALOG_PINS[pin.num], value);
  } else if (pin.type == MOTOR) {
    MOTORS[pin.num]->setSpeed(value);
  }
//  Serial.print("Writing to pin ");
//  Serial.print(pin.type);
//  Serial.print(pin.num);
//  Serial.print(" value ");
//  Serial.println(value);
}

void setMode(Pin pin, int mode) {
  if (pin.type == DIGITAL) {
    pinMode(pin.num, mode);
  } else if (pin.type == ANALOG) {
    pinMode(ANALOG_PINS[pin.num], mode);
  } else if (pin.type == MOTOR) {
    MOTORS[pin.num]->run(mode);
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

void printMatrixRecursive(Pin pin, int index) {
  if (index == NUM_MATPINS) {
    if (pin.type == DIGITAL) {
      Serial.print(digitalRead(pin.num));
    } else {
      int value = analogRead(ANALOG_PINS[pin.num]);
      boolean magnet = MATRIX_THRESHOLD > value;
      Serial.print(magnet);
    }
    delay(1);
  } else {
    digitalWrite(MATPINS[index], HIGH);
    printMatrixRecursive(pin, index + 1);
    digitalWrite(MATPINS[index], LOW);
    printMatrixRecursive(pin, index + 1);
    if (index == NUM_MATPINS / 2) {
      Serial.print(';');
    }
  }
}

void printMatrix(Pin pin) {
  printMatrixRecursive(pin, 0);
  Serial.println();
}

int parseNum(int startPos, int base) {
  int value = 0;
  for (int pos = startPos; pos < BUFFER_LEN && commandBuffer[pos] != '\r'; pos++) {
    value *= base;
    value += commandBuffer[pos] <= '9' ? (commandBuffer[pos] - '0') : (commandBuffer[pos] - 'a' + 10);
  }
  return value;
}

void processCommand() {
  Pin pin = getPin(0);

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
      int sign = commandBuffer[3] == '-' ? -1 : 1;
      value = parseNum(sign == 1 ? 3 : 4, 16) * sign;
    }
    writeToPin(pin, value);
  }

  // Set mode
  if (commandBuffer[2] == '-') {
    if (commandBuffer[3] == 'I') {
      setMode(pin, INPUT);
      if (pin.type == ANALOG || pin.type == DIGITAL) {
        MATRIX[pin.num + (pin.type == ANALOG ? 14 : 0)] = false;
      }
    } else if (commandBuffer[3] == 'O') {
      setMode(pin, OUTPUT);
      if (pin.type == ANALOG || pin.type == DIGITAL) {
        MATRIX[pin.num + (pin.type == ANALOG ? 14 : 0)] = false;
      }
    } else if (commandBuffer[3] == 'S') {
      setMode(pin, BRAKE);
    } else if (commandBuffer[3] == 'R') {
      setMode(pin, RELEASE);
    } else if (commandBuffer[3] == 'F') {
      setMode(pin, FORWARD);
    } else if (commandBuffer[3] == 'B') {
      setMode(pin, BACKWARD);
    } else if (commandBuffer[3] == 'M') {
      setMode(pin, INPUT);
      MATRIX[pin.num + (pin.type == ANALOG ? 14 : 0)] = true;
    } else if (commandBuffer[3] >= '0' && commandBuffer[3] <= '9') {
      setMode(pin, parseNum(3, 10));
    }
  }

  // Read value
  if (commandBuffer[2] == '?') {
    if ((pin.type == ANALOG || pin.type == DIGITAL) && MATRIX[pin.num + (pin.type == ANALOG ? 14 : 0)]) {
      printMatrix(pin);
    }
    else {
      int value = readValue(pin);
      Serial.println(value);
    }
  }
}

void setup() {
  Serial.begin(115200);
  
//  if (!motorShield.begin()) {
//    Serial.println("Motor Shield not found. Check wiring.");
//    while (true) {}
//  }
//  
//  // Initialize motors
//  for (int i = 0; i < NUM_MOTORS; i++) {
//    MOTORS[i] = motorShield.getMotor(i + 1);
//    MOTORS[i]->run(BRAKE);
//  }

  // Setup matrix
  for (int i = 0; i < NUM_MATPINS; i++) {
    pinMode(MATPINS[i], OUTPUT);
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
