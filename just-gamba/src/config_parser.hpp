#include <string>
#include <vector>

#ifndef CONFIG
#define CONFIG

struct Config {
  float probability;
  std::string path;
  std::string label;
};

std::vector<Config> ParseConfig(const char *filePath);

#endif
