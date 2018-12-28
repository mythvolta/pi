// mythclock.h
//
// Clock that also shows messages received via UDP
//
// This code is public domain
// (but note, that the led-matrix library this depends on is GPL v2)

#pragma once

#include "led-matrix.h"
#include "graphics.h"

// Put most of the old variables in the class
class MythClock {
 public:
  MythClock();
  ~MythClock();
  // Initialize the matrix 
  bool createMatrix();
  void writeToScreen();
  void setMessage(const std::string& s);
  // Check to see what digit has changed
  bool showClock();
  void getNewTime();
  bool changeOfSecond();
  bool changeOfMinute();
  bool changeOfHour();
  // Randomize the clock position every hour
  void changeClockPosition();
  //bool check_for_file(std::string &s);
  // Show a message instead of a clock
  void showMessage();
  // Sleep for a certain amount of time
  void sleep();
  // Clear the matrix
  void clear();

 private:
  // Constants for the paths
  const std::string matrix_path;
  //const static std::string status_filename = "/home/pi/toolbox/code/mythclock/mythclock.status";
  // More constants for the font
  const std::string font_large;
  //const std::string font_small = matrix_path + "/fonts/4x6.bdf";
  const std::string font_small;
  const static int font_small_width = 4 + 1;
  const char *time_format;
  // Set the color intensity at every hour
  const int intensity_hour[24] = {24, 24, 24, 24, 24, 24, 32, 36, 40, 48, 64, 96, 128, 128, 128, 128, 96, 64, 48, 36, 32, 24, 24, 24};
  //                                     00                      06                       12                         18                  23
  // These are constant
  const int brightness = 20;
  const int letter_spacing = 1;
  
  // Data members
  rgb_matrix::RGBMatrix::Options matrix_options;
  rgb_matrix::RuntimeOptions runtime_opt;
  // These can change every hour
  int x_offset;
  int y_offset;
  int font_intensity;
  bool with_outline;
  // Vector of colors
  const rgb_matrix::Color bg_color; // Black (0, 0, 0);
  rgb_matrix::Color outline_color;  // Weak blue (0, 0, 32)
  rgb_matrix::Color font_color; // Default is teal (0, font_intensity, font_intensity)
  // Font
  rgb_matrix::Font font;
  rgb_matrix::Font *outline_font;
  // Actual RGB matrix
  rgb_matrix::RGBMatrix *matrix;
  rgb_matrix::FrameCanvas *offscreen;
  // Buffers
  char text_buffer[256];
  char clock_buffer[256];
  struct timespec next_time;
  time_t time_initial;
  struct tm tm;
  // Compare to the previous clock time
  int old_hour;
  int old_min;
  int old_sec;
  int old_intensity;
  // Start with a string instead of the clock 
  std::string text_string;
  unsigned int text_string_start;
  int text_offset;
  bool show_clock;
};
