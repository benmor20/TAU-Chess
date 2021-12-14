#include <SpeedyStepper.h>

#define BUFFER_LEN 128
#define NUM_STEPPERS 3
#define MAX_DISTANCE 350
#define NUM_ANALOG 6

int ANALOG_PINS[NUM_ANALOG] = {A0, A1, A2, A3, A4, A5};

SpeedyStepper STEPPERS[NUM_STEPPERS];
const int STEPPER_PINS[NUM_STEPPERS][2] = {{2, 5}, {3, 6}, {4, 7}};

boolean runReset = true;

enum PinType {
  DIGITAL,
  ANALOG,
  STEPPER
};

struct Pin {
  PinType type;
  int num;
};

void setup() {
  Serial.begin(9600);
  
  // Initialize steppers
  for (int i = 0; i < NUM_STEPPERS; i++) {
    STEPPERS[i].connectToPins(STEPPER_PINS[i][0], STEPPER_PINS[i][1]);
    STEPPERS[i].setAccelerationInStepsPerSecondPerSecond(1000);
    STEPPERS[i].setStepsPerMillimeter(133);
    STEPPERS[i].setSpeedInMillimetersPerSecond(5);
  }

  runReset = true;
  
  Serial.println("Setup done.");
}

void loop() {
  if (runReset) {
    STEPPERS[2].moveRelativeInMillimeters(50);
    STEPPERS[0].moveRelativeInMillimeters(50);
    
    STEPPERS[0].setupRelativeMoveInMillimeters(-350);
    STEPPERS[2].setupRelativeMoveInMillimeters(-350);

    boolean foundX = false;
    boolean foundY = false;
    boolean stoppedX = false;
    boolean stoppedY = false;

    while (true) {
      if (!foundX) {
        STEPPERS[0].processMovement();
        foundX = !digitalRead(13);
        if (foundX) {
          STEPPERS[0].setupStop();
          Serial.println("Found x");
        }
      } else if (!stoppedX) {
        stoppedX = STEPPERS[0].processMovement();
        if (stoppedX) {
          Serial.println("Stopped x");
        }
      }

      if (!foundY) {
        STEPPERS[2].processMovement();
        foundY = !digitalRead(12);
        if (foundY) {
          STEPPERS[2].setupStop();
          Serial.println("Found y");
        }
      } else if (!stoppedY) {
        stoppedY = STEPPERS[2].processMovement();
        if (stoppedY) {
          Serial.println("Stopped y");
        }
      }
    }
    Serial.println("Finished reset");
    runReset = false;
  } else {
    Serial.print("X: ");
    Serial.print(digitalRead(13) ? "Magnet" : "None");
    Serial.print(" Y: ");
    Serial.println(digitalRead(12) ? "Magnet" : "None");
  }
}
