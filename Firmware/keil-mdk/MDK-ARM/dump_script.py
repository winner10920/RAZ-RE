#import gdb
import os

class FlashDumper:
    def __init__(self):
        self.LIMIT = 256
        self.buffer_size = 4096
        self.filename = "full_flash_dump.bin"
        
        # Remove old file if it exists so we start fresh
        if os.path.exists(self.filename):
            try:
                os.remove(self.filename)
                print(f"[PyScript] Removed old {self.filename}")
            except OSError:
                print(f"[PyScript] Warning: Could not remove {self.filename}")

        # Connect the "stop" event to our handler function
        gdb.events.stop.connect(self.on_stop_handler)
        print(f"[PyScript] Automation armed! Waiting for next breakpoint...")
        
        # Determine if we are currently stopped and should trigger immediately
        # (Optional: checks if the program is active)
        if gdb.selected_thread() is not None:
             print("[PyScript] System appears stopped. Resume manually or via script to start loop.")

    def on_stop_handler(self, event):
        # 1. Read the current page index from the watched variable
        try:
            # We use parse_and_eval to get the value of page[0] from the target
            page_val = gdb.parse_and_eval("page[0]")
            current_page = int(page_val)
        except gdb.error:
            # If variable isn't in scope (e.g. paused in wrong file), ignore this stop
            return

        # 2. Check if we reached the limit
        if current_page >= self.LIMIT:
            print(f"[PyScript] Reached limit of {self.LIMIT} pages. Automation Complete.")
            gdb.events.stop.disconnect(self.on_stop_handler)
            return

        # 3. Read the Buffer Memory
        try:
            # Get the address of 'buffer'. We use '&buffer' to ensure we get the address.
            buffer_addr = gdb.parse_and_eval("&buffer")
            
            # Access the inferior (the MCU process) to read raw memory
            inf = gdb.inferiors()[0]
            # read_memory returns a memoryview object
            memory_data = inf.read_memory(buffer_addr, self.buffer_size)
            
            # 4. Append to file
            # 'ab' mode opens the file for Appending in Binary mode
            with open(self.filename, 'ab') as f:
                f.write(memory_data)
            
            print(f"[PyScript] Dumped Page {current_page} (4096 bytes).")

            # 5. Increment page[0]
            # We execute a GDB command to update the variable on the MCU
            gdb.execute(f"set var page[0] = {current_page + 1}")

            # 6. Resume execution safely
            # We use post_event to schedule the 'continue' command.
            # This prevents recursion errors inside the event handler.
            gdb.post_event(lambda: gdb.execute("continue"))
            
        except Exception as e:
            print(f"[PyScript] Error during dump: {e}")
            gdb.events.stop.disconnect(self.on_stop_handler)

# Register a custom command in GDB to start this class
class StartAutoDump(gdb.Command):
    """Starts the automated buffer dumping script."""
    def __init__(self):
        super(StartAutoDump, self).__init__("start_dump", gdb.COMMAND_USER)

    def invoke(self, arg, from_tty):
        FlashDumper()

# Instantiate the command so it's available
StartAutoDump()
