// fpt university
// semester: spring 2024
// class: se1828
// group: group 9
// author: phathnhe187251
// date created: jan 28, 2024
// last modified:  14:143 jan 29, 2024
// license: creative commons attribution non commercial share alike (cc by-nc-sa 3.0)

// name: auto feeder
// version: v0.8.1-beta
// description: a machine that detects when food and water has run out and automatically attempts to refill
// intelligently only carries out refills when it is safe to do so
// perfect for use in cattle and poultry farming

// system schematics (incomplete): https://www.tinkercad.com/things/fEtl90w5uD5-project-auto-feeder
// hardware used:
// microcontroller: arduino uno r3
// presence detector: hy-srf05 ultrasonic distance sensor
// feeder box actuator: sg90 micro servo
// bowl state detector: generic photoresistor
// water level sensor: sen18 water level sensor
// water pump: mini submersible auto-priming pump

// notes:
// tune values before running to ensure correct operation
// uncomment line 58 to enable serial debugging


#include <Servo.h>


Servo feederBox;
const int feederBoxPin = 8;
const int close = 75, open = 20;

const int pumpPin = 7;

// ultrasonic presence detector calibrations
const int trigPin = 3, echoPin = 4;
bool presence = true;
const float distanceThreshold = 10; // change to configure how close to the sensor is considered present, in cm

// photoresistor calibrations
const int bowlBottom = A0, bowlTop = A1;
bool bowlFull = true, bowlEmpty = false;
int photoresMin = 1, photoresMax = 860; // tune these values before running (measured with 330 ohm resistors)
int photoresThreshold = (photoresMin + photoresMax) / 60;

// water sensor calibrations
const int waterPowerPin = 6, waterReadPin = A2;
const int waterEmpty = 0, waterFull = 280; // tune these values before running
const int waterThreshold = (waterEmpty + waterFull) / 2; // how low is considered low, default is 1/4
int waterLevel = waterFull;


void setup() {
  Serial.begin(9600);

  pinMode(bowlBottom, INPUT);
  pinMode(bowlTop, INPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(waterPowerPin, OUTPUT);
  pinMode(waterReadPin, INPUT);

  feederBox.attach(feederBoxPin);
  feederBox.write(close); // ensure feeder box is closed and  door secured

  pinMode(pumpPin, OUTPUT);
  digitalWrite(pumpPin, HIGH); // pump is connected to external power source, grounded to this pin, controls are inverted
}

void loop() {
  // reads bowl state and detect presence
  bowlEmpty = !dark(analogRead(bowlBottom));
  bowlFull = dark(analogRead(bowlTop));
  waterLevel = measureWater();
  presence = detectPresence();

  Serial.println(waterLevel);

  // feeder box is open only when bowl is empty and it is not full and no presence is detected
  if (bowlEmpty && !bowlFull && !presence) { 
    feederBox.write(open); 
  }
  // if the bowl becomes full or presence is detected, refilling stops
  if (bowlFull || presence) { 
    feederBox.write(close); 
  }

  // pump water in when the water level goes below the threshold
  if (waterLevel <= waterThreshold ) {
    digitalWrite(pumpPin, LOW);
  }
  // if the water bowl is full, pumping stops
  if (waterLevel >= waterFull) {
    digitalWrite(pumpPin, HIGH);
  } 

  delay(250);
}


// checks photoresistor state based on readings
// returns true if the photoresistor is obscured, otherwise return false
bool dark(int reading) {
  if (reading < photoresThreshold) {
    return true;
  } else {
    return false;
  }
}

// helper function
// returns a float signifying distance measured in cm
float measureDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  float duration = pulseIn(echoPin, HIGH);
  float distance = duration * 0.0343 / 2;

  return distance;
}

// detects presence using the ultrasonic distance sensor
// detection threshold can be tuned (default is 50 cm)
bool detectPresence() {
  float distance = measureDistance();
  if (distance < distanceThreshold) return true;
  return false;
}

// measures water using water level sensor
// returns an integer signifying the water level in the container
int measureWater() {
	digitalWrite(waterPowerPin, HIGH);
	delay(10);
	int val = analogRead(waterReadPin);
	digitalWrite(waterPowerPin, LOW);
	return val;
}
