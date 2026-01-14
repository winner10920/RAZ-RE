To download flash from onboard flash 

move dump_script.gdb to keil-mdk/MDK-ARM
Upload main.c with cmsis extension, attach debugger
watch continue_flag[0]
set continue_flag[0] to 1 by right clicking on expression and click set value. have to pause first
have breakpoint set where flash code sets status[0] to 7
when it stops at breaakpoint
in debug console, type >source dump_script.gdb
then >auto_dump
full_flash_dump will appear in same folder as MDK-ARM





mcu binary was dumped with JLinkEXE , can use script below 

```bash

#! /bin/bash

BOARDNAME="XD0011"
echo Connect > cmd.jlink
echo 'WRITE4 40022004 45670123' >> cmd.jlink 
echo 'WRITE4 40022004 CDEF89AB' >> cmd.jlink
echo 'MEM32 40022010 1' >> cmd.jlink
echo 'WRITE4 400022010 4' >> cmd.jlink
echo 'WRITE4 40022010 44' >> cmd.jlink
echo 'WRITE4 40022010 210' >> cmd.jlink
echo "SaveBin RAZ-RE/Firmware/$BOARDNAME.BIN 0 FFFF" >> cmd.jlink
echo Exit >> cmd.jlink

JLinkExe -device N32G031K8 -if swd -speed 4000 -commandfile cmd.jlink 

#JLinkExe -device N32G031K8 -if swd -speed 1000 -commandfile cmd.jlink 

```



upload original file

```bash

Connect
WRITE4 40022004 45670123
WRITE4 40022004 CDEF89AB
MEM32 40022010 1
WRITE4 400022010 4
WRITE4 40022010 44
WRITE4 40022010 210
MEM32 40022010 1
loadfile xd0011.new.bin 08000000
exit


```




GDB SERVER


```bash

JLinkGDBServer -device N32G031K8 -if swd -speed 4000 -port 3333


```

