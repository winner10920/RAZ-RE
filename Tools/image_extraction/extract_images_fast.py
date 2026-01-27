#!/usr/bin/env python3
"""
Extract RGB565 images from flash binary using known structure and create GIFs
"""

import os
import argparse
import numpy as np
from PIL import Image
from pathlib import Path

def rgb565_to_rgb888(data):
    """Convert RGB565 bytes to RGB888 numpy array"""
    if len(data) % 2 != 0:
        data = data[:-1]
    
    # RGB565 is stored as big-endian: first byte is high byte, second is low byte
    rgb565 = np.frombuffer(data, dtype='>u2')  # big-endian uint16
    r = ((rgb565 >> 11) & 0x1F) * 255 // 31
    g = ((rgb565 >> 5) & 0x3F) * 255 // 63
    b = (rgb565 & 0x1F) * 255 // 31
    
    return np.stack([r, g, b], axis=-1).astype(np.uint8)

def extract_image(data, offset, width, height):
    """Extract a single image from binary data"""
    bytes_needed = width * height * 2
    if offset + bytes_needed > len(data):
        return None
    
    img_data = data[offset:offset + bytes_needed]
    rgb888 = rgb565_to_rgb888(img_data)
    
    if len(rgb888) != width * height:
        return None
    
    img_array = rgb888.reshape(height, width, 3)
    return Image.fromarray(img_array, 'RGB')

def create_gif(images, output_path, duration=100):
    """Create GIF from list of PIL images"""
    if len(images) == 0:
        return
    
    images[0].save(
        output_path,
        save_all=True,
        append_images=images[1:],
        duration=duration,
        loop=0
    )

def main():
    parser = argparse.ArgumentParser(description='Extract images from flash binary and create GIFs')
    parser.add_argument('input', help='Input binary file')
    parser.add_argument('-o', '--output', help='Output directory')
    parser.add_argument('-d', '--duration', type=int, default=100, help='GIF frame duration in ms')
    
    args = parser.parse_args()
    
    input_path = Path(args.input)
    if not input_path.exists():
        print(f"Error: File not found: {input_path}")
        return 1
    
    if args.output:
        output_dir = Path(args.output)
    else:
        output_dir = input_path.parent / f"{input_path.stem}_extracted"
    
    output_dir.mkdir(exist_ok=True)
    
    print(f"Reading {input_path}...")
    with open(input_path, 'rb') as f:
        data = f.read()
    
    print(f"File size: {len(data)} bytes")
    
    # Define image map from binF.js structure
    image_map = [
        {'name': 'Background', 'offset': 0x00000, 'width': 128, 'height': 160, 'count': 1},
        {'name': 'Battery', 'offset': 0x0A000, 'width': 40, 'height': 40, 'count': 6, 'step': 0x0BE0},
        {'name': 'Juice', 'offset': 0x0F320, 'width': 40, 'height': 40, 'count': 7, 'step': 0x0BE0},
        {'name': 'Normal_Mode', 'offset': 0x14642, 'width': 75, 'height': 80, 'count': 1},
        {'name': 'Boost_Mode', 'offset': 0x17522, 'width': 75, 'height': 80, 'count': 1},
        {'name': 'Moon', 'offset': 0x1A400, 'width': 43, 'height': 36, 'count': 1},
        {'name': 'Sun', 'offset': 0x1B018, 'width': 43, 'height': 36, 'count': 1},
        {'name': '12W', 'offset': 0x1BC30, 'width': 72, 'height': 36, 'count': 1},
        {'name': '24W', 'offset': 0x1D070, 'width': 72, 'height': 36, 'count': 1},
        {'name': 'Flame', 'offset': 0x1E4B0, 'width': 120, 'height': 32, 'count': 30, 'step': 0x1E00},
        {'name': 'Flame2', 'offset': 0x54AB0, 'width': 120, 'height': 32, 'count': 30, 'step': 0x1E00},
        {'name': 'Unknown_Bg', 'offset': 0x8B0B0, 'width': 42, 'height': 68, 'count': 84, 'step': 0x1650},
    ]
    
    md_lines = []
    md_lines.append("# Flash Image Extraction Report\n\n")
    md_lines.append(f"**Source:** `{input_path.name}`\n\n")
    md_lines.append("---\n\n")
    
    for entry in image_map:
        name = entry['name']
        offset = entry['offset']
        width = entry['width']
        height = entry['height']
        count = entry['count']
        step = entry.get('step', width * height * 2)
        
        print(f"\nProcessing: {name} ({count} images)")
        
        images = []
        for i in range(count):
            img_offset = offset + (i * step)
            img = extract_image(data, img_offset, width, height)
            
            if img is None:
                print(f"  Warning: Failed to extract {name} #{i}")
                continue
            
            images.append(img)
        
        if len(images) == 0:
            continue
        
        if count > 1:
            # Create GIF
            gif_path = output_dir / f"{name}_{width}x{height}.gif"
            create_gif(images, gif_path, duration=args.duration)
            print(f"  Created GIF: {gif_path.name} ({len(images)} frames)")
            
            md_lines.append(f"## {name.replace('_', ' ')}\n\n")
            md_lines.append(f"- **Type:** Animation\n")
            md_lines.append(f"- **Frames:** {len(images)}\n")
            md_lines.append(f"- **Dimensions:** {width}x{height}\n")
            md_lines.append(f"- **Offset:** 0x{offset:05X}\n\n")
            md_lines.append(f"![{name}]({gif_path.name})\n\n")
            md_lines.append("---\n\n")
        else:
            # Save as PNG
            png_path = output_dir / f"{name}_{width}x{height}.png"
            images[0].save(png_path)
            print(f"  Saved PNG: {png_path.name}")
            
            md_lines.append(f"## {name.replace('_', ' ')}\n\n")
            md_lines.append(f"- **Type:** Static Image\n")
            md_lines.append(f"- **Dimensions:** {width}x{height}\n")
            md_lines.append(f"- **Offset:** 0x{offset:05X}\n\n")
            md_lines.append(f"![{name}]({png_path.name})\n\n")
            md_lines.append("---\n\n")
    
    # Write markdown report
    md_path = output_dir / "report.md"
    with open(md_path, 'w') as f:
        f.writelines(md_lines)
    
    print(f"\nComplete! Output directory: {output_dir}")
    print(f"Markdown report: {md_path}")
    
    return 0

if __name__ == "__main__":
    exit(main())
