#!/usr/bin/env python3
"""
Image to RGB565 Binary Converter
Converts an image file to RGB565 format (128x160 pixels) and writes to binary file.

Usage:
    python3 image_to_rgb565.py input_image.png
    python3 image_to_rgb565.py input_image.jpg

Output:
    input_image.rgb565 (binary file containing RGB565 pixel data)

The binary format is 16-bit little-endian RGB565 (5 bits red, 6 bits green, 5 bits blue).
Image is resized to fit 128×160 pixels.
"""

import sys
import os
from pathlib import Path

try:
    from PIL import Image
except ImportError:
    print("Error: PIL/Pillow is required. Install with:")
    print("  pip install Pillow")
    sys.exit(1)


def rgb888_to_rgb565(r, g, b):
    """
    Convert 8-bit RGB to 16-bit RGB565 format.
    
    RGB565: [RRRRRGGG GGGBBBBB]
    - R: 5 bits (bits 11-15)
    - G: 6 bits (bits 5-10)
    - B: 5 bits (bits 0-4)
    
    Args:
        r, g, b: 8-bit color values (0-255)
    
    Returns:
        16-bit RGB565 value (0-65535)
    """
    r5 = (r >> 3) & 0x1F       # Convert 8-bit to 5-bit
    g6 = (g >> 2) & 0x3F       # Convert 8-bit to 6-bit
    b5 = (b >> 3) & 0x1F       # Convert 8-bit to 5-bit
    
    rgb565 = (r5 << 11) | (g6 << 5) | b5
    return rgb565


def rgb565_to_bytes(rgb565):
    """
    Convert 16-bit RGB565 to big-endian bytes for Binary Ninja viewer.
    Binary Ninja reads as: (hi << 8) | lo
    
    Args:
        rgb565: 16-bit RGB565 value
    
    Returns:
        Tuple of (high_byte, low_byte)
    """
    lo = rgb565 & 0xFF
    hi = (rgb565 >> 8) & 0xFF
    return (hi, lo)


def convert_image(input_path, output_path=None, width=128, height=160):
    """
    Convert an image to RGB565 binary format.
    
    Args:
        input_path: Path to input image file
        output_path: Path to output binary file (default: same name, .rgb565 extension)
        width: Target width in pixels (default 128)
        height: Target height in pixels (default 160)
    
    Returns:
        True if successful, False otherwise
    """
    try:
        # Open and convert image to RGB
        print(f"Loading image: {input_path}")
        img = Image.open(input_path)
        img = img.convert('RGB')
        
        # Resize to target dimensions
        print(f"Resizing to {width}×{height}...")
        img = img.resize((width, height), Image.Resampling.LANCZOS)
        
        # Convert to RGB565 binary
        print("Converting to RGB565...")
        binary_data = bytearray()
        
        for y in range(height):
            for x in range(width):
                r, g, b = img.getpixel((x, y))
                rgb565 = rgb888_to_rgb565(r, g, b)
                lo, hi = rgb565_to_bytes(rgb565)
                binary_data.append(lo)
                binary_data.append(hi)
        
        # Determine output path
        if output_path is None:
            input_file = Path(input_path)
            output_path = input_file.with_suffix('.rgb565')
        
        # Write binary file
        print(f"Writing binary file: {output_path}")
        with open(output_path, 'wb') as f:
            f.write(binary_data)
        
        file_size = len(binary_data)
        print(f"✓ Success! ({file_size} bytes, {width}×{height} pixels)")
        return True
    
    except FileNotFoundError:
        print(f"Error: Input file not found: {input_path}")
        return False
    except Exception as e:
        print(f"Error: {e}")
        return False


def main():
    if len(sys.argv) < 2:
        print("Image to RGB565 Binary Converter")
        print()
        print("Usage:")
        print(f"  python3 {os.path.basename(__file__)} <input_image>")
        print()
        print("Examples:")
        print(f"  python3 {os.path.basename(__file__)} photo.png")
        print(f"  python3 {os.path.basename(__file__)} photo.jpg")
        print()
        print("Output: <input_filename>.rgb565")
        print()
        print("Supported formats: PNG, JPG, BMP, GIF, etc. (any PIL-supported format)")
        print("Output dimensions: 128×160 pixels (resized from input)")
        print("Output format: RGB565 16-bit little-endian binary")
        sys.exit(1)
    
    input_path = sys.argv[1]
    
    if convert_image(input_path):
        sys.exit(0)
    else:
        sys.exit(1)


if __name__ == '__main__':
    main()
