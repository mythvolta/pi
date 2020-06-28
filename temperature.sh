#!/bin/bash
# Log the temperature to ~/camera/log/*.log

# Calculate rough CPU and GPU temperatures
if [[ -f "/sys/class/thermal/thermal_zone0/temp" ]]; then
    cpuTempC=$(($(cat /sys/class/thermal/thermal_zone0/temp)/1000))
    cpuTempF=$((cpuTempC*9/5+32))
fi
if [[ -f "/opt/vc/bin/vcgencmd" ]]; then
    if gpuTempC=$(/opt/vc/bin/vcgencmd measure_temp); then
        gpuTempC=${gpuTempC:5:2}
        gpuTempF=$((gpuTempC*9/5+32))
    else
        gpuTempC=""
    fi
fi

# Get the CPU frequency
armFreqMHz=`vcgencmd get_config arm_freq`

# Show the output
echo
date
echo "CPU: $cpuTempC°C/$cpuTempF°F @ ${armFreqMHz/arm_freq=} MHz"
#echo "GPU: $gpuTempC°C/$gpuTempF°F"
