/*
Adafruit Arduino - Lesson 16. Stepper
*/

#include <Stepper.h>

int in1Pin = 3;
int in2Pin = 9;
int in3Pin = 11;
int in4Pin = 10;

Stepper motor(32, in1Pin, in2Pin, in3Pin, in4Pin);  

void setup()
{
//  pinMode(in1Pin, OUTPUT);
//  pinMode(in2Pin, OUTPUT);
//  pinMode(in3Pin, OUTPUT);
//  pinMode(in4Pin, OUTPUT);

  // this line is for Leonardo's, it delays the serial interface
  // until the terminal window is opened
  // while (!Serial);
  
  Serial.begin(9600);
  motor.setSpeed(1100);
}

void loop()
{
    // if(Serial.available() > 0) {
    int steps = Serial.read();
    motor.step(steps);
    // delay(500);
    // } 
    
}
