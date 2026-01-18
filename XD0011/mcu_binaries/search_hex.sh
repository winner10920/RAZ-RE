#!/bin/bash

# Search for a hex pattern in binary file(s) and return the address
# Usage: ./search_hex.sh <filename(s)> <hex_pattern>
# Example: ./search_hex.sh file.bin "08 60 00 00 00 00 00"
# Example: ./search_hex.sh file*.bin "08 60 00 00 00 00 00"

if [ $# -lt 2 ]; then
    echo "Usage: $0 <filename(s)> <hex_pattern>"
    echo "Example: $0 file.bin '08 60 00 00 00 00 00'"
    echo "Example: $0 file*.bin '08 60 00 00 00 00 00'"
    echo ""
    echo "Hex pattern can be formatted as:"
    echo "  - Space-separated: 08 60 00 00"
    echo "  - No spaces: 08600000"
    echo "  - Mixed case: 08 60 00 00 (case-insensitive)"
    exit 1
fi

# Last argument is the pattern
pattern="${@: -1}"

# All arguments except the last are filenames
files=("${@:1:$#-1}")

# Normalize the pattern - remove spaces and convert to lowercase for searching
#normalized_pattern=$(echo "$pattern" | sed 's/[> ]//g' | tr '[:upper:]' '[:lower:]')
normalized_pattern=$(echo "$pattern" | sed 's/ //g' | tr '[:upper:]' '[:lower:]')

# Process each file
for filename in "${files[@]}"; do
    # Check if file exists
    if [ ! -f "$filename" ]; then
        echo "$filename: File not found" >&2
        continue
    fi

    # Search in xxd output for the pattern
    match=$(xxd "$filename" | grep -i "$normalized_pattern" | head -1)

    if [ -z "$match" ]; then
        echo "$filename: Pattern not found"
    else
        # Extract the offset (first column from xxd output)
        offset=$(echo "$match" | awk '{print $1}')
        echo "$filename: 0x$offset"
    fi
done
