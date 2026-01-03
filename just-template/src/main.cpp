#include "raylib.h"

#define TRANSPARENT CLITERAL(Color){0x00, 0x00, 0x00, 0x00}

#define WINDOW_HEIGHT 1080.0
#define WINDOW_WIDTH 1920.0

// You look great today! - Red_Epicness
int main() {
  SetConfigFlags(FLAG_WINDOW_TRANSPARENT | FLAG_WINDOW_ALWAYS_RUN);
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "just-pacman");
  SetTargetFPS(60);

  while (!WindowShouldClose()) {

    BeginDrawing();
    ClearBackground(TRANSPARENT);

    DrawText("hello world", 0, 0, 32, WHITE);

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
