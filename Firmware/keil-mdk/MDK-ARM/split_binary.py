#!/usr/bin/env python3
"""
Split ReneePatched.binF into 4096-byte pieces named ReneePatched.binF.1, .2, etc.
"""

import sys
from pathlib import Path

BUFFER_SIZE = 4096
INPUT_FILE = "ReneePatched.binF"
NUM_CHUNKS = 256

def split_binary():
    input_path = Path(INPUT_FILE)
    
    if not input_path.exists():
        print(f"Error: {INPUT_FILE} not found in current directory")
        sys.exit(1)
    
    # Read file
    with open(input_path, 'rb') as f:
        data = f.read()
    
    file_size = len(data)
    print(f"Splitting {INPUT_FILE}: {file_size} bytes ({file_size // 1024} KB)")
    
    # Write chunks 1-256
    for i in range(1, NUM_CHUNKS + 1):
        start = (i - 1) * BUFFER_SIZE
        end = start + BUFFER_SIZE
        
        # Get chunk data
        if start < file_size:
            chunk_data = data[start:end]
            # Pad if needed
            if len(chunk_data) < BUFFER_SIZE:
                chunk_data += b'\xFF' * (BUFFER_SIZE - len(chunk_data))
                print(f"  {INPUT_FILE}.{i}: {len(data[start:end])} bytes + {BUFFER_SIZE - len(data[start:end])} padding")
            else:
                print(f"  {INPUT_FILE}.{i}: {BUFFER_SIZE} bytes")
        else:
            # File ended, fill with 0xFF
            chunk_data = b'\xFF' * BUFFER_SIZE
            print(f"  {INPUT_FILE}.{i}: {BUFFER_SIZE} bytes (all 0xFF)")
        
        # Write chunk
        chunk_path = Path(f"{INPUT_FILE}.{i}")
        with open(chunk_path, 'wb') as f:
            f.write(chunk_data)
    
    print(f"\n✓ Created 256 chunk files: {INPUT_FILE}.1 through {INPUT_FILE}.256")

if __name__ == '__main__':
    split_binary()
