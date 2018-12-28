// -*- mode: c++; c-basic-offset: 2; indent-tabs-mode: nil; -*-
// Example of a clock. This is very similar to the text-example,
// except that it shows the time :)
//
// This code is public domain
// (but note, that the led-matrix library this depends on is GPL v2)

#include "led-matrix.h"
#include "graphics.h"
#include "socket_listener.h"

#include <iostream>
#include <fstream>

#include <getopt.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

using namespace std;

// Global constants for the paths
const static std::string matrix_path = "/home/pi/code/rpi-rgb-led-matrix";
const static std::string status_filename = "/home/pi/toolbox/code/mythclock/mythclock.status";

// Function to read the status file, if it exists
bool check_for_file(std::string &s);

// Actually listen in a new thread
void spawnThread(SocketListener* sock);

// Set the color intensity at every hour
const int intensity_hour[24] = {24, 24, 24, 24, 24, 24, 32, 36, 40, 48, 64, 96, 128, 128, 128, 128, 96, 64, 48, 36, 32, 24, 24, 24};
//                              00                      06                       12                         18                  23

// Interrupt handling
volatile bool interrupt_received = false;
static void InterruptHandler(int signo) {
  interrupt_received = true;
}

// Main function
int main(int argc, char *argv[]) {
  // Don't read any command-line arguments
  rgb_matrix::RGBMatrix::Options matrix_options;
  rgb_matrix::RuntimeOptions runtime_opt;

  // Some constants
  const std::string font_large = matrix_path + "/fonts/6x13B.bdf";
  //const std::string font_small = matrix_path + "/fonts/4x6.bdf";
  const std::string font_small = matrix_path + "/fonts/5x7.bdf";
  const int font_small_width = 4 + 1;
  const char *time_format = "%H%M";

  // These can change every hour
  int x_offset = 0;
  int y_offset = 0;
  int font_intensity = 128;

  // These are constant
  const int brightness = 20;
  const int letter_spacing = 1;

  // Vector of colors
  rgb_matrix::Color font_color(0, font_intensity, font_intensity); // Default scroll is teal
  rgb_matrix::Color outline_color(0, 0, 32); // Outline is always a weak blue
  const rgb_matrix::Color bg_color(0, 0, 0); // Background is always black
  bool with_outline = true;

  // Load font
  rgb_matrix::Font font;
  font.LoadFont(font_large.c_str());
  rgb_matrix::Font *outline_font = font.CreateOutlineFont();
  font.LoadFont(font_small.c_str());

  // Create the matrix
  rgb_matrix::RGBMatrix *matrix = rgb_matrix::CreateMatrixFromOptions(matrix_options,
                                                          runtime_opt);
  if (matrix == NULL) {
    return 1;
  }
  matrix->SetBrightness(brightness);
  rgb_matrix::FrameCanvas *offscreen = matrix->CreateFrameCanvas();

  char text_buffer[256];
  char clock_buffer[256];
  struct timespec next_time;
  time_t time_initial = time(NULL);
  next_time.tv_sec = time_initial;
  next_time.tv_nsec = 0;
  struct tm tm;

  // Signal handlers
  signal(SIGTERM, InterruptHandler);
  signal(SIGINT, InterruptHandler);

  // Compare to the previous clock time
  int old_hour = -1;
  int old_min = -1;
  int old_sec = -1;
  int old_intensity = -1;

  // Start with a string instead of the clock 
  std::string text_string = "mythclock";
  unsigned int text_string_start = 0;
  int text_offset = -32;
  bool show_clock = false;
  
  cout << "Creating a SocketListener, PID=" << getpid() << "\n";
  auto sock = make_unique<SocketListener>(1234);

  cout << "Listening for UDP message in a new thread\n";
  thread t(spawnThread, sock.get());

  // Loop forever
  string s;
  while (!interrupt_received) {
    // Get the time
    localtime_r(&next_time.tv_sec, &tm);
    
    // Either show the date or a scrolling message
    if (show_clock) {
      strftime(clock_buffer, sizeof(clock_buffer), time_format, &tm);

      // Handle changes of second
      if (tm.tm_sec != old_sec) {
        if (sock->isMessage()) {
          do {
            s = sock->getMessage();
            cout << " : [" << s << "]\n";
            if (s == "exit") {
              running = false;
              break;
            }
          } while (sock->isMessage());
        }
        else {
          cout << "No new messages\n";
        }
          
        // Handle changes of minute
        if (tm.tm_min != old_min) {
          // Handle changes of hour
          if (tm.tm_hour != old_hour) {
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

            old_hour = tm.tm_hour;
          }

          // Check for files every half hour
          if (tm.tm_min % 30 == 0) {
            printf("[%02d:%02d:%02d] : ", tm.tm_hour, tm.tm_min, tm.tm_sec);
            if (check_for_file(text_string)) {
              std::cout << "Clock mode disabled, text is (" << text_string << ")" << std::endl;
              //next_time.tv_sec += 1;
              font_color = rgb_matrix::Color(font_intensity, 0, font_intensity);
              text_string_start = 0;
              font.LoadFont(font_small.c_str());
              show_clock = false;
            }
            else {
              std::cout << "Clock mode still enabled, text is (" << text_string << ")" << std::endl;
            }
          }

          old_min = tm.tm_min;
        }

        old_sec = tm.tm_sec;
      }
  
      // Write to the screen
      offscreen->Fill(bg_color.r, bg_color.g, bg_color.b);
      if (with_outline) {
        rgb_matrix::DrawText(offscreen, *outline_font, x_offset - 1, y_offset + font.baseline(),
          outline_color, NULL, clock_buffer, letter_spacing - 2);
      }
      rgb_matrix::DrawText(offscreen, font, x_offset, y_offset + font.baseline(),
        font_color, NULL, clock_buffer, letter_spacing);
    }
    else {
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
      }
    }

    // Sleep different amounts depending on what we're doing
    if (show_clock) {
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
  matrix->Clear();
  delete matrix;
  
  cout << "Terminating thread\n";
  t.detach();

  // Create a fresh new line after ^C on screen
  write(STDOUT_FILENO, "\n", 1);

  return 0;
}

// Check for a file
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

// Actually listen in a new thread
void spawnThread(SocketListener* sock) {
  sock->listen();
}

// Main function
int main() {
