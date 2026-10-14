openocd -f n32g031_wifi.cfg


While OpenOCD is running:
​The Wi-Fi Debugger Active binary sensor in Home Assistant turns ON and pauses local polling.
​You can attach arm-none-eabi-gdb to localhost:3333 or telnet into localhost:4444 to halt the core (reset halt), inspect memory (mdw 0x08000000 16), unlock the N32G031 Flash Controller (n32_unlock_flash), or run flash programming scripts over Wi-Fi.
