#ifndef Recipe_h
#define Recipe_h

#include "Particle.h"


struct steepRec{
  int steepLength;
  float steepTemp;
  float tempOffset;
  int volumeMax;
  int airOnMax;
  int airOffMax;
};

extern steepRec Steep1;


#endif