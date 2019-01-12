// socket_listener.h

#pragma once

// Listen for messages on a socket in a new thread
#include <mutex>
#include <queue>
#include <string>

// Class
class SocketListener {
 public:
  SocketListener(uint16_t port_number=8243);
  ~SocketListener();
  inline bool isMessage() {
    return !messageQueue.empty();
  }
  std::string getMessage();
  void listen();

 private:
  // Constants
  const static uint16_t BUFSIZE = 128; // Increase the buffer size

  // Member functions
  bool addMessage(char buf[], int len);

  // Member variables
  uint16_t port;
  std::queue<std::string> messageQueue;
  std::mutex messageQueueLock;
};
