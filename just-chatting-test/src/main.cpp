#include "raylib.h"
#include "twitch_chat_stream.hpp"

#define TRANSPARENT CLITERAL(Color){0x00, 0x00, 0x00, 0x00}

#define WINDOW_HEIGHT 1080.0
#define WINDOW_WIDTH 1920.0

auto channel =
    grpc::CreateChannel("127.0.0.1:50051", grpc::InsecureChannelCredentials());

TwitchChatStream stream(channel, [](const twapi::TwitchChat &chat) {
  // Minimal: print
  std::cout << "[" << chat.user_display_name() << "] " << chat.msg_content()
            << "\n";

  // You also have:
  // chat.user_color(), chat.message_bits(), chat.raw_message(),
  // chat.emotes(i)...
});

// ... your raylib loop runs here ...

// You look great today! - Red_Epicness
int main() {
  SetConfigFlags(FLAG_WINDOW_TRANSPARENT | FLAG_WINDOW_ALWAYS_RUN);
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "just-whatever");
  SetTargetFPS(60);
  stream.Start();

  while (!WindowShouldClose()) {

    BeginDrawing();
    ClearBackground(TRANSPARENT);

    DrawText("hello world", 0, 0, 32, WHITE);

    EndDrawing();
  }

  stream.Stop();
  CloseWindow();
  return 0;
}
