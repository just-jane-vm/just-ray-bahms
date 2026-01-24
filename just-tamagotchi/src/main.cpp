#include "raylib.h"

#include "cookie.h"
#include "tama.h"

#define TRANSPARENT CLITERAL(Color){0x00, 0x00, 0x00, 0x00}

#define WINDOW_HEIGHT 250.0
#define WINDOW_WIDTH 180.0

#include <condition_variable>
#include <hiredis/hiredis.h> // Example using hiredis
#include <mutex>
#include <queue>
#include <thread>

std::queue<std::string> eventQueue;
std::mutex mtx;
std::condition_variable cv;
bool running = true;

// 1. Worker Thread Function
void redisSubscriberThread() {
  redisContext *c = redisConnect("127.0.0.1", 6379);
  redisReply *reply = (redisReply *)redisCommand(c, "SUBSCRIBE mychannel");
  freeReplyObject(reply);

  while (running) {
    if (redisGetReply(c, (void **)&reply) == REDIS_OK) {
      // Check if message, push to queue
      if (reply->type == REDIS_REPLY_ARRAY && reply->elements == 3) {
        std::string msg = reply->element[2]->str;
        {
          std::lock_guard<std::mutex> lock(mtx);
          eventQueue.push(msg); // 2. Push Event
        }
        cv.notify_one(); // 3. Notify Main Thread
      }
      freeReplyObject(reply);
    }
  }
  redisFree(c);
}

int main() {
  std::thread subThread(redisSubscriberThread);
  Color screen = {200, 200, 200, 128};
  SetConfigFlags(FLAG_WINDOW_TRANSPARENT | FLAG_WINDOW_ALWAYS_RUN);
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "just-tamagotchi");
  SetTargetFPS(60);

  Tama tama = Tama(WINDOW_WIDTH, WINDOW_HEIGHT, "juniper");
  Image cookieImage = LoadImage("/home/jane/just-stream/just-ray-bahms/"
                                "just-tamagotchi/assets/cookie.png");
  int animationStep = 0;

  Image egg = LoadImage(
      "/home/jane/just-stream/just-ray-bahms/just-tamagotchi/assets/egg.png");
  Texture2D bg = LoadTextureFromImage(egg);

  Rectangle gameZone = Rectangle{.x = 28, .y = 64, .width = 62, .height = 60};
  Rectangle gameZone2 =
      Rectangle{.x = 27, .y = 64, .width = 124, .height = 120};
  Lightning bolt = Lightning(gameZone);

  int count = 0;
  while (!WindowShouldClose()) {
    count += 1;
    tama.Update();

    BeginDrawing();
    ClearBackground(TRANSPARENT);

    DrawTexture(bg, 0, 0, WHITE);
    if (bolt.Update()) {
      bolt.Draw();
      DrawRectangleRec(gameZone2, screen);
    }

    tama.Draw();
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
