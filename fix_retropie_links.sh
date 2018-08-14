#!/bin/bash
# Script that sets up RetroPie on a clean system,
#  and makes the appropriate links to other things

# First, actually run RetroPie setup
#sudo /home/pi/RetroPie-Setup/retropie_setup.sh
# Manage optional packages
#  lr-mrboom
#  lr-nxengine
#  lr-parallel-n64
#  lr-ppsspp
#  lr-prboom
#  lr-snex9x
#  lr-tgbdual
#  kodi
#  lincity-ng
#  sdlpop
#  solarus
#  uqm
# Manage driver packages
#  ps3controller
# Manage experimental packages
#  lr-dinothawr

DATETIME=`date +%Y-%m-%d_%H:%M:%S`

# Function to replace links
replace_link() {
  SRC=$1
  DST=$2
  echo "Link \"$SRC\" to \"$DST\""
  if [[ $SRC -ef $DST ]]; then
    echo "$SRC already points to $DST"
  else
    if [[ -L $SRC ]]; then
      rm $SRC
    else
      mv $SRC $SRC.$DATETIME.bak
    fi
    ln -s $DST $SRC
  fi
  ls --color -ald $SRC* $DST
  echo
}

# All commands are run from /home/pi
cd /home/pi

# Check out some git code
#git clone https://github.com/RetroPie/RetroPie-Setup.git
#git clone https://github.com/mythvolta/pi.git toolbox

# ~/.gitconfig
replace_link /home/pi/.gitconfig /home/pi/toolbox/gitconfig

# Make psx-hidden a link to the NFS share
replace_link /home/pi/retropie/roms/psx-hidden /home/pi/media/games/roms/psx-hidden

# Replace these links
replace_link /home/pi/retropie /home/pi/RetroPie
#replace_link /opt/retropie/configs/all/autoconf.cfg /home/pi/retropie/configs/autoconf.cfg
replace_link /opt/retropie/configs/all/emulationstation /home/pi/retropie/emulationstation
#replace_link /opt/retropie/configs/all/retroarch/autoconfig /home/pi/retropie/configs/autoconfig
replace_link /opt/retropie/configs/all/retroarch.cfg /home/pi/retropie/configs/retroarch.cfg
#replace_link /opt/retropie/configs/all/retroarch-joypads /opt/retropie/configs/all/retroarch/autoconfig
replace_link /opt/retropie/configs/all/retroarch/screenshots /home/pi/retropie/screenshots
#replace_link /opt/retropie/configs/nes/launching.jpg /home/pi/retropie/launching/nes.jpg
replace_link /opt/retropie/configs/nes/retroarch.cfg /home/pi/retropie/configs/retroarch_nes.cfg
#replace_link /opt/retropie/configs/psp/PSP/GAME /home/pi/RetroPie/roms/psp
#replace_link /opt/retropie/configs/psx/launching.jpg /home/pi/retropie/launching/psx.jpg
replace_link /opt/retropie/configs/psx/retroarch.cfg /home/pi/retropie/configs/retroarch_psx.cfg
#replace_link /opt/retropie/configs/snes/launching.jpg /home/pi/retropie/launching/snes.jpg
replace_link /opt/retropie/configs/snes/retroarch.cfg /home/pi/retropie/configs/retroarch_snes.cfg

# UQM
replace_link /home/pi/.uqm /home/pi/retropie/uqm
