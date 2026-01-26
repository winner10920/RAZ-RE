#!/bin/bash
# Reset J-Link - requires sudo password

echo "Resetting J-Link USB device..."
sudo usbreset /dev/bus/usb/001/029

if [ $? -eq 0 ]; then
    echo "Reset complete!"
    sleep 2
    echo "Checking device..."
    lsusb | grep -i segger
else
    echo "Reset failed"
fi

#sudo usbreset 001/029