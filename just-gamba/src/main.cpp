#include "payouts.h"
#include "raylib.h"
#include "rngesus.h"
#include "wheel.h"
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include <sw/redis++/redis.h>
#include <vector>

#define TRANSPARENT CLITERAL(Color){0x00, 0x00, 0x00, 0x00}

#define WINDOW_HEIGHT 216.0
#define WINDOW_WIDTH 384.0

#include <cstdlib>
#include <iostream>

std::vector<Texture2D> GetThings() {
  std::string assetsDirectoryPath =
      "/home/jane/just-stream/just-ray-bahms/just-gamba/assets/";

  std::vector<Texture2D> textures;
  try {
    for (const auto &entry :
         std::filesystem::directory_iterator(assetsDirectoryPath)) {

      Texture2D texture = LoadTexture(entry.path().string().c_str());
      textures.push_back(texture);
    }
  } catch (const std::filesystem::filesystem_error &e) {
    std::cerr << "Error accessing directory: " << e.what() << std::endl;
  }

  return textures;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    return 1;
  }

  std::string userID = argv[1];
  Payouts payoutThing = Payouts(userID);

  SetConfigFlags(FLAG_WINDOW_TRANSPARENT | FLAG_WINDOW_ALWAYS_RUN);
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "just-gamba");
  SetTargetFPS(60);

  Texture2D slotMachine = LoadTexture("/home/jane/just-stream/just-ray-bahms/"
                                      "just-gamba/slot_machine.png");
  std::vector<Texture2D> things = GetThings();
  const int firstCol = 64;
  const int secondCol = 160;
  const int thirdCol = 256;

  std::vector<Wheel *> wheels;

  wheels.push_back(new Wheel(Shuffle(things), 64, RNG(240, 480)));
  wheels.push_back(new Wheel(Shuffle(things), 160, RNG(240, 480)));
  wheels.push_back(new Wheel(Shuffle(things), 256, RNG(240, 480)));

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(WHITE);

    bool isRunning = true;
    for (Wheel *wheel : wheels) {
      isRunning = wheel->Update();
    }

    for (Wheel *wheel : wheels) {
      wheel->Draw();
    }

    DrawTexture(slotMachine, 0, 0, WHITE);

    EndDrawing();
    if (!isRunning) {
      std::cout << "user has won a thing! " + userID << std::endl;
      payoutThing.GiveTTSTokens(5);
      break;
    }
  }

  UnloadTexture(slotMachine); // Unload texture
  CloseWindow();
  return 0;
}
