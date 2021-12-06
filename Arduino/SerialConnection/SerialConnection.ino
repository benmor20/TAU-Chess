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

void writeToPin(PinType type, int pin, int value) {
  if (type == DIGITAL) {
    digitalWrite(pin, value);
  } else if (type == ANALOG) {
    analogWrite(ANALOG_PINS[pin], value);
  } else if (type == MOTOR) {
    MOTORS[pin]->setSpeed(value);
  }
//  Serial.print("Writing to pin ");
//  Serial.print(type);
//  Serial.print(pin);
//  Serial.print(" value ");
//  Serial.println(value);
}

void setMode(PinType type, int pin, int mode) {
  if (type == DIGITAL) {
    pinMode(pin, mode);
  } else if (type == ANALOG) {
    pinMode(ANALOG_PINS[pin], mode);
  } else if (type == MOTOR) {
    MOTORS[pin]->run(mode);
  }
}

int readValue(PinType type, int pin) {
  if (MATRIX[pin + (type == ANALOG ? 14 : 0)]) {
    
  } else {
    int value = -1;
    if (type == DIGITAL) {
      value = analogRead(pin);
    } else if (type == ANALOG) {
      value = analogRead(ANALOG_PINS[pin]);
    }
    return value;
  }
}

void printMatrixRecursive(PinType type, int pin, int index) {
  if (index == NUM_MATPINS) {
    if (type == DIGITAL) {
      Serial.print(digitalRead(pin));
    } else {
      int value = analogRead(ANALOG_PINS[pin]);
      boolean magnet = MATRIX_THRESHOLD > value;
      Serial.print(magnet);
    }
    delay(1);
  } else {
    digitalWrite(MATPINS[index], HIGH);
    printMatrixRecursive(type, pin, index + 1);
    digitalWrite(MATPINS[index], LOW);
    printMatrixRecursive(type, pin, index + 1);
    if (index == NUM_MATPINS / 2) {
      Serial.print(';');
    }
  }
}

void printMatrix(PinType type, int pin) {
  printMatrixRecursive(type, pin, 0);
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
  // Find pin
  PinType type = DIGITAL;
  if (commandBuffer[0] == 'A') {
    type = ANALOG;
  }
  else if (commandBuffer[0] == 'M') {
    type = MOTOR;
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
      int sign = commandBuffer[3] == '-' ? -1 : 1;
      value = parseNum(sign == 1 ? 3 : 4, 16) * sign;
    }
    writeToPin(type, pin, value);
  }

  // Set mode
  if (commandBuffer[2] == '-') {
    if (commandBuffer[3] == 'I') {
      setMode(type, pin, INPUT);
      if (type == ANALOG || type == DIGITAL) {
        MATRIX[pin + (type == ANALOG ? 14 : 0)] = false;
      }
    } else if (commandBuffer[3] == 'O') {
      setMode(type, pin, OUTPUT);
      if (type == ANALOG || type == DIGITAL) {
        MATRIX[pin + (type == ANALOG ? 14 : 0)] = false;
      }
    } else if (commandBuffer[3] == 'S') {
      setMode(type, pin, BRAKE);
    } else if (commandBuffer[3] == 'R') {
      setMode(type, pin, RELEASE);
    } else if (commandBuffer[3] == 'F') {
      setMode(type, pin, FORWARD);
    } else if (commandBuffer[3] == 'B') {
      setMode(type, pin, BACKWARD);
    } else if (commandBuffer[3] == 'M') {
      setMode(type, pin, INPUT);
      MATRIX[pin + (type == ANALOG ? 14 : 0)] = true;
    } else if (commandBuffer[3] >= '0' && commandBuffer[3] <= '9') {
      setMode(type, pin, parseNum(3, 10));
    }
  }

  // Read value
  if (commandBuffer[2] == '?') {
    if ((type == ANALOG || type == DIGITAL) && MATRIX[pin + (type == ANALOG ? 14 : 0)]) {
      printMatrix(type, pin);
    }
    else {
      int value = readValue(type, pin);
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
