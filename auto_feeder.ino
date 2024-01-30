// fpt university
// semester: spring 2024
// class: se1828
// group: group 9
// author: phathnhe187251
// date created: jan 28, 2024
// last modified:  23:16 jan 30, 2024
// license: creative commons attribution non commercial share alike (cc by-nc-sa 3.0)

// name: auto feeder
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
// relay switch: jqc-3ff-s-z 5v 12a relay

// notes:
// tune values before running to ensure correct operation


#include <Servo.h>


const String version = "v1.1.1r1-release";
const bool serialDebug = false;  // set to true to enable serial debugging
const int baudRate = 9600;

// timeout values
// after a determined period of time passes and no changes are registered on the presence detection sensor, the system stops working and must be manually restarted
const unsigned long timeOutInterval = 72;  // time interval in hour, default is 72h
const int loopDelay = 250;       // time interval in ms, default is 250ms
const unsigned long timeOutCount = timeOutInterval * 3600000 / loopDelay;
int timeSinceLastUpdate = 0;

// the feeder box has 2 states: closed and open
// these values can be changed to suit a particular installation
Servo feederBox;
const int feederBoxPin = 8;
const int close = 75, open = 20;  // values in degrees

// pump is connected to external power source and controlled by a relay switch
// this particular switch is low-active, so controls are inverted
const int pumpRelayPin = 7;

// ultrasonic presence detector calibrations
const int trigPin = 3, echoPin = 4;
bool presence = true;
bool prevPresence = presence;
const float distanceThreshold = 10;  // change to configure how close to the sensor is considered present, in cm

// photoresistor calibrations
const int bowlBottom = A0, bowlTop = A1;
bool bowlFull = true, bowlEmpty = false;
int photoresMin = 1, photoresMax = 860;  // tune these values before running (measured with 330 ohm resistors)
const float sensitivity = 0.025;         // value from 0 to 1
int photoresThreshold = (photoresMin + photoresMax) * sensitivity;

// water sensor calibrations
const int waterReadPin = A2;
const int waterEmpty = 0, waterFull = 450;  // tune these values before running
const float multiplier = 0.95;              // value from 0 to 1, values closer to 1 means higher threshold
const int waterThreshold = (waterEmpty + waterFull) * multiplier;
int waterLevel = waterFull;


void setup() {
  if (serialDebug == true) Serial.begin(baudRate);

  pinMode(bowlBottom, INPUT);
  pinMode(bowlTop, INPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(waterReadPin, INPUT);

  feederBox.attach(feederBoxPin);
  feederBox.write(close);  // ensures feeder box is closed and  door secured

  pinMode(pumpRelayPin, OUTPUT);
  digitalWrite(pumpRelayPin, HIGH);  // ensures pump is off
}

void loop() {
  // reads bowl state and detect presence
  bowlEmpty = !dark(analogRead(bowlBottom));
  bowlFull = dark(analogRead(bowlTop));
  waterLevel = analogRead(waterReadPin);
  ;
  presence = detectPresence();

  if (serialDebug == true) printDebug();

  // feeder box is open only when bowl is empty and it is not full and no presence is detected
  if (bowlEmpty && !bowlFull && !presence) {
    feederBox.write(open);
  }
  // if the bowl becomes full or presence is detected, refilling stops
  if (bowlFull || presence) {
    feederBox.write(close);
  }

  // pump water in when the water level goes below the threshold
  if (waterLevel <= waterThreshold && waterLevel < waterFull) {
    digitalWrite(pumpRelayPin, LOW);
  }
  // if the water bowl is full, pumping stops
  if (waterLevel >= waterFull) {
    digitalWrite(pumpRelayPin, HIGH);
  }

  // advances timeout counter if presence status does not change
  if (presence == prevPresence) {
    timeSinceLastUpdate++;
  } else {
    timeSinceLastUpdate = 0;
  }

  if (timeSinceLastUpdate >= timeOutCount) return;

  prevPresence = presence;
  
  delay(loopDelay);
}


// checks photoresistor state based on readings
// returns true if the photoresistor is obscured, otherwise return false
bool dark(int reading) {
  return (reading < photoresThreshold) ? true : false;
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
  return (distance <= distanceThreshold) ? true : false;
}

// for development use only
// prints debug info to the serial monitor
void printDebug() {
  Serial.print("FarmWare ");
  Serial.print(version);
  Serial.println(" serial debugger\n");

  Serial.print("Presence detection: ");
  if (presence == true) {
    Serial.println("detected");
  } else {
    Serial.println("none");
  }
  Serial.print("Detection range: ");
  Serial.print((int)distanceThreshold);
  Serial.println("cm");
  Serial.print("Main loop iterations since last update: ");
  Serial.print(timeSinceLastUpdate);
  Serial.print(" / ");
  Serial.println(timeOutCount);
  Serial.println();

  Serial.print("Bowl state: ");
  if (presence == true) {
    Serial.println("presence detected");
    Serial.println("Feeder box: closed\n");
  } else {
    if (bowlFull) {
      Serial.println("full");
      Serial.println("Feeder box: closed\n");
    } else if (bowlEmpty) {
      Serial.println("empty");
      Serial.println("Feeder box: open\n");
    } else {
      Serial.println("nominal");
      Serial.println("Feeder box: closed\n");
    }
  }

  Serial.print("Water level: ");
  Serial.println(waterLevel);
  Serial.print("Water level threshold: ");
  Serial.println(waterThreshold);
  if (waterLevel <= waterThreshold) {
    Serial.println("Pump: running\n");
  } else {
    Serial.println("Pump: standby\n");
  }

  delay(750);
}
