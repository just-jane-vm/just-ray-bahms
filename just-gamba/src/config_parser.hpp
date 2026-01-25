#include <string>
#include <vector>

#ifndef CONFIG
#define CONFIG

struct Config {
  float probability;
  std::string path;
  std::string label;
  std::string win2;
  std::string win3;
};

std::vector<Config> ParseConfig(const char *filePath);

#endif
