// This #include statement was automatically added by the Particle IDE.
#include "Particle.h"

#include "system_update.h"
#include <ArduinoJson.h>
#include "SDCardReader.h"
#include "RecipeDetails.h"
#include "Recipe.h"
// #include "SdFat.h"

RecipeDetails RecipeDetails;

String helloWorld = "Hello Brian. Welcome to a world of pure imagination";
sDCardStatus SDCardStatus = {0, 0, 0, 0, 0, 0, "", 0};

void SDCardReader::poll() {
  if (SDCardStatus.isReading == 1) {
    if (SDCardStatus.getRecipes == 1 || SDCardStatus.getFirmware == 1) {
      // randomSeed(analogRead(A0)); //Use the analog pins for a good seed value
      // int fileNumber = random(999); //Select a random file #, 0 to 999
      // char fileName[12]; //Max file name length is "12345678.123" (12 characters)

      Serial.println("Steep Length Before:");
      Serial.println(Steep1.steepLength);

      //Now let's read back
      gotoCommandMode(); //Puts OpenLog in command mode

      if (SDCardStatus.getRecipes == 1) {
        readFile("recipe.txt");
      } else if (SDCardStatus.getFirmware == 1) {
        readFirmware("conf_ph.bin");
      } else {
        Serial1.write(13); //This is \r
        Serial.println("out of command mode");
      }

      //Now let's read back
      readDisk(); //Check the size and stats of the SD card

      Serial.println();
      Serial.println("File read complete");
      Serial.println("Steep Length After:");
      Serial.println(Steep1.steepLength);
      //Infinite loop
      Serial.println("Yay!");
    }
  } else if (SDCardStatus.isWriting == 1 && SDCardStatus.moreToWrite == 1) {
    Serial.println("SDCardStatus.isWriting == 1 && SDCardStatus.moreToWrite == 1");
    gotoCommandMode(); //Puts OpenLog in command mode

    appendToFile("datalog.txt");
  }
}

//Setups up the software serial, resets OpenLog so we know what state it's in, and waits
//for OpenLog to come online and report '<' that it is ready to receive characters to record
void SDCardReader::setupOpenLog(void) {
  if (SDCardStatus.hasBeenSetup == 0) {
    gotoCommandMode(); //Puts OpenLog in command mode

    Serial1.print("reset");
    Serial1.write(13); //This is \r
    Serial.println("soft reset");

    while(1) {
    if(Serial1.available())
      if(Serial1.read() == '<') break;
    }

    SDCardStatus.hasBeenSetup = true;
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
      // Serial.write(totalString);//Take the string from OpenLog and push it to the Arduino terminal
           totalString += tempString;
           timeOut = 0;
    }

    delay(1);
  }

  String subString = totalString.substring(totalString.indexOf('['), totalString.lastIndexOf('>') - 2);
  RecipeDetails.handleArray(subString);

  //This is not perfect. The above loop will print the '.'s from the log file. These are the two escape characters
  //recorded before the third escape character is seen.
  //It will also print the '>' character. This is the OpenLog telling us it is done reading the file.

  //This function leaves OpenLog in command mode
}

void SDCardReader::appendToFile(char *fileName) {

  //Old way
  Serial1.print("append ");
  Serial1.print(fileName);
  Serial1.write(13); //This is \r

  //Wait for OpenLog to indicate file is open and ready for writing
  while (1) {
    if (Serial1.available())
      if (Serial1.read() == '<') break;
  }
  Serial.print("writing ");
  Serial.print(SDCardStatus.dataToLog);
  Serial.print(" to ");
  Serial.println(fileName);
  Serial1.write(SDCardStatus.dataToLog);
  Serial1.write(13); //This is \r
  SDCardStatus.moreToWrite = false;
}


int SDCardReader::getFileSize(char *fileName) {
   //Old way
  Serial1.print("size ");
  Serial1.print(fileName);
  Serial1.write(13); //This is \r

  //The OpenLog echos the commands we send it by default so we have 'read log823.txt\r' sitting
  //in the RX buffer. Let's try to not print this.
  while(1) {
    if(Serial1.available())
      if(Serial1.read() == '\r') break;
  }

  Serial.write("size buffer cleared");

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
           totalString += tempString;
           timeOut = 0;
    }

    delay(1);
  }

  // Serial.write("fileLength ");
  // Serial.write(totalString);

  fileSize = totalString.toInt();
  // sscanf(totalString, "%zu", fileSize);
  return fileSize;
}

//Reads the contents of a given file and dumps it to the serial terminal
//This function assumes the OpenLog is in command mode
void SDCardReader::readFirmware(char *fileName) {
  Serial.write("Read Firmware");

  int sizeFile = getFileSize(fileName);

  //Old way
  Serial1.print("read ");
  Serial1.print(fileName);
  Serial1.write(13); //This is \r

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

  char file_bytes[sizeFile];
  for(int _timeOut = 0 ; _timeOut < 2000 ; _timeOut++) {
    while(Serial1.available()) {
      int spot = 0;
      while(Serial1.available()) {
            file_bytes[spot++] = Serial1.read();
            if(spot > sizeFile) break;

      }
           Serial.write(file_bytes);
           _timeOut = 0;
    }

    delay(1);
  }
  uint8_t file_bytes_ = *file_bytes;
  updateFirmwareFromFile(&file_bytes_);
}

void SDCardReader::updateFirmwareFromFile(uint8_t* file_bytes) {
  FileTransfer::Descriptor file;

  Serial.printlnf("starting flash size=%d", fileSize);

  file.file_length = fileSize;
  file.file_address = 0; // Automatically set to HAL_OTA_FlashAddress if store is FIRMWARE
  file.chunk_address = 0;
  file.chunk_size = 0; // use default
  file.store = FileTransfer::Store::FIRMWARE;

  int result = Spark_Prepare_For_Firmware_Update(file, 0, NULL);
  if (result != 0) {
    Serial.printlnf("prepare failed %d", result);
    return;
  }

  Serial.printlnf("chunk_size=%d file_address=0x%x fileSize=%d", file.chunk_size, file.file_address, fileSize);
  if (file.chunk_size == 0) {
    file.chunk_size = 512;
  }

  // Note that Spark_Prepare_For_Firmware_Update sets file.file_address so it's not really zero here
  // even though it's what we initialize it to above!
  file.chunk_address = file.file_address;

  size_t offset = 0;

  while(offset < file.file_length) {
    if (file.chunk_size > (file.file_length - offset)) {
      file.chunk_size = (file.file_length - offset);
    }

    Serial.printlnf("chunk_address=0x%x chunk_size=%d (%d < %d == %s)", file.chunk_address, file.chunk_size, offset, file.file_length, (offset < file.file_length) ? "continue" : "break");
    result = Spark_Save_Firmware_Chunk(file, &file_bytes[offset], NULL);

    if (result != 0) {
      Serial.printlnf("save chunk failed %d", result);
      return;
    }
    file.chunk_address += file.chunk_size;
    offset += file.chunk_size;
  }

  result = Spark_Finish_Firmware_Update(file, 0x1, NULL);
  if (result != 0) {
    Serial.printlnf("finish failed %d", result);
    return;
  }

  Serial.printlnf("update complete");

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
