#!/usr/bin/env python3
"""
Parse C functions containing LCD initialization sequences and convert them
to LcdPacket array declarations.

Usage: ./parse_lcd_init.py <input_file> [output_file]
"""

import re
import sys

def parse_function_calls(code_block):
    """Extract function calls and their arguments from a code block."""
    packets = []
    
    # Match function calls like: function_name(arg); or function_name?(arg);
    # Updated pattern to allow optional '?' in function name
    pattern = r'([\w\?]+)\s*\(\s*([^)]+)\s*\);'
    matches = re.finditer(pattern, code_block)
    
    for match in matches:
        func_name = match.group(1)
        arg = match.group(2).strip()
        
        # Skip function calls that aren't lcd commands
        if 'lcd_send' not in func_name.lower() and 'send_cmd' not in func_name.lower() and 'send_data' not in func_name.lower():
            continue
        
        # Convert argument to hex format if needed
        if arg.startswith('0x') or arg.startswith('0X'):
            value = arg.upper()
        elif arg.isdigit():
            value = f"0x{int(arg):02X}"
        else:
            # Try to parse as integer for other numeric representations
            try:
                value = f"0x{int(arg, 0):02X}"
            except ValueError:
                value = arg
        
        # Determine if it's a command or data
        if 'cmd' in func_name.lower():
            packets.append(f"{{CMD_LOW, {value}}}")
        elif 'data' in func_name.lower():
            packets.append(f"{{DATA_HIGH, {value}}}")
    
    return packets

def extract_if_blocks(content):
    """Extract if/else if blocks from the function."""
    blocks = []
    
    # Find if statements and their conditions - updated to handle character literals
    # Matches: cVar1 == '\0' or cVar1 == '\x01' or cVar1 == 0, etc.
    if_pattern = r'if\s*\(\s*cVar\d+\s*==\s*([^)]+)\)\s*\{'
    
    # Split by 'if' and 'else if' to find blocks
    parts = re.split(r'(?:else\s+)?if\s*\([^)]+\)\s*\{', content)
    conditions = re.findall(if_pattern, content)
    
    # Process each block (skip first part which is before any if)
    for i, part in enumerate(parts[1:]):
        # Find the end of this block (look for closing brace at same level)
        brace_count = 1
        end_pos = 0
        for j, char in enumerate(part):
            if char == '{':
                brace_count += 1
            elif char == '}':
                brace_count -= 1
                if brace_count == 0:
                    end_pos = j
                    break
        
        if end_pos > 0:
            block_code = part[:end_pos]
            if i < len(conditions):
                condition = conditions[i].strip()
                # Convert character literal to readable name
                if condition.startswith("'"):
                    # Handle '\0', '\x01', etc.
                    if condition == "'\\0'":
                        condition = "type_0"
                    elif '\\x' in condition:
                        hex_val = condition.split('\\x')[1].rstrip("'")
                        condition = f"type_{hex_val}"
                    else:
                        # Remove quotes and convert to ordinal
                        char = condition.strip("'")
                        if len(char) == 1:
                            condition = f"type_{ord(char)}"
                        else:
                            condition = "type_unknown"
                else:
                    condition = f"type_{condition}"
            else:
                condition = f"unknown_{i}"
            blocks.append((condition, block_code))
    
    return blocks

def generate_lcd_packet_arrays(input_file, output_file=None):
    """Generate LcdPacket array declarations from input C file."""
    
    with open(input_file, 'r') as f:
        content = f.read()
    
    # Extract if blocks
    blocks = extract_if_blocks(content)
    
    if not blocks:
        print("No if blocks found in the input file.")
        return
    
    # Generate output
    output_lines = []
    output_lines.append("// LCD initialization sequences parsed from decompiled code")
    output_lines.append("// CMD_LOW = command, DATA_HIGH = data")
    output_lines.append("")
    
    for idx, (condition, code) in enumerate(blocks):
        # Parse the code block
        packets = parse_function_calls(code)
        
        if not packets:
            print(f"Warning: No packets found for condition '{condition}'")
            continue
        
        # Generate array declaration with meaningful name
        array_name = f"lcd_init_{condition}"
        output_lines.append(f"// Initialization sequence for {condition} ({len(packets)} commands)")
        output_lines.append(f"static const LcdPacket {array_name}[] = {{")
        
        # Format packets with 2 per line for readability
        for i in range(0, len(packets), 2):
            if i + 1 < len(packets):
                output_lines.append(f"    {packets[i]}, {packets[i+1]},")
            else:
                output_lines.append(f"    {packets[i]},")
        
        output_lines.append("};")
        output_lines.append("")
    
    # Write output
    output_text = '\n'.join(output_lines)
    
    if output_file:
        with open(output_file, 'w') as f:
            f.write(output_text)
        print(f"Generated {len(blocks)} array(s) in {output_file}")
    else:
        print(output_text)
        print(f"\n// Generated {len(blocks)} array(s)")

def main():
    if len(sys.argv) < 2:
        print("Usage: ./parse_lcd_init.py <input_file> [output_file]")
        print("\nExample: ./parse_lcd_init.py xd0022s output.c")
        sys.exit(1)
    
    input_file = sys.argv[1]
    output_file = sys.argv[2] if len(sys.argv) > 2 else None
    
    try:
        generate_lcd_packet_arrays(input_file, output_file)
    except FileNotFoundError:
        print(f"Error: File '{input_file}' not found")
        sys.exit(1)
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()
