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

using namespace rgb_matrix;

// Hardcode some colors
const static Color font_red(128, 0, 0);
const static Color font_orange(128, 64, 0);
const static Color font_yellow(128, 128, 0);
const static Color font_white(128, 128, 128);
const static Color font_green(0, 128, 0);
const static Color font_blue(0, 0, 128);
const static Color font_purple(0, 64, 128);

volatile bool interrupt_received = false;
static void InterruptHandler(int signo) {
  interrupt_received = true;
}

static int usage(const char *progname) {
  fprintf(stderr, "usage: %s [options]\n", progname);
  fprintf(stderr, "Reads text from stdin and displays it. "
          "Empty string: clear screen\n");
  fprintf(stderr, "Options:\n");
  rgb_matrix::PrintMatrixFlags(stderr);
  fprintf(stderr,
          "\t-d <time-format>  : Default '%%H:%%M'. See strftime()\n"
          "\t-f <font-file>    : Use given font.\n"
          "\t-b <brightness>   : Sets brightness percent. Default: 100.\n"
          "\t-x <x-origin>     : X-Origin of displaying text (Default: 0)\n"
          "\t-y <y-origin>     : Y-Origin of displaying text (Default: 0)\n"
          "\t-S <spacing>      : Spacing pixels between letters (Default: 0)\n"
          "\t-C <r,g,b>        : Color. Default 255,255,0\n"
          "\t-B <r,g,b>        : Background-Color. Default 0,0,0\n"
          "\t-O <r,g,b>        : Outline-Color, e.g. to increase contrast.\n"
          );

  return 1;
}

/*
static bool parseColor(Color *c, const char *str) {
  return sscanf(str, "%hhu,%hhu,%hhu", &c->r, &c->g, &c->b) == 3;
}
*/

/*
static bool FullSaturation(const Color &c) {
    return (c.r == 0 || c.r == 255)
        && (c.g == 0 || c.g == 255)
        && (c.b == 0 || c.b == 255);
}
*/

int main(int argc, char *argv[]) {
  RGBMatrix::Options matrix_options;
  rgb_matrix::RuntimeOptions runtime_opt;
  if (!rgb_matrix::ParseOptionsFromFlags(&argc, &argv,
                                         &matrix_options, &runtime_opt)) {
    return usage(argv[0]);
  }

  // Some constants
  const std::string matrix_path = "/home/pi/code/rpi-rgb-led-matrix";
  //const std::string bdf_font_file_str = matrix_path + "/fonts/4x6.bdf";
  const std::string bdf_font_file_str = matrix_path + "/fonts/6x13B.bdf";
  const char *bdf_font_file = bdf_font_file_str.c_str();
  const char *time_format = "%H%M";

  // These can change every hour
  int x_orig = 3;
  int y_orig = -1;
  int brightness = 20;
  int letter_spacing = 0;

  // Vector of colors
  //const std::vector<Color> color_vector(font_orange, font_red, font_purple, font_white);
  unsigned int color_index = 0;
  Color font_color = font_red; //color_vector[color_index]);
  Color bg_color(0, 0, 0);
  Color outline_color(0, 0, 32);
  bool with_outline = true;

  // Load font. This needs to be a filename with a bdf bitmap font.
  rgb_matrix::Font font;
  if (!font.LoadFont(bdf_font_file)) {
    fprintf(stderr, "Couldn't load font '%s'\n", bdf_font_file);
    return 1;
  }
  rgb_matrix::Font *outline_font = NULL;
  if (with_outline) {
      outline_font = font.CreateOutlineFont();
  }

  // Create the matrix
  RGBMatrix *matrix = rgb_matrix::CreateMatrixFromOptions(matrix_options,
                                                          runtime_opt);
  if (matrix == NULL) {
    return 1;
  }
  matrix->SetBrightness(brightness);

  // X and Y offsets
  int x = x_orig;
  int y = y_orig;

  FrameCanvas *offscreen = matrix->CreateFrameCanvas();

  char text_buffer[256];
  struct timespec next_time;
  next_time.tv_sec = time(NULL);
  next_time.tv_nsec = 0;
  struct tm tm;

  // Signal handlers
  signal(SIGTERM, InterruptHandler);
  signal(SIGINT, InterruptHandler);

  // Check for night mode
  bool night_mode = false;

  // Loop forever
  while (!interrupt_received) {
    // Get the time
    localtime_r(&next_time.tv_sec, &tm);
    strftime(text_buffer, sizeof(text_buffer), time_format, &tm);

    // Show the time
    printf("Time is [%02d:%02d:%02d]\n", tm.tm_hour, tm.tm_min, tm.tm_sec);

    // Night mode is enabled from 1800 - 0800
    if (!night_mode && (tm.tm_hour >= 18 || tm.tm_hour < 8)) {
      night_mode = true;
      brightness = 4;
      with_outline = false;
      matrix->SetBrightness(brightness);
    }
    else if (night_mode && (tm.tm_hour >= 8 && tm.tm_hour < 18)) {
      night_mode = false;
      brightness = 10;
      with_outline = true;
      matrix->SetBrightness(brightness);
    }

    // Only show the outline on odd seconds
    /*
    if (tm.tm_sec % 2 == 1) {
      with_outline = true;
    }
    else {
      with_outline = false;
    }
    */

    // Change the text font_color every 10 seconds
    /*
    if (tm.tm_sec % 10 == 0) {
      if (++color_index == 1) {
        font_color = font_white;
      }
      else {
        font_color = font_purple;
        color_index = 0;
      }
    }
    */

    // Write to the screen
    offscreen->Fill(bg_color.r, bg_color.g, bg_color.b);
    if (with_outline && outline_font) {
        rgb_matrix::DrawText(offscreen, *outline_font,
                             x - 1, y + font.baseline(),
                             outline_color, NULL, text_buffer,
                             letter_spacing - 2);
    }
    rgb_matrix::DrawText(offscreen, font, x, y + font.baseline(),
                         font_color, NULL, text_buffer,
                         letter_spacing);

    // Wait until we're ready to show it.
    clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &next_time, NULL);

    // Atomic swap with double buffer
    offscreen = matrix->SwapOnVSync(offscreen);

    next_time.tv_sec += 1;
  }

  // Finished. Shut down the RGB matrix.
  matrix->Clear();
  delete matrix;

  write(STDOUT_FILENO, "\n", 1);  // Create a fresh new line after ^C on screen
  return 0;
}
