import os, time, argparse, swd, pylink
from PIL import Image


import pylink



DATA_BUFFER_ADDR        = 0X20000015
LEVEL_BUFFER_ADDR       = 0X20001071
STATUS_REG_ADDR         = 0X2000116C
WRITE_FLAG_ADDR         = 0X20001170
LVL_RESET_FLAG_ADDR     = 0X2000107C
CONTINUE_FLAG_ADDR      = 0X20001018
PAGE_ADDR               = 0X20001084
MAINRAN_ADDR            = 0X20001081

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
FLASH_BLOCKS            = 1024
FLASH_BLOCK_SIZE        = 4096
FLASH_START_ADDR        = 0x0


# -------------------------------------
# STATUS VALUES
# -------------------------------------

from enum import Enum

class StatusValues(Enum):
    IDLE = 0
    READY = 1
    STARTING = 2
    LEVEL_RESET = 3
    START_WRITE = 4
    WRITE_DONE = 5
    START_READ = 6
    READ_DONE = 7
    OPERATION_COMPLETE = 8
    



jlink = pylink.JLink()
jlink.open()
print(jlink.product_name)
jlink.oem
print(f"jlink.opened(): {jlink.opened()}")
print(f"jlink.connected(): {jlink.connected()}")
jlink.set_tif(pylink.JLinkInterfaces.SWD)
print(f"Set Speed=4000: {jlink.set_speed(4000)}")
print(f"jlink.connect(): {jlink.connect('N32G031K8', verbose=True)}")
print(f"jlink.target_connected(): {jlink.target_connected()}")
print(f"set Endianess = little: {jlink.set_little_endian()}")
print(f"jlink.core_id(): {jlink.core_id()}")
print(f"jlink.device_family(): {jlink.device_family()}")
print(f"jlink.target_connected(): {jlink.target_connected()}")
print(f"jlink.reset(): {jlink.reset()}")
jlink.breakpoint_clear_all()
jlink.restart()
# reg_list = jlink.register_list()
# for reg_num in reg_list:
#     reg_name = jlink.register_name(reg_num)
#     print(f"R{reg_num}: {reg_name}")
print(f"jlink.halted(): {jlink.halted()}")
jlink.halt()
print(f"MSP: {jlink.register_read('MSP').to_bytes(4, 'big').hex()}")
print(f"PC: {jlink.register_read(15).to_bytes(4, 'big').hex()}")
jlink.restart()
print(f"jlink.halted(): {jlink.halted()}")






print(f"MAINRAN: {jlink.memory_read8(MAINRAN_ADDR, 1)}")
print(f"DATA_BUFFER: {jlink.memory_read8(DATA_BUFFER_ADDR, 4)}")
print(f"LEVEL_BUFFER: {jlink.memory_read8(LEVEL_BUFFER_ADDR, 1)}")
print(f"STATUS_REG: {jlink.memory_read8(STATUS_REG_ADDR, 1)}")
print(f"WRITE_FLAG: {jlink.memory_read8(WRITE_FLAG_ADDR, 1)}")
print(f"LVL_RESET_FLAG: {jlink.memory_read8(LVL_RESET_FLAG_ADDR, 1)}")
print(f"CONTINUE_FLAG: {jlink.memory_read8(CONTINUE_FLAG_ADDR, 1)}")
print(f"PAGE: {jlink.memory_read8(PAGE_ADDR, 1)}")


while(int.from_bytes(bytes(jlink.memory_read8(STATUS_REG_ADDR,1)), 'big') != StatusValues.READY.value):
                    time.sleep(1)
                    print(f"status: {jlink.memory_read8(STATUS_REG_ADDR, 1)}")
                    pass

def dump_flash(self, file_path=""):
        try:
            print("Dumping Flash > " + file_path)
            #self.connect()
            
            print("BLOCK_SIZE:\t\t" + str(FLASH_BLOCK_SIZE))
            print("BLOCKS:\t\t\t" + str(FLASH_BLOCKS))
            start_time = time.time()
            self.memory_write8(CONTINUE_FLAG_ADDR, [1])
            print(f"CONTINUE_FLAG: {self.memory_read8(CONTINUE_FLAG_ADDR, 1)}")
            
            print(f"STATUS_REG: {self.memory_read8(STATUS_REG_ADDR, 1)}")

        

            #Wait for read to complete
            
            while(int.from_bytes(bytes(self.memory_read8(STATUS_REG_ADDR,1)), 'big') != StatusValues.READ_DONE.value):
                    #time.sleep(.1)
                    elapsed_ms = int((time.time() - start_time) * 1000)
                    print(f"status: {self.memory_read8(STATUS_REG_ADDR, 1)} ({elapsed_ms}ms)")
                    pass
            
            print(f"PAGE: {jlink.memory_read8(PAGE_ADDR, 1)}")
            # Read data from memory
            data = b''
            page = 0;
            if file_path != "":
                
                for block in range(FLASH_BLOCKS):
                    block_start_time = time.time()
                    
                
                    #Wait for read to complete
                    
                    while(int.from_bytes(bytes(self.memory_read8(STATUS_REG_ADDR,1)), 'big') != StatusValues.READ_DONE.value):
                            elapsed_ms = int((time.time() - block_start_time) * 1000)
                            #print(f"status: {self.memory_read8(STATUS_REG_ADDR, 1)} ({elapsed_ms}ms)")
                            time.sleep(0.1)
                            pass
                    # Read from memory  and add to existing data buffer
                    d = bytes(self.memory_read8(DATA_BUFFER_ADDR, FLASH_BLOCK_SIZE))
                    data += d
                    page += 1
                    self.memory_write16(PAGE_ADDR, [page])
                    print(f"Page: {self.memory_read16(PAGE_ADDR, 2)} data: {d[0:16].hex()}...")
                    elapsed_ms = int((time.time() - block_start_time) * 1000)
                    print(f"Block {block} read complete ({elapsed_ms}ms)")

                print("")
                with open(file_path, 'wb') as f:
                    f.write(data)
                print("Flash dump complete.")
                self.memory_write8(STATUS_REG_ADDR, [StatusValues.OPERATION_COMPLETE.value])
        except Exception as e:
            print("Error during flash dump: " + str(e)) 










def upload_flash(self,flash_input_file=""):
        if os.path.exists(flash_input_file):
            print("Writing Flash < " + flash_input_file)
            
            print("BLOCK_SIZE:\t\t" + str(FLASH_BLOCK_SIZE))
            print("BLOCKS:\t\t\t" + str(FLASH_BLOCKS))
                
            with open(flash_input_file, 'rb') as f:
                
                buffer = f.read()
                self.memory_write8(WRITE_FLAG_ADDR, [1])
                self.memory_write8(CONTINUE_FLAG_ADDR, [1])
                
                # Wait for erase
                erase_start_time = time.time()
                while(int.from_bytes(bytes(self.memory_read8(STATUS_REG_ADDR,1)), 'big') != StatusValues.WRITE_DONE.value):
                        pass
                erase_elapsed_ms = int((time.time() - erase_start_time) * 1000)
                print(f"Erase done. STATUS_REG: {self.memory_read8(STATUS_REG_ADDR, 1)} ({erase_elapsed_ms}ms)")
                
                # Write data to memory
                for block in range(FLASH_BLOCKS):
                    write_start_time = time.time()
                    bl_data = buffer[block * FLASH_BLOCK_SIZE: (block * FLASH_BLOCK_SIZE) + FLASH_BLOCK_SIZE]
                    print(f"Writing Block {block} / {FLASH_BLOCKS}...")
                    for offset in range(0, len(bl_data), 1):
                        d = [bl_data[offset]]
                        self.memory_write8(DATA_BUFFER_ADDR + offset, d)

                    #Se to 4 to write data to flash
                    self.memory_write8(STATUS_REG_ADDR, [StatusValues.START_WRITE.value]) 

                    #Wait for write to complete
                    
                    while(int.from_bytes(bytes(self.memory_read8(STATUS_REG_ADDR,1)), 'big') != StatusValues.WRITE_DONE.value):
                            elapsed_ms = int((time.time() - write_start_time) * 1000)
                            print(f"status: {self.memory_read8(STATUS_REG_ADDR, 1)} ({elapsed_ms}ms)")
                            time.sleep(.1)
                            if int.from_bytes(bytes(self.memory_read8(STATUS_REG_ADDR,1)), 'big') == StatusValues.OPERATION_COMPLETE.value:
                                break
                            pass

                self.memory_write8(CONTINUE_FLAG_ADDR, [0])
                print("Flash write complete.")
                        
        else:
            print(f"Error - Invalid file path: {flash_input_file}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Flash read/write utility for MCU')
    parser.add_argument('action', choices=['dump', 'upload'], help='Action to perform: dump (read) or upload (write)')
    parser.add_argument('-f', '--file', type=str, help='File path for dump output or upload input')
    
    args = parser.parse_args()
    
    try:
        if args.action == 'dump':
            output_file = args.file if args.file else "flash_dump.bin"
            dump_flash(jlink, output_file)
        elif args.action == 'upload':
            input_file = args.file if args.file else "ReneePatched.binF"
            upload_flash(jlink, input_file)
    except Exception as e:
        print("Error during flash operation: " + str(e))
        jlink.close()

    jlink.reset()
    jlink.restart()
    #jlink.memory_write8(MAINRAN_ADDR, [0x0])  # Example: Write to Flash control register
    jlink.close()
