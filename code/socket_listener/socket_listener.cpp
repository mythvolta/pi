// socket_listener.cpp

// Listen for messages on a socket in a new thread

#include <algorithm>
#include <chrono>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

#include <cctype>
#include <cstring>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

// Method to see if a character is printable
/*
bool IsUnprintable(char c) {
  cout << "c = " << c << " = " << static_cast<unsigned char>(c) << " = " << isprint(static_cast<unsigned char>(c)) << "\n";
  return (isprint(static_cast<unsigned char>(c) == 0));
}
*/

// Class
class SocketListener {
 public:
  SocketListener(uint16_t port_number=8243);
  ~SocketListener();
  bool isMessage();
  string getMessage();

 private:
  // Constants
  const static uint16_t BUFSIZE = 32;

  // Member functions
  int spawnThread();
  void listen();
  bool addMessage(char buf[], int len);

  // Member variables
  uint16_t port;
  queue<string> messageQueue;
  mutex messageQueueLock;
};

// Public Interface
SocketListener::SocketListener(uint16_t port_number) : port(port_number) {
  cout << "Listening for messages on port " << port << "\n";
  spawnThread();
}
SocketListener::~SocketListener() {
  cout << "Done listening for messages on port " << port << "\n";
}
bool SocketListener::isMessage() {
  return (!messageQueue.empty());
}
string SocketListener::getMessage() {
  // Make sure we have a message in the queue
  if (messageQueue.empty()) {
    return ""; // nullptr;
  }

  // Lock the queue while we're removing from it
  lock_guard<mutex> lk(messageQueueLock);
  
  // Return the first message in the queue
  string s = messageQueue.front();
  messageQueue.pop();
  return s;
}
// -------------------------------------

// Spawn another thread to listen to UDP messages
int SocketListener::spawnThread() {
  listen();
  return 0;
}

// Actually listen to UDP messages
void SocketListener::listen() {
  cout << "Not yet.\n";

  // Create a UDP socket
  int fd = socket(AF_INET, SOCK_DGRAM, 0);
  cout << "File descriptor = " << fd << "\n";
  if (fd < 0) {
    cerr << "Could not create a UDP socket!\n";
    return;
  }

  // No long timeout after we kill the process
  int optval = 1;
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));

  // Clear the socket address
  sockaddr_in addr;
  socklen_t len = sizeof(addr);
  memset(&addr, 0, len);
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(port);
  //addr.sin_port = 0;

  // Associate the socket with the port
  if (bind(fd, (sockaddr *)&addr, len) < 0) {
    cerr << "ERROR on binding\n";
    return;
  }
  cout << "bound to port " << addr.sin_port << " = " << htons(addr.sin_port) << "\n";

  // Show the data
  char buf[BUFSIZE];
  int bytes_read;
  bool success = true;
  while (success) {
    bytes_read = recvfrom(fd, buf, BUFSIZE, 0, (struct sockaddr *)&addr, &len);
    if (bytes_read < 0) {
      cerr << "ERROR in recvfrom\n";
			return;
    }

    // Add this message to the queue
    success = addMessage(buf, bytes_read);
  }

  //this_thread::sleep_for(chrono::seconds(20));
  cout << "File descriptor was " << fd << "\n";
}

// Manipulate the message queue
bool SocketListener::addMessage(char buf[], int len) {
  // Lock the queue while we're adding to it
  lock_guard<mutex> lk(messageQueueLock);

  // Push the new message onto the queue, ignoring the newlines
  if (buf[len - 1] == '\n') {
    --len;
  }
  string s(buf, buf + len);
  //replace_if(s.begin(), s.end(), IsUnprintable, 'X');
  messageQueue.push(s);
  
  // Check for exit condition
  if (s == "exit") {
    return false;
  }
  
  cout << s.size() << " : " << s << "\n";
  return true;
}

int main() {
  cout << "Creating a SocketListener\n";
  auto s = make_unique<SocketListener>(1234);
  cout << "Done, here are the messages:\n";
  int i = 0;
  while (s->isMessage()) {
    cout << ++i << " : [" << s->getMessage() << "]\n";
  }
}
