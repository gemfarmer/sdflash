#ifndef recipe_details_h
#define recipe_details_h

#include "Particle.h"
#include <ArduinoJson.h>

class RecipeDetails {
  public:
    static int handle(String response);
    int handleArray(String response);
    static void updateMemory(JsonVariant variant);
};

#endif
