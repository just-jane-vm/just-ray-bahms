#include "raylib.h"

class Pellet {
public:
  Vector2 Position;

  Pellet(Vector2 position) { Position = position; }
  void Draw() { DrawCircleV(Position, 2, YELLOW); }
};
