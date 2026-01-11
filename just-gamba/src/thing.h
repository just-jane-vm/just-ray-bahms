#include "config_parser.hpp"
#include "raylib.h"

#ifndef THING
#define THING

class Thing {
public:
  Texture2D Texture;
  Config Con;

  Thing(Texture2D texture, Config config) {
    Texture = texture;
    Con = config;
  }
};

#endif
