#!/usr/bin/env python3

# Imports
import os
import time
from pwd import getpwnam

# Camera basedir
camera_dir = '/media/large/camera'

# Get the uid/gid for user pi
pi_uid = getpwnam('pi').pw_uid
pi_gid = getpwnam('pi').pw_gid

# Add a folder
'''Class that contains the number of days to keep each folder'''
class Folder:
  '''Constructor'''
  def __init__(self, folder='directory', keep_days=7):
    self.folder    = camera_dir + '/' + folder
    self.keep_days = keep_days
  # def
# class

# Subdirectories to sort through
folders = list()
folders.append(Folder('pictures',  7))    # 1 week
folders.append(Folder('movies',    7*4))  # 4 weeks
folders.append(Folder('timelapse', 7*52)) # 1 year

# Get the current time
now = time.time()

# Loop through all the directories
for d in folders:
  print(d.folder, '=', d.keep_days, 'days')
  for f in os.listdir(d.folder):
    full_filename = os.path.join(d.folder, f)
    # Always chown/chgrp the file
    os.chown(full_filename, pi_uid, pi_gid)
    # Delete files they are too old
    days_old = (now - os.stat(full_filename).st_mtime) / 86400
    if days_old > d.keep_days:
      print('  deleting', full_filename)
      os.remove(full_filename)
    # if
  # f
# for
