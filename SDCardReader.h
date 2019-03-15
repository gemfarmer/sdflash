#ifndef sd_card_reader_h
#define sd_card_reader_h

#include "Particle.h"
#include <ArduinoJson.h>
// #include "SdFat.h"

class SDCardReader {
  size_t fileSize;
  public:
    void poll();
    void setupOpenLog(void);
    void createFile(char *fileName);
    void readFile(char *fileName);
    void appendToFile(char *fileName);
    void readFirmware(char *fileName);
    // void readFirmwareRickkas(char *fileName);
    // void flashTinkerRickkas(File &firmwareFile);
    void flashTinker();
    void updateFirmwareFromFile(uint8_t* file_bytes);
    void readDisk();
    void gotoCommandMode(void);
    int getFileSize(char *fileName);
};


struct sDCardStatus {
  bool getRecipes;
  bool hasBeenSetup;
  bool getFirmware;
  bool isWriting;
  bool isReading;
  bool moreToWrite;
  String dataToLog;
  int currentOffset;
};

extern sDCardStatus SDCardStatus;

#endif
