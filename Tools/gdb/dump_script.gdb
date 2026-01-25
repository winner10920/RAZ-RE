# Define a custom command called 'auto_dump'
define auto_dump
# Enable logging so you can see progress in the Debug Console
set pagination off
set $limit = 256
                
# Optional: Delete previous file if it exists (using shell command)
shell rm -f full_flash_dump.bin

printf "Starting automated dump for %d pages...\n", $limit

# Loop until page[0] reaches the limit
while page[0] < $limit

# 1. Append the current buffer contents to the file
# Syntax: append binary memory <filename> <start_addr> <end_addr>
# We use C-style pointer arithmetic for the end address
append binary memory full_flash_dump.bin buffer &buffer[4096]

printf "Dumped Page %d. Incrementing and resuming...\n", page[0]
# 2. Increment the watched variable
set variable page[0] = page[0] + 1
# 3. Resume the MCU
# The script will pause here until the next breakpoint is hit
continue
end

printf "Completed dumping %d pages.\n", $limit
end
