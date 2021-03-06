// socket_listener.cpp

// Listen for messages on a socket in a new thread

#include "socket_listener.h"

#include <algorithm>
#include <iostream>
#include <memory>

#include <cctype>
#include <cstring>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

/*
bool IsUnprintable(char c) {
  cout << "c = " << c << " = " << static_cast<unsigned char>(c) << " = " << isprint(static_cast<unsigned char>(c)) << "\n";
  return (isprint(static_cast<unsigned char>(c) == 0));
}
*/

// Constructor/Destructor
SocketListener::SocketListener(uint16_t port_number) : port(port_number) {}
SocketListener::~SocketListener() = default;

// Get a message if there is one
string SocketListener::getMessage() {
  // Make sure we have a message in the queue
  if (!isMessage()) {
    return "";
  }

  // Return the first message in the queue,
  //  being sure to lock the queue beforehand
  lock_guard<mutex> lk(messageQueueLock);
  string s = messageQueue.front();
  messageQueue.pop();
  return s;
}
// -------------------------------------

// Actually listen to UDP messages
void SocketListener::listen() {
  // Create a UDP socket
  int fd = socket(AF_INET, SOCK_DGRAM, 0);
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
}

// Manipulate the message queue
bool SocketListener::addMessage(char buf[], int len) {
  // Push the new message onto the queue, ignoring the newlines
  if (buf[len - 1] == '\n') {
    --len;

    // Short circuit if all we had was a newline
    if (len <= 0) {
      return true;
    }
  }

  // Create the string
  string s(buf, buf + len);
  //replace_if(s.begin(), s.end(), IsUnprintable, 'X');
  cout << "UDP:" << port << " [" << s.size() << "] : " << s << "\n";

  // Lock the queue while we're adding to it
  lock_guard<mutex> lk(messageQueueLock);
  messageQueue.push(s);
  return true;
}
