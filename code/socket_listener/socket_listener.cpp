// socket_listener.cpp

// Listen for messages on a socket in a new thread

#include <iostream>
#include <memory>
#include <queue>

using namespace std;

class SocketListener {
 public:
  SocketListener(uint16_t port_number=8243) : port(port_number) {
    cout << "Listening for messages on port " << port << "\n";
    spawnThread();
  }
  ~SocketListener() {
    cout << "Done listening for messages on port " << port << "\n";
  }
  string getMessage() {
    // Make sure we have a message in the queue
    if (messageQueue.empty()) {
      return ""; // nullptr;
    }

    // Return the first message in the queue
    return "Potato";
  }

 private:
  // Member functions
  int spawnThread();
  void listen();

  // Member variables
  uint16_t port;
  queue<string> messageQueue;
};

// Spawn another thread to listen to UDP messages
int SocketListener::spawnThread() {
  listen();
}

// Actually listen to UDP messages
void SocketListener::listen() {
  cout << "Not yet.\n";
}

int main() {
  cout << "Creating a SocketListener\n";
  auto s = make_unique<SocketListener>(1234);
  cout << "Done\n";
}
