// This #include statement was automatically added by the Particle IDE.
#include "Particle.h"
#include <ArduinoJson.h>
#include "SDCardReader.h"
#include "RecipeDetails.h"
#include "Recipe.h"

RecipeDetails RecipeDetails;

void SDCardReader::poll() {

  randomSeed(analogRead(A0)); //Use the analog pins for a good seed value
  int fileNumber = random(999); //Select a random file #, 0 to 999
  char fileName[12]; //Max file name length is "12345678.123" (12 characters)
  sprintf(fileName, "logg%03d.txt", fileNumber);

  gotoCommandMode(); //Puts OpenLog in command mode
  createFile(fileName); //Creates a new file called log###.txt where ### is random

  Serial.println("Steep Length Before:");
  Serial.println(Steep1.steepLength);

  Serial.print("Random file created: ");
  Serial.println(fileName);

  Serial1.println("[{\"i\":\"s1\",\"steepLength\":25200,\"steepTemp\":\"59.9\"},{\"i\":\"s1\",\"tempOffset\":\"0.9\",\"volumeMax\":13,\"airOnMax\":180},{\"i\":\"s1\",\"airOffMax\":180},{\"i\":\"s2\",\"steepLength\":28800,\"steepTemp\":\"59.9\"},{\"i\":\"s2\",\"tempOffset\":\"0.9\",\"volumeMax\":13,\"airOnMax\":180},{\"i\":\"s2\",\"airOffMax\":180},{\"i\":\"s3\",\"steepLength\":25200,\"steepTemp\":\"59.9\"},{\"i\":\"s3\",\"tempOffset\":\"0.9\",\"volumeMax\":13,\"airOnMax\":180},{\"i\":\"s3\",\"airOffMax\":180},{\"i\":\"a1\",\"germLength\":43200,\"germTemp\":\"59.9\"},{\"i\":\"a1\",\"tempOffset\":\"0.9\",\"fanOnMax\":180,\"fanOffMax\":180},{\"i\":\"a2\",\"germLength\":32400,\"germTemp\":\"59.9\"},{\"i\":\"a2\",\"tempOffset\":\"0.9\",\"fanOnMax\":180,\"fanOffMax\":180},{\"i\":\"g1\",\"germLength\":259200,\"germTemp\":\"55.0\"},{\"i\":\"g1\",\"tempOffset\":\"1.0\",\"fanOnMax\":180,\"fanOffMax\":180},{\"i\":\"k1\",\"freeDryLength\":18000,\"freeDryTemp\":\"150.0\"},{\"i\":\"k1\",\"interDryLength\":18000,\"interDryTemp\":\"200.0\"},{\"i\":\"k1\",\"boundWaterDryLength\":18000},{\"i\":\"k1\",\"boundWaterDryTemp\":\"220.0\",\"cureDryLength\":18000},{\"i\":\"k1\",\"cureDryTemp\":\"240.0\"},{\"i\":\"c1\"},{\"t\":3}]");
  Serial.println("Text written to file");
  Serial.println("Reading file contents...");
  Serial.println();

  //Now let's read back
  gotoCommandMode(); //Puts OpenLog in command mode
  // readFile("test.txt");
  readFile(fileName); //This dumps the contents of a given file to the serial terminal

  //Now let's read back
  readDisk(); //Check the size and stats of the SD card

  Serial.println();
  Serial.println("File read complete");
  Serial.println("Steep Length After:");
  Serial.println(Steep1.steepLength);

  //Infinite loop
  Serial.println("Yay!");
  // while(1) {
  //   digitalWrite(13, HIGH);
  //   delay(250);
  //   digitalWrite(13, LOW);
  //   delay(250);
  // }
}

//Setups up the software serial, resets OpenLog so we know what state it's in, and waits
//for OpenLog to come online and report '<' that it is ready to receive characters to record
void SDCardReader::setupOpenLog(void) {

  gotoCommandMode(); //Puts OpenLog in command mode

  Serial1.print("reset");
  Serial1.write(13); //This is \r
  Serial.println("soft reset");

    while(1) {
    if(Serial1.available())
      if(Serial1.read() == '<') break;
  }

}

//This function creates a given file and then opens it in append mode (ready to record characters to the file)
//Then returns to listening mode
void SDCardReader::createFile(char *fileName) {

  //Old way
  Serial1.print("new ");
  Serial1.print(fileName);
  Serial1.write(13); //This is \r

  //New way
  //OpenLog.print("new ");
  //OpenLog.println(filename); //regular println works with OpenLog v2.51 and above

  //Wait for OpenLog to return to waiting for a command
  while(1) {
    if(Serial1.available())
      if(Serial1.read() == '>') break;
  }

  Serial1.print("append ");
  Serial1.print(fileName);
  Serial1.write(13); //This is \r

  //Wait for OpenLog to indicate file is open and ready for writing
  while(1) {
    if(Serial1.available())
      if(Serial1.read() == '<') break;
  }

  //OpenLog is now waiting for characters and will record them to the new file
}

//Reads the contents of a given file and dumps it to the serial terminal
//This function assumes the OpenLog is in command mode
void SDCardReader::readFile(char *fileName) {

  //Old way
  Serial1.print("read ");
  Serial1.print(fileName);
  Serial1.write(13); //This is \r

  //New way
  //OpenLog.print("read ");
  //OpenLog.println(filename); //regular println works with OpenLog v2.51 and above

  //The OpenLog echos the commands we send it by default so we have 'read log823.txt\r' sitting
  //in the RX buffer. Let's try to not print this.
  while(1) {
    if(Serial1.available())
      if(Serial1.read() == '\r') break;
  }

  //This will listen for characters coming from OpenLog and print them to the terminal
  //This relies heavily on the SoftSerial buffer not overrunning. This will probably not work
  //above 38400bps.
  //This loop will stop listening after 1 second of no characters received
  String totalString = "";
  for(int timeOut = 0 ; timeOut < 1000 ; timeOut++) {
    while(Serial1.available()) {
      char tempString[100];
      int spot = 0;
      while(Serial1.available()) {
            tempString[spot++] = Serial1.read();
            if(spot > 98) break;

      }
           tempString[spot] = '\0';
           Serial.write(tempString);
    //   Serial.write(totalString);//Take the string from OpenLog and push it to the Arduino terminal
           totalString += tempString;
           timeOut = 0;
    }

    delay(1);
  }
    String subString = totalString.substring(totalString.indexOf('.') + 5, totalString.lastIndexOf('>') - 2);
    RecipeDetails.handleArray(subString);

  //This is not perfect. The above loop will print the '.'s from the log file. These are the two escape characters
  //recorded before the third escape character is seen.
  //It will also print the '>' character. This is the OpenLog telling us it is done reading the file.

  //This function leaves OpenLog in command mode
}

//Check the stats of the SD card via 'disk' command
//This function assumes the OpenLog is in command mode
void SDCardReader::readDisk() {

  //Old way
  Serial1.print("disk");
  Serial1.write(13); //This is \r

  //New way
  //OpenLog.print("read ");
  //OpenLog.println(filename); //regular println works with OpenLog v2.51 and above

  //The OpenLog echos the commands we send it by default so we have 'disk\r' sitting
  //in the RX buffer. Let's try to not print this.
  while(1) {
    if(Serial1.available())
      if(Serial1.read() == '\r') break;
  }

  //This will listen for characters coming from OpenLog and print them to the terminal
  //This relies heavily on the SoftSerial buffer not overrunning. This will probably not work
  //above 38400bps.
  //This loop will stop listening after 1 second of no characters received
  for(int timeOut = 0 ; timeOut < 1000 ; timeOut++) {
    while(Serial1.available()) {
      char tempString[100];

      int spot = 0;
      while(Serial1.available()) {
        tempString[spot++] = Serial1.read();
        if(spot > 98) break;
      }
      tempString[spot] = '\0';
      Serial.write(tempString); //Take the string from OpenLog and push it to the Arduino terminal
      timeOut = 0;
    }

    delay(1);
  }

  //This is not perfect. The above loop will print the '.'s from the log file. These are the two escape characters
  //recorded before the third escape character is seen.
  //It will also print the '>' character. This is the OpenLog telling us it is done reading the file.

  //This function leaves OpenLog in command mode
}

//This function pushes OpenLog into command mode
void SDCardReader::gotoCommandMode(void) {
  //Send three control z to enter OpenLog command mode
  //Works with Arduino v1.0
  Serial1.write(26);
  Serial1.write(26);
  Serial1.write(26);
  Serial1.write(13); //This is \r

  Serial.println("go to command mode");
  //Wait for OpenLog to respond with '>' to indicate we are in command mode
  while(1) {
    if(Serial1.available())
      if(Serial1.read() == '>') break;
  }
  Serial.println("in command mode");
}
