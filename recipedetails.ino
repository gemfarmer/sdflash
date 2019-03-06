// This #include statement was automatically added by the Particle IDE.
#include "SDCardReader.h"

// This #include statement was automatically added by the Particle IDE.
#include "Recipe.h"


// This #include statement was automatically added by the Particle IDE.
#include <ArduinoJson.h>

// This #include statement was automatically added by the Particle IDE.
// #include "RecipeDetails.h"

// RecipeDetails RecipeDetails;
SDCardReader SDCardReader;

SYSTEM_MODE(MANUAL);

// float dummyVoltage = 3.50; //This just shows to to write variables to OpenLog
// char mystr[13]; //Initialized variable to store recieved data

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
}
