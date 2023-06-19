#!/bin/bash
echo 0 | tee /sys/class/backlight/intel_backlight/brightness
sleep 10
echo 96000 | tee /sys/class/backlight/intel_backlight/brightness
