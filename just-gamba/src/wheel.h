#include "raylib.h"
#include "rngesus.h"
#include <vector>

class Wheel {
public:
  Wheel(const std::vector<Texture2D> whatevers, int x, int count) {
    _offset = RNG(69, 420);
    _images = whatevers;
    _x = x;
    _max = (whatevers.size()) * 72;
    _count = count;
  }

  void Draw() {
    for (int i = 0; i <= _images.size(); i++) {
      int y = ((i * 72) + _offset) % _max - 64;
      DrawTexture(_images[i], _x, y, WHITE);
    }
  }

  bool Update() {
    if (_count <= 0) {
      return false;
    }

    _offset += 2 + (_count / 60);
    _count--;
    return true;
  }

private:
  std::vector<Texture2D> _images;
  int _x;
  int _offset;
  int _max;
  int _count;
};
