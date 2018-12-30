// mythclock.cpp
//
// Clock that also shows messages received via UDP
//
// This code is public domain
// (but note, that the led-matrix library this depends on is GPL v2)

#include "mythclock.h"
#include "socket_listener.h"

#include <chrono>
#include <iostream>
#include <fstream>
#include <memory>
#include <thread>

#include <getopt.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

using namespace std;

// Handle interrupts
volatile bool interrupt_received = false;
static void InterruptHandler(int signo) {
  interrupt_received = true;
}

// Launch the UDP listener in a new thread
void spawnThread(SocketListener* sock);

// Constructor
MythClock::MythClock() : 
    matrix_path("/home/pi/code/rpi-rgb-led-matrix"),
    font_large(matrix_path + "/fonts/6x13B.bdf"),
    font_small(matrix_path + "/fonts/6x13B.bdf"),
    font_small_width(6),
    time_format("%H%M"), x_offset(2), y_offset(2), x_scroll(0),
    bg_color(0, 0, 0), outline_color(0, 0, 32), font_color(0, 128, 0),
    old_hour(-1), old_min(-1), old_sec(-1), old_intensity(-1),
    text_string("mythclock"), text_string_start(0), text_offset(0),
    show_message(true) {
  // Load font
  font.LoadFont(font_large.c_str());
  outline_font = font.CreateOutlineFont();
 
  // Set the inital message
  setMessage("mythclock");

  // Signal handlers
  signal(SIGTERM, InterruptHandler);
  signal(SIGINT, InterruptHandler);
}

// Actually create the RGB matrix
bool MythClock::createMatrix() {
  // Create the RGB Matrix
  matrix = rgb_matrix::CreateMatrixFromOptions(matrix_options, runtime_opt);
  if (matrix == NULL) {
    return false;
  }

  // Set the brightness and create the canvas
  matrix->SetBrightness(brightness);
  offscreen = matrix->CreateFrameCanvas();

  // Set the initial time
  time_initial = time(NULL);
  next_time.tv_sec = time_initial;
  next_time.tv_nsec = 0;
  return true;
}


// Update the clock every loop
void MythClock::getNewTime() {
  localtime_r(&next_time.tv_sec, &tm);
}

// Perform other actions every time the clock changes
bool MythClock::changeOfSecond() {
  if (tm.tm_sec == old_sec) {
    return false;
  }

  // Update the clock_buffer with the time
  strftime(clock_buffer, sizeof(clock_buffer), time_format, &tm);

  old_sec = tm.tm_sec;
  return true;
}
bool MythClock::changeOfMinute() {
  if (tm.tm_min == old_min) {
    return false;
  }
  
  old_min = tm.tm_min;
  return true;
}
bool MythClock::changeOfHour() {
  if (tm.tm_hour == old_hour) {
    return false;
  }

  old_hour = tm.tm_hour;
  return true;
}

// Change the clock position on the hour
void MythClock::changeClockPosition() {
  // Change the clock position every hour
  x_offset = rand() % 4 + 1;
  y_offset = rand() % 6 - 1;
          
  // Get the new intensity
  font_intensity = intensity_hour[tm.tm_hour];
  if (font_intensity != old_intensity) {
    // Red only for the clock
    font_color = rgb_matrix::Color(font_intensity, 0, 0);
    if (font_intensity >= 80) {
      with_outline = true;
    }
    else {
      with_outline = false;
    }
    old_intensity = font_intensity;
  }

  // Show a log message
  logTime();
  cout << "Clock intensity " << font_intensity << " @ (" << x_offset << ", " << y_offset << ")" << endl;
}

// Set a message to display
void MythClock::setMessage(const string& s) {
  text_string = s;
  text_string_start = 0;
  show_message = true;
  x_scroll = -1;
}

// Show a message
void MythClock::showMessage() {
  // Make sure the clock is not still scrolling
  if (x_scroll < 0) {
    // If we have scrolled off the screen, then show the message
    if (x_scroll < -32) {
      // Bright blue for messages
      font_color = rgb_matrix::Color(32, 128, 128);
      //font.LoadFont(font_small.c_str());
      x_scroll = 48;
    }
    else {
      showClock();
      return;
    }
  }

  // Copy part of the text
  strcpy(text_buffer, text_string.substr(text_string_start, 8).c_str());

  // Clear the screen
  offscreen->Fill(bg_color.r, bg_color.g, bg_color.b);

  // Draw the message
  rgb_matrix::DrawText(offscreen, font, -text_offset, font.baseline(),
    font_color, NULL, text_buffer, 1);

  // Once text_offset reaches font_small_width, go to the next letter
  if (++text_offset > font_small_width) {
    // Log whether or not we are done with the message
    logTime();

    // Reset to clock mode when the string is done
    if (++text_string_start > text_string.size()) {
      cout << "Text buffer is complete" << endl;
      font_color = rgb_matrix::Color(font_intensity, 0, 0);
      text_string_start = 0;
      text_offset = -32;
      next_time.tv_nsec = 0;
      //font.LoadFont(font_large.c_str());
      show_message = false;
    }
    else {
      cout << text_buffer << "(" << text_string_start << ")\n";
      text_offset = 0;
    }
  }
}

// Actually write to the screen
void MythClock::showClock() {
  // Clear the screen
  offscreen->Fill(bg_color.r, bg_color.g, bg_color.b);
 
  // Draw the outline
  if (with_outline) {
    rgb_matrix::DrawText(offscreen, *outline_font, x_offset - 1 + x_scroll, y_offset + font.baseline(),
      outline_color, NULL, clock_buffer, letter_spacing - 2);
  }

  // Draw the clock in red
  rgb_matrix::DrawText(offscreen, font, x_offset + x_scroll, y_offset + font.baseline(),
    font_color, NULL, clock_buffer, letter_spacing);
}

// Determine how long to sleep based on the mode
void MythClock::sleep() {
  if (isMessage() || x_scroll != 0) {
    // Wait 30 ms before showing the next part of the text
    next_time.tv_nsec += 30000000;
    if (next_time.tv_nsec >= 1000000000) {
      next_time.tv_nsec -= 1000000000;
      next_time.tv_sec += 1;
    }
    
    // Move the clock left if we are showing it
    if (x_scroll != 0 && (isMessage() == (x_scroll < 0))) {
      --x_scroll;
    }
  }
  else {
    // Wait 1 second before showing the next clock
    next_time.tv_sec += 1;
  }

  // Wait until we're ready to show it.
  clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &next_time, NULL);

  // Atomic swap with double buffer
  offscreen = matrix->SwapOnVSync(offscreen);
}

// Finished. Shut down the RGB matrix.
void MythClock::clear() {
  matrix->Clear();
  delete matrix;
}

// Print a log message with the time
void MythClock::logTime() {
  getNewTime();
  struct timeval time_now;
  gettimeofday(&time_now, NULL);
  printf("[%04d-%02d-%02d %02d:%02d:%02d.%06ld] : ", tm.tm_year + 1900,
    tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, time_now.tv_usec);
}

// Main function
int main(int argc, char *argv[]) {
  MythClock mc;

  // Create the matrix
  if (!mc.createMatrix()) {
    cerr << "Could not create the matrix!" << endl;
    return 1;
  }

  // Start listening for UDP messages
  mc.logTime();
  cout << "Creating a SocketListener, PID=" << getpid() << endl;
  const uint16_t udp_port = 23714;
  auto sock = make_unique<SocketListener>(udp_port);
  mc.logTime();
  cout << "Listening for UDP messages on port " << udp_port << endl;
  thread t(spawnThread, sock.get());

  // Loop forever
  while (!interrupt_received) {
    // Get the time
    mc.getNewTime();
    
    // Show the clock unless we have a message
    if (mc.isMessage()) {
      mc.showMessage();
    }
    else {
      // Handle changes of second/minute/hour
      if (mc.changeOfSecond()) {
        // Check for socket messages every second
        if (sock->isMessage()) {
          const string socket_message = sock->getMessage();
          mc.logTime();
          cout << "UDP:" << udp_port << " message (" << socket_message << ")" << endl;
          mc.setMessage(socket_message);
        }

        // Only move the clock every hour
        if (mc.changeOfMinute()) {
          if (mc.changeOfHour()) {
            mc.changeClockPosition();
          }
        }
      }

      // Show the clock now
      mc.showClock();
    }

    // Sleep different amounts depending on what we're doing
    mc.sleep();
  }

  // Clear everything
  mc.clear();
  mc.logTime();
  cout << "Terminating thread" << endl;
  t.detach();

  return 0;
}

// Actually listen in a new thread
void spawnThread(SocketListener* sock) {
  sock->listen();
}
