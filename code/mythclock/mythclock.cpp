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
    font_small(matrix_path + "/fonts/5x7.bdf"),
    time_format("%H%M"),
    x_offset(0), y_offset(0), font_intensity(128), with_outline(true),
    bg_color(0, 0, 0), outline_color(0, 0, 32), font_color(0, font_intensity, font_intensity),
    old_hour(-1), old_min(-1), old_sec(-1), old_intensity(-1),
    text_string("mythclock"), text_string_start(0), text_offset(-32), show_clock(false) {
  cout << "MythClock()\n";
  // Set the initial time
  time_initial = time(NULL);
  next_time.tv_sec = time_initial;
  next_time.tv_nsec = 0;

  // Load font
  font.LoadFont(font_large.c_str());
  outline_font = font.CreateOutlineFont();
 
  // Set the inital message
  setMessage("mythclock");

  // Signal handlers
  signal(SIGTERM, InterruptHandler);
  signal(SIGINT, InterruptHandler);
}

// Destructor
MythClock::~MythClock() {
  cout << "~MythClock()\n";
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
  return true;
}

// Whether we are showing the clock or a message
bool MythClock::showClock() {
  return show_clock;
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
    font_color = rgb_matrix::Color(font_intensity, 0, 0);
    if (font_intensity >= 80) {
      with_outline = true;
    }
    else {
      with_outline = false;
    }
    old_intensity = font_intensity;
  }
  
  printf("[%02d:%02d:%02d] i=%d o=%d @ (%d, %d)\n", tm.tm_hour, tm.tm_min, tm.tm_sec,
    font_intensity, with_outline, x_offset, y_offset);
}

// Set a message to display
void MythClock::setMessage(const std::string& s) {
  text_string = s;
  text_string_start = 0;
  font_color = rgb_matrix::Color(font_intensity, 0, font_intensity);
  font.LoadFont(font_small.c_str());
  show_clock = false;
}

// Actually write to the screen
void MythClock::writeToScreen() {
  // Write to the screen
  offscreen->Fill(bg_color.r, bg_color.g, bg_color.b);
  if (with_outline) {
    rgb_matrix::DrawText(offscreen, *outline_font, x_offset - 1, y_offset + font.baseline(),
      outline_color, NULL, clock_buffer, letter_spacing - 2);
  }

  rgb_matrix::DrawText(offscreen, font, x_offset, y_offset + font.baseline(),
    font_color, NULL, clock_buffer, letter_spacing);
}

// Show a message
void MythClock::showMessage() {
  // Copy part of the text
  strcpy(text_buffer, text_string.substr(text_string_start, 8).c_str());
  strftime(clock_buffer, sizeof(clock_buffer), "%H%M%S", &tm);

  // Write to the screen
  offscreen->Fill(bg_color.r, bg_color.g, bg_color.b);
  rgb_matrix::DrawText(offscreen, font, -text_offset, font.baseline(),
    font_color, NULL, text_buffer, 0);
  rgb_matrix::DrawText(offscreen, font, 0, font.baseline() + 8,
    font_color, NULL, clock_buffer, 0);

  // Once text_offset reaches font_small_width, go to the next letter
  if (++text_offset >= font_small_width) {
    printf("[%02d:%02d:%02d] : ", tm.tm_hour, tm.tm_min, tm.tm_sec);

    // Reset to clock mode when the string is done
    if (++text_string_start > text_string.size()) {
      std::cout << "Text buffer is complete" << std::endl;
      font_color = rgb_matrix::Color(font_intensity, 0, 0);
      text_string_start = 0;
      text_offset = -32;
      next_time.tv_nsec = 0;
      font.LoadFont(font_large.c_str());
      show_clock = true;
    }
    else {
      std::cout << "Text buffer[" << text_string_start << "] = (" << text_buffer << ")" << std::endl;
      text_offset = 0;
    }
    getNewTime();
    printf("[%02d:%02d:%02d] : ", tm.tm_hour, tm.tm_min, tm.tm_sec);
    cout << "Done updating\n";
  }
}

// Determine how long to sleep based on the mode
void MythClock::sleep() {
  if (showClock()) {
    // Wait 1 second before showing the next clock
    next_time.tv_sec += 1;
  }
  else {
    // Wait 100 ms before showing the next part of the text
    next_time.tv_nsec += 100000000;
    if (next_time.tv_nsec >= 1000000000) {
      next_time.tv_nsec -= 1000000000;
      next_time.tv_sec += 1;
    }
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

// Main function
int main(int argc, char *argv[]) {
  MythClock mc;

  // Create the matrix
  if (!mc.createMatrix()) {
    return 1;
  }

  cout << "Creating a SocketListener, PID=" << getpid() << "\n";
  auto sock = make_unique<SocketListener>(1234);
  cout << "Listening for UDP message in a new thread\n";
  thread t(spawnThread, sock.get());

  // Loop forever
  while (!interrupt_received) {
    // Get the time
    mc.getNewTime();
    
    // Either show the date or a scrolling message
    if (mc.showClock()) {
      // Handle changes of second/minute/hour
      if (mc.changeOfSecond()) {
        // Check for socket messages
        if (sock->isMessage()) {
          mc.setMessage(sock->getMessage());
        }

        if (mc.changeOfMinute()) {
          if (mc.changeOfHour()) {
            mc.changeClockPosition();
          }
        }
      }
      
      // Actually write to the screen every loop
      mc.writeToScreen();
    }
    else {
      // Show a message
      mc.showMessage();
    }

    // Sleep different amounts depending on what we're doing
    mc.sleep();
  }

  mc.clear();

  cout << "Terminating thread\n";
  t.detach();

  // Create a fresh new line after ^C on screen
  cout << endl;
  //write(STDOUT_FILENO, "\n", 1);

  return 0;
}

// Check for a file
/*
bool check_for_file(std::string &s) {
  // Make sure the file exists
  std::ifstream fin(status_filename.c_str());
  if (fin.fail()) {
    return false;
  }
  
  // Read the first line of the file
  std::getline(fin, s);

  // Close the file and delte it
  fin.close();
  if (remove(status_filename.c_str()) != 0) {
    std::cout << "Failed to remove " << status_filename << std::endl;
  }
  else {
    std::cout << "Successfully removed " << status_filename << std::endl;
  }

  return true;
}
*/

// Actually listen in a new thread
void spawnThread(SocketListener* sock) {
  sock->listen();
}
