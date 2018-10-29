#!/usr/bin/env python3
#
# Run in a loop, showing every time a new file plays in mpd

import subprocess
import re
from time import sleep

# Use this random separator that shouldn't appear elsewhere
sep = 'EJR22'
mpc = 'mpc -h dragon'
parts = '%%albumartist%% %s %%date%% %%album%% %s (%%track%%) %%title%%' % (sep, sep)
is_playing = re.compile('^\[play')
album_year = re.compile('^(\d{4})?(-\d{2})?(-\d{2})? (.*)')
extra_prefix = re.compile('^ - ')

# Use object-oriented python
'''Class that contains information about a track'''
class Track:
  artist = 'Artist'
  year = 2000
  album = 'Album'
  title = 'Track'

  '''Constructor'''
  def __init__(self, mpc_string):
    self.split_track_info(mpc_string)
  # def __init__

  '''Split a track into its important elements'''
  def split_track_info(self, s):
    parts = s.split(' %s ' % sep)
    num_parts = len(parts)

    # First part is always the album artist
    if (num_parts >= 1):
      self.artist = parts[0]
    # if

    # Second part is the album year and album title   
    if (num_parts >= 2):
      result = album_year.match(parts[1])
      if (result):
        num_groups = len(result.groups())
        if (num_groups >= 2):
          self.year = int(result.group(1))
          self.album = result.group(num_groups)
        else:
          self.album = parts[1]
        # if
      # if
    # if

    # Third part is always the track title
    if (num_parts >= 3):
      self.title = parts[2]
    # if
  # def split_track_info()

  '''Overloaded output operator'''
  def __str__(self):
    return '%s - [%04d] %s - %s' % (self.artist, self.year, self.album, self.title)
  # def __str__
# class

# Run the mpc command
encoding = 'utf-8'
track = '%s current -f "%s"' % (mpc, parts)
status = '%s status | head -n2 | tail -n1' % (mpc)
def run_mpc(cmd):
  p = subprocess.Popen(cmd, stdout=subprocess.PIPE, shell=True)
  (output, err) = p.communicate()
  p_status = p.wait()
  return (output, err)
# def run_mpc()

# Compare two tracks, and return only the differences
def compare_tracks(track_new, track_old):
  out = ''
  different = False

  # Artist
  different |= (track_new.artist != track_old.artist)
  if (different):
    out += ' - %s' % track_new.artist
    different = True
  # if

  # Year and Album
  different |= (track_new.year != track_old.year)
  different |= (track_new.album != track_old.album)
  if (different):
    out += ' - [%04d] %s' % (track_new.year, track_new.album)
  # if

  # Title
  different |= (track_new.title != track_old.title)
  if (different):
    out += ' - %s' % track_new.title
  # if

  # Strip prefix
  return out.lstrip(' - ')
# def compare_tracks()

# The sleep timer defaults to 10 seconds,
#  but resets to 60 once playback is stopped
sleep_time = 10
track_previous = Track
while True:
  # Check the status
  (output, err) = run_mpc(status)
  playing = False
  if is_playing.match(output.decode(encoding)):
    playing = True
  # if

  # Always loop 6 times, and either read track info or do nothing
  for i in range(1, 6):
    # Normal mode
    if playing:
      (output, err) = run_mpc(track)
      track_current = Track(output.decode(encoding).rstrip())
      s = compare_tracks(track_current, track_previous)
      if (s):
        print(s)
      # if
      track_previous = track_current
    # if
    sleep(sleep_time)
  # for
# while
