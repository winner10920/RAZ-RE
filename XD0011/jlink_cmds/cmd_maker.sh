

read -rp "Enter board name (e.g., XD0011): " BOARDNAME

BOARDNAME="XD0011"
CMDNAME="DOWNLOADBIN"
echo Connect > $CMDNAME.cmd.jlink
#echo 'WRITE4 40022004 45670123' >> $CMDNAME.cmd.jlink 
#echo 'WRITE4 40022004 CDEF89AB' >> $CMDNAME.cmd.jlink
#echo 'MEM32 40022010 1' >> $CMDNAME.cmd.jlink
#echo 'WRITE4 400022010 4' >> $CMDNAME.cmd.jlink
#echo 'WRITE4 40022010 44' >> $CMDNAME.cmd.jlink
#echo 'WRITE4 40022010 210' >> $CMDNAME.cmd.jlink
echo "SaveBin RAZ-RE/Firmware/$BOARDNAME.BIN 0 FFFF" >> $CMDNAME.cmd.jlink
echo Exit >> $CMDNAME.cmd.jlink



#JLinkExe -device N32G031K8 -if swd -speed 4000 -commandfile cmd.jlink 
