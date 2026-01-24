#!/usr/bin/env python3
"""
Script to extract variable addresses from .map file and update myFlasher.py
"""
import re
import os

# Map Python variable names to C variable names (without case sensitivity)
VARIABLE_MAPPING = {
    'DATA_BUFFER_ADDR': 'buffer',
    'LEVEL_BUFFER_ADDR': 'lvl_buffer',
    'STATUS_REG_ADDR': 'status',
    'WRITE_FLAG_ADDR': 'write_flag',
    'LVL_RESET_FLAG_ADDR': 'lvl_reset_flag',
    'CONTINUE_FLAG_ADDR': 'continue_flag',
    'PAGE_ADDR': 'page',
    'MAINRAN_ADDR': 'mainran'
}

def parse_map_file(map_file_path):
    """Parse the .map file and extract symbol addresses"""
    symbols = {}
    
    with open(map_file_path, 'r') as f:
        content = f.read()
    
    # Look for patterns like: "    buffer                                   0x20000015   Data        4096  main.o(.bss.buffer)"
    # Format: symbol_name whitespace address
    pattern = r'^\s+(\w+)\s+(0x[0-9a-fA-F]{8})\s+Data'
    
    for line in content.split('\n'):
        match = re.match(pattern, line)
        if match:
            symbol = match.group(1)
            address = match.group(2)
            symbols[symbol.lower()] = address.upper()
    
    return symbols

def find_address(symbols, c_var_name):
    """Find address for a C variable name (case insensitive)"""
    c_var_lower = c_var_name.lower()
    
    # Try exact match first
    if c_var_lower in symbols:
        return symbols[c_var_lower]
    
    # Try to find partial matches
    for symbol, addr in symbols.items():
        if c_var_lower in symbol or symbol in c_var_lower:
            return addr
    
    return None

def update_python_script(python_file, address_map):
    """Update the Python script with new addresses"""
    with open(python_file, 'r') as f:
        lines = f.readlines()
    
    updated_lines = []
    updates_made = {}
    
    for line in lines:
        updated = False
        for py_var, c_var in VARIABLE_MAPPING.items():
            # Match lines like: "DATA_BUFFER_ADDR        = 0x20000015"
            pattern = rf'^({py_var}\s*=\s*)0x[0-9a-fA-F]+(.*)$'
            match = re.match(pattern, line)
            
            if match and py_var in address_map:
                new_addr = address_map[py_var]
                new_line = f"{match.group(1)}{new_addr}{match.group(2)}\n"
                updated_lines.append(new_line)
                updates_made[py_var] = new_addr
                updated = True
                break
        
        if not updated:
            updated_lines.append(line)
    
    # Write back to file
    with open(python_file, 'w') as f:
        f.writelines(updated_lines)
    
    return updates_made

def main():
    # File paths
    map_file = "/home/cody/RAZ-RE/Firmware/keil-mdk/MDK-ARM/out/ReFlasher/RAZ/ReFlasher.axf.map"
    python_file = "/home/cody/RAZ-RE/Flash/reflasher/myFlasher.py"
    
    print("Parsing map file...")
    symbols = parse_map_file(map_file)
    
    print(f"Found {len(symbols)} symbols in map file")
    
    # Find addresses for each variable
    address_map = {}
    not_found = []
    
    for py_var, c_var in VARIABLE_MAPPING.items():
        addr = find_address(symbols, c_var)
        if addr:
            address_map[py_var] = addr
            print(f"✓ {py_var:25s} -> {c_var:20s} = {addr}")
        else:
            not_found.append((py_var, c_var))
            print(f"✗ {py_var:25s} -> {c_var:20s} = NOT FOUND")
    
    if not_found:
        print(f"\n⚠ Warning: {len(not_found)} variables not found in map file")
        print("\nSearching for similar symbols...")
        for py_var, c_var in not_found:
            # Show potential matches
            c_var_lower = c_var.lower()
            matches = [s for s in symbols.keys() if c_var_lower in s or s in c_var_lower]
            if matches:
                print(f"\n  {py_var} (looking for '{c_var}'):")
                for m in matches[:5]:  # Show up to 5 matches
                    print(f"    - {m}: {symbols[m]}")
    
    if address_map:
        print(f"\n\nUpdating {python_file}...")
        updates = update_python_script(python_file, address_map)
        print(f"\n✓ Updated {len(updates)} addresses:")
        for var, addr in updates.items():
            print(f"  {var} = {addr}")
    else:
        print("\n✗ No addresses found to update")

if __name__ == "__main__":
    main()
