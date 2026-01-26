#!/bin/bash
# Reset J-Link USB device without sudo (if permissions allow)

# Find J-Link device
DEVICE_PATH=$(for dev in /sys/bus/usb/devices/*; do 
    if [ -f "$dev/idVendor" ]; then 
        vendor=$(cat "$dev/idVendor" 2>/dev/null)
        product=$(cat "$dev/idProduct" 2>/dev/null)
        if [ "$vendor" = "1366" ] && [ "$product" = "0101" ]; then 
            echo "$(basename $dev)"
            break
        fi
    fi
done)

if [ -z "$DEVICE_PATH" ]; then
    echo "J-Link not found!"
    exit 1
fi

echo "Found J-Link at: $DEVICE_PATH"

# Try to authorize/deauthorize to reset (may not require sudo)
if [ -w "/sys/bus/usb/devices/$DEVICE_PATH/authorized" ]; then
    echo "Resetting via authorized flag..."
    echo 0 > /sys/bus/usb/devices/$DEVICE_PATH/authorized
    sleep 1
    echo 1 > /sys/bus/usb/devices/$DEVICE_PATH/authorized
    echo "Reset complete!"
else
    echo "Need sudo to reset. Run: sudo usbreset /dev/bus/usb/001/029"
fi


#sudo usbreset 001/029