import os, time, argparse, swd, pylink
from PIL import Image


import pylink



DATA_BUFFER_ADDR        = 0x20000015
LEVEL_BUFFER_ADDR       = 0x20001049
STATUS_REG_ADDR         = 0x20001068
WRITE_FLAG_ADDR         = 0x2000106c
LVL_RESET_FLAG_ADDR     = 0x20001054
CONTINUE_FLAG_ADDR      = 0x20001018
PAGE_ADDR               = 0x2000105c

# -------------------------------------
# MCU PROG MEM
# -------------------------------------
MCU_PROGMEM_ADDR        = 0x08000000
MCU_PROGMEM_LEN         = 0x10000

# -------------------------------------
# MCU FLASH REGISTERS
# -------------------------------------
MCU_FLASH_KEY_REG       = 0x40022004
MCU_FLASH_CTRL_REG      = 0x40022010
MCU_FLASH_ADDR_REG      = 0x40022014

# -------------------------------------
# MCU FLASH KEYS
# -------------------------------------
MCU_FLASH_KEY_1         = 0x45670123
MCU_FLASH_KEY_2         = 0xCDEF89AB

# -------------------------------------
# SPI FLASH
# -------------------------------------
FLASH_BLOCKS            = 256
FLASH_BLOCK_SIZE        = 4096
FLASH_START_ADDR        = 0x0




jlink = pylink.JLink()
jlink.open()
print(jlink.product_name)
jlink.oem
print(f"jlink.opened(): {jlink.opened()}")
print(f"jlink.connected(): {jlink.connected()}")
jlink.set_tif(pylink.JLinkInterfaces.SWD)
print(f"jlink.connect(): {jlink.connect('N32G031K8', verbose=True)}")
print(f"jlink.target_connected(): {jlink.target_connected()}")
print(f"jlink.core_id(): {jlink.core_id()}")
print(f"jlink.device_family(): {jlink.device_family()}")
print(f"jlink.target_connected(): {jlink.target_connected()}")
#print(f"jlink.reset(): {jlink.reset()}")
#jlink.restart()
print(f"jlink.halted(): {jlink.halted()}")






print(f"DATA_BUFFER: {jlink.memory_read8(DATA_BUFFER_ADDR, 4)}")
print(f"LEVEL_BUFFER: {jlink.memory_read8(LEVEL_BUFFER_ADDR, 1)}")
print(f"STATUS_REG: {jlink.memory_read8(STATUS_REG_ADDR, 1)}")
print(f"WRITE_FLAG: {jlink.memory_read8(WRITE_FLAG_ADDR, 1)}")
print(f"LVL_RESET_FLAG: {jlink.memory_read8(LVL_RESET_FLAG_ADDR, 1)}")
print(f"CONTINUE_FLAG: {jlink.memory_read8(CONTINUE_FLAG_ADDR, 1)}")
print(f"PAGE: {jlink.memory_read8(PAGE_ADDR, 1)}")


while(int.from_bytes(bytes(jlink.memory_read8(STATUS_REG_ADDR,1)), 'big') != 1):
                    time.sleep(1)
                    print(f"status: {jlink.memory_read8(STATUS_REG_ADDR, 1)}")
                    pass

def dump_flash(self, file_path=""):
        try:
            print("Dumping Flash > " + file_path)
            #self.connect()
            
            print("BLOCK_SIZE:\t\t" + str(FLASH_BLOCK_SIZE))
            print("BLOCKS:\t\t\t" + str(FLASH_BLOCKS))
            self.memory_write8(CONTINUE_FLAG_ADDR, [1])
            print(f"CONTINUE_FLAG: {self.memory_read8(CONTINUE_FLAG_ADDR, 1)}")
            
            print(f"STATUS_REG: {self.memory_read8(STATUS_REG_ADDR, 1)}")

        

            #Wait for read to complete
            while(int.from_bytes(bytes(self.memory_read8(STATUS_REG_ADDR,1)), 'big') != 7):
                    time.sleep(1)
                    print(f"status: {self.memory_read8(STATUS_REG_ADDR, 1)}")
                    pass
            print(f"PAGE: {jlink.memory_read8(PAGE_ADDR, 1)}")
            # Read data from memory
            data = b''
            page = 0;
            if file_path != "":
                
                for block in range(FLASH_BLOCKS):
                    if(block % 64 == 0 and block != 0):
                        print(f"Block: {int(block / 64)}")
                    

                    #Wait for read to complete
                    while(int.from_bytes(bytes(self.memory_read8(STATUS_REG_ADDR,1)), 'big') != 7):
                            #print(f"status: {self.memory_read8(STATUS_REG_ADDR, 1)}")
                            #time.sleep(1)
                            pass
                    # Read from memory and add to existing data buffer
                    d = bytes(self.memory_read8(DATA_BUFFER_ADDR, FLASH_BLOCK_SIZE))
                    data += d
                    page += 1
                    self.memory_write8(PAGE_ADDR, [page])
                    print(f"Page: {self.memory_read8(PAGE_ADDR, 1)} data: {d[0:16].hex()}...")
                

                print("")
                with open(file_path, 'wb') as f:
                    f.write(data)
        except Exception as e:
            print("Error during flash dump: " + str(e)) 


dump_flash(jlink, "flash_dump.bin")





#  def upload_flash(self):
#         if os.path.exists(self.flash_input_file):
#             print("Writing Flash < " + self.flash_input_file)
#             if self.verbose:
#                 print("BLOCK_SIZE:\t\t" + str(FLASH_BLOCK_SIZE))
#                 print("BLOCKS:\t\t\t" + str(FLASH_BLOCKS))
                
#             with open(self.flash_input_file, 'rb') as f:
                
#                 buffer = f.read()
#                 self.dev.swd_write8(WRITE_FLAG_ADDR, 1)
#                 self.dev.swd_write8(CONTINUE_FLAG_ADDR, 1)
                
#                 # Wait for erase
#                 while(int.from_bytes(bytes(self.dev.swd_read8(STATUS_REG_ADDR)), 'big') != 5):
#                         pass
                
#                 # Write data to memory
#                 for block in range(FLASH_BLOCKS):
#                     bl_data = buffer[block * FLASH_BLOCK_SIZE: (block * FLASH_BLOCK_SIZE) + FLASH_BLOCK_SIZE]
#                     # self.dev.write_mem8(STATUS_REG_ADDR, [1])
#                     for offset in range(0, len(bl_data), 4):
#                         d = [bl_data[offset], bl_data[offset+1], bl_data[offset+2], bl_data[offset+3]]
#                         self.dev.swd_write32(DATA_BUFFER_ADDR + offset, d)

#                     #Se to 4 to write data to flash
#                     self.dev.swd_write8(STATUS_REG_ADDR, 4) 

#                     #Wait for write to complete
#                     while(int.from_bytes(bytes(self.dev.swd_read8(STATUS_REG_ADDR)), 'big') != 5):
#                             pass

#                 self.dev.swd_write8(CONTINUE_FLAG_ADDR, 0)
                        
#         else:
#             print(f"Error - Invalid file path: {self.flash_input_file}")     