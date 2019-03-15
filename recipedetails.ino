#include "Particle.h"

#include "system_update.h"
#include "SDCardReader.h"
SDCardReader SDCardReader;

// SYSTEM_MODE(SEMI_AUTOMATIC);
bool uploaded = false;
SerialDebugOutput debugOutput(9600, ALL_LEVEL);

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  delay(1000);
  Serial.println("I'm alive");
  SDCardReader.setupOpenLog(); //Resets logger and waits for the '<' I'm alive character
  Serial.println("OpenLog online");
}

void loop() {
  SDCardReader.poll();
  // SDCardStatus.getRecipes = true;
  SDCardStatus.getFirmware = true;
}
