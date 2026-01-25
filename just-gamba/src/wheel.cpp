#include <iostream>
#include <string>
#include <vector>

#include "wheel.h"

std::string GetWinner(std::vector<Wheel *> wheels) {
  std::cout << wheels.size() << std::endl;

  if (wheels[0]->_winner.label == wheels[1]->_winner.label &&
      wheels[1]->_winner.label == wheels[2]->_winner.label) {
    return wheels[0]->_winner.win3;
  }

  if (wheels[0]->_winner.label == wheels[1]->_winner.label) {
    return wheels[0]->_winner.win2;
  }

  if (wheels[1]->_winner.label == wheels[2]->_winner.label) {
    return wheels[1]->_winner.win2;
  }

  if (wheels[0]->_winner.label == wheels[2]->_winner.label) {
    return wheels[0]->_winner.win2;
  }

  return "";
}
