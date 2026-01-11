#include "config_parser.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

std::vector<Config> ParseConfig(const char *configPath) {
  std::ifstream inputFile(configPath);
  std::string line;
  std::vector<Config> icons;

  if (!inputFile.is_open()) {
    std::cerr << "err opening file: " << configPath << std::endl;
    throw std::invalid_argument("something has exploded, oh no!");
  }

  int count = 0;
  while (std::getline(inputFile, line)) {
    if (line.empty() || line[0] == '#') {
      continue;
    }

    std::stringstream ss(line);

    std::string label;
    std::string path;
    std::string probability;
    std::getline(ss, label, ',');
    std::getline(ss, path, ',');
    std::getline(ss, probability, ',');
    float p = std::stof(probability);
    icons.push_back(Config{.probability = p, .path = path, .label = label});
  }

  return icons;
}
