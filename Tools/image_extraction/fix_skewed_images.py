#!/usr/bin/env python3
"""
Quick tool to detect and fix skewed images by trying different dimensions.
Focuses on images that show horizontal banding/skewing artifacts.
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

def detect_row_correlation(img_array):
    """
    Calculate average correlation between adjacent rows.
    Low correlation indicates wrong dimensions (venetian blind effect).
    """
    if img_array.shape[0] < 10:
        return 1.0
    
    correlations = []
    for i in range(min(img_array.shape[0] - 1, 30)):
        row1 = img_array[i, :].flatten().astype(float)
        row2 = img_array[i + 1, :].flatten().astype(float)
        
        if np.std(row1) > 5 and np.std(row2) > 5:
            corr = np.corrcoef(row1, row2)[0, 1]
            if not np.isnan(corr):
                correlations.append(corr)
    
    if len(correlations) == 0:
        return 1.0
    
    return np.mean(correlations)

def try_dimensions(data, offset, base_w, base_h, max_pixels=2048):
    """
    Try different dimensions around the base size.
    Look for dimensions that maximize row correlation.
    """
    print(f"\nTesting dimensions for image at offset 0x{offset:05X}")
    print(f"  Base size: {base_w}x{base_h}")
    
    best_corr = -1.0
    best_dims = (base_w, base_h)
    best_img = None
    
    # Calculate total pixels
    total_pixels = base_w * base_h
    
    # Try different widths while keeping similar total pixel count
    width_range = range(max(10, base_w - 20), min(base_w + 20, 200))
    
    results = []
    
    for test_w in width_range:
        # Calculate height to maintain similar pixel count
        test_h = total_pixels // test_w
        
        # Also try nearby heights
        for h_offset in [-2, -1, 0, 1, 2]:
            test_h_actual = test_h + h_offset
            
            if test_h_actual < 10 or test_h_actual > 200:
                continue
            
            bytes_needed = test_w * test_h_actual * 2
            
            if offset + bytes_needed > len(data):
                continue
            
            try:
                img_data = data[offset:offset + bytes_needed]
                rgb888 = rgb565_to_rgb888(img_data)
                
                if len(rgb888) != test_w * test_h_actual:
                    continue
                
                img_array = rgb888.reshape(test_h_actual, test_w, 3)
                
                # Check row correlation
                corr = detect_row_correlation(img_array)
                
                if corr > best_corr:
                    best_corr = corr
                    best_dims = (test_w, test_h_actual)
                    best_img = Image.fromarray(img_array, 'RGB')
                
                if corr > 0.5:  # Good correlation
                    results.append((test_w, test_h_actual, corr))
                
            except Exception as e:
                continue
    
    # Print top candidates
    if results:
        results.sort(key=lambda x: x[2], reverse=True)
        print(f"\n  Top candidates (by row correlation):")
        for w, h, corr in results[:5]:
            marker = " <-- BEST" if (w, h) == best_dims else ""
            print(f"    {w}x{h}: correlation={corr:.3f}{marker}")
    
    print(f"\n  Selected: {best_dims[0]}x{best_dims[1]} (correlation={best_corr:.3f})")
    
    if best_corr < 0.3:
        print(f"  WARNING: Low correlation, image may still be skewed!")
    
    return best_dims, best_img, best_corr

def main():
    parser = argparse.ArgumentParser(
        description='Fix skewed images by testing different dimensions'
    )
    parser.add_argument('input', help='Input binary file')
    parser.add_argument('offset', help='Hex offset of image (e.g., 0x26600)')
    parser.add_argument('width', type=int, help='Initial width guess')
    parser.add_argument('height', type=int, help='Initial height guess')
    parser.add_argument('-o', '--output', help='Output image file (PNG)')
    parser.add_argument('-c', '--count', type=int, default=1, 
                       help='Number of sequential images (for animations)')
    parser.add_argument('-s', '--stride', help='Hex stride between frames (e.g., 0x1E00)')
    
    args = parser.parse_args()
    
    # Parse offset
    offset = int(args.offset, 16) if args.offset.startswith('0x') else int(args.offset)
    
    # Read binary file
    input_path = Path(args.input)
    print(f"Reading {input_path}...")
    with open(input_path, 'rb') as f:
        data = f.read()
    
    print(f"File size: {len(data)} bytes")
    
    # Process single image or animation
    if args.count == 1:
        # Single image
        best_dims, best_img, corr = try_dimensions(data, offset, args.width, args.height)
        
        if best_img:
            output_path = args.output or f"fixed_{args.width}x{args.height}_0x{offset:05X}.png"
            best_img.save(output_path)
            print(f"\nSaved to: {output_path}")
    else:
        # Animation sequence
        if not args.stride:
            print("Error: --stride required for animations")
            return 1
        
        stride = int(args.stride, 16) if args.stride.startswith('0x') else int(args.stride)
        
        print(f"\nProcessing animation: {args.count} frames, stride=0x{stride:X}")
        
        # Test first frame to find best dimensions
        best_dims, _, _ = try_dimensions(data, offset, args.width, args.height)
        
        # Extract all frames with best dimensions
        frames = []
        for i in range(args.count):
            frame_offset = offset + (i * stride)
            bytes_needed = best_dims[0] * best_dims[1] * 2
            
            if frame_offset + bytes_needed > len(data):
                print(f"  Warning: Frame {i} extends beyond file")
                break
            
            img_data = data[frame_offset:frame_offset + bytes_needed]
            rgb888 = rgb565_to_rgb888(img_data)
            
            if len(rgb888) == best_dims[0] * best_dims[1]:
                img_array = rgb888.reshape(best_dims[1], best_dims[0], 3)
                frames.append(Image.fromarray(img_array, 'RGB'))
        
        if frames:
            output_path = args.output or f"fixed_animation_{best_dims[0]}x{best_dims[1]}_0x{offset:05X}.gif"
            frames[0].save(
                output_path,
                save_all=True,
                append_images=frames[1:],
                duration=100,
                loop=0
            )
            print(f"\nSaved {len(frames)} frames to: {output_path}")
    
    return 0

if __name__ == "__main__":
    exit(main())
