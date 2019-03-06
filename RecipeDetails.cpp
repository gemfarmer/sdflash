#include "Particle.h"
#include <ArduinoJson.h>
#include "RecipeDetails.h"
#include "Recipe.h"

// Based on this:
// https://arduinojson.org/v6/assistant/
// and this JSON
// [{"i":"s1","steepLength":25200,"steepTemp":"59.9"},{"i":"s1","tempOffset":"0.9","volumeMax":13,"airOnMax":180},{"i":"s1","airOffMax":180},{"i":"s2","steepLength":28800,"steepTemp":"59.9"},{"i":"s2","tempOffset":"0.9","volumeMax":13,"airOnMax":180},{"i":"s2","airOffMax":180},{"i":"s3","steepLength":25200,"steepTemp":"59.9"},{"i":"s3","tempOffset":"0.9","volumeMax":13,"airOnMax":180},{"i":"s3","airOffMax":180},{"i":"a1","germLength":43200,"germTemp":"59.9"},{"i":"a1","tempOffset":"0.9","fanOnMax":180,"fanOffMax":180},{"i":"a2","germLength":32400,"germTemp":"59.9"},{"i":"a2","tempOffset":"0.9","fanOnMax":180,"fanOffMax":180},{"i":"g1","germLength":259200,"germTemp":"55.0"},{"i":"g1","tempOffset":"1.0","fanOnMax":180,"fanOffMax":180},{"i":"k1","freeDryLength":18000,"freeDryTemp":"150.0"},{"i":"k1","interDryLength":18000,"interDryTemp":"200.0"},{"i":"k1","boundWaterDryLength":18000},{"i":"k1","boundWaterDryTemp":"220.0","cureDryLength":18000},{"i":"k1","cureDryTemp":"240.0"},{"i":"c1"},{"t":3}]
int RecipeDetails::handleArray(String response) {
  const size_t capacity = JSON_ARRAY_SIZE(22) + 2*JSON_OBJECT_SIZE(1) + 5*JSON_OBJECT_SIZE(2) + 9*JSON_OBJECT_SIZE(3) + 6*JSON_OBJECT_SIZE(4) + 1000;
  DynamicJsonBuffer jsonBuffer(capacity);

  int len = response.length() + 1;
  char json[len];
  response.toCharArray(json, len);
  JsonArray& arr = jsonBuffer.parseArray(json);
  arr.printTo(Serial);
  for (auto value : arr) {
    updateMemory(value);
  }
  return 1;
}

// Expects an objects with a 'cycle' field fromt he app. like so:
// {i: "s1", title: "as", cycle: 'cleaning', (other variable fields)}
int RecipeDetails::handle(String response) {
  Serial.println("RecipeDetails hit");
  Serial.println(response);

  const size_t bufferSize = JSON_OBJECT_SIZE(7) + 100;
  DynamicJsonBuffer jsonBuffer(bufferSize);

  int len = response.length() + 1;
  char json[len];
  response.toCharArray(json, len);

  JsonVariant root = jsonBuffer.parseObject(json);

  if (!root.success()) {
    Serial.println("parseObject() failed");
  }

//   JsonVariant variant = root;
  updateMemory(root);
  return 1;
}

void RecipeDetails::updateMemory(JsonVariant variant) {
    JsonObject& root = variant;

    if (root.containsKey("steepLength")) Steep1.steepLength = root["steepLength"];
}
