#include "raylib.h"
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#define TRANSPARENT CLITERAL(Color){0x00, 0x00, 0x00, 0x00}
#define deadff CLITERAL(Color){0xde, 0xad, 0xff, 0xff}

#define WINDOW_HEIGHT 128.0
#define WINDOW_WIDTH 550.0

std::string formatMilliseconds(long long milliseconds) {

  std::chrono::milliseconds ms_duration(milliseconds);

  auto minutes = std::chrono::duration_cast<std::chrono::minutes>(ms_duration);
  ms_duration -= minutes;
  auto seconds = std::chrono::duration_cast<std::chrono::seconds>(ms_duration);
  ms_duration -= seconds;
  std::stringstream ss;

  ss << std::setfill('0') << std::setw(2) << minutes.count() << ":";

  ss << std::setfill('0') << std::setw(2) << seconds.count() << ".";

  ss << std::setfill('0') << std::setw(3) << ms_duration.count();

  return ss.str();
}

// You look great today! - Red_Epicness
int main(int argc, char **argv) {
  SetConfigFlags(FLAG_WINDOW_TRANSPARENT | FLAG_WINDOW_ALWAYS_RUN);
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "just-timer");
  SetTargetFPS(60);

  InitAudioDevice();
  Music music =
      LoadMusicStream("/home/jane/Downloads/zen-tone-mid-high-202557.mp3");

  std::string fontPath = std::string(
      "/home/jane/Downloads/Terminus/TerminessNerdFontMono-Bold.ttf");

  Font font = LoadFontEx(fontPath.c_str(), 128, NULL, 0);

  Vector2 textSize = MeasureTextEx(GetFontDefault(), "00:00.000", 128, 0.0);
  std::cout << textSize.x << " " << textSize.y << std::endl;

  if (argc != 2) {
    std::cout << "missing required argument" << std::endl;
    return 1;
  }

  int num = std::stoi(argv[1]);
  if (num <= 0) {
    std::cerr << "provided value is not a number: " << num << std::endl;
  }

  num = num * 1000;
  bool isFinished = false;

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(TRANSPARENT);

    float deltaTime = GetFrameTime();       // Time in seconds
    int milliseconds = deltaTime * 1000.0f; // Time in milliseconds

    if (isFinished) {
      UpdateMusicStream(music);
    } else {
      num = std::max(0, num - milliseconds);
      DrawTextEx(font, formatMilliseconds(num).c_str(),
                 Vector2{.x = 0.0f, .y = 0.0f}, 128, 2.0, deadff);
    }

    if (!isFinished && num <= 0) {
      num = 0.0f;
      std::cout << "playing music\n";
      PlayMusicStream(music);
      isFinished = true;
    }

    if (IsKeyDown(KEY_ESCAPE)) {
      break;
    }

    if (IsKeyDown(KEY_SPACE)) {
      num += 500;
      isFinished = false;
    }

    int key = GetKeyPressed();

    int increase = 0;
    switch (key) {
    case KEY_ONE:
      increase += 60 * 1;
      break;
    case KEY_TWO:
      increase += 60 * 2;
      break;
    case KEY_THREE:
      increase += 60 * 3;
      break;
    case KEY_FOUR:
      increase += 60 * 4;
      break;
    case KEY_FIVE:
      increase += 60 * 5;
      break;
    case KEY_SIX:
      increase += 60 * 6;
      break;
    case KEY_SEVEN:
      increase += 60 * 7;
      break;
    case KEY_EIGHT:
      increase += 60 * 8;
      break;
    case KEY_NINE:
      increase += 60 * 9;
      break;
    default:
      break;
    }

    increase *= 1000;
    num += increase;

    EndDrawing();
  }

  StopMusicStream(music);   // Stop the music stream
  UnloadMusicStream(music); // Unload music file from memory
  CloseAudioDevice();
  CloseWindow();
  return 0;
}
