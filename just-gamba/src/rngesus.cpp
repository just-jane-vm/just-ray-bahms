#include "config_parser.hpp"
#include "thing.h"
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

std::vector<Thing> Shuffle(std::vector<Thing> textures) {
  std::random_device rd;
  std::mt19937 generator(rd());
  std::shuffle(textures.begin(), textures.end(), generator);

  return textures;
}

std::vector<Thing> GetThings() {
  std::string path =
      "/home/jane/just-stream/just-ray-bahms/just-gamba/config.txt";
  std::vector<Config> configs = ParseConfig(path.c_str());
  std::vector<Thing> things;

  for (Config c : configs) {
    Texture2D texture = LoadTexture(c.path.c_str());
    things.push_back(Thing(texture, c));
  }

  return things;
}

Config GetRandomConfig(std::vector<Config> items) {
  std::vector<double> weights;
  for (Config c : items) {
    weights.push_back(c.probability);
  }

  std::random_device rd;
  std::mt19937 gen(rd());
  std::discrete_distribution<> d(weights.begin(), weights.end());
  return items[d(gen)];
}
