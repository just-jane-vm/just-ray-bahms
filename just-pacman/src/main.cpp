#include "map.h"
#include "raylib.h"

#define TRANSPARENT CLITERAL(Color){0x00, 0x00, 0x00, 0x00}
#define DEADFF CLITERAL(Color){0xde, 0xad, 0xff, 0xff}

#define WINDOW_HEIGHT 32 * TILE_SIZE
#define WINDOW_WIDTH 22 * TILE_SIZE
#define SPEED 3

// You look great today! - Red_Epicness
int main() {
  SetConfigFlags(FLAG_WINDOW_TRANSPARENT | FLAG_WINDOW_ALWAYS_RUN);

  Map map =
      Map("/home/jane/just-stream/just-ray-bahms/just-pacman/map.whatever");

  std::cout << map.Width << "x" << map.Height << std::endl;
  InitWindow((map.Width + 2) * TILE_SIZE, (map.Height + 2) * TILE_SIZE,
             "just-pacman");
  SetTargetFPS(60);

  PuckMan puck = PuckMan(12.0f, map.PlayerStartPosition);

  while (!WindowShouldClose()) {

    BeginDrawing();
    ClearBackground(TRANSPARENT);

    Vector2 moveTo = Vector2(puck.Position);

    if (IsKeyDown(KEY_W)) {
      moveTo.y -= SPEED;
    } else if (IsKeyDown(KEY_S)) {
      moveTo.y += SPEED;
    } else if (IsKeyDown(KEY_A)) {
      moveTo.x -= SPEED;
    } else if (IsKeyDown(KEY_D)) {
      moveTo.x += SPEED;
    }

    map.TryMove(&puck, moveTo);

    map.Draw();
    DrawCircleV(puck.Position, puck.Radius, DEADFF);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
