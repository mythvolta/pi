// -*- mode: c++; c-basic-offset: 2; indent-tabs-mode: nil; -*-
// Example of a clock. This is very similar to the text-example,
// except that it shows the time :)
//
// This code is public domain
// (but note, that the led-matrix library this depends on is GPL v2)

#include "led-matrix.h"
#include "graphics.h"

#include <iostream>

#include <getopt.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

//using namespace rgb_matrix;

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
  const std::string matrix_path = "/home/pi/code/rpi-rgb-led-matrix";
  const std::string bdf_font_file_str = matrix_path + "/fonts/6x13B.bdf";
  const char *bdf_font_file = bdf_font_file_str.c_str();
  const char *time_format = "%H%M";

  // These can change every hour
  int x_offset = 0;
  int y_offset = 0;
  int font_intensity = 256;

  // These are constant
  const int brightness = 20;
  const int letter_spacing = 1;

  // Vector of colors
  rgb_matrix::Color font_color(0, 0, 128);
  rgb_matrix::Color outline_color(0, 0, 32);
  const rgb_matrix::Color bg_color(0, 0, 0);
  bool with_outline = true;

  // Load font. This needs to be a filename with a bdf bitmap font.
  rgb_matrix::Font font;
  if (!font.LoadFont(bdf_font_file)) {
    fprintf(stderr, "Couldn't load font '%s'\n", bdf_font_file);
    return 1;
  }
  rgb_matrix::Font *outline_font = font.CreateOutlineFont();

  // Create the matrix
  rgb_matrix::RGBMatrix *matrix = rgb_matrix::CreateMatrixFromOptions(matrix_options,
                                                          runtime_opt);
  if (matrix == NULL) {
    return 1;
  }
  matrix->SetBrightness(brightness);
  rgb_matrix::FrameCanvas *offscreen = matrix->CreateFrameCanvas();

  char text_buffer[256];
  struct timespec next_time;
  next_time.tv_sec = time(NULL);
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

  // Loop forever
  while (!interrupt_received) {
    // Get the time
    localtime_r(&next_time.tv_sec, &tm);
    strftime(text_buffer, sizeof(text_buffer), time_format, &tm);

    // Handle changes of minute/seconds
    if (tm.tm_sec != old_sec) {
      // Blink the outline every second
      //with_outline = !with_outline;
          
      if (tm.tm_min != old_min) {

        if (tm.tm_hour != old_hour) {
          // Change the clock position every hour
          x_offset = rand() % 4 + 1;
          y_offset = rand() % 5 - 1;
          
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

        old_min = tm.tm_min;
      }

      old_sec = tm.tm_sec;
    }

    // Write to the screen
    offscreen->Fill(bg_color.r, bg_color.g, bg_color.b);
    if (with_outline) {
      rgb_matrix::DrawText(offscreen, *outline_font, x_offset - 1, y_offset + font.baseline(),
        outline_color, NULL, text_buffer, letter_spacing - 2);
    }
    rgb_matrix::DrawText(offscreen, font, x_offset, y_offset + font.baseline(),
      font_color, NULL, text_buffer, letter_spacing);

    // Wait until we're ready to show it.
    clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &next_time, NULL);

    // Atomic swap with double buffer
    offscreen = matrix->SwapOnVSync(offscreen);

    next_time.tv_sec += 1;
  }

  // Finished. Shut down the RGB matrix.
  matrix->Clear();
  delete matrix;

  // Create a fresh new line after ^C on screen
  write(STDOUT_FILENO, "\n", 1);

  return 0;
}
