#include "config_parser.hpp"
#include "raylib.h"
#include "rngesus.h"
#include "wheel.h"

#include <cstdlib>
#include <iostream>
#include <string>
#include <sw/redis++/redis.h>
#include <unordered_set>

#define TRANSPARENT CLITERAL(Color){0x00, 0x00, 0x00, 0x00}
#define DEADFF CLITERAL(Color){0xde, 0xad, 0xff, 0xff}
#define WINDOW_HEIGHT 216.0
#define WINDOW_WIDTH 384.0

int main() {
  SetConfigFlags(FLAG_WINDOW_TRANSPARENT | FLAG_WINDOW_ALWAYS_RUN);
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "just-gamba");
  SetTargetFPS(60);

  InitAudioDevice();

  Texture2D slotMachine = LoadTexture("/home/jane/just-stream/just-ray-bahms/"
                                      "just-gamba/slot_machine.png");
  std::vector<Thing> things = GetThings();
  const int firstCol = 64;
  const int secondCol = 160;
  const int thirdCol = 256;

  std::vector<Wheel *> wheels;

  int count = RNG(240, 480);
  wheels.push_back(new Wheel(Shuffle(things), 64, 90));
  wheels.push_back(new Wheel(Shuffle(things), 160, 90));
  wheels.push_back(new Wheel(Shuffle(things), 256, 90));

  std::unordered_set<std::string> results;

  for (Wheel *wheel : wheels) {
    results.insert(wheel->_winner.label);
  }

  bool playMusic = false;
  if (results.size() < 3) {
    playMusic = true;
  }

  Music music;

  std::string thing = std::to_string(RNG(1, 3));
  std::string path = "/home/jane/just-stream/just-ray-bahms/"
                     "just-gamba/assets/audio/music_" +
                     thing + ".mp3";

  music = LoadMusicStream(path.c_str());

  PlayMusicStream(music);
  SetMusicVolume(music, 1.0);

  int endFrame = 180;
  bool isEnded = false;

  std::string message;

  MinimizeWindow();
  while (!WindowShouldClose()) {
    if (playMusic) {
      UpdateMusicStream(music);
    }

    count -= 1;
    BeginDrawing();
    ClearBackground(WHITE);
    bool shouldContinue = false;
    bool prev = true;
    for (Wheel *wheel : wheels) {
      prev = !wheel->Update(count < 0 && prev);
      shouldContinue |= !prev;
    }

    for (Wheel *wheel : wheels) {
      wheel->Draw();
    }

    DrawTexture(slotMachine, 0, 0, WHITE);

    DrawRectangle(62, 90, 256, 4, DEADFF);

    if (!shouldContinue && !isEnded) {
      isEnded = true;

      std::cout << "--result" << " " << wheels[0]->Current() << "|"
                << wheels[1]->Current() << "|" << wheels[2]->Current()
                << std::endl;

      std::string winner = GetWinner(wheels);
      if (winner.empty()) {
        message = "you lose, skill issue";
      } else {
        message = "you won " + winner;
      }
    }

    if (isEnded) {
      endFrame -= 1;
      if (message != "") {
        DrawText(message.c_str(), 48, 164, 24, Color{0xde, 0xad, 0xff, 0xff});
      }
    }

    if (endFrame < 0)
      break;

    EndDrawing();
  }

  UnloadTexture(slotMachine); // Unload texture
  CloseWindow();
  return 0;
}
