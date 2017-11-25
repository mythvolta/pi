// -*- mode: c++; c-basic-offset: 2; indent-tabs-mode: nil; -*-
// LED clock, that also shows updates from mythtv status.

// sudo $LED/eric/mythclock -f $LED/fonts/6x13B.bdf -b10 -d '%H%M' --led-rows=16 -S1 -y-2 -x0 -C 128,0,0

#include "led-matrix.h"
#include "graphics.h"

#include <getopt.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

using namespace rgb_matrix;

volatile bool interrupt_received = false;
static void InterruptHandler(int signo) {
  interrupt_received = true;
}

int main(int argc, char *argv[]) {
  // All options are hardcoded
  const std::string time_format = "%H:%M:%S";
  Color color(128, 0, 0);
  Color bg_color(0, 0, 0);
  Color outline_color(0,0,0);
  //bool with_outline = false;
  const bool with_outline = true;
  const std::string bdf_font_file = "/home/pi/code/rpi-rgb-led-matrix/fonts/6x13B.bdf";
  const int x_orig = 0;
  const int y_orig = 0;
  int brightness = 20; // Changes based on time of day
  const int letter_spacing = 0;

  // Load font. This needs to be a filename with a bdf bitmap font.
  rgb_matrix::Font font;
  if (!font.LoadFont(bdf_font_file.c_str())) {
    fprintf(stderr, "Couldn't load font '%s'\n", bdf_font_file.c_str());
    return 1;
  }
  rgb_matrix::Font *outline_font = NULL;
  if (with_outline) {
      outline_font = font.CreateOutlineFont();
  }

  // Error checking on the brightness
  if (brightness < 1 || brightness > 100) {
    fprintf(stderr, "Brightness is outside usable range.\n");
    return 1;
  }

  // Create the matrix
  RGBMatrix::Options matrix_options;
  rgb_matrix::RuntimeOptions runtime_opt;
  RGBMatrix *matrix = rgb_matrix::CreateMatrixFromOptions(matrix_options, runtime_opt);
  if (matrix == NULL)
    return 1;

  matrix->SetBrightness(brightness);

  const int x = x_orig;
  int y = y_orig;

  // Create the canvas and buffer
  FrameCanvas *offscreen = matrix->CreateFrameCanvas();
  char text_buffer[256];
  struct timespec next_time;
  next_time.tv_sec = time(NULL);
  next_time.tv_nsec = 0;
  struct tm tm;

  // Set up signal handlers
  signal(SIGTERM, InterruptHandler);
  signal(SIGINT, InterruptHandler);

  // Loop until we receive a signal
  while (!interrupt_received) {
      localtime_r(&next_time.tv_sec, &tm);
      strftime(text_buffer, sizeof(text_buffer), time_format.c_str(), &tm);
      offscreen->Fill(bg_color.r, bg_color.g, bg_color.b);
      if (outline_font) {
          rgb_matrix::DrawText(offscreen, *outline_font,
                               x - 1, y + font.baseline(),
                               outline_color, NULL, text_buffer,
                               letter_spacing - 2);
      }
      rgb_matrix::DrawText(offscreen, font, x, y + font.baseline(),
                           color, NULL, text_buffer,
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
