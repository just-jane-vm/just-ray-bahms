#include "raylib.h"
#include <algorithm>
#include <chrono>
#include <random>
#include <vector>

int RNG(int min, int max) {
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::mt19937 engine(seed); // Mersenne Twister 32-bit engine
  std::uniform_int_distribution<int> dist(min, max);
  return dist(engine);
}

std::vector<Texture2D> Shuffle(std::vector<Texture2D> textures) {
  std::random_device rd;
  std::mt19937 generator(rd());
  std::shuffle(textures.begin(), textures.end(), generator);

  return textures;
}
