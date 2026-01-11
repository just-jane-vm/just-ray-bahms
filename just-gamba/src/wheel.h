#include "config_parser.hpp"
#include "raylib.h"
#include "rngesus.h"
#include "thing.h"
#include <vector>

#ifndef WHEEL
#define WHEEL

class Wheel {
public:
  Config _winner;

  Wheel(const std::vector<Thing> whatevers, int x, int center_line) {
    _offset = 0;
    _x = x;
    _max = (whatevers.size()) * 64;
    _center = Rectangle{
        .x = float(x), .y = float(center_line), .width = 500.0, .height = 4};
    _images = whatevers;

    std::vector<Config> configs;
    for (Thing t : whatevers) {
      configs.push_back(t.Con);
    }

    _winner = GetRandomConfig(configs);
  }

  void Draw() {
    for (int i = 0; i < _images.size(); i++) {
      int y = (((i * 64) + _offset) % _max) - 64;
      DrawTexture(_images[i].Texture, _x, y, WHITE);
    }
  }

  std::string Current() {
    for (int i = 0; i < _images.size(); i++) {
      int y = (((i * 64) + _offset) % _max) - 64;
      Rectangle thing =
          Rectangle{.x = float(_x), .y = float(y), .width = 64, .height = 64};
      if (CheckCollisionRecs(_center, thing)) {
        return _images[i].Con.label;
      }
    }

    return "ERROR";
  }

  int CurrentIndex() {
    for (int i = 0; i < _images.size(); i++) {
      int y = (((i * 64) + _offset) % _max) - 64;
      Rectangle thing =
          Rectangle{.x = float(_x), .y = float(y), .width = 64, .height = 64};
      if (CheckCollisionRecs(_center, thing)) {
        return i;
      }
    }

    return -1;
  }

  // the center line is known, we want to stop updating when a Thing is centered
  // on the center line
  bool Update(bool die) {
    if (die) {
      int idx = CurrentIndex();
      std::string c = _images[idx].Con.label;

      if (c == _winner.label) {
        int y = (((idx * 64) + _offset) % _max) - 64;
        if (!CheckCollisionPointRec(
                Vector2{.x = _center.x + 1.0f, .y = y + 36.0f}, _center)) {
          _offset += 1;
          return true;
        } else {
          return false;
        }
      }
    }

    _offset += 12;
    return true;
  }

private:
  std::vector<Thing> _images;
  int _x;
  int _offset;
  int _max;
  int _count;
  Rectangle _center;
  bool _hasSignaledEnd;
};

#endif
