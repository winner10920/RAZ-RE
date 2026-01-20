#!/usr/bin/env python3
"""
Flash Image Patcher - Patch RGB565 image data into flash binary

Takes an RGB565 image file and writes it into a flash binary (.binF) at a
specified address, maintaining the original binary size.

Usage:
    python3 patch_flash_image.py <rgb565_file> <flash_binary> <hex_address>

Example:
    python3 patch_flash_image.py image.rgb565 flash.binF 0x0A000

The script will:
- Read the RGB565 image data
- Read the destination flash binary
- Overwrite the image data at the specified address
- Write the modified binary with the same filename (creates backup)
- Preserve the original file size
"""

import sys
import os
import shutil
from pathlib import Path


def parse_hex_address(addr_str):
    """Parse hex address string (with or without 0x prefix)."""
    addr_str = addr_str.strip()
    if addr_str.startswith('0x') or addr_str.startswith('0X'):
        return int(addr_str, 16)
    else:
        return int(addr_str, 16)


def patch_flash_binary(rgb565_path, flash_binary_path, address):
    """
    Patch RGB565 image data into flash binary at specified address.
    
    Args:
        rgb565_path: Path to .rgb565 source image file
        flash_binary_path: Path to .binF destination flash binary
        address: Integer address where to write the image data
    
    Returns:
        True if successful, False otherwise
    """
    # Read source RGB565 image
    try:
        with open(rgb565_path, 'rb') as f:
            rgb565_data = f.read()
        image_size = len(rgb565_data)
        print(f"✓ Loaded RGB565 image: {rgb565_path}")
        print(f"  Image size: {image_size} bytes ({image_size // 2} pixels)")
    except FileNotFoundError:
        print(f"✗ Error: RGB565 file not found: {rgb565_path}")
        return False
    except Exception as e:
        print(f"✗ Error reading RGB565 file: {e}")
        return False
    
    # Read destination flash binary
    try:
        with open(flash_binary_path, 'rb') as f:
            flash_data = bytearray(f.read())
        flash_size = len(flash_data)
        print(f"✓ Loaded flash binary: {flash_binary_path}")
        print(f"  Flash size: {flash_size} bytes (0x{flash_size:X})")
    except FileNotFoundError:
        print(f"✗ Error: Flash binary not found: {flash_binary_path}")
        return False
    except Exception as e:
        print(f"✗ Error reading flash binary: {e}")
        return False
    
    # Validate address and size
    if address < 0:
        print(f"✗ Error: Address cannot be negative: 0x{address:X}")
        return False
    
    if address >= flash_size:
        print(f"✗ Error: Address 0x{address:X} is beyond flash size 0x{flash_size:X}")
        return False
    
    end_address = address + image_size
    if end_address > flash_size:
        print(f"✗ Error: Image data would overflow flash boundary")
        print(f"  Start address: 0x{address:X}")
        print(f"  End address:   0x{end_address:X} (needs {image_size} bytes)")
        print(f"  Flash size:    0x{flash_size:X}")
        print(f"  Overflow by:   {end_address - flash_size} bytes")
        return False
    
    print(f"✓ Address range valid: 0x{address:X} - 0x{end_address-1:X}")
    
    # Create backup
    backup_path = flash_binary_path + '.backup'
    try:
        shutil.copy2(flash_binary_path, backup_path)
        print(f"✓ Created backup: {backup_path}")
    except Exception as e:
        print(f"⚠ Warning: Could not create backup: {e}")
    
    # Patch the data
    print(f"⚙ Patching {image_size} bytes at address 0x{address:X}...")
    flash_data[address:end_address] = rgb565_data
    
    # Write modified flash binary
    try:
        with open(flash_binary_path, 'wb') as f:
            f.write(flash_data)
        print(f"✓ Successfully patched: {flash_binary_path}")
        print(f"  Output size: {len(flash_data)} bytes (unchanged)")
        return True
    except Exception as e:
        print(f"✗ Error writing patched binary: {e}")
        return False


def main():
    if len(sys.argv) != 4:
        print("Flash Image Patcher")
        print()
        print("Usage:")
        print(f"  python3 {os.path.basename(__file__)} <rgb565_file> <flash_binary> <hex_address>")
        print()
        print("Arguments:")
        print("  rgb565_file    : Source RGB565 image file (.rgb565)")
        print("  flash_binary   : Destination flash binary (.binF or similar)")
        print("  hex_address    : Hexadecimal address where to patch (e.g., 0x0A000)")
        print()
        print("Example:")
        print(f"  python3 {os.path.basename(__file__)} image.rgb565 onboard_flash_dump.binF 0x0A000")
        print()
        print("Notes:")
        print("  - Creates a .backup file before modifying")
        print("  - Validates address boundaries")
        print("  - Maintains original binary size")
        print("  - Address can be specified with or without '0x' prefix")
        sys.exit(1)
    
    rgb565_file = sys.argv[1]
    flash_binary = sys.argv[2]
    address_str = sys.argv[3]
    
    # Parse address
    try:
        address = parse_hex_address(address_str)
        print(f"Target address: 0x{address:X} ({address} decimal)")
        print()
    except ValueError:
        print(f"✗ Error: Invalid hex address: {address_str}")
        print("  Address must be hexadecimal (e.g., 0x0A000 or A000)")
        sys.exit(1)
    
    # Perform patching
    success = patch_flash_binary(rgb565_file, flash_binary, address)
    
    if success:
        print()
        print("✓ Patching complete!")
        sys.exit(0)
    else:
        print()
        print("✗ Patching failed")
        sys.exit(1)


if __name__ == '__main__':
    main()
