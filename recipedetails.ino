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
  delay(3000);
  SDCardStatus.getRecipes = true;
  // SDCardStatus.getFirmware = true;
  SDCardStatus.isReading = true;
  // SDCardStatus.isWriting = true;

  // randomSeed(analogRead(A0)); //Use the analog pins for a good seed value
  // int randomNumber = random(999); //Select a random file #, 0 to 999
  // if (randomNumber > 500) {
  //   SDCardStatus.moreToWrite = true;
  //   char output[3]; //Max file name length is "12345678.123" (12 characters)

  //   sprintf(output, "%03d", randomNumber);
  //   Serial.printlnf("output=%s",output);
  //   SDCardStatus.dataToLog = output;
  // } else {
  //   Serial.println("No random match");
  // }
}
