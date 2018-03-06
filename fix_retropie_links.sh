#!/bin/bash
cd ~pi
#git clone https://github.com/RetroPie/RetroPie-Setup.git
#git clone https://github.com/mythvolta/pi.git toolbox
DATETIME=`date +%Y-%m-%d_%H:%M:%S`

replace_link() {
  SRC=$1
  DST=$2
  echo $DATETIME
  ls -al $SRC
  ls -al $DST
  echo
}


replace_link "/opt/retropie/configs/all/autoconf.cfg" "/home/pi/retropie/configs/autoconf.cfg"
replace_link "/opt/retropie/configs/all/emulationstation" "/home/pi/retropie/emulationstation"
replace_link "/opt/retropie/configs/all/retroarch/autoconfig" "/home/pi/retropie/configs/autoconfig"
replace_link "/opt/retropie/configs/all/retroarch.cfg" "/home/pi/retropie/configs/retroarch.cfg"
replace_link "/opt/retropie/configs/all/retroarch-joypads" "/opt/retropie/configs/all/retroarch/autoconfig"
replace_link "/opt/retropie/configs/all/retroarch/screenshots" "/home/pi/retropie/screenshots"
replace_link "/opt/retropie/configs/nes/launching.jpg" "/home/pi/retropie/launching/nes.jpg"
replace_link "/opt/retropie/configs/nes/retroarch.cfg" "/home/pi/retropie/configs/retroarch_nes.cfg"
replace_link "/opt/retropie/configs/psp/PSP/GAME" "/home/pi/RetroPie/roms/psp"
replace_link "/opt/retropie/configs/psx/launching.jpg" "/home/pi/retropie/launching/psx.jpg"
replace_link "/opt/retropie/configs/psx/retroarch.cfg" "/home/pi/retropie/configs/retroarch_psx.cfg"
replace_link "/opt/retropie/configs/snes/launching.jpg" "/home/pi/retropie/launching/snes.jpg"
replace_link "/opt/retropie/configs/snes/retroarch.cfg" "/home/pi/retropie/configs/retroarch_snes.cfg"
replace_link "/opt/retropie/emulators/retroarch/assets" "/opt/retropie/configs/all/retroarch/assets"
replace_link "/opt/retropie/emulators/retroarch/overlays "/opt/retropie/configs/all/retroarch/overlay"
replace_link "/opt/retropie/emulators/retroarch/shader "/opt/retropie/configs/all/retroarch/shaders"
replace_link "/opt/retropie/supplementary/xpad/xpad "/opt/retropie/supplementary/xpad"
