#include <cstddef>
#include <fstream>
#include <iostream>
#include <sw/redis++/redis++.h>
#include <sw/redis++/redis.h>

class Payouts {
public:
  Payouts(std::string userID) {
    _userID = userID;
    LoadEnv();
  }

  void GiveTTSTokens(float tokenCount) {
    sw::redis::ConnectionOptions opts;
    std::string port = getenv("PORT");
    opts.port = std::stoi(port);
    opts.host = getenv("HOST");
    opts.password = getenv("PASS");

    sw::redis::Redis redis(opts);
    redis.incrbyfloat("tts-bank/user/" + _userID, tokenCount);
  }

private:
  std::string _userID;

  void LoadEnv(
      const std::string &path = "/home/jane/just-stream/just-ray-bahms/.env") {
    std::ifstream file(path);
    std::string line;

    if (!file.is_open()) {
      std::cerr << "Warning: Could not open .env file at " << path << std::endl;
      return;
    }

    while (std::getline(file, line)) {
      if (line.empty() || line[0] == '#')
        continue;

      size_t delimiterPos = line.find('=');
      if (delimiterPos != std::string::npos) {
        std::string key = line.substr(0, delimiterPos);
        std::string value = line.substr(delimiterPos + 1);
        std::string envVar = key + "=" + value;
        setenv(key.c_str(), value.c_str(), 1);
      }
    }
  }
};
