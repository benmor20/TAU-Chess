#include <SpeedyStepper.h>

#define BUFFER_LEN 128
#define NUM_STEPPERS 3

SpeedyStepper STEPPERS[NUM_STEPPERS];
const int STEPPER_PINS[NUM_STEPPERS][2] = {{2, 5}, {3, 6}, {4, 7}};

char commandBuffer[BUFFER_LEN];
int bufferPos = 0;

void setMode(boolean isStepper, int pin, int mode) {
  if (isStepper) {
    STEPPERS[pin].setSpeedInMillimetersPerSecond(mode);
  }
  else {
    pinMode(pin, mode);
  }
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
  // Find pin
  boolean isStepper = commandBuffer[0] == 'S';
  int pin = parseNum(isStepper ? 1 : 0, 10);

  // Set value
  if (commandBuffer[2] == ':') {
    if (commandBuffer[3] == 'H') {
      digitalWrite(pin, HIGH);
      Serial.print("Turning on ");
      Serial.println(pin);
    }
    else if (commandBuffer[3] == 'L') {
      digitalWrite(pin, LOW);
      Serial.print("Turning off ");
      Serial.println(pin);
    }
    else {
      int sign = commandBuffer[3] == '-' ? -1 : 1;
      int value = parseNum(sign == 1 ? 3 : 4, 16) * sign;
      STEPPERS[pin].moveRelativeInMillimeters(value);
    }
  }

  // Set mode
  if (commandBuffer[2] == '-') {
    setMode(isStepper, pin, parseNum(3, 10));
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
