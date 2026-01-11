#include "raylib.h"

class PuckMan {
public:
  float Radius;
  Vector2 Position;

  PuckMan(float radius, Vector2 position) {
    Radius = radius;
    Position = position;
  }
};
