#include "thing.h"
#include <vector>

int RNG(int min, int max);
std::vector<Thing> Shuffle(std::vector<Thing> textures);
std::vector<Thing> GetThings();
Config GetRandomConfig(std::vector<Config> items);
