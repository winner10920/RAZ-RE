# -------------------------------------------------------------------------
# GDB Script for chunked binary flashing without Python support
# Target: GNU gdb (Arm GNU Toolchain 14.3.Rel1)
# -------------------------------------------------------------------------

# 1. Configuration
#set pagination off
#set output-radix 10

# Define constants matching your C code
set $BUFFER_ADDR  = &buffer
set $CHUNK_SIZE   = 4096
set $TOTAL_CHUNKS = 256
set $FILENAME     = "ReneePatched.binF"

# Initialize loop counters
set $curr_chunk   = 0
set $file_offset  = 0

# -------------------------------------------------------------------------
# 2. Start the Sequence
# -------------------------------------------------------------------------

echo [GDB] Setting write_flag to trigger erase...\n
set write_flag[0] = 1

echo [GDB] Continuing target. Please wait for Flash Erase to complete...\n
echo [GDB] Target should halt at the 'while(status[0] == 5)' breakpoint.\n

# We continue here to let the C code perform sFLASH_EraseBulk()
# and reach the first synchronization point inside the for-loop.
continue

# -------------------------------------------------------------------------
# 3. The Flashing Loop
# -------------------------------------------------------------------------

while ($curr_chunk < $TOTAL_CHUNKS)
    
    # Calculate the end offset for the current chunk
    set $end_offset = $file_offset + $CHUNK_SIZE
    
    # Feedback to user
    printf "[GDB] Processing Chunk %d/%d (Offset: %d)\n", ($curr_chunk + 1), $TOTAL_CHUNKS, $file_offset

    # ---------------------------------------------------------------------
    # CRITICAL: Use 'eval' to dynamically construct the restore command.
    # Syntax: restore filename binary load_address file_start file_end
    # We cast &buffer to (void*) or (unsigned int) to ensure proper formatting
    # ---------------------------------------------------------------------
    eval "restore %s binary 0x%x %d %d", $FILENAME, (unsigned int)$BUFFER_ADDR, $file_offset, $end_offset

    # ---------------------------------------------------------------------
    # Synchronization
    # ---------------------------------------------------------------------
    
    # 1. Clear status to 0. This breaks the C code out of 'while(status[0] == 5)'
    set status[0] = 4

    # 2. Update counters for the next iteration
    set $file_offset = $end_offset
    set $curr_chunk = $curr_chunk + 1

    # 3. Continue execution. 
    # The target will:
    #    - Write the buffer to flash (sFLASH_WriteBuffer)
    #    - Set status[0] = 5
    #    - Loop around
    #    - Hit the breakpoint at 'while(status[0] == 5)' again
    
    if ($curr_chunk < $TOTAL_CHUNKS)
        continue
    end
end

echo \n[GDB] Flashing sequence completed successfully.\n
