#include <SpeedyStepper.h>

#define BUFFER_LEN 128
#define NUM_STEPPERS 3

SpeedyStepper STEPPERS[NUM_STEPPERS];
const int STEPPER_PINS[NUM_STEPPERS][2] = {{2, 5}, {3, 6}, {4, 7}};

char commandBuffer[BUFFER_LEN];
int bufferPos = 0;

void writeToPin(int pin, int value) {
  STEPPERS[pin].moveRelativeInMillimeters(value);
}

void setMode(int pin, int mode) {
  STEPPERS[pin].setSpeedInMillimetersPerSecond(mode);
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
  int pin = (int)(commandBuffer[1] - '0');
  if (commandBuffer[0] == '1') {
    pin += 10;
  }

  // Set value
  if (commandBuffer[2] == ':') {
    int sign = commandBuffer[3] == '-' ? -1 : 1;
    int value = parseNum(sign == 1 ? 3 : 4, 16) * sign;
    writeToPin(pin, value);
  }

  // Set mode
  if (commandBuffer[2] == '-') {
    setMode(pin, parseNum(3, 10));
  }
}

void setup() {
  Serial.begin(115200);
  
  // Initialize steppers
  for (int i = 0; i < NUM_STEPPERS; i++) {
    STEPPERS[i].connectToPins(STEPPER_PINS[i][0], STEPPER_PINS[i][1]);
    STEPPERS[i].setAccelerationInStepsPerSecondPerSecond(100000);
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
