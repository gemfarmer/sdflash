#ifndef sd_card_reader_h
#define sd_card_reader_h

#include "Particle.h"
#include <ArduinoJson.h>

class SDCardReader {
  public:
    void poll();
    void setupOpenLog(void);
    void createFile(char *fileName);
    void readFile(char *fileName);
    void readDisk();
    void gotoCommandMode(void);
};

#endif
