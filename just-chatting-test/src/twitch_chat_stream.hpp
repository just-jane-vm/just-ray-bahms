#include "twapi.grpc.pb.h"

#include <grpcpp/grpcpp.h>

#include <atomic>
#include <functional>
#include <memory>
#include <thread>

class TwitchChatStream {
public:
  using ChatCallback = std::function<void(const twapi::TwitchChat &)>;

  TwitchChatStream(std::shared_ptr<grpc::Channel> channel, ChatCallback on_chat)
      : stub_(twapi::TwitchEvents::NewStub(std::move(channel))),
        on_chat_(std::move(on_chat)) {}

  void Start() {
    if (running_.exchange(true))
      return;
    worker_ = std::thread([this] { Run(); });
  }

  void Stop() {
    if (!running_.exchange(false))
      return;

    ctx_.TryCancel();

    cq_.Shutdown();
    if (worker_.joinable())
      worker_.join();
  }

  ~TwitchChatStream() { Stop(); }

private:
  enum class Tag : uint8_t { kStart, kRead, kFinish };

  void Run() {
    twapi::SubscribeChatRequest req;

    reader_ = stub_->AsyncSubscribeChats(&ctx_, req, &cq_, (void *)Tag::kStart);

    void *tag = nullptr;
    bool ok = false;

    while (cq_.Next(&tag, &ok)) {
      auto t = static_cast<Tag>(reinterpret_cast<uintptr_t>(tag));

      switch (t) {
      case Tag::kStart: {
        if (!ok) {
          // Start failed; request final status.
          reader_->Finish(&status_, (void *)Tag::kFinish);
          break;
        }
        // Start succeeded; begin reading messages.
        reader_->Read(&msg_, (void *)Tag::kRead);
        break;
      }

      case Tag::kRead: {
        if (!ok) {
          // Server closed stream (or canceled). Now finalize status.
          reader_->Finish(&status_, (void *)Tag::kFinish);
          break;
        }

        // Deliver the message.
        if (on_chat_)
          on_chat_(msg_);

        // Read next message.
        reader_->Read(&msg_, (void *)Tag::kRead);
        break;
      }

      case Tag::kFinish: {
        // Stream is fully finished.
        // status_ tells you if it ended cleanly or errored/canceled.
        return;
      }
      }
    }
  }

  std::unique_ptr<twapi::TwitchEvents::Stub> stub_;
  ChatCallback on_chat_;

  grpc::ClientContext ctx_;
  grpc::CompletionQueue cq_;
  std::unique_ptr<grpc::ClientAsyncReader<twapi::TwitchChat>> reader_;

  twapi::TwitchChat msg_;
  grpc::Status status_;

  std::atomic<bool> running_{false};
  std::thread worker_;
};
