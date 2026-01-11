#include "pellet.h"
#include "puckman.h"
#include "raylib.h"
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>

#define COLOR_WALL CLITERAL(Color){0x00, 0x00, 0x00, 0xff}
#define COLOR_GROUND CLITERAL(Color){0xff, 0xff, 0xff, 0xff}
#define TILE_SIZE 32

enum class TileType { UNSET, WALL, OPEN, PLAYER_START };
enum class Direction {
  UNSET,
  NORTH,
  SOUTH,
  EAST,
  WEST,
  NORTH_EAST,
  SOUTH_EAST,
  NORTH_WEST,
  SOUTH_WEST
};

struct Whatever {
  int Row;
  int Col;
};

static const std::unordered_map<char, TileType> tileTypes = {
    {'*', TileType::WALL},
    {'.', TileType::OPEN},
    {'@', TileType::PLAYER_START}};

static const std::unordered_map<Direction, Whatever> directionMapThing = {
    {Direction::EAST, Whatever{.Row = 0, .Col = 1}},
    {Direction::WEST, Whatever{.Row = 0, .Col = -1}},
    {Direction::SOUTH, Whatever{.Row = 1, .Col = 0}},
    {Direction::NORTH, Whatever{.Row = -1, .Col = 0}},
    {Direction::NORTH_EAST, Whatever{.Row = -1, .Col = 1}},
    {Direction::NORTH_WEST, Whatever{.Row = -1, .Col = -1}},
    {Direction::SOUTH_EAST, Whatever{.Row = 1, .Col = 1}},
    {Direction::SOUTH_WEST, Whatever{.Row = 1, .Col = -1}},
};

class Map {
public:
  int Width;
  int Height;
  Vector2 PlayerStartPosition;
  std::vector<Pellet> Pellets;

  // this is a load bearing comment - JanMarten_Jongerius
  Map(const char *filePath) {
    std::ifstream inputFile(filePath);

    if (!inputFile.is_open()) {
      std::cerr << "err opening file: " << filePath << std::endl;
      throw std::invalid_argument("something has exploded, oh no!");
    }

    std::string line;
    int i = 0;
    int j = 0;
    while (std::getline(inputFile, line)) {
      i = 0;
      std::vector<TileType> row;
      for (char c : line) {
        TileType t = tileTypes.at(c);
        if (t == TileType::PLAYER_START) {
          PlayerStartPosition.x = j * TILE_SIZE + (TILE_SIZE / 2);
          PlayerStartPosition.y = i * TILE_SIZE + (TILE_SIZE / 2);
        }

        if (t == TileType::OPEN) {
          float x = float(i) * TILE_SIZE + (TILE_SIZE / 2);
          float y = float(j) * TILE_SIZE + (TILE_SIZE / 2);
          Pellets.push_back(Pellet(Vector2{.x = x, .y = y}));
        }

        row.push_back(t);
        i += 1;
        this->Width = i;
      }

      this->map.push_back(row);
      j += 1;
    }

    this->Height = j;
  }

  void Draw() {
    int i = 0;
    int j = 0;
    for (std::vector<TileType> row : this->map) {
      i = 0;
      for (TileType tile : row) {
        switch (tile) {
        case TileType::WALL:
          DrawRectangle(i * TILE_SIZE, j * TILE_SIZE, TILE_SIZE, TILE_SIZE,
                        COLOR_WALL);
          break;
        case TileType::PLAYER_START:
        case TileType::OPEN:
          DrawRectangle(i * TILE_SIZE, j * TILE_SIZE, TILE_SIZE, TILE_SIZE,
                        COLOR_GROUND);
          break;
        case TileType::UNSET:
          std::cerr << "erm what the glerm" << i << " " << j << std::endl;
          break;
        }
        i += 1;
      }
      j += 1;
    }

    for (Pellet pellet : Pellets) {
      pellet.Draw();
    }
  }

  bool TryMove(PuckMan *puck, Vector2 position) {
    if (CollidesWithWall(position, puck->Radius)) {
      return false;
    }

    puck->Position = position;

    for (int i; i < Pellets.size(); i += 1) {
      Pellet pellet = Pellets[i];
      if (puck->Position.x == pellet.Position.x &&
          puck->Position.y == pellet.Position.y) {
        Pellets.erase(Pellets.begin() + i);
        break;
      }
    }

    return true;
  }

private:
  std::vector<std::vector<TileType>> map;
  std::vector<Whatever> GetNeighbors(Vector2 point) {
    Whatever whateverPoint = Whatever{
        .Row = int((point.y - (TILE_SIZE / 2)) / TILE_SIZE),
        .Col = int((point.x - (TILE_SIZE / 2)) / TILE_SIZE),
    };

    std::vector<Whatever> neighbors;
    neighbors.push_back(whateverPoint);

    for (std::pair<Direction, Whatever> thing : directionMapThing) {
      neighbors.push_back(Whatever{
          .Row = thing.second.Row + whateverPoint.Row,
          .Col = thing.second.Col + whateverPoint.Col,
      });
    }

    return neighbors;
  }

  bool CollidesWithWall(Vector2 origin, float radius) {
    std::vector<Whatever> neighbors = this->GetNeighbors(origin);

    for (Whatever tile : neighbors) {
      if (tile.Row < 0) {
        continue;
      }

      if (tile.Col < 0) {
        continue;
      }

      if (tile.Col >= this->map[0].size()) {
        continue;
      }

      if (tile.Row >= this->map.size()) {
        continue;
      }

      if (this->map[tile.Row][tile.Col] != TileType::WALL) {
        continue;
      }

      float x = tile.Col * TILE_SIZE;
      float y = tile.Row * TILE_SIZE;

      if (CheckCollisionCircleRec(
              origin, radius,
              Rectangle{
                  .x = x, .y = y, .width = TILE_SIZE, .height = TILE_SIZE})) {
        return true;
      };
    }

    return false;
  }
};
